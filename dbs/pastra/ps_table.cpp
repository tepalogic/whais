/******************************************************************************
 PASTRA - A light database one file system and more.
 Copyright (C) 2008  Iulian Popa

 Address: Str Olimp nr. 6
 Pantelimon Ilfov,
 Romania
 Phone:   +40721939650
 e-mail:  popaiulian@gmail.com

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <assert.h>
#include <string.h>
#include <vector>
#include <algorithm>

#include "utils/wfile.h"

#include "dbs/dbs_mgr.h"
#include "dbs_exception.h"

#include "ps_table.h"
#include "ps_valintep.h"

using namespace std;

namespace whisper {
namespace pastra {

static const char PS_TEMP_TABLE_SUFFIX[]   = "pttable_";
static const char PS_TABLE_FIXFIELDS_EXT[] = "_f";
static const char PS_TABLE_VARFIELDS_EXT[] = "_v";

static const uint8_t PS_TABLE_SIGNATURE[] =
  { 0x50, 0x41, 0x53, 0x54, 0x52, 0x41, 0x54, 0x42 };

static const uint_t PS_HEADER_SIZE = 128;

static const uint_t PS_TABLE_SIG_OFF               = 0; //Signature
static const uint_t PS_TABLES_SIG_LEN              = 8;
static const uint_t PS_TABLE_FIELDS_COUNT_OFF      = 8; //Number of fields.
static const uint_t PS_TABLE_FIELDS_COUNT_LEN      = 4;
static const uint_t PS_TABLE_ELEMS_SIZE_OFF        = 12; //Size of the fields description area
static const uint_t PS_TABLE_ELEMS_SIZE_LEN        = 4;
static const uint_t PS_TABLE_RECORDS_COUNT_OFF     = 16; //Number of allocated records.
static const uint_t PS_TABLE_RECORDS_COUNT_LEN     = 8;
static const uint_t PS_TABLE_MAX_FILE_SIZE_OFF     = 24; //The maximum file size allowe for this table
static const uint_t PS_TABLE_MAX_FILE_SIZE_LEN     = 8;
static const uint_t PS_TABLE_MAINTABLE_SIZE_OFF    = 32; //Size of variable storage
static const uint_t PS_TABLE_MAINTABLE_SIZE_LEN    = 8;
static const uint_t PS_TABLE_VARSTORAGE_SIZE_OFF   = 40; //Size of variable storage
static const uint_t PS_TABLE_VARSTORAGE_SIZE_LEN   = 8;
static const uint_t PS_TABLE_BT_ROOT_OFF           = 48; //The root node of BTree holding the removed rows.
static const uint_t PS_TABLE_BT_ROOT_LEN           = 4;
static const uint_t PS_TABLE_BT_HEAD_OFF           = 52; //First node pointing to the removed BT nodes.
static const uint_t PS_TABLE_BT_HEAD_LEN           = 4;
static const uint_t PS_TABLE_ROW_SIZE_OFF          = 56; //Size of a row.
static const uint_t PS_TABLE_ROW_SIZE_LEN          = 4;

static const uint_t PS_RESEVED_FOR_FUTURE_OFF = 60;
static const uint_t PS_RESEVED_FOR_FUTURE_LEN = PS_HEADER_SIZE - PS_RESEVED_FOR_FUTURE_OFF;

static const uint_t MAX_FIELD_VALUE_ALIGN = 16; /* Bytes. */


struct PaddInterval
{
  PaddInterval(uint32_t begin_byte, uint32_t bytes_count) :
    mBegin(begin_byte * 8),
    mEnd(((begin_byte + bytes_count) * 8) - 1)
  {
    assert (mBegin <= mEnd);
  }

  uint32_t mBegin;
  uint32_t mEnd;

};

static uint32_t
get_strlens_till_index (const DBSFieldDescriptor* pFields, uint_t index)
{
  uint32_t result = 0;

  while (index-- > 0)
    {
      result += strlen(pFields->m_pFieldName) + 1;
      ++pFields;
    }

  return result;
}

static void
validate_field_name (const char* pFieldName)
{

  while (pFieldName[0])
    {
      if (!(((pFieldName[0] >= 'a') && (pFieldName[0] <= 'z'))
            || ((pFieldName[0] >= 'A') && (pFieldName[0] <= 'Z'))
            || ((pFieldName[0] >= '0') && (pFieldName[0] <= '9'))
            || (pFieldName[0] >= '_')))
        {
          throw DBSException(NULL, _EXTRA (DBSException::FIELD_NAME_INVALID));
        }
      ++pFieldName;
    }
}

static void
validate_field_descriptors (const DBSFieldDescriptor* const pFields,
                            const uint_t                    fieldsCount)
{
  assert ((fieldsCount > 0) && (pFields != NULL));
  for (uint_t firstIt = 0; firstIt < fieldsCount; ++firstIt)
    {
      validate_field_name(pFields[firstIt].m_pFieldName);

      for (uint_t secondIt = firstIt; secondIt < fieldsCount; ++secondIt)
        if (firstIt == secondIt)
          continue;
        else if (strcmp(pFields[firstIt].m_pFieldName,
                 pFields[secondIt].m_pFieldName) == 0)
          {
            throw DBSException (
                                pFields[firstIt].m_pFieldName,
                                _EXTRA (DBSException::FIELD_NAME_DUPLICATED)
                               );
          }

      if ((pFields[firstIt].m_FieldType == T_UNKNOWN)
          || (pFields[firstIt].m_FieldType >= T_END_OF_TYPES))
        {
          throw DBSException(NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));
        }

      else if (pFields[firstIt].isArray
               && (pFields[firstIt].m_FieldType == T_TEXT))
        {
          throw DBSException(NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));
        }
    }
}

static int
get_next_alignment (int size)
{
  int result = 1;

  size &= 0x0F, size |= 0x10;

  while ((size & 1) == 0)
    result <<= 1, size >>= 1;

  return result;
}

static void
arrange_field_entries (vector<DBSFieldDescriptor>& rvFields,
                       uint8_t* const              pOutFieldsDescription,
                       uint32_t&                   uOutRowSize)
{

  vector<PaddInterval> padds;

  FieldDescriptor* const pFieldDesc = _RC (FieldDescriptor*,
                                           pOutFieldsDescription);
  const int nullBitsRequested = rvFields.size ();      //One for each field.
  int       paddingBytesCount = 0;
  int       currentAlignment  = MAX_FIELD_VALUE_ALIGN; //Force the best choice

  //Find the best fields position in order to minimize the numbers of padding
  //bytes that are need it for values alignments
  for (uint_t fieldIndex = 0; fieldIndex < rvFields.size(); ++fieldIndex)
    {
      int foundIndex         = -1;    //-1 Nothing found!
      int foundReqAlign      = 1;     //Worst requested align
      int foundResultedAlign = 1;     //Worst resulted align
      int foundReqPaddsCount = MAX_FIELD_VALUE_ALIGN - 1;
      int foundSize          = 0;

      for (uint_t schIndex = fieldIndex; schIndex < rvFields.size(); ++schIndex)
        {
          int currIndexSize = PSValInterp::Size (
                                               rvFields[schIndex].m_FieldType,
                                               rvFields[schIndex].isArray
                                                  );

          int currReqAlign = PSValInterp::Alignment (
                                      rvFields[schIndex].m_FieldType,
                                      rvFields[schIndex].isArray
                                                      );
          int currReqPaddsCount = (currReqAlign > currentAlignment) ?
                                      (currReqAlign - currentAlignment) : 0;
          int currResultedAlign = get_next_alignment (
                          currReqPaddsCount + currIndexSize + uOutRowSize
                                                       );

          //Lets check if is better than what we have found until now
          if (foundReqPaddsCount > currReqPaddsCount)
            {
              //New best choice!
              foundIndex         = schIndex;
              foundReqAlign      = currReqAlign;
              foundResultedAlign = currResultedAlign;
              foundReqPaddsCount = currReqPaddsCount;
              foundSize          = currIndexSize;
            }
          else if (foundReqPaddsCount == currReqPaddsCount)
            {
              if ((foundReqAlign < currReqAlign)
                  || ((foundReqAlign == currReqAlign)
                      && (foundResultedAlign < currResultedAlign)))
                {
                  //New best choice!
                  foundIndex         = schIndex;
                  foundReqAlign      = currReqAlign;
                  foundResultedAlign = currResultedAlign;
                  foundReqPaddsCount = currReqPaddsCount;
                  foundSize          = currIndexSize;
                }
              else if ((foundReqAlign == currReqAlign)
                        && (foundResultedAlign == currResultedAlign))
                {
                  if (strcmp (rvFields[foundIndex].m_pFieldName,
                              rvFields[schIndex].m_pFieldName) > 0)
                    {
                      //New best choice!
                      foundIndex = schIndex;
                    }
                  else
                    continue; //Get the next one
                }
              else
                continue; //Get the next one
            }
          else
            continue; //Get the next one!
        }

      //We made our choice! Let's note it.
      assert ((foundIndex >= 0) && (foundSize > 0));

      DBSFieldDescriptor temp = rvFields[foundIndex];
      rvFields[foundIndex]    = rvFields[fieldIndex];
      rvFields[fieldIndex]    = temp;

      pFieldDesc[fieldIndex].m_NameOffset = get_strlens_till_index (
                                                            &rvFields.front(),
                                                            fieldIndex
                                                                   );
      pFieldDesc[fieldIndex].m_NameOffset += sizeof(FieldDescriptor) *
                                             rvFields.size();

      strcpy(_RC (char*,
                  pOutFieldsDescription + pFieldDesc[fieldIndex].m_NameOffset),
             _RC (const char *, temp.m_pFieldName));

      pFieldDesc[fieldIndex].m_Aquired         = 0;
      pFieldDesc[fieldIndex].m_IndexNodeSizeKB = 0;
      pFieldDesc[fieldIndex].m_IndexUnitsCount = 0;
      pFieldDesc[fieldIndex].m_StoreIndex      = uOutRowSize;
      pFieldDesc[fieldIndex].m_TypeDesc        = temp.m_FieldType;

      if (temp.isArray)
        pFieldDesc[fieldIndex].m_TypeDesc |= PS_TABLE_ARRAY_MASK;

      currentAlignment = foundResultedAlign;

      if (foundReqPaddsCount > 0)
        padds.push_back(PaddInterval(uOutRowSize, foundReqPaddsCount));

      uOutRowSize       += foundSize + foundReqPaddsCount;
      paddingBytesCount += foundReqPaddsCount;
    }

  //Increase the row size if we need more bytes to keep the null bits.
  const int32_t extraPaddBytesNeeded = ((nullBitsRequested + 7) / 8) -
                                       paddingBytesCount;
  if (extraPaddBytesNeeded > 0)
    {
      padds.push_back(PaddInterval(uOutRowSize, extraPaddBytesNeeded));
      uOutRowSize += extraPaddBytesNeeded;
    }

  //Round the row size so the first element of the next row is alligned
  int32_t needExtraAlign = PSValInterp::Alignment (rvFields[0].m_FieldType,
                                                   rvFields[0].isArray);
  needExtraAlign -= get_next_alignment(uOutRowSize);
  if (needExtraAlign > 0)
    uOutRowSize += needExtraAlign;

  //Reuse the padding bytes to hold the fileds value null bits indicators.
  for (uint_t fieldIndex = 0; fieldIndex < rvFields.size(); ++fieldIndex)
    {
      pFieldDesc[fieldIndex].m_NullBitIndex = padds[0].mBegin++;

      if (padds[0].mBegin > padds[0].mEnd)
        padds.erase(padds.begin());
    }
}

static void
create_table_file (const uint64_t            maxFileSize,
                   const char*             pBaseFileName,
                   const DBSFieldDescriptor* pFields,
                   uint_t                    fieldsCount)
{
  //Check the arguments
  if ((pFields == NULL) || (fieldsCount == 0) || (fieldsCount > 0xFFFFu))
    throw DBSException (NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED));

  //Compute the table header descriptor size
  const uint32_t descriptorsSize = sizeof (FieldDescriptor) * fieldsCount +
                                   get_strlens_till_index(pFields, fieldsCount);

  //Validate the optimally rearrange the fields for minimum row size
  uint_t rowSize = 0;
  validate_field_descriptors (pFields, fieldsCount);

  vector<DBSFieldDescriptor> vect (pFields + 0, pFields + fieldsCount);
  auto_ptr<uint8_t> apFieldDescription (new uint8_t[descriptorsSize]);

  arrange_field_entries(vect, apFieldDescription.get(), rowSize);
  pFields = &vect.front();

  File tableFile (pBaseFileName, WHC_FILECREATE_NEW | WHC_FILERDWR);

  auto_ptr<uint8_t> apBuffer(new uint8_t[PS_HEADER_SIZE]);
  uint8_t* const    pBuffer = apBuffer.get();

  memcpy (pBuffer, PS_TABLE_SIGNATURE, sizeof PS_TABLE_SIGNATURE);

  *_RC (uint32_t*, pBuffer + PS_TABLE_FIELDS_COUNT_OFF)    = fieldsCount;
  *_RC (uint32_t*, pBuffer + PS_TABLE_ELEMS_SIZE_OFF)      = descriptorsSize;
  *_RC (uint64_t*, pBuffer + PS_TABLE_RECORDS_COUNT_OFF)   = 0;
  *_RC (uint64_t*, pBuffer + PS_TABLE_VARSTORAGE_SIZE_OFF) = 0;
  *_RC (uint32_t*, pBuffer + PS_TABLE_ROW_SIZE_OFF)        = rowSize;
  *_RC (NODE_INDEX*, pBuffer + PS_TABLE_BT_ROOT_OFF)       = NIL_NODE;
  *_RC (NODE_INDEX*, pBuffer + PS_TABLE_BT_HEAD_OFF)       = NIL_NODE;
  *_RC (uint64_t*, pBuffer + PS_TABLE_MAX_FILE_SIZE_OFF)   = maxFileSize;
  *_RC (uint64_t*, pBuffer + PS_TABLE_MAINTABLE_SIZE_OFF)  = ~0;


  assert (sizeof (NODE_INDEX) == PS_TABLE_BT_HEAD_LEN);
  assert (sizeof (NODE_INDEX) == PS_TABLE_BT_ROOT_LEN);

  memset(pBuffer + PS_RESEVED_FOR_FUTURE_OFF, 0, PS_RESEVED_FOR_FUTURE_LEN);

  //Write the first header part to reserve the space!
  tableFile.Write(pBuffer, PS_HEADER_SIZE);

  //Write the field descriptors;
  tableFile.Write(apFieldDescription.get(), descriptorsSize);

  const uint_t toFill = tableFile.Tell () % TableRmNode::RAW_NODE_SIZE;

  if (toFill != 0)
    {
      static const uint8_t dump [TableRmNode::RAW_NODE_SIZE] = {0,};
      assert ((tableFile.Tell() == tableFile.GetSize()));
      assert ((tableFile.Tell() < sizeof dump));

      tableFile.Write (dump, sizeof dump - toFill);
    }
  assert ((tableFile.Tell() == tableFile.GetSize()));

  *_RC (uint64_t*, pBuffer + PS_TABLE_MAINTABLE_SIZE_OFF) = tableFile.GetSize ();

  tableFile.Seek (0, WHC_SEEK_BEGIN);
  tableFile.Write (pBuffer, PS_HEADER_SIZE);
}


PersistentTable::PersistentTable (DbsHandler&   dbsHandler,
                                  const string& tableName)
  : PrototypeTable (dbsHandler),
    m_DbsSettings (DBSGetSeettings ()),
    m_MaxFileSize (0),
    m_VariableStorageSize (0),
    m_BaseFileName (dbsHandler.WorkingDir () + tableName),
    m_apMainTable (NULL),
    m_apFixedFields (NULL),
    m_pVariableFields (NULL),
    m_Removed (false)
{
  InitFromFile ();

  if (m_MaxFileSize != dbsHandler.MaxFileSize ())
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_INCONSITENCY));

  assert (m_apMainTable.get () != NULL);

  uint_t       blkSize  = DBSSettings ().m_TableCacheBlkSize;
  const uint_t blkCount = DBSSettings ().m_TableCacheBlkCount;
  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < m_RowSize)
    blkSize *= 2;

  m_RowCache.Init (*this, m_RowSize, blkSize, blkCount);

  InitVariableStorages ();
  InitIndexedFields ();
}


PersistentTable::PersistentTable (DbsHandler&               dbsHandler,
                                  const string&             tableName,
                                  const DBSFieldDescriptor* pFields,
                                  const uint_t              fieldsCount,
                                  const bool                temporal)
  : PrototypeTable (dbsHandler),
    m_DbsSettings (DBSGetSeettings ()),
    m_MaxFileSize (0),
    m_VariableStorageSize (0),
    m_BaseFileName (dbsHandler.WorkingDir () + tableName),
    m_apMainTable (NULL),
    m_apFixedFields (NULL),
    m_pVariableFields (NULL),
    m_Removed (false)
{
  create_table_file (dbsHandler.MaxFileSize (),
                     m_BaseFileName.c_str (),
                     pFields,
                     fieldsCount);
  InitFromFile ();

  assert (m_apMainTable.get () != NULL);

  uint_t       blkSize  = DBSSettings ().m_TableCacheBlkSize;
  const uint_t blkCount = DBSSettings ().m_TableCacheBlkCount;
  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < m_RowSize)
    blkSize *= 2;

  m_RowCache.Init (*this, m_RowSize, blkSize, blkCount);

  InitVariableStorages ();
  InitIndexedFields ();

}

PersistentTable::~PersistentTable ()
{
  Flush ();

  for (FIELD_INDEX fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex)
    if (m_vIndexNodeMgrs[fieldIndex] != NULL)
      {
        FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

        uint64_t unitsCount = m_MaxFileSize - 1;

        unitsCount += m_vIndexNodeMgrs[fieldIndex]->GetIndexRawSize();
        unitsCount /= m_MaxFileSize;

        field.m_IndexUnitsCount = unitsCount;
        delete m_vIndexNodeMgrs [fieldIndex];
      }
  MakeHeaderPersistent ();

  if (m_pVariableFields != NULL)
    m_pVariableFields->ReleaseReference ();
}

bool
PersistentTable::IsTemporal () const
{
  return false;
}

I_DBSTable&
PersistentTable::Spawn () const
{
  return *(new TemporalTable (*this));
}

void
PersistentTable::InitFromFile ()
{
  uint64_t   mainTableSize = 0;
  uint8_t    aTableHdr[PS_HEADER_SIZE];

  File mainTableFile (m_BaseFileName.c_str(),
                       WHC_FILEOPEN_EXISTING | WHC_FILEREAD);

  mainTableFile.Seek(0, WHC_SEEK_BEGIN);
  mainTableFile.Read(aTableHdr, PS_HEADER_SIZE);

  if (memcmp(aTableHdr, PS_TABLE_SIGNATURE, PS_TABLES_SIG_LEN) != 0)
    throw DBSException(NULL, _EXTRA (DBSException::TABLE_INVALID));

  //Retrieve the header information.
  m_FieldsCount          = *_RC (uint32_t*, aTableHdr + PS_TABLE_FIELDS_COUNT_OFF);
  m_DescriptorsSize      = *_RC (uint32_t*, aTableHdr + PS_TABLE_ELEMS_SIZE_OFF);
  m_RowsCount            = *_RC (uint64_t*, aTableHdr + PS_TABLE_RECORDS_COUNT_OFF);
  m_VariableStorageSize  = *_RC (uint64_t*, aTableHdr + PS_TABLE_VARSTORAGE_SIZE_OFF);
  m_RowSize              = *_RC (uint32_t*, aTableHdr + PS_TABLE_ROW_SIZE_OFF);
  m_RootNode             = *_RC (NODE_INDEX*, aTableHdr + PS_TABLE_BT_ROOT_OFF);
  m_FirstUnallocatedRoot = *_RC (NODE_INDEX*, aTableHdr + PS_TABLE_BT_HEAD_OFF);
  m_MaxFileSize          = *_RC (uint64_t*, aTableHdr + PS_TABLE_MAX_FILE_SIZE_OFF);
  mainTableSize          = *_RC (uint64_t*, aTableHdr + PS_TABLE_MAINTABLE_SIZE_OFF);

  if ((m_FieldsCount == 0) ||
      (m_DescriptorsSize < (sizeof(FieldDescriptor) * m_FieldsCount)) ||
      (mainTableSize < PS_HEADER_SIZE))
    {
      throw DBSException(NULL, _EXTRA (DBSException::TABLE_INVALID));
    }

  //Cache the field descriptors in memory
  m_FieldsDescriptors.reset(new uint8_t[m_DescriptorsSize]);
  mainTableFile.Read(_CC (uint8_t*, m_FieldsDescriptors.get ()),
                     m_DescriptorsSize);
  mainTableFile.Close ();

  m_apMainTable.reset (new FileContainer (
                          m_BaseFileName.c_str(),
                          m_MaxFileSize,
                          (mainTableSize + m_MaxFileSize - 1) / m_MaxFileSize
                                         ));
}

void
PersistentTable::InitVariableStorages ()
{
  m_apFixedFields.reset (
      new FileContainer(
          (m_BaseFileName + PS_TABLE_FIXFIELDS_EXT).c_str(),
          m_MaxFileSize,
          ((m_RowSize * m_RowsCount) + m_MaxFileSize - 1) / m_MaxFileSize
                       )
                        );

  for (FIELD_INDEX fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex)
    {
        DBSFieldDescriptor fieldDesc = GetFieldDescriptor (fieldIndex);

        assert ((fieldDesc.m_FieldType > T_UNKNOWN)
                && (fieldDesc.m_FieldType < T_UNDETERMINED));

        if (fieldDesc.isArray || (fieldDesc.m_FieldType == T_TEXT))
          {
            auto_ptr<VLVarsStore> hold (new VLVarsStore);

            hold->Init ((m_BaseFileName + PS_TABLE_VARFIELDS_EXT).c_str(),
                        m_VariableStorageSize,
                        m_MaxFileSize);
            m_pVariableFields = hold.release ();
            m_pVariableFields->RegisterReference ();

            break; //We finished here!
          }
    }
}

void
PersistentTable::InitIndexedFields ()
{
  for (FIELD_INDEX fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex)
    {
      FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

      if (field.m_IndexNodeSizeKB == 0)
        {
          assert (field.m_IndexUnitsCount == 0);
          m_vIndexNodeMgrs.push_back (NULL);
          continue;
        }

      string containerNameBase = m_BaseFileName;

      containerNameBase += '_';
      containerNameBase += _RC (char*, m_FieldsDescriptors.get ()) +
                            field.m_NameOffset;
      containerNameBase += "_bt";

      auto_ptr <I_DataContainer> apIndexContainer (
                               new FileContainer (containerNameBase.c_str (),
                                                  m_MaxFileSize,
                                                  field.m_IndexUnitsCount )
                                                  );
      m_vIndexNodeMgrs.push_back (
            new FieldIndexNodeManager (apIndexContainer,
                                       field.m_IndexNodeSizeKB * 1024,
                                       0x400000, //4MB
                                       _SC (DBS_FIELD_TYPE, field.m_TypeDesc),
                                       false)
                                  );
    }
}

void
PersistentTable::MakeHeaderPersistent ()
{
  if (m_Removed)
    return ; //We were removed. We were removed.

  uint8_t aTableHdr[PS_HEADER_SIZE];

  memcpy (aTableHdr, PS_TABLE_SIGNATURE, sizeof PS_TABLE_SIGNATURE);

  *_RC (uint32_t*, aTableHdr + PS_TABLE_FIELDS_COUNT_OFF)    = m_FieldsCount;
  *_RC (uint32_t*, aTableHdr + PS_TABLE_ELEMS_SIZE_OFF)      = m_DescriptorsSize;
  *_RC (uint64_t*, aTableHdr + PS_TABLE_RECORDS_COUNT_OFF)   = m_RowsCount;
  *_RC (uint64_t*, aTableHdr + PS_TABLE_VARSTORAGE_SIZE_OFF) =
      (m_pVariableFields != NULL) ? m_pVariableFields->Size () : 0;
  *_RC (uint32_t*, aTableHdr + PS_TABLE_ROW_SIZE_OFF)        = m_RowSize;
  *_RC (NODE_INDEX*, aTableHdr + PS_TABLE_BT_ROOT_OFF)       = m_RootNode;
  *_RC (NODE_INDEX*, aTableHdr + PS_TABLE_BT_HEAD_OFF)       = m_FirstUnallocatedRoot;
  *_RC (uint64_t*, aTableHdr + PS_TABLE_MAX_FILE_SIZE_OFF)   = m_MaxFileSize;
  *_RC (uint64_t*, aTableHdr + PS_TABLE_MAINTABLE_SIZE_OFF)  = m_apMainTable->Size ();

  memset(aTableHdr + PS_RESEVED_FOR_FUTURE_OFF, 0, PS_RESEVED_FOR_FUTURE_LEN);

  m_apMainTable->Write (0, sizeof aTableHdr, aTableHdr);
  m_apMainTable->Write (sizeof aTableHdr,
                        m_DescriptorsSize,
                        m_FieldsDescriptors.get ());
}

void
PersistentTable::RemoveFromDatabase ()
{
  if (m_apFixedFields.get() != NULL)
    m_apFixedFields->MarkForRemoval();

  if (m_pVariableFields != NULL)
    m_pVariableFields->MarkForRemoval();

  for (FIELD_INDEX fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex )
    if (m_vIndexNodeMgrs[fieldIndex]  != NULL)
      m_vIndexNodeMgrs[fieldIndex]->MarkForRemoval ();

  m_apMainTable->MarkForRemoval ();

  m_Removed = true;
}

I_DataContainer*
PersistentTable::CreateIndexContainer (const FIELD_INDEX field)
{
  assert (m_BaseFileName.size () > 0);

  DBSFieldDescriptor desc = GetFieldDescriptor (field);

  string containerNameBase = m_BaseFileName;

  containerNameBase += '_';
  containerNameBase += desc.m_pFieldName;
  containerNameBase += "_bt";

  return new FileContainer (containerNameBase.c_str (),
                            m_DbsSettings.m_MaxFileSize,
                            0);
}

void
PersistentTable::Flush ()
{
  if (m_pVariableFields != NULL)
    m_pVariableFields->Flush ();

  PrototypeTable::Flush ();
}

I_DataContainer&
PersistentTable::FixedFieldsContainer ()
{
  assert (m_apFixedFields.get () != NULL);
  return *m_apFixedFields.get ();
}

I_DataContainer&
PersistentTable::MainTableContainer ()
{
  assert (m_apMainTable.get () != NULL);
  return *m_apMainTable.get ();
}

VLVarsStore&
PersistentTable::VariableFieldsStore ()
{
  assert (m_pVariableFields != NULL);
  return *m_pVariableFields;
}

////////////////////TemporalTable//////////////////////////////////////////////


TemporalTable::TemporalTable (DbsHandler&               dbsHandler,
                              const DBSFieldDescriptor* pFields,
                              const FIELD_INDEX         fieldsCount)
  : PrototypeTable (dbsHandler),
    m_apMainTable (NULL),
    m_apFixedFields (NULL),
    m_pVariableFields (NULL)
{

  //Check the arguments
  if ((pFields == NULL) || (fieldsCount == 0) || (fieldsCount > 0xFFFFu))
    throw(DBSException(NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED)));

  //Compute the table header descriptor size
  const uint32_t descriptorsSize = sizeof(FieldDescriptor) * fieldsCount +
                                   get_strlens_till_index(pFields,
                                                          fieldsCount);

  //Optimally rearrange the fields for minimum row size
  uint_t rowSize = 0;
  validate_field_descriptors(pFields, fieldsCount);

  vector<DBSFieldDescriptor> vect(pFields + 0, pFields + fieldsCount);
  auto_ptr<uint8_t> apFieldDescription(new uint8_t[descriptorsSize]);

  arrange_field_entries(vect, apFieldDescription.get(), rowSize);
  pFields = &vect.front();

  m_FieldsCount     = fieldsCount;
  m_DescriptorsSize = descriptorsSize;
  m_RowSize         = rowSize;
  m_FieldsDescriptors.reset (apFieldDescription.release ());

  m_vIndexNodeMgrs.insert (m_vIndexNodeMgrs.begin (),
                           m_FieldsCount,
                           NULL);

  uint_t       blkSize  = DBSSettings ().m_TableCacheBlkSize;
  const uint_t blkCount = DBSSettings ().m_TableCacheBlkCount;
  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < m_RowSize)
    blkSize *= 2;

  m_RowCache.Init (*this, m_RowSize, blkSize, blkCount);
}

TemporalTable::TemporalTable (const PrototypeTable& prototype)
  : PrototypeTable (prototype),
    m_apMainTable (NULL),
    m_apFixedFields (NULL),
    m_pVariableFields (NULL)
{

  m_vIndexNodeMgrs.insert (m_vIndexNodeMgrs.begin (),
                           m_FieldsCount,
                           NULL);

  uint_t       blkSize  = DBSSettings ().m_TableCacheBlkSize;
  const uint_t blkCount = DBSSettings ().m_TableCacheBlkCount;
  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < m_RowSize)
    blkSize *= 2;

  m_RowCache.Init (*this, m_RowSize, blkSize, blkCount);
}

TemporalTable::~TemporalTable ()
{
  Flush ();

  for (FIELD_INDEX fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex)
    delete m_vIndexNodeMgrs [fieldIndex];

  if (m_pVariableFields != NULL)
    m_pVariableFields->ReleaseReference ();
}

bool
TemporalTable::IsTemporal () const
{
  return true;
}

I_DBSTable&
TemporalTable::Spawn () const
{
  return *(new TemporalTable (*this));
}

void
TemporalTable::Flush ()
{
  if (m_pVariableFields != NULL)
    m_pVariableFields->Flush ();

  PrototypeTable::Flush ();
}

void
TemporalTable::MakeHeaderPersistent ()
{
  //Do nothing!
}

I_DataContainer*
TemporalTable::CreateIndexContainer (const FIELD_INDEX)
{
  return new TempContainer (m_Dbs.WorkingDir ().c_str (), 4096);
}

I_DataContainer&
TemporalTable::MainTableContainer ()
{

  if (m_apMainTable.get () == NULL)
    m_apMainTable.reset (new TempContainer (m_Dbs.WorkingDir ().c_str (),
                                            4096));

  return *m_apMainTable.get ();
}

I_DataContainer&
TemporalTable::FixedFieldsContainer ()
{
  if (m_apFixedFields.get () == NULL)
    m_apFixedFields.reset (new TempContainer (m_Dbs.WorkingDir ().c_str (),
                                              4096));

  return *m_apFixedFields.get ();
}

VLVarsStore&
TemporalTable::VariableFieldsStore ()
{
  if (m_pVariableFields == NULL)
    {
      auto_ptr<VLVarsStore> hold (new VLVarsStore ());
      hold->Init (m_Dbs.WorkingDir ().c_str (), 4096);

      m_pVariableFields = hold.release ();
      m_pVariableFields->RegisterReference ();
    }

  return *m_pVariableFields;
}

} //namespace pastra
} //namespace whisper

