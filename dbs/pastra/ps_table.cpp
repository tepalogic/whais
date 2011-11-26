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

#include "utils/include/wfile.h"

#include "dbs_mgr.h"
#include "dbs_exception.h"

#include "ps_valintep.h"
#include "ps_table.h"
#include "ps_textstrategy.h"
#include "ps_arraystrategy.h"

using namespace pastra;
using namespace std;

static const D_CHAR PS_TABLE_FIXFIELDS_EXT[] = "_f";
static const D_CHAR PS_TABLE_VARFIELDS_EXT[] = "_v";

static const D_UINT8 PS_TABLE_SIGNATURE[] =
  { 0x50, 0x41, 0x53, 0x54, 0x52, 0x41, 0x54, 0x42 };

static const D_UINT PS_TABLE_SIG_OFF = 0; //Signature
static const D_UINT PS_TABLES_SIG_lEN = 8;
static const D_UINT PS_TABLE_FIELDS_COUNT_OFF = 8; //Number of fields.
static const D_UINT PS_TABLE_FIELDS_COUNT_LEN = 4;
static const D_UINT PS_TABLE_ELEMS_SIZE_OFF = 12; //Size of the fields description area
static const D_UINT PS_TABLE_ELEMS_SIZE_LEN = 4;
static const D_UINT PS_TABLE_RECORDS_COUNT_OFF = 16; //Number of allocated records.
static const D_UINT PS_TABLE_RECORDS_COUNT_LEN = 8;
static const D_UINT PS_TABLE_VARSTORAGE_SIZE_OFF = 24; //Size of variable storage file
static const D_UINT PS_TABLE_VARSTORAGE_SIZE_LEN = 8;
static const D_UINT PS_TABLE_ROW_SIZE_OFF = 32; //Size of a row.
static const D_UINT PS_TABLE_ROW_SIZE_LEN = 4;

static const D_UINT PS_RESEVED_FOR_FUTURE_OFF = 36;
static const D_UINT PS_RESEVED_FOR_FUTURE_LEN = 52;

static const D_UINT PS_HEADER_SIZE = 88;

static const D_UINT MAX_FIELD_VALUE_ALIGN = 8; /* Bytes. */

struct PaddInterval
{
  PaddInterval(D_UINT32 begin_byte, D_UINT32 bytes_count) :
    mBegin(begin_byte * 8),
    mEnd(((begin_byte + bytes_count) * 8) - 1)
  {
    assert (mBegin <= mEnd);
  }

  D_UINT32 mBegin;
  D_UINT32 mEnd;

};

static D_UINT32
get_strlens_till_index(const DBSFieldDescriptor * pFields, D_UINT index)
{
  D_UINT32 result = 0;

  while (index-- > 0)
    {
      result += strlen(pFields->mpFieldName) + 1;
      ++pFields;
    }

  return result;
}

static void
validate_field_name(const D_CHAR * pFieldName)
{

  while (pFieldName[0])
    {
      if (!(((pFieldName[0] >= 'a') && (pFieldName[0] <= 'z'))
	    || ((pFieldName[0] >= 'A') && (pFieldName[0] <= 'Z'))
	    || ((pFieldName[0] >= '0') && (pFieldName[0] <= '9'))
	    || (pFieldName[0] >= '_')))
	throw DBSException(NULL, _EXTRA (DBSException::FIELD_NAME_INVALID));

      ++pFieldName;
    }
}

static void
validate_field_descriptors(const DBSFieldDescriptor * const pFields,
			   const D_UINT fieldsCount)
{
  assert ((fieldsCount > 0) && (pFields != NULL));
  for (D_UINT firstIt = 0; firstIt < fieldsCount; ++firstIt)
    {
      validate_field_name(pFields[firstIt].mpFieldName);

      for (D_UINT secondIt = firstIt; secondIt < fieldsCount; ++secondIt)
	if (firstIt == secondIt)
	  continue;
	else if (strcmp(pFields[firstIt].mpFieldName, pFields[secondIt].mpFieldName) == 0)
	  throw DBSException (
                              pFields[firstIt].mpFieldName,
                              _EXTRA (DBSException::FIELD_NAME_DUPLICATED));

      if ((pFields[firstIt].mFieldType == T_UNKNOWN) ||
          (pFields[firstIt].mFieldType >= T_END_OF_TYPES))
	throw DBSException(NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));

      else if (pFields[firstIt].isArray && (pFields[firstIt].mFieldType == T_TEXT))
        throw DBSException(NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));
    }
}

static D_INT
get_next_alignment(D_INT size)
{
  D_INT result = 1;

  size &= 0x7, size |= 0x8;

  while ((size & 1) == 0)
    result <<= 1, size >>= 1;

  return result;
}

static void
arrange_field_entries(vector<DBSFieldDescriptor> &rvFields,
		      D_UINT8 * const pOutFieldsDescription, D_UINT32 &uOutRowSize)
{

  vector<PaddInterval> padds;

  PSFieldDescriptor * const pFieldDesc = _RC (PSFieldDescriptor*, pOutFieldsDescription);
  const D_INT nullBitsRequested = rvFields.size (); //One for each field.
  D_INT paddingBytesCount = 0;
  D_INT currentAlignment = 8;   //Force the best choice

  //Find the best fields position in order to minimize the numbers of padding
  //bytes that are need it for values alignments
  for (D_UINT fieldIndex = 0; fieldIndex < rvFields.size(); ++fieldIndex)
    {
      D_INT foundIndex = -1;            //-1 Nothing found!
      D_INT foundReqAlign = 1;          //Worst requested align
      D_INT foundResultedAlign = 1;     //Worst resulted align
      D_INT foundReqPaddsCount = MAX_FIELD_VALUE_ALIGN - 1; //Worst number of bytes required to padd
      D_INT foundSize = 0;

      for (D_UINT schIndex = fieldIndex; schIndex < rvFields.size(); ++schIndex)
	{
	  D_INT currIndexSize = PSValInterp::GetSize(
                                                      rvFields[schIndex].mFieldType,
                                                      rvFields[schIndex].isArray);

	  D_INT currReqAlign = PSValInterp::GetAlignment(
                                                         rvFields[schIndex].mFieldType,
                                                         rvFields[schIndex].isArray);

	  D_INT currReqPaddsCount = (currReqAlign > currentAlignment) ?
                                      (currReqAlign - currentAlignment) : 0;
	  D_INT currResultedAlign = get_next_alignment(currReqPaddsCount + currIndexSize + uOutRowSize);

	  //Lets check if is btter than what we have found until now
	  if (foundReqPaddsCount > currReqPaddsCount)
	    {
	      //New best choice!
	      foundIndex = schIndex;
	      foundReqAlign = currReqAlign;
	      foundResultedAlign = currResultedAlign;
	      foundReqPaddsCount = currReqPaddsCount;
	      foundSize = currIndexSize;
	    }
	  else if (foundReqPaddsCount == currReqPaddsCount)
	    {
	      if ((foundReqAlign < currReqAlign)
		  || ((foundReqAlign == currReqAlign) && (foundResultedAlign
							< currResultedAlign)))
		{
		  //New best choice!
		  foundIndex = schIndex;
		  foundReqAlign = currReqAlign;
		  foundResultedAlign = currResultedAlign;
		  foundReqPaddsCount = currReqPaddsCount;
		  foundSize = currIndexSize;
		}
	      else
		continue; //Get the next one!
	    }
	  else
	    continue; //Get the next one!
	}

      //We made our choice! Let's note it.
      assert ((foundIndex >= 0) && (foundSize > 0));

      DBSFieldDescriptor temp = rvFields[foundIndex];
      rvFields[foundIndex] = rvFields[fieldIndex];
      rvFields[fieldIndex] = temp;

      pFieldDesc[fieldIndex].mNameOffset = get_strlens_till_index (&rvFields.front(), fieldIndex);
      pFieldDesc[fieldIndex].mNameOffset += sizeof(PSFieldDescriptor) * rvFields.size();

      strcpy(
	     _RC (D_CHAR *, pOutFieldsDescription + pFieldDesc[fieldIndex].mNameOffset),
	     _RC (const D_CHAR *, temp.mpFieldName));

      pFieldDesc[fieldIndex].mStoreIndex = uOutRowSize;
      pFieldDesc[fieldIndex].mTypeDesc = temp.mFieldType;

      if (temp.isArray)
	pFieldDesc[fieldIndex].mTypeDesc |= PS_TABLE_ARRAY_MASK;

      currentAlignment = foundResultedAlign;

      if (foundReqPaddsCount > 0)
	padds.push_back(PaddInterval(uOutRowSize, foundReqPaddsCount));

      uOutRowSize += foundSize + foundReqPaddsCount;
      paddingBytesCount += foundReqPaddsCount;
    }

  //Increase the row size if we need more bytes to keep the null bits.
  const D_INT32 extraPaddBytesNeeded = ((nullBitsRequested + 7) / 8) - paddingBytesCount;
  if (extraPaddBytesNeeded > 0)
    {
      padds.push_back(PaddInterval(uOutRowSize, extraPaddBytesNeeded));
      uOutRowSize += extraPaddBytesNeeded;
    }

  //Round the row size so the first element of the next row is alligned
  D_INT32 needExtraAlign = PSValInterp::GetAlignment( rvFields[0].mFieldType, rvFields[0].isArray);
  needExtraAlign -= get_next_alignment(uOutRowSize);
  if (needExtraAlign > 0)
    uOutRowSize += needExtraAlign;

  //Reuse the padding bytes to hold the fileds value null bits indicators.
  for (D_UINT fieldIndex = 0; fieldIndex < rvFields.size(); ++fieldIndex)
    {
      pFieldDesc[fieldIndex].mNullBitIndex = padds[0].mBegin++;

      if (padds[0].mBegin > padds[0].mEnd)
	padds.erase(padds.begin());
    }
}

static WFile
create_table_file(const string &baseFileName, const DBSFieldDescriptor *pFields, D_UINT fieldsCount)
{

  //Check the arguments
  if ((pFields == NULL) || (fieldsCount == 0) || (fieldsCount > 0xFFFFu))
    throw(DBSException(NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED)));

  //Compute the table header descriptor size
  const D_UINT32 descriptorsSize = sizeof(PSFieldDescriptor) * fieldsCount +
				   get_strlens_till_index(pFields, fieldsCount);

  //Validate the optimally rearange the fileds for minimum rowsize
  D_UINT rowSize = 0;
  validate_field_descriptors(pFields, fieldsCount);

  vector<DBSFieldDescriptor> vect(pFields + 0, pFields + fieldsCount);
  auto_ptr<D_UINT8> apFieldDescription(new D_UINT8[descriptorsSize]);

  arrange_field_entries(vect, apFieldDescription.get(), rowSize);
  pFields = &vect.front();

  WFile tableFile(baseFileName.c_str(), WHC_FILECREATE_NEW | WHC_FILERDWR | WHC_FILESYNC);

  auto_ptr<D_UINT8> apBuffer(new D_UINT8[PS_HEADER_SIZE]);
  D_UINT8 * const pBuffer = apBuffer.get();

  memcpy (pBuffer, PS_TABLE_SIGNATURE, sizeof PS_TABLE_SIGNATURE);

  _RC (D_UINT32 *, pBuffer + PS_TABLE_FIELDS_COUNT_OFF)[0] = fieldsCount;
  _RC (D_UINT32 *, pBuffer + PS_TABLE_ELEMS_SIZE_OFF)[0] = descriptorsSize;
  _RC (D_UINT64 *, pBuffer + PS_TABLE_RECORDS_COUNT_OFF)[0] = 0;
  _RC (D_UINT64 *, pBuffer + PS_TABLE_VARSTORAGE_SIZE_OFF)[0] = 0;
  _RC (D_UINT32 *, pBuffer + PS_TABLE_ROW_SIZE_OFF)[0] = rowSize;
  memset(pBuffer + PS_RESEVED_FOR_FUTURE_OFF, 0, PS_RESEVED_FOR_FUTURE_LEN);

  //Write the first header part!
  tableFile.Write(pBuffer, PS_HEADER_SIZE);

  //Write the field descriptors;
  tableFile.Write(apFieldDescription.get(), descriptorsSize);

  return tableFile;
}


///////////////////////////////////////////////////////////////////////////////////////////////////



PSTable::PSTable (DbsHandler & dbsHandler, const string & tableName) :
  m_RowsCount (0),
  m_ReferenceCount (0),
  m_DescriptorsSize (~0),
  m_FieldsCount (0),
  m_BaseFileName (DBSGetWorkingDir() + tableName),
  m_FieldsDescriptors(NULL),
  m_MainTableFile (m_BaseFileName.c_str(), WHC_FILEOPEN_EXISTING | WHC_FILERDWR | WHC_FILESYNC),
  m_apFixedFields (NULL),
  m_apVariableFields (NULL),
  m_RowCache (*this),
  m_Removed (false)


{
  InitFromFile ();

  m_RowCache.Init (m_RowSize, 4096, 1024);

  InitVariableStorages ();
}


PSTable::PSTable(DbsHandler & dbsHandler,
		 const string & tableName,
		 const DBSFieldDescriptor * pFields,
		 D_UINT fieldsCount) :
  m_RowsCount (0),
  m_ReferenceCount (0),
  m_DescriptorsSize (~0),
  m_FieldsCount (0),
  m_BaseFileName (DBSGetWorkingDir() + tableName),
  m_FieldsDescriptors(NULL),
  m_MainTableFile (create_table_file (m_BaseFileName, pFields, fieldsCount)),
  m_apFixedFields (NULL),
  m_apVariableFields (NULL),
  m_RowCache (*this),
  m_Removed (false)
{
  InitFromFile ();

  m_RowCache.Init (m_RowSize, 4096, 1024);

  InitVariableStorages ();
}

PSTable::~PSTable()
{
  if (m_ReferenceCount != 0)
    throw DBSException(NULL, _EXTRA (DBSException::TABLE_IN_USE));

  SyncToFile ();
}

D_UINT
PSTable::GetFieldsCount ()
{
  return m_FieldsCount;
}

DBSFieldDescriptor
PSTable::GetFieldDescriptor (D_UINT fieldIndex)
{
  const PSFieldDescriptor * const pDesc = _RC(const PSFieldDescriptor*, m_FieldsDescriptors.get ());

  if (fieldIndex >= m_FieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  DBSFieldDescriptor result;

  result.isArray = (pDesc[fieldIndex].mTypeDesc & PS_TABLE_ARRAY_MASK) != 0;
  result.mFieldType = _SC (DBS_FIELD_TYPE, pDesc[fieldIndex].mTypeDesc & PS_TABLE_FIELD_TYPE_MASK);
  result.mpFieldName = _RC (const D_CHAR *, pDesc) + pDesc[fieldIndex].mNameOffset;

  return result;
}

DBSFieldDescriptor
PSTable::GetFieldDescriptor(const D_CHAR * const pFieldName)
{

  const PSFieldDescriptor * const pDesc = _RC(const PSFieldDescriptor*,
					      m_FieldsDescriptors.get ());
  D_UINT iterator = m_FieldsCount * sizeof(PSFieldDescriptor);

  for (D_UINT index = 0; index < m_FieldsCount; ++index)
    {
      if (strcmp(_RC (const D_CHAR *, m_FieldsDescriptors.get() + iterator), pFieldName) == 0)
	{
	  DBSFieldDescriptor result;

	  result.mpFieldName = _RC (const D_CHAR*, pDesc) + iterator;
	  result.isArray = (pDesc[index].mTypeDesc & PS_TABLE_ARRAY_MASK) != 0;
	  result.mFieldType = _SC (DBS_FIELD_TYPE, pDesc[index].mTypeDesc & PS_TABLE_FIELD_TYPE_MASK);

	  return result;
	}
      iterator += strlen(_RC(const D_CHAR*, pDesc) + iterator) + 1;
    }

  throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));
}

D_UINT64
PSTable::GetAllocatedRows()
{
  return m_RowsCount;
}

void
PSTable::MarkRowForReuse(D_UINT64 rowIndex)
{
  D_UINT fieldsCount = m_FieldsCount;

  StoredItem cachedItem (m_RowCache.RetriveItem (rowIndex));
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

    while (fieldsCount-- > 0)
      {
        PSFieldDescriptor field = GetFieldDescriptorInternal (fieldsCount);
        D_UINT byte_off = field.mNullBitIndex / 8;
        D_UINT8 bit_off = field.mNullBitIndex % 8;

        pRawData[byte_off] |= (1 << bit_off);
      }

    //TODO: You may want to update the some other issues
}

D_UINT64
PSTable::AddRow()
{
  D_UINT64 lastRowPosition = m_RowsCount * m_RowSize;
  D_UINT toWrite = m_RowSize;
  D_UINT8 dummyValue[128];

  memset(dummyValue, 0xFF, sizeof(dummyValue));

  while (toWrite)
    {
      D_UINT writeChunk = MIN (toWrite, sizeof (dummyValue));

      m_apFixedFields.get ()->StoreData (lastRowPosition, writeChunk, dummyValue);

      toWrite -= writeChunk;
      lastRowPosition += writeChunk;
    }

  IncreaseRowCount ();
  m_RowCache.ForceItemUpdate ( m_RowsCount - 1);

  return m_RowsCount;
}

D_UINT64
PSTable::AddReusedRow ()
{
  //TODO: Not supported yet;
  return 0;
}


D_UINT
PSTable::GetRowSize() const
{
  return m_RowSize;
}

PSFieldDescriptor
PSTable::GetFieldDescriptorInternal(D_UINT fieldIndex) const
{
  const PSFieldDescriptor * const pDesc = _RC(const PSFieldDescriptor*, m_FieldsDescriptors.get ());

  if (fieldIndex >= m_FieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  return pDesc[fieldIndex];
}

PSFieldDescriptor
PSTable::GetFieldDescriptorInternal(const D_CHAR * const pFieldName) const
{

  const PSFieldDescriptor * const pDesc = _RC(const PSFieldDescriptor*,
					      m_FieldsDescriptors.get ());
  D_UINT iterator = m_FieldsCount * sizeof(PSFieldDescriptor);

  for (D_UINT index = 0; index < m_FieldsCount; ++index)
    {
      if (strcmp ( _RC(const D_CHAR *, m_FieldsDescriptors.get() + iterator), pFieldName) == 0)
	return pDesc[index];

      iterator += strlen( _RC(const D_CHAR*, pDesc) + iterator) + 1;
    }

  throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));
}

void
PSTable::RetrieveItems (D_UINT8 *pDestBuffer,
                        D_UINT64 firstItem,
                        D_UINT itemsCount)
{
  if (itemsCount + firstItem > m_RowsCount)
    itemsCount = m_RowsCount - firstItem;

  m_apFixedFields.get()->RetrieveData( firstItem * m_RowSize, itemsCount * m_RowSize, pDestBuffer);
}

void
PSTable::StoreItems (const D_UINT8 *pSrcBuffer,
                     D_UINT64 firstItem,
                     D_UINT itemsCount)
{
  m_apFixedFields.get()->StoreData( firstItem * m_RowSize, itemsCount * m_RowSize, pSrcBuffer);
}

void
PSTable::InitVariableStorages ()
{
  const D_UINT64 maxFileSize = DBSGetMaxFileSize() - (DBSGetMaxFileSize() % m_RowSize);

  m_apFixedFields.reset (
      new FileContainer((m_BaseFileName + PS_TABLE_FIXFIELDS_EXT).c_str(),
                         maxFileSize,
                         ((m_RowSize * m_RowsCount) + maxFileSize - 1) / maxFileSize));

  for (D_UINT fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex)
    {
        DBSFieldDescriptor fieldDesc = GetFieldDescriptor (fieldIndex);

        assert ((fieldDesc.mFieldType > T_UNKNOWN) && (fieldDesc.mFieldType < T_UNDETERMINED));

        if (fieldDesc.isArray || (fieldDesc.mFieldType == T_TEXT))
          {
            m_apVariableFields.reset (new VaribaleLenghtStore ());
            m_apVariableFields->Init ((m_BaseFileName + PS_TABLE_VARFIELDS_EXT).c_str(),
                                      m_VariableStorageSize,
                                      maxFileSize);
            break; //We finshed here!
          }
    }
}

void
PSTable::SyncToFile ()
{

  if (m_Removed)
    return ; //Do nothing! We are removed anyway

  D_UINT8 aTableHdr[PS_HEADER_SIZE];

  memcpy (aTableHdr, PS_TABLE_SIGNATURE, sizeof PS_TABLE_SIGNATURE);

  _RC (D_UINT32 *, aTableHdr + PS_TABLE_FIELDS_COUNT_OFF)[0] = m_FieldsCount;
  _RC (D_UINT32 *, aTableHdr + PS_TABLE_ELEMS_SIZE_OFF)[0] = m_DescriptorsSize;
  _RC (D_UINT64 *, aTableHdr + PS_TABLE_RECORDS_COUNT_OFF)[0] = m_RowsCount;
  _RC (D_UINT64 *, aTableHdr + PS_TABLE_VARSTORAGE_SIZE_OFF)[0] =
      (m_apVariableFields.get() != NULL) ? m_apVariableFields->GetRawSize() : 0;
  _RC (D_UINT32 *, aTableHdr + PS_TABLE_ROW_SIZE_OFF)[0] = m_RowSize;
  memset(aTableHdr + PS_RESEVED_FOR_FUTURE_OFF, 0, PS_RESEVED_FOR_FUTURE_LEN);

  m_MainTableFile.Seek (0, WHC_SEEK_BEGIN);
  m_MainTableFile.Write (aTableHdr, sizeof aTableHdr);

}

void
PSTable::InitFromFile()
{
  D_UINT8 aTableHdr[PS_HEADER_SIZE];

  m_MainTableFile.Seek(0, WHC_SEEK_BEGIN);
  m_MainTableFile.Read(aTableHdr, PS_HEADER_SIZE);

  if (memcmp(aTableHdr, PS_TABLE_SIGNATURE, PS_TABLES_SIG_lEN) != 0)
    throw DBSException(NULL, _EXTRA (DBSException::TABLE_INVALID));

  //Retreive the header information.
  m_FieldsCount = _RC (D_UINT32 *, aTableHdr + PS_TABLE_FIELDS_COUNT_OFF)[0];
  m_DescriptorsSize = _RC (D_UINT32 *, aTableHdr + PS_TABLE_ELEMS_SIZE_OFF)[0];
  m_RowsCount = _RC (D_UINT64 *, aTableHdr + PS_TABLE_RECORDS_COUNT_OFF)[0];
  m_VariableStorageSize = _RC (D_UINT64 *, aTableHdr + PS_TABLE_VARSTORAGE_SIZE_OFF)[0];
  m_RowSize = _RC (D_UINT32 *, aTableHdr + PS_TABLE_ROW_SIZE_OFF)[0];

  if ((m_FieldsCount == 0) || (m_DescriptorsSize < (sizeof(PSFieldDescriptor) * m_FieldsCount)))
    throw DBSException(NULL, _EXTRA (DBSException::TABLE_INVALID));

  //Cache the field descriptors in memory
  m_FieldsDescriptors.reset(new D_UINT8[m_DescriptorsSize]);
  m_MainTableFile.Read(_CC(D_UINT8 *, m_FieldsDescriptors.get ()), m_DescriptorsSize);
}


D_UINT64
PSTable::IncreaseRowCount()
{
  return ++m_RowsCount;
}

void
PSTable::RemoveFromDatabase()
{
  if (m_apFixedFields.get() != NULL)
    m_apFixedFields->MarkForRemoval();

  if (m_apVariableFields.get() != NULL)
    m_apVariableFields->MarkForRemoval();

  m_MainTableFile.Close();
  whc_fremove(m_BaseFileName.c_str());

  m_Removed = true;
}

void
PSTable::CheckRowToDelete (const D_UINT64 rowIndex)
{
  //TODO: Need to implement
}

void
PSTable::CheckRowToReuse (const D_UINT64 rowIndex)
{
  //TODO: Need to implement
}

void
PSTable::SetEntry (const DBSChar &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSBool &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSDate &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSDateTime &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSHiresDate &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSInt8 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSInt16 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSInt32 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSInt64 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSReal &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSRichReal &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSUInt8 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSUInt16 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSUInt32 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSUInt64 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PSTable::SetEntry (const DBSText &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  bool  fieldValueWasNull = false;
  bool  checkForDeletion = false;
  bool  checkForReusage = false;

  const PSFieldDescriptor field = GetFieldDescriptorInternal (fieldIndex);

  StoredItem cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

  D_UINT64 *const fieldFirstEntry = _RC (D_UINT64*, pRawData + field.mStoreIndex + 0);
  D_UINT64 *const fieldValueSize  = _RC (D_UINT64*,
                                         pRawData + field.mStoreIndex + sizeof (D_UINT64));
  D_UINT64 newFirstEntry = 0;
  D_UINT64 newFieldValueSize = 0;

  assert (field.mNullBitIndex > 0);

  const D_UINT byte_off = field.mNullBitIndex / 8;
  const D_UINT8 bit_off = field.mNullBitIndex % 8;

  if ((pRawData [byte_off] & (1 << bit_off)) != 0)
    fieldValueWasNull = true;

  if ( (fieldValueWasNull == false) && rSource.IsNull())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == ~0)
        checkForDeletion = true;
    }
  else if (rSource.IsNull() == false)
    {
      if (pRawData [byte_off] == ~0)
        {
          assert (fieldValueWasNull == true);
          checkForReusage = true;
        }
      pRawData [byte_off] &= ~(1 << bit_off);

      I_TextStrategy &text = rSource;
      if (text.IsRowValue())
        {
          RowFieldText& value = text.GetRowValue();
          newFieldValueSize = value.m_BytesSize;
          newFirstEntry = m_apVariableFields->AddRecord (rowIndex,
                                                         value.m_Storage,
                                                         value.m_FirstEntry,
                                                         0,
                                                         value.m_BytesSize);
        }
      else
        {
          TemporalText& value = text.GetTemporal();
          newFieldValueSize = value.m_BytesSize;
          newFirstEntry = m_apVariableFields->AddRecord (rowIndex,
                                                         value.m_Storage,
                                                         0,
                                                         newFieldValueSize);
        }
    }

  if (fieldValueWasNull == false)
    m_apVariableFields->RemoveRecord (*fieldFirstEntry);

  *fieldFirstEntry = newFirstEntry;
  *fieldValueSize = newFieldValueSize;

  if (checkForReusage)
    CheckRowToReuse (rowIndex);
  else if (checkForDeletion)
    CheckRowToDelete (rowIndex);
}

void
PSTable::SetEntry (const DBSArray &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  bool  fieldValueWasNull = false;
  bool  checkForDeletion = false;
  bool  checkForReusage = false;

  const PSFieldDescriptor field = GetFieldDescriptorInternal (fieldIndex);

  StoredItem cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

  D_UINT64 *const fieldFirstEntry = _RC (D_UINT64*, pRawData + field.mStoreIndex + 0);
  D_UINT64 *const fieldValueSize  = _RC (D_UINT64*,
                                         pRawData + field.mStoreIndex + sizeof (D_UINT64));
  D_UINT64 newFirstEntry = 0;
  D_UINT64 newFieldValueSize = 0;

  assert (field.mNullBitIndex > 0);

  const D_UINT byte_off = field.mNullBitIndex / 8;
  const D_UINT8 bit_off = field.mNullBitIndex % 8;

  if ((pRawData [byte_off] & (1 << bit_off)) != 0)
    fieldValueWasNull = true;

  if ( (fieldValueWasNull == false) && rSource.IsNull())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == ~0)
        checkForDeletion = true;
    }
  else if (rSource.IsNull() == false)
    {
      if (pRawData [byte_off] == ~0)
        {
          assert (fieldValueWasNull == true);
          checkForReusage = true;
        }
      pRawData [byte_off] &= ~(1 << bit_off);

      I_ArrayStrategy &array = rSource;
      if (array.IsRowValue())
        {
          RowFieldArray& value = array.GetRowValue();
          newFieldValueSize = value.GetRawDataSize();
          newFirstEntry = m_apVariableFields->AddRecord (rowIndex,
                                                         value.m_Storage,
                                                         value.m_FirstRecordEntry,
                                                         0,
                                                         newFieldValueSize);
        }
      else
        {
          TemporalArray& value = array.GetTemporal ();
          newFieldValueSize = value.GetRawDataSize();

          const D_UINT64 elemsCount = value.GetElementsCount();
          newFirstEntry = m_apVariableFields->AddRecord (rowIndex,
                                                         _RC(const D_UINT8*, &elemsCount),
                                                         sizeof elemsCount);
          m_apVariableFields->UpdateRecord (newFirstEntry,
                                            sizeof elemsCount,
                                            value.m_Storage,
                                            0,
                                            newFieldValueSize);
          newFieldValueSize += sizeof elemsCount;
        }
    }

  if (fieldValueWasNull == false)
    m_apVariableFields->RemoveRecord (*fieldFirstEntry);

  *fieldFirstEntry = newFirstEntry;
  *fieldValueSize = newFieldValueSize;

  if (checkForReusage)
    CheckRowToReuse (rowIndex);
  else if (checkForDeletion)
    CheckRowToDelete (rowIndex);
}

void
PSTable::GetEntry (DBSChar &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSBool &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSDate &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSDateTime &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSHiresDate &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSInt8 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSInt16 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSInt32 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSInt64 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSReal &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSRichReal &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSUInt8 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSUInt16 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSUInt32 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PSTable::GetEntry (DBSUInt64 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}



//From this point this functions shall not allocated memory
//Other way will not be traceable.
#ifdef new
#undef new
#endif

void
PSTable::GetEntry (DBSText &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const PSFieldDescriptor field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.mTypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.mTypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, T_TEXT)))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  StoredItem cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8 *const pRawData = cachedItem.GetDataForRead();

  const D_UINT64& fieldFirstEntry = *_RC (const D_UINT64*, pRawData + field.mStoreIndex + 0);
  const D_UINT64& fieldValueSize  = *_RC (const D_UINT64*,
                                          pRawData + field.mStoreIndex + sizeof (D_UINT64));

  const D_UINT byte_off = field.mNullBitIndex / 8;
  const D_UINT8 bit_off = field.mNullBitIndex % 8;

  if (pRawData[byte_off] & (1 << bit_off))
    {
      rDestination.~DBSText ();
      new (&rDestination) DBSText(NULL);
    }
  else
    {
      new (&rDestination) DBSText(* (new RowFieldText (*m_apVariableFields.get(),
                                                       fieldFirstEntry,
                                                       fieldValueSize)));
    }
}

void
PSTable::GetEntry (DBSArray &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{

  const PSFieldDescriptor field = GetFieldDescriptorInternal (fieldIndex);

  if ( ((field.mTypeDesc & PS_TABLE_ARRAY_MASK) == 0) ||
      ((field.mTypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, rDestination.GetElementsType())))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  StoredItem cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8 *const pRawData = cachedItem.GetDataForRead();

  const D_UINT64& fieldFirstEntry = *_RC (const D_UINT64*,
                                          pRawData + field.mStoreIndex + 0);

  const D_UINT byte_off = field.mNullBitIndex / 8;
  const D_UINT8 bit_off = field.mNullBitIndex % 8;

  if (pRawData[byte_off] & (1 << bit_off))
    {
      rDestination.~DBSArray ();

      switch (field.mTypeDesc & PS_TABLE_FIELD_TYPE_MASK)
      {
      case T_BOOL:
        new (&rDestination) DBSArray(_SC(DBSBool *, NULL));
        break;
      case T_CHAR:
        new (&rDestination) DBSArray(_SC(DBSChar *, NULL));
        break;
      case T_DATE:
        new (&rDestination) DBSArray(_SC(DBSDate *, NULL));
        break;
      case T_DATETIME:
        new (&rDestination) DBSArray(_SC(DBSDateTime *, NULL));
        break;
      case T_HIRESTIME:
        new (&rDestination) DBSArray(_SC(DBSHiresDate *, NULL));
        break;
      case T_UINT8:
        new (&rDestination) DBSArray(_SC(DBSUInt8 *, NULL));
        break;
      case T_UINT16:
        new (&rDestination) DBSArray(_SC(DBSUInt16 *, NULL));
        break;
      case T_UINT32:
        new (&rDestination) DBSArray(_SC(DBSUInt32 *, NULL));
        break;
      case T_UINT64:
        new (&rDestination) DBSArray(_SC(DBSUInt64 *, NULL));
        break;
      case T_REAL:
        new (&rDestination) DBSArray(_SC(DBSReal *, NULL));
        break;
      case T_RICHREAL:
        new (&rDestination) DBSArray(_SC(DBSRichReal *, NULL));
        break;
      case T_INT8:
        new (&rDestination) DBSArray(_SC(DBSInt8 *, NULL));
        break;
      case T_INT16:
        new (&rDestination) DBSArray(_SC(DBSInt16 *, NULL));
        break;
      case T_INT32:
        new (&rDestination) DBSArray(_SC(DBSInt32 *, NULL));
        break;
      case T_INT64:
        new (&rDestination) DBSArray(_SC(DBSInt64 *, NULL));
        break;
      default:
        assert (0);
      }
    }
  else
    {
      new (&rDestination) DBSArray(*
            (new RowFieldArray (*m_apVariableFields.get(),
                                fieldFirstEntry,
                                _SC(DBS_FIELD_TYPE, field.mTypeDesc & PS_TABLE_FIELD_TYPE_MASK))));
    }
}

template <class T> void
PSTable::StoreEntry (const T &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const PSFieldDescriptor field = GetFieldDescriptorInternal (fieldIndex);
  bool  checkForDeletion = false;
  bool  checkForReusage = false;

  if ((field.mTypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.mTypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, _SC(DBS_FIELD_TYPE, rSource))))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  StoredItem cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

  assert (field.mNullBitIndex > 0);

  const D_UINT byte_off = field.mNullBitIndex / 8;
  const D_UINT8 bit_off = field.mNullBitIndex % 8;

  if (rSource.IsNull ())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == ~0)
        checkForDeletion = true;
    }
  else
    {
      if (pRawData [byte_off] == ~0)
        checkForReusage = true;

      pRawData [byte_off] &= ~(1 << bit_off);

      PSValInterp::Store (rSource, pRawData + field.mStoreIndex);
    }

  if (checkForReusage)
    CheckRowToReuse (rowIndex);
  else if (checkForDeletion)
    CheckRowToDelete (rowIndex);
}

template <class T> void
PSTable::RetrieveEntry (T &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  T *const pDestination = &rDestination;
  const PSFieldDescriptor field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.mTypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.mTypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, _SC(DBS_FIELD_TYPE, rDestination))))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  StoredItem cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8 *const pRawData = cachedItem.GetDataForRead();

  const D_UINT byte_off = field.mNullBitIndex / 8;
  const D_UINT8 bit_off = field.mNullBitIndex % 8;

  if (pRawData[byte_off] & (1 << bit_off))
    {
      pDestination->~T ();
      new (pDestination) T (true);
    }
  else
    {
      PSValInterp::Retrieve (pDestination, pRawData + field.mStoreIndex);
    }
}
