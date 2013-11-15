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
#include "utils/le_converter.h"

#include "dbs/dbs_mgr.h"
#include "dbs_exception.h"

#include "ps_table.h"
#include "ps_serializer.h"

using namespace std;

namespace whisper {
namespace pastra {



static const char PS_TEMP_TABLE_SUFFIX[]   = "pttable_";
static const char PS_TABLE_FIXFIELDS_EXT[] = "_f";
static const char PS_TABLE_VARFIELDS_EXT[] = "_v";


static const uint8_t PS_TABLE_SIGNATURE[] = { 0x50, 0x41, 0x53, 0x54,
                                              0x52, 0x41, 0x54, 0x42 };


static const uint_t PS_HEADER_SIZE = 128;

static const uint_t PS_TABLE_SIG_OFF               = 0;
static const uint_t PS_TABLES_SIG_LEN              = 8;
static const uint_t PS_TABLE_FIELDS_COUNT_OFF      = 8;
static const uint_t PS_TABLE_FIELDS_COUNT_LEN      = 4;
static const uint_t PS_TABLE_ELEMS_SIZE_OFF        = 12;
static const uint_t PS_TABLE_ELEMS_SIZE_LEN        = 4;
static const uint_t PS_TABLE_RECORDS_COUNT_OFF     = 16;
static const uint_t PS_TABLE_RECORDS_COUNT_LEN     = 8;
static const uint_t PS_TABLE_MAX_FILE_SIZE_OFF     = 24;
static const uint_t PS_TABLE_MAX_FILE_SIZE_LEN     = 8;
static const uint_t PS_TABLE_MAINTABLE_SIZE_OFF    = 32;
static const uint_t PS_TABLE_MAINTABLE_SIZE_LEN    = 8;
static const uint_t PS_TABLE_VARSTORAGE_SIZE_OFF   = 40;
static const uint_t PS_TABLE_VARSTORAGE_SIZE_LEN   = 8;
static const uint_t PS_TABLE_BT_ROOT_OFF           = 48;
static const uint_t PS_TABLE_BT_ROOT_LEN           = 4;
static const uint_t PS_TABLE_BT_HEAD_OFF           = 52;
static const uint_t PS_TABLE_BT_HEAD_LEN           = 4;
static const uint_t PS_TABLE_ROW_SIZE_OFF          = 56;
static const uint_t PS_TABLE_ROW_SIZE_LEN          = 4;

static const uint_t PS_RESEVED_FOR_FUTURE_OFF = 60;
static const uint_t PS_RESEVED_FOR_FUTURE_LEN = PS_HEADER_SIZE -
                                                  PS_RESEVED_FOR_FUTURE_OFF;

static uint_t
get_fields_names_len (const DBSFieldDescriptor* fields, uint_t count)
{
  uint_t result = 0;

  while (count-- > 0)
    {
      result += strlen (fields[0].name) + 1;
      ++fields;
    }

  return result;
}


static void
validate_field_name (const char* name)
{

  while (name[0])
    {
      if (! (((name[0] >= 'a') && (name[0] <= 'z'))
              || ((name[0] >= 'A') && (name[0] <= 'Z'))
              || ((name[0] >= '0') && (name[0] <= '9'))
              || (name[0] >= '_')))
        {
          throw DBSException(NULL, _EXTRA (DBSException::FIELD_NAME_INVALID));
        }
      ++name;
    }
}


static void
validate_field_descriptors (const DBSFieldDescriptor* const fields,
                            const uint_t                    fieldsCount)
{
  assert ((fieldsCount > 0) && (fields != NULL));

  for (uint_t i = 0; i < fieldsCount; ++i)
    {
      validate_field_name (fields[i].name);

      // Check that all fields have different names.
      for (uint_t j = i + 1; j < fieldsCount; ++j)
        {
          if (strcmp(fields[i].name, fields[j].name) == 0)
            {
              throw DBSException (fields[i].name,
                                  _EXTRA (DBSException::FIELD_NAME_DUPLICATED));
            }
        }

      if ((fields[i].type == T_UNKNOWN) || (fields[i].type >= T_END_OF_TYPES))
        throw DBSException(NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));

      if (fields[i].isArray && (fields[i].type == T_TEXT))
        throw DBSException(NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));
    }
}


static bool
compare_fields (const DBSFieldDescriptor& f1, const DBSFieldDescriptor& f2)
{
  assert (strcmp (f1.name, f2.name) != 0);

  return strcmp (f1.name, f2.name) < 0;
}


static void
normalize_fields (vector<DBSFieldDescriptor>&   fields,
                  uint_t* const                 outRowsSize,
                  uint8_t* const                outFields)
{
  const uint_t fieldsCount = fields.size ();

  assert (fieldsCount > 0);

  FieldDescriptor* const fieldsDesc   = _RC (FieldDescriptor*, outFields);
  uint_t                 fieldNameOff = sizeof (fieldsDesc[0]) * fieldsCount;

  memset (outFields, 0, sizeof (fieldsDesc[0]) * fieldsCount);
  sort (fields.begin (), fields.end (), compare_fields);

  *outRowsSize = (fieldsCount + 7) / 8;

  for (uint_t i = 0; i <fieldsCount; i++)
    {
      fieldsDesc[i].NullBitIndex (i);
      fieldsDesc[i].RowDataOff (*outRowsSize);
      fieldsDesc[i].NameOffset (fieldNameOff);

      if (fields[i].isArray)
        fieldsDesc[i].Type (fields[i].type | PS_TABLE_ARRAY_MASK);

      else
        fieldsDesc[i].Type (fields[i].type);


      const uint_t nameLen = strlen (fields[i].name) + 1;

      assert (nameLen > 1);

      memcpy (outFields + fieldNameOff, fields[i].name, nameLen);
      fieldNameOff += nameLen;

      *outRowsSize += Serializer::Size (fields[i].type, fields[i].isArray);
    }
}


static void
create_table_file (const uint64_t                  maxFileSize,
                   const char* const               filePrefix,
                   const DBSFieldDescriptor* const fields,
                   const uint_t                    fieldsCount)
{
  //Check the arguments
  if ((fields == NULL) || (fieldsCount == 0) || (fieldsCount > 0xFFFFu))
    throw DBSException (NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED));

  //Compute the table header descriptor size
  const uint32_t descriptorsSize = sizeof (FieldDescriptor) * fieldsCount +
                                     get_fields_names_len(fields, fieldsCount);

  validate_field_descriptors (fields, fieldsCount);

  vector<DBSFieldDescriptor> vect (fields + 0, fields + fieldsCount);
  auto_ptr<uint8_t>          fieldsDescs (new uint8_t[descriptorsSize]);
  uint_t                     rowSize;

  normalize_fields (vect, &rowSize, fieldsDescs.get ());

  File tableFile (filePrefix, WHC_FILECREATE_NEW | WHC_FILERDWR);

  auto_ptr<uint8_t> tableHeader(new uint8_t[PS_HEADER_SIZE]);
  uint8_t* const    header = tableHeader.get();

  memcpy (header, PS_TABLE_SIGNATURE, sizeof PS_TABLE_SIGNATURE);

  store_le_int32 (fieldsCount,     header + PS_TABLE_FIELDS_COUNT_OFF);
  store_le_int32 (descriptorsSize, header + PS_TABLE_ELEMS_SIZE_OFF);
  store_le_int64 (0,               header + PS_TABLE_RECORDS_COUNT_OFF);
  store_le_int64 (0,               header + PS_TABLE_VARSTORAGE_SIZE_OFF);
  store_le_int32 (rowSize,         header + PS_TABLE_ROW_SIZE_OFF);
  store_le_int32 (NIL_NODE,        header + PS_TABLE_BT_ROOT_OFF);
  store_le_int32 (NIL_NODE,        header + PS_TABLE_BT_HEAD_OFF);
  store_le_int64 (maxFileSize,     header + PS_TABLE_MAX_FILE_SIZE_OFF);
  store_le_int64 (~(uint64_t)0,    header + PS_TABLE_MAINTABLE_SIZE_OFF);

  assert (sizeof (NODE_INDEX) == PS_TABLE_BT_HEAD_LEN);
  assert (sizeof (NODE_INDEX) == PS_TABLE_BT_ROOT_LEN);

  memset (header + PS_RESEVED_FOR_FUTURE_OFF, 0, PS_RESEVED_FOR_FUTURE_LEN);

  //Write the first header part to reserve the space!
  tableFile.Write (header, PS_HEADER_SIZE);

  //Write the field descriptors;
  tableFile.Write (fieldsDescs.get(), descriptorsSize);

  const uint_t toFill = tableFile.Tell () % TableRmNode::RAW_NODE_SIZE;

  if (toFill != 0)
    {
      static const uint8_t dump [TableRmNode::RAW_NODE_SIZE] = {0,};

      assert ((tableFile.Tell () == tableFile.GetSize()));
      assert ((tableFile.Tell () < sizeof dump));

      tableFile.Write (dump, sizeof dump - toFill);
    }
  assert ((tableFile.Tell() == tableFile.GetSize()));

  store_le_int64 (tableFile.GetSize (), header + PS_TABLE_MAINTABLE_SIZE_OFF);

  tableFile.Seek (0, WHC_SEEK_BEGIN);
  tableFile.Write (header, PS_HEADER_SIZE);
}


PersistentTable::PersistentTable (DbsHandler&       dbs,
                                  const string&     name)
  : PrototypeTable (dbs),
    mDbsSettings (DBSGetSeettings ()),
    mMaxFileSize (0),
    mVSDataSize (0),
    mFileNamePrefix (dbs.WorkingDir () + name),
    mTableData (NULL),
    mRowsData (NULL),
    mVSData (NULL),
    mRemoved (false)
{
  InitFromFile ();

  if (mMaxFileSize != dbs.MaxFileSize ())
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_INCONSITENCY));

  assert (mTableData.get () != NULL);

  uint_t       blkSize  = DBSSettings ().mTableCacheBlkSize;
  const uint_t blkCount = DBSSettings ().mTableCacheBlkCount;

  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < mRowSize)
    blkSize *= 2;

  mRowCache.Init (*this, mRowSize, blkSize, blkCount);

  InitVariableStorages ();
  InitIndexedFields ();
}


PersistentTable::PersistentTable (DbsHandler&                     dbs,
                                  const string&                   name,
                                  const DBSFieldDescriptor* const fields,
                                  const uint_t                    fieldsCount)
  : PrototypeTable (dbs),
    mDbsSettings (DBSGetSeettings ()),
    mMaxFileSize (0),
    mVSDataSize (0),
    mFileNamePrefix (dbs.WorkingDir () + name),
    mTableData (NULL),
    mRowsData (NULL),
    mVSData (NULL),
    mRemoved (false)
{
  create_table_file (dbs.MaxFileSize (),
                     mFileNamePrefix.c_str (),
                     fields,
                     fieldsCount);
  InitFromFile ();

  assert (mTableData.get () != NULL);

  uint_t       blkSize  = DBSSettings ().mTableCacheBlkSize;
  const uint_t blkCount = DBSSettings ().mTableCacheBlkCount;

  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < mRowSize)
    blkSize *= 2;

  mRowCache.Init (*this, mRowSize, blkSize, blkCount);

  InitVariableStorages ();
  InitIndexedFields ();
}


PersistentTable::~PersistentTable ()
{
  Flush ();

  for (FIELD_INDEX fieldIndex = 0; fieldIndex < mFieldsCount; ++fieldIndex)
    if (mvIndexNodeMgrs[fieldIndex] != NULL)
      {
        FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

        uint64_t unitsCount = mMaxFileSize - 1;

        unitsCount += mvIndexNodeMgrs[fieldIndex]->IndexRawSize();
        unitsCount /= mMaxFileSize;

        field.IndexUnitsCount (unitsCount);
        delete mvIndexNodeMgrs [fieldIndex];
      }
  MakeHeaderPersistent ();

  if (mVSData != NULL)
    mVSData->ReleaseReference ();
}


bool
PersistentTable::IsTemporal () const
{
  return false;
}


ITable&
PersistentTable::Spawn () const
{
  return *(new TemporalTable (*this));
}


void
PersistentTable::InitFromFile ()
{
  uint64_t   mainTableSize = 0;
  uint8_t    tableHdr[PS_HEADER_SIZE];

  File mainTableFile (mFileNamePrefix.c_str(),
                      WHC_FILEOPEN_EXISTING | WHC_FILEREAD);

  mainTableFile.Seek (0, WHC_SEEK_BEGIN);
  mainTableFile.Read (tableHdr, PS_HEADER_SIZE);

  if (memcmp (tableHdr, PS_TABLE_SIGNATURE, PS_TABLES_SIG_LEN) != 0)
    throw DBSException(NULL, _EXTRA (DBSException::TABLE_INVALID));

  mFieldsCount     = load_le_int32 (tableHdr + PS_TABLE_FIELDS_COUNT_OFF);
  mDescriptorsSize = load_le_int32 (tableHdr + PS_TABLE_ELEMS_SIZE_OFF);
  mRowsCount       = load_le_int64 (tableHdr + PS_TABLE_RECORDS_COUNT_OFF);
  mVSDataSize      = load_le_int64 (tableHdr + PS_TABLE_VARSTORAGE_SIZE_OFF);
  mRowSize         = load_le_int32 (tableHdr + PS_TABLE_ROW_SIZE_OFF);
  mRootNode        = load_le_int32 (tableHdr + PS_TABLE_BT_ROOT_OFF);
  mUnallocatedHead = load_le_int32 (tableHdr + PS_TABLE_BT_HEAD_OFF);
  mMaxFileSize     = load_le_int64 (tableHdr + PS_TABLE_MAX_FILE_SIZE_OFF);
  mainTableSize    = load_le_int64 (tableHdr + PS_TABLE_MAINTABLE_SIZE_OFF);

  if ((mFieldsCount == 0) ||
      (mDescriptorsSize < (sizeof(FieldDescriptor) * mFieldsCount)) ||
      (mainTableSize < PS_HEADER_SIZE))
    {
      throw DBSException(NULL, _EXTRA (DBSException::TABLE_INVALID));
    }

  //Cache the field descriptors in memory
  mFieldsDescriptors.reset(new uint8_t[mDescriptorsSize]);
  mainTableFile.Read(_CC (uint8_t*, mFieldsDescriptors.get ()),
                     mDescriptorsSize);
  mainTableFile.Close ();

  mTableData.reset (new FileContainer (
                          mFileNamePrefix.c_str(),
                          mMaxFileSize,
                          (mainTableSize + mMaxFileSize - 1) / mMaxFileSize
                                         ));
}


void
PersistentTable::InitVariableStorages ()
{
  // Loading the rows regular should be done upfront.
  mRowsData.reset (
              new FileContainer (
                (mFileNamePrefix + PS_TABLE_FIXFIELDS_EXT).c_str(),
                mMaxFileSize,
                ((mRowSize * mRowsCount) + mMaxFileSize - 1) / mMaxFileSize
                        )
                  );

  //Check if are fields demanding variale size store.
  for (FIELD_INDEX i = 0; i < mFieldsCount; ++i)
    {
        DBSFieldDescriptor fieldDesc = DescribeField (i);

        assert ((fieldDesc.type > T_UNKNOWN)
                && (fieldDesc.type < T_UNDETERMINED));

        if (fieldDesc.isArray || (fieldDesc.type == T_TEXT))
          {
            auto_ptr<VariableSizeStore> hold (new VariableSizeStore);

            hold->Init ((mFileNamePrefix + PS_TABLE_VARFIELDS_EXT).c_str(),
                        mVSDataSize,
                        mMaxFileSize);

            mVSData = hold.release ();
            mVSData->RegisterReference ();

            //We only need on field to require variable storage initialisation
            //and it would be enough for the (if they are present).
            break;
          }
    }
}


void
PersistentTable::InitIndexedFields ()
{
  for (FIELD_INDEX fieldIndex = 0; fieldIndex < mFieldsCount; ++fieldIndex)
    {
      FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

      if (field.IndexNodeSizeKB () == 0)
        {
          assert (field.IndexUnitsCount () == 0);

          mvIndexNodeMgrs.push_back (NULL);
          continue;
        }

      string containerName = mFileNamePrefix;

      containerName += '_';
      containerName += _RC (const char*, mFieldsDescriptors.get ()) +
                              field.NameOffset ();
      containerName += "_bt";

      auto_ptr<IDataContainer> indexContainer (
                             new FileContainer (containerName.c_str (),
                                                mMaxFileSize,
                                                field.IndexUnitsCount () )
                                               );
      mvIndexNodeMgrs.push_back (
            new FieldIndexNodeManager (indexContainer,
                                       field.IndexNodeSizeKB () * 1024,
                                       0x400000, //4MB
                                       _SC (DBS_FIELD_TYPE, field.Type ()),
                                       false)
                                );
    }
}


void
PersistentTable::MakeHeaderPersistent ()
{
  if (mRemoved)
    return ; //We were removed. We were removed.

  uint8_t tableHdr[PS_HEADER_SIZE];

  memcpy (tableHdr, PS_TABLE_SIGNATURE, sizeof PS_TABLE_SIGNATURE);

  store_le_int32 (mFieldsCount,        tableHdr + PS_TABLE_FIELDS_COUNT_OFF);
  store_le_int32 (mDescriptorsSize,    tableHdr + PS_TABLE_ELEMS_SIZE_OFF);
  store_le_int64 (mRowsCount,          tableHdr + PS_TABLE_RECORDS_COUNT_OFF);
  store_le_int32 (mRowSize,            tableHdr + PS_TABLE_ROW_SIZE_OFF);
  store_le_int32 (mRootNode,           tableHdr + PS_TABLE_BT_ROOT_OFF);
  store_le_int32 (mUnallocatedHead,    tableHdr + PS_TABLE_BT_HEAD_OFF);
  store_le_int64 (mMaxFileSize,        tableHdr + PS_TABLE_MAX_FILE_SIZE_OFF);
  store_le_int64 (mTableData->Size (), tableHdr + PS_TABLE_MAINTABLE_SIZE_OFF);

  store_le_int64 ((mVSData != NULL) ? mVSData->Size () : 0,
                  tableHdr + PS_TABLE_VARSTORAGE_SIZE_OFF);

  memset(tableHdr + PS_RESEVED_FOR_FUTURE_OFF, 0, PS_RESEVED_FOR_FUTURE_LEN);

  mTableData->Write (0, sizeof tableHdr, tableHdr);
  mTableData->Write (sizeof tableHdr,
                     mDescriptorsSize,
                     mFieldsDescriptors.get ());
}


void
PersistentTable::RemoveFromDatabase ()
{
  if (mRowsData.get() != NULL)
    mRowsData->MarkForRemoval();

  if (mVSData != NULL)
    mVSData->MarkForRemoval();

  for (FIELD_INDEX i = 0; i < mFieldsCount; ++i )
    {
      if (mvIndexNodeMgrs[i]  != NULL)
        mvIndexNodeMgrs[i]->MarkForRemoval ();
    }
  mTableData->MarkForRemoval ();
  mRemoved = true;
}


IDataContainer*
PersistentTable::CreateIndexContainer (const FIELD_INDEX field)
{
  assert (mFileNamePrefix.size () > 0);

  DBSFieldDescriptor desc = DescribeField (field);

  string containerNameBase = mFileNamePrefix;

  containerNameBase += '_';
  containerNameBase += desc.name;
  containerNameBase += "_bt";

  return new FileContainer (containerNameBase.c_str (),
                            mDbsSettings.mMaxFileSize,
                            0);
}


void
PersistentTable::Flush ()
{
  if (mVSData != NULL)
    mVSData->Flush ();

  PrototypeTable::Flush ();
}


IDataContainer&
PersistentTable::RowsContainer ()
{
  assert (mRowsData.get () != NULL);
  return *mRowsData.get ();
}


IDataContainer&
PersistentTable::TableContainer ()
{
  assert (mTableData.get () != NULL);
  return *mTableData.get ();
}


VariableSizeStore&
PersistentTable::VSStore ()
{
  assert (mVSData != NULL);
  return *mVSData;
}




TemporalTable::TemporalTable (DbsHandler&                     dbs,
                              const DBSFieldDescriptor* const fields,
                              const FIELD_INDEX               fieldsCount)
  : PrototypeTable (dbs),
    mTableData (NULL),
    mRowsData (NULL),
    mVSData (NULL)
{

  //Check the arguments
  if ((fields == NULL)
      || (fieldsCount == 0)
      || (fieldsCount > 0xFFFFu))
    {
      throw(DBSException(NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED)));
    }

  //Compute the table header descriptor size
  const uint32_t descriptorsSize = sizeof(FieldDescriptor) * fieldsCount +
                                     get_fields_names_len(fields, fieldsCount);

  validate_field_descriptors(fields, fieldsCount);

  vector<DBSFieldDescriptor> vect (fields + 0, fields + fieldsCount);
  auto_ptr<uint8_t> fieldDescs(new uint8_t[descriptorsSize]);

  uint_t rowSize;

  normalize_fields (vect, &rowSize, fieldDescs.get());

  mFieldsCount     = fieldsCount;
  mDescriptorsSize = descriptorsSize;
  mRowSize         = rowSize;
  mFieldsDescriptors.reset (fieldDescs.release ());

  mvIndexNodeMgrs.insert (mvIndexNodeMgrs.begin (), mFieldsCount, NULL);

  uint_t       blkSize  = DBSSettings ().mTableCacheBlkSize;
  const uint_t blkCount = DBSSettings ().mTableCacheBlkCount;

  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < mRowSize)
    blkSize *= 2;

  mRowCache.Init (*this, mRowSize, blkSize, blkCount);
}


TemporalTable::TemporalTable (const PrototypeTable& prototype)
  : PrototypeTable (prototype),
    mTableData (NULL),
    mRowsData (NULL),
    mVSData (NULL)
{

  mvIndexNodeMgrs.insert (mvIndexNodeMgrs.begin (), mFieldsCount, NULL);

  uint_t       blkSize  = DBSSettings ().mTableCacheBlkSize;
  const uint_t blkCount = DBSSettings ().mTableCacheBlkCount;

  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < mRowSize)
    blkSize *= 2;

  mRowCache.Init (*this, mRowSize, blkSize, blkCount);
}


TemporalTable::~TemporalTable ()
{
  Flush ();

  for (FIELD_INDEX fieldIndex = 0; fieldIndex < mFieldsCount; ++fieldIndex)
    delete mvIndexNodeMgrs [fieldIndex];

  if (mVSData != NULL)
    mVSData->ReleaseReference ();
}


bool
TemporalTable::IsTemporal () const
{
  return true;
}


ITable&
TemporalTable::Spawn () const
{
  return *(new TemporalTable (*this));
}


void
TemporalTable::Flush ()
{
  if (mVSData != NULL)
    mVSData->Flush ();

  PrototypeTable::Flush ();
}


void
TemporalTable::MakeHeaderPersistent ()
{
  //Do nothing!
}


IDataContainer*
TemporalTable::CreateIndexContainer (const FIELD_INDEX)
{
  return new TemporalContainer ();
}


IDataContainer&
TemporalTable::TableContainer ()
{

  if (mTableData.get () == NULL)
    mTableData.reset (new TemporalContainer ());

  return *mTableData.get ();
}


IDataContainer&
TemporalTable::RowsContainer ()
{
  if (mRowsData.get () == NULL)
    mRowsData.reset (new TemporalContainer ());

  return *mRowsData.get ();
}


VariableSizeStore&
TemporalTable::VSStore ()
{
  if (mVSData == NULL)
    {
      auto_ptr<VariableSizeStore> hold (new VariableSizeStore ());

      hold->Init (mDbs.WorkingDir ().c_str (), 4096);

      mVSData = hold.release ();
      mVSData->RegisterReference ();
    }

  return *mVSData;
}

} //namespace pastra
} //namespace whisper

