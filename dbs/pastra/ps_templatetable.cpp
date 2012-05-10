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

#include "ps_templatetable.h"

#include "ps_valintep.h"
#include "ps_textstrategy.h"
#include "ps_arraystrategy.h"

using namespace pastra;
using namespace std;

static const D_CHAR PS_TABLE_FIXFIELDS_EXT[] = "_f";
static const D_CHAR PS_TABLE_VARFIELDS_EXT[] = "_v";

static const D_UINT8 PS_TABLE_SIGNATURE[] =
  { 0x50, 0x41, 0x53, 0x54, 0x52, 0x41, 0x54, 0x42 };

static const D_UINT PS_HEADER_SIZE = 128;

static const D_UINT PS_TABLE_SIG_OFF               = 0; //Signature
static const D_UINT PS_TABLES_SIG_LEN              = 8;
static const D_UINT PS_TABLE_FIELDS_COUNT_OFF      = 8; //Number of fields.
static const D_UINT PS_TABLE_FIELDS_COUNT_LEN      = 4;
static const D_UINT PS_TABLE_ELEMS_SIZE_OFF        = 12; //Size of the fields description area
static const D_UINT PS_TABLE_ELEMS_SIZE_LEN        = 4;
static const D_UINT PS_TABLE_RECORDS_COUNT_OFF     = 16; //Number of allocated records.
static const D_UINT PS_TABLE_RECORDS_COUNT_LEN     = 8;
static const D_UINT PS_TABLE_MAX_FILE_SIZE_OFF     = 24; //The maximum file size allowe for this table
static const D_UINT PS_TABLE_MAX_FILE_SIZE_LEN     = 8;
static const D_UINT PS_TABLE_MAINTABLE_SIZE_OFF    = 32; //Size of variable storage
static const D_UINT PS_TABLE_MAINTABLE_SIZE_LEN    = 8;
static const D_UINT PS_TABLE_VARSTORAGE_SIZE_OFF   = 40; //Size of variable storage
static const D_UINT PS_TABLE_VARSTORAGE_SIZE_LEN   = 8;
static const D_UINT PS_TABLE_BT_ROOT_OFF           = 48; //The root node of BTree holding the removed rows.
static const D_UINT PS_TABLE_BT_ROOT_LEN           = 4;
static const D_UINT PS_TABLE_BT_HEAD_OFF           = 52; //First node pointing to the removed BT nodes.
static const D_UINT PS_TABLE_BT_HEAD_LEN           = 4;
static const D_UINT PS_TABLE_ROW_SIZE_OFF          = 56; //Size of a row.
static const D_UINT PS_TABLE_ROW_SIZE_LEN          = 4;

static const D_UINT PS_RESEVED_FOR_FUTURE_OFF = 60;
static const D_UINT PS_RESEVED_FOR_FUTURE_LEN = PS_HEADER_SIZE - PS_RESEVED_FOR_FUTURE_OFF;

static const D_UINT MAX_FIELD_VALUE_ALIGN = 16; /* Bytes. */


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
get_strlens_till_index (const DBSFieldDescriptor* pFields, D_UINT index)
{
  D_UINT32 result = 0;

  while (index-- > 0)
    {
      result += strlen(pFields->m_pFieldName) + 1;
      ++pFields;
    }

  return result;
}

static void
validate_field_name (const D_CHAR* pFieldName)
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
validate_field_descriptors (const DBSFieldDescriptor* const pFields,
                            const D_UINT                    fieldsCount)
{
  assert ((fieldsCount > 0) && (pFields != NULL));
  for (D_UINT firstIt = 0; firstIt < fieldsCount; ++firstIt)
    {
      validate_field_name(pFields[firstIt].m_pFieldName);

      for (D_UINT secondIt = firstIt; secondIt < fieldsCount; ++secondIt)
        if (firstIt == secondIt)
          continue;
        else if (strcmp(pFields[firstIt].m_pFieldName, pFields[secondIt].m_pFieldName) == 0)
          throw DBSException (
                              pFields[firstIt].m_pFieldName,
                              _EXTRA (DBSException::FIELD_NAME_DUPLICATED));

      if ((pFields[firstIt].m_FieldType == T_UNKNOWN) ||
          (pFields[firstIt].m_FieldType >= T_END_OF_TYPES))
        throw DBSException(NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));

      else if (pFields[firstIt].isArray && (pFields[firstIt].m_FieldType == T_TEXT))
        throw DBSException(NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));
    }
}

static D_INT
get_next_alignment (D_INT size)
{
  D_INT result = 1;

  size &= 0x0F, size |= 0x10;

  while ((size & 1) == 0)
    result <<= 1, size >>= 1;

  return result;
}

static void
arrange_field_entries (vector<DBSFieldDescriptor>& rvFields,
                       D_UINT8* const              pOutFieldsDescription,
                       D_UINT32&                   uOutRowSize)
{

  vector<PaddInterval> padds;

  FieldDescriptor* const pFieldDesc = _RC (FieldDescriptor*, pOutFieldsDescription);
  const D_INT nullBitsRequested = rvFields.size ();      //One for each field.
  D_INT       paddingBytesCount = 0;
  D_INT       currentAlignment  = MAX_FIELD_VALUE_ALIGN; //Force the best choice

  //Find the best fields position in order to minimize the numbers of padding
  //bytes that are need it for values alignments
  for (D_UINT fieldIndex = 0; fieldIndex < rvFields.size(); ++fieldIndex)
    {
      D_INT foundIndex         = -1;    //-1 Nothing found!
      D_INT foundReqAlign      = 1;     //Worst requested align
      D_INT foundResultedAlign = 1;     //Worst resulted align
      D_INT foundReqPaddsCount = MAX_FIELD_VALUE_ALIGN - 1; //Worst number of bytes required to padd
      D_INT foundSize          = 0;

      for (D_UINT schIndex = fieldIndex; schIndex < rvFields.size(); ++schIndex)
        {
          D_INT currIndexSize = PSValInterp::GetSize(
                                                      rvFields[schIndex].m_FieldType,
                                                      rvFields[schIndex].isArray);

          D_INT currReqAlign = PSValInterp::GetAlignment(
                                                         rvFields[schIndex].m_FieldType,
                                                         rvFields[schIndex].isArray);

          D_INT currReqPaddsCount = (currReqAlign > currentAlignment) ?
                                      (currReqAlign - currentAlignment) : 0;
          D_INT currResultedAlign = get_next_alignment (currReqPaddsCount + currIndexSize + uOutRowSize);

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
              if ((foundReqAlign < currReqAlign) ||
                  ((foundReqAlign == currReqAlign) && (foundResultedAlign < currResultedAlign)))
                {
                  //New best choice!
                  foundIndex         = schIndex;
                  foundReqAlign      = currReqAlign;
                  foundResultedAlign = currResultedAlign;
                  foundReqPaddsCount = currReqPaddsCount;
                  foundSize          = currIndexSize;
                }
              else if ((foundReqAlign == currReqAlign) && (foundResultedAlign == currResultedAlign))
                {
                  if (strcmp (rvFields[foundIndex].m_pFieldName, rvFields[schIndex].m_pFieldName) > 0)
                    {
                      //New best choice!
                      assert (foundReqAlign == currReqAlign);
                      assert (foundResultedAlign == currResultedAlign);
                      assert (foundReqPaddsCount == currReqPaddsCount);
                      assert (foundSize == currIndexSize);

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

      pFieldDesc[fieldIndex].m_NameOffset = get_strlens_till_index (&rvFields.front(), fieldIndex);
      pFieldDesc[fieldIndex].m_NameOffset += sizeof(FieldDescriptor) * rvFields.size();

      strcpy(_RC (D_CHAR *, pOutFieldsDescription + pFieldDesc[fieldIndex].m_NameOffset),
             _RC (const D_CHAR *, temp.m_pFieldName));

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
  const D_INT32 extraPaddBytesNeeded = ((nullBitsRequested + 7) / 8) - paddingBytesCount;
  if (extraPaddBytesNeeded > 0)
    {
      padds.push_back(PaddInterval(uOutRowSize, extraPaddBytesNeeded));
      uOutRowSize += extraPaddBytesNeeded;
    }

  //Round the row size so the first element of the next row is alligned
  D_INT32 needExtraAlign = PSValInterp::GetAlignment( rvFields[0].m_FieldType, rvFields[0].isArray);
  needExtraAlign -= get_next_alignment(uOutRowSize);
  if (needExtraAlign > 0)
    uOutRowSize += needExtraAlign;

  //Reuse the padding bytes to hold the fileds value null bits indicators.
  for (D_UINT fieldIndex = 0; fieldIndex < rvFields.size(); ++fieldIndex)
    {
      pFieldDesc[fieldIndex].m_NullBitIndex = padds[0].mBegin++;

      if (padds[0].mBegin > padds[0].mEnd)
        padds.erase(padds.begin());
    }
}
static void
create_table_file (const D_UINT64            maxFileSize,
                   const D_CHAR*             pBaseFileName,
                   const DBSFieldDescriptor* pFields,
                   D_UINT                    fieldsCount)
{

  //Check the arguments
  if ((pFields == NULL) || (fieldsCount == 0) || (fieldsCount > 0xFFFFu))
    throw(DBSException(NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED)));

  //Compute the table header descriptor size
  const D_UINT32 descriptorsSize = sizeof(FieldDescriptor) * fieldsCount +
                                   get_strlens_till_index(pFields, fieldsCount);

  //Validate the optimally rearrange the fields for minimum row size
  D_UINT rowSize = 0;
  validate_field_descriptors(pFields, fieldsCount);

  vector<DBSFieldDescriptor> vect(pFields + 0, pFields + fieldsCount);
  auto_ptr<D_UINT8> apFieldDescription(new D_UINT8[descriptorsSize]);

  arrange_field_entries(vect, apFieldDescription.get(), rowSize);
  pFields = &vect.front();

  WFile tableFile(pBaseFileName, WHC_FILECREATE_NEW | WHC_FILERDWR);

  auto_ptr<D_UINT8> apBuffer(new D_UINT8[PS_HEADER_SIZE]);
  D_UINT8* const    pBuffer = apBuffer.get();

  memcpy (pBuffer, PS_TABLE_SIGNATURE, sizeof PS_TABLE_SIGNATURE);

  *_RC (D_UINT32*, pBuffer + PS_TABLE_FIELDS_COUNT_OFF)    = fieldsCount;
  *_RC (D_UINT32*, pBuffer + PS_TABLE_ELEMS_SIZE_OFF)      = descriptorsSize;
  *_RC (D_UINT64*, pBuffer + PS_TABLE_RECORDS_COUNT_OFF)   = 0;
  *_RC (D_UINT64*, pBuffer + PS_TABLE_VARSTORAGE_SIZE_OFF) = 0;
  *_RC (D_UINT32*, pBuffer + PS_TABLE_ROW_SIZE_OFF)        = rowSize;
  *_RC (NODE_INDEX*, pBuffer + PS_TABLE_BT_ROOT_OFF)       = NIL_NODE;
  *_RC (NODE_INDEX*, pBuffer + PS_TABLE_BT_HEAD_OFF)       = NIL_NODE;
  *_RC (D_UINT64*, pBuffer + PS_TABLE_MAX_FILE_SIZE_OFF)   = maxFileSize;
  *_RC (D_UINT64*, pBuffer + PS_TABLE_MAINTABLE_SIZE_OFF)  = ~0;


  assert (sizeof (NODE_INDEX) == PS_TABLE_BT_HEAD_LEN);
  assert (sizeof (NODE_INDEX) == PS_TABLE_BT_ROOT_LEN);

  memset(pBuffer + PS_RESEVED_FOR_FUTURE_OFF, 0, PS_RESEVED_FOR_FUTURE_LEN);

  //Write the first header part to reserve the space!
  tableFile.Write(pBuffer, PS_HEADER_SIZE);

  //Write the field descriptors;
  tableFile.Write(apFieldDescription.get(), descriptorsSize);

  const D_UINT toFill = tableFile.Tell () % TableRmNode::RAW_NODE_SIZE;

  if (toFill != 0)
    {
      static const D_UINT8 dump [TableRmNode::RAW_NODE_SIZE] = {0,};
      assert ((tableFile.Tell() == tableFile.GetSize()));
      assert ((tableFile.Tell() < sizeof dump));

      tableFile.Write (dump, sizeof dump - toFill);
    }
  assert ((tableFile.Tell() == tableFile.GetSize()));

  *_RC (D_UINT64*, pBuffer + PS_TABLE_MAINTABLE_SIZE_OFF) = tableFile.GetSize ();

  tableFile.Seek (0, WHC_SEEK_BEGIN);
  tableFile.Write (pBuffer, PS_HEADER_SIZE);
}


///////////////////////////////////////////////////////////////////////////////////////////////////

TemplateTable::TemplateTable (DbsHandler& dbsHandler, const string& tableName) :
  m_MaxFileSize (0),
  m_RowsCount (0),
  m_RootNode (NIL_NODE),
  m_FirstUnallocatedRoot (NIL_NODE),
  m_DescriptorsSize (~0),
  m_FieldsCount (0),
  m_BaseFileName (dbsHandler.GetDir() + tableName),
  m_FieldsDescriptors(NULL),
  m_apMainTable (NULL),
  m_apFixedFields (NULL),
  m_apVariableFields (NULL),
  m_RowCache (*this),
  m_Sync (),
  m_IndexSync (),
  m_Removed (false)
{
  InitFromFile ();

  assert (m_apMainTable.get () != NULL);
  assert (m_RootNode != NIL_NODE);

  m_RowCache.Init (m_RowSize, 4096, 1024);

  InitVariableStorages ();
  InitIndexedFields ();
}


TemplateTable::TemplateTable (DbsHandler&               dbsHandler,
                              const string&             tableName,
                              const DBSFieldDescriptor* pFields,
                              const D_UINT              fieldsCount,
                              const bool                temporal) :
  m_MaxFileSize (0),
  m_RowsCount (0),
  m_RootNode (NIL_NODE),
  m_FirstUnallocatedRoot (NIL_NODE),
  m_DescriptorsSize (~0),
  m_FieldsCount (0),
  m_BaseFileName (dbsHandler.GetDir() + tableName),
  m_FieldsDescriptors(NULL),
  m_apMainTable (NULL),
  m_apFixedFields (NULL),
  m_apVariableFields (NULL),
  m_RowCache (*this),
  m_Sync (),
  m_IndexSync (),
  m_Removed (false)
{
  create_table_file (dbsHandler.GetMaxFileSize (),
                     m_BaseFileName.c_str (),
                     pFields,
                     fieldsCount);
  InitFromFile ();

  assert (m_apMainTable.get () != NULL);

  m_RowCache.Init (m_RowSize, 4096, 1024);

  InitVariableStorages ();
  InitIndexedFields ();

  assert (m_RootNode == NIL_NODE);

  BTreeNodeHandler rootNode (RetrieveNode (AllocateNode (NIL_NODE, 0)));
  rootNode->SetNext (NIL_NODE);
  rootNode->SetPrev (NIL_NODE);
  rootNode->SetKeysCount (0);
  rootNode->SetLeaf (true);
  rootNode->InsertKey (rootNode->GetSentinelKey ());

  SetRootNodeId (rootNode->GetNodeId());
}

TemplateTable::~TemplateTable ()
{
  FlushNodes ();

  for (D_UINT fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex)
    if (m_vIndexNodeMgrs[fieldIndex] != NULL)
      {
        FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

        D_UINT64 unitsCount = DBSGetMaxFileSize() - 1;

        unitsCount += m_vIndexNodeMgrs[fieldIndex]->GetIndexRawSize();
        unitsCount /= DBSGetMaxFileSize ();

        field.m_IndexUnitsCount = unitsCount;
        delete m_vIndexNodeMgrs [fieldIndex];
      }
  SyncToFile ();
}

bool
TemplateTable::IsTemporal () const
{
  return false;
}

D_UINT
TemplateTable::GetFieldsCount ()
{
  return m_FieldsCount;
}

DBSFieldDescriptor
TemplateTable::GetFieldDescriptor (D_UINT fieldIndex)
{
  const FieldDescriptor * const pDesc = _RC(const FieldDescriptor*, m_FieldsDescriptors.get ());

  if (fieldIndex >= m_FieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  DBSFieldDescriptor result;

  result.isArray      = (pDesc[fieldIndex].m_TypeDesc & PS_TABLE_ARRAY_MASK) != 0;
  result.m_FieldType  = _SC (DBS_FIELD_TYPE, pDesc[fieldIndex].m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK);
  result.m_pFieldName = _RC (const D_CHAR *, pDesc) + pDesc[fieldIndex].m_NameOffset;

  return result;
}

DBSFieldDescriptor
TemplateTable::GetFieldDescriptor (const D_CHAR* const pFieldName)
{

  const FieldDescriptor* const pDesc = _RC(const FieldDescriptor*, m_FieldsDescriptors.get ());
  D_UINT iterator = m_FieldsCount * sizeof(FieldDescriptor);

  for (D_UINT index = 0; index < m_FieldsCount; ++index)
    {
      if (strcmp(_RC (const D_CHAR *, m_FieldsDescriptors.get() + iterator), pFieldName) == 0)
        {
          DBSFieldDescriptor result;

          result.m_pFieldName = _RC (const D_CHAR*, pDesc) + iterator;
          result.isArray      = (pDesc[index].m_TypeDesc & PS_TABLE_ARRAY_MASK) != 0;
          result.m_FieldType  = _SC (DBS_FIELD_TYPE, pDesc[index].m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK);

          return result;
        }
      iterator += strlen(_RC(const D_CHAR*, pDesc) + iterator) + 1;
    }

  throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));
}

D_UINT64
TemplateTable::GetAllocatedRows ()
{
  return m_RowsCount;
}

D_UINT64
TemplateTable::AddRow ()
{
  D_UINT64 lastRowPosition = m_RowsCount * m_RowSize;
  D_UINT   toWrite         = m_RowSize;
  D_UINT8  dummyValue[128];

  memset(dummyValue, 0xFF, sizeof(dummyValue));

  while (toWrite)
    {
      D_UINT writeChunk = MIN (toWrite, sizeof (dummyValue));

      m_apFixedFields.get ()->StoreData (lastRowPosition, writeChunk, dummyValue);

      toWrite -= writeChunk;
      lastRowPosition += writeChunk;
    }

  NODE_INDEX dummyNode;
  KEY_INDEX  dummyKey;
  BTree      removedRows (*this);

  removedRows.InsertKey (TableRmKey (m_RowsCount), dummyNode, dummyKey);

  IncreaseRowCount ();

  assert (m_RowsCount > 0);

  m_RowCache.ForceItemUpdate (m_RowsCount - 1);

  return m_RowsCount - 1;
}

D_UINT64
TemplateTable::AddReusedRow ()
{
  D_UINT64     result = 0;
  NODE_INDEX   node;
  KEY_INDEX    keyIndex;
  TableRmKey key (0);
  BTree        removedRows (*this);

  if (removedRows.FindBiggerOrEqual(key, node, keyIndex) == false)
    result = TemplateTable::AddRow ();
  else
    {
      BTreeNodeHandler keyNode (RetrieveNode (node));
      TableRmNode *const pNode = _SC (TableRmNode *, &(*keyNode));

      assert (keyNode->IsLeaf() );
      assert (keyIndex < keyNode->GetKeysCount());

      const D_UINT64 *const pRows = _RC (D_UINT64*,
                                         pNode->GetRawData () + sizeof (TableRmNode::NodeHeader));
      result = pRows [keyIndex];
    }

  assert (result > 0);

  return result;
}

void
TemplateTable::MarkRowForReuse (D_UINT64 rowIndex)
{
  D_UINT fieldsCount = m_FieldsCount;

  StoredItem cachedItem (m_RowCache.RetriveItem (rowIndex));
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

    while (fieldsCount-- > 0)
      {
        const FieldDescriptor&   field    = GetFieldDescriptorInternal (fieldsCount);
        D_UINT                   byte_off = field.m_NullBitIndex / 8;
        D_UINT8                  bit_off  = field.m_NullBitIndex % 8;

        pRawData[byte_off] |= (1 << bit_off);
      }

    NODE_INDEX   node;
    KEY_INDEX    keyIndex;
    TableRmKey   key (rowIndex);
    BTree        removedRows (*this);

    removedRows.InsertKey (key, node, keyIndex);
}

template <class T> static void
insert_row_field (TemplateTable &table,
                  BTree &tree,
                  const D_UINT64 rowIndex,
                  const D_UINT fieldIndex)
{
  T rowValue;
  table.GetEntry (rowValue, rowIndex, fieldIndex);


  NODE_INDEX dummyNode;
  KEY_INDEX  dummyKey;

  T_BTreeKey<T> keyValue (rowValue, rowIndex);
  tree.InsertKey (keyValue, dummyNode, dummyKey);
}

void
TemplateTable::CreateFieldIndex (const D_UINT                      fieldIndex,
                           CREATE_INDEX_CALLBACK_FUNC* const cb_func,
                           CallBackIndexData* const          pCbData)
{
  if (TemplateTable::IsFieldIndexed(fieldIndex))
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_INDEXED));

  if ((cb_func == NULL) && (pCbData != NULL))
    throw DBSException (NULL, _EXTRA (DBSException::INVALID_PARAMETERS));

  FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc == T_TEXT) ||
      (field.m_TypeDesc & PS_TABLE_ARRAY_MASK) != 0)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));

  const D_UINT nodeSizeKB  = 16; //16KB
  string containerNameBase = m_BaseFileName;

  containerNameBase += "_idf";
  containerNameBase += fieldIndex;
  containerNameBase += "bt";

  auto_ptr <I_DataContainer> apIndexContainer (new FileContainer (containerNameBase.c_str (),
                                                                  DBSGetMaxFileSize(),
                                                                  0));

  auto_ptr <FieldIndexNodeManager> apFieldMgr (new FieldIndexNodeManager (apIndexContainer,
                                                                          nodeSizeKB * 1024,
                                                                          0x400000, //4MB
                                                                          _SC (DBS_FIELD_TYPE,
                                                                               field.m_TypeDesc),
                                                                          true));

  BTreeNodeHandler rootNode (apFieldMgr->RetrieveNode (apFieldMgr->AllocateNode (NIL_NODE, 0)));
  rootNode->SetNext (NIL_NODE);
  rootNode->SetPrev (NIL_NODE);
  rootNode->SetKeysCount (0);
  rootNode->SetLeaf (true);
  rootNode->InsertKey (rootNode->GetSentinelKey ());

  apFieldMgr->SetRootNodeId (rootNode->GetNodeId());
  BTree fieldTree (* apFieldMgr.get ());

  for (D_UINT64 rowIndex = 0; rowIndex < m_RowsCount; ++rowIndex)
    {
      switch (field.m_TypeDesc)
      {
      case T_BOOL:
        insert_row_field<DBSBool> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_CHAR:
        insert_row_field<DBSChar> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_DATE:
        insert_row_field<DBSDate> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_DATETIME:
        insert_row_field<DBSDateTime> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_HIRESTIME:
        insert_row_field<DBSHiresTime> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_UINT8:
        insert_row_field<DBSUInt8> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_UINT16:
        insert_row_field<DBSUInt16> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_UINT32:
        insert_row_field<DBSUInt32> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_UINT64:
        insert_row_field<DBSUInt64> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_INT8:
        insert_row_field<DBSInt8> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_INT16:
        insert_row_field<DBSInt16> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_INT32:
        insert_row_field<DBSInt32> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_INT64:
        insert_row_field<DBSInt64> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_REAL:
        insert_row_field<DBSReal> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_RICHREAL:
        insert_row_field<DBSRichReal> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      default:
        assert (false);
      }

      if (cb_func != NULL)
        {
          if (pCbData != NULL)
            {
              pCbData->m_RowsCount = m_RowsCount;
              pCbData->m_RowIndex  = rowIndex;
            }
          cb_func (pCbData);
        }
    }

  field.m_IndexNodeSizeKB = nodeSizeKB;
  field.m_IndexUnitsCount = 1;
  SyncToFile ();

  assert (m_vIndexNodeMgrs[fieldIndex] == NULL);
  m_vIndexNodeMgrs[fieldIndex] = apFieldMgr.release ();

}

void
TemplateTable::RemoveFieldIndex (const D_UINT fieldIndex)
{
  if (TemplateTable::IsFieldIndexed(fieldIndex) == false)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_NOT_INDEXED));

  FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  assert (field.m_IndexNodeSizeKB > 0);
  assert (field.m_IndexUnitsCount > 0);

  field.m_IndexNodeSizeKB = 0;
  field.m_IndexUnitsCount = 0;
  SyncToFile ();

  auto_ptr <FieldIndexNodeManager> apFieldMgr (m_vIndexNodeMgrs [fieldIndex]);
  m_vIndexNodeMgrs[fieldIndex] = NULL;

  apFieldMgr->MarkForRemoval ();
}

bool
TemplateTable::IsFieldIndexed (const D_UINT fieldIndex) const
{
  if (fieldIndex >= m_FieldsCount)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  assert (m_vIndexNodeMgrs.size () == m_FieldsCount);

  return (m_vIndexNodeMgrs[fieldIndex] != NULL);
}

D_UINT
TemplateTable::GetRowSize() const
{
  return m_RowSize;
}

FieldDescriptor&
TemplateTable::GetFieldDescriptorInternal(D_UINT fieldIndex) const
{
  FieldDescriptor* const pDesc = _RC(FieldDescriptor*, m_FieldsDescriptors.get ());

  if (fieldIndex >= m_FieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  return pDesc[fieldIndex];
}

FieldDescriptor&
TemplateTable::GetFieldDescriptorInternal(const D_CHAR* const pFieldName) const
{

  FieldDescriptor* const   pDesc    = _RC (FieldDescriptor*, m_FieldsDescriptors.get ());
  D_UINT                   iterator = m_FieldsCount * sizeof(FieldDescriptor);

  for (D_UINT index = 0; index < m_FieldsCount; ++index)
    {
      if (strcmp ( _RC(const D_CHAR *, m_FieldsDescriptors.get() + iterator), pFieldName) == 0)
        return pDesc[index];

      iterator += strlen( _RC(const D_CHAR*, pDesc) + iterator) + 1;
    }

  throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));
}

D_UINT
TemplateTable::GetRawNodeSize () const
{
  return TableRmNode::RAW_NODE_SIZE;
}

NODE_INDEX
TemplateTable::AllocateNode (const NODE_INDEX parent, KEY_INDEX parentKey)
{
  NODE_INDEX nodeIndex = m_FirstUnallocatedRoot;

  if (nodeIndex != NIL_NODE)
    {
      BTreeNodeHandler freeNode (RetrieveNode (nodeIndex));

      m_FirstUnallocatedRoot = freeNode->GetNext ();
      SyncToFile ();
    }
  else
    {
      assert (m_apMainTable->GetContainerSize () % GetRawNodeSize () == 0);

      nodeIndex = m_apMainTable->GetContainerSize () / GetRawNodeSize ();
    }

  if (parent != NIL_NODE)
    {
      BTreeNodeHandler parentNode (RetrieveNode (parent));
      parentNode->SetChildNode (parentKey, nodeIndex);

      assert (parentNode->IsLeaf() == false);
    }

  return nodeIndex;
}

void
TemplateTable::FreeNode (const NODE_INDEX nodeId)
{
  BTreeNodeHandler node (RetrieveNode (nodeId));

  node->MarkAsRemoved();
  node->SetNext (m_FirstUnallocatedRoot);

  m_FirstUnallocatedRoot = node->GetNodeId();
  SyncToFile ();
}

NODE_INDEX
TemplateTable::GetRootNodeId () const
{
  return m_RootNode;
}

void
TemplateTable::SetRootNodeId (const NODE_INDEX node)
{
  m_RootNode = node;
  SyncToFile ();
}


D_UINT
TemplateTable::GetMaxCachedNodes ()
{
  return 128;
}

I_BTreeNode*
TemplateTable::GetNode (const NODE_INDEX node)
{
  std::auto_ptr <TableRmNode> apNode (new TableRmNode (*this, node));

  assert (m_apMainTable->GetContainerSize () % GetRawNodeSize () == 0);

  if (m_apMainTable->GetContainerSize () > node * GetRawNodeSize ())
    {
      m_apMainTable->RetrieveData (apNode->GetNodeId () * GetRawNodeSize (),
                                   GetRawNodeSize (),
                                   apNode->GetRawData ());
    }
  else
    {
      //Reserve space for this node
      assert (m_apMainTable->GetContainerSize () == (node * GetRawNodeSize ()));
      m_apMainTable->StoreData (m_apMainTable->GetContainerSize (),
                                GetRawNodeSize (),
                                apNode->GetRawData ());
    }

  return apNode.release ();
}


void
TemplateTable::StoreNode (I_BTreeNode* const pNode)
{
  if (pNode->IsDirty() == false)
    return ;

  m_apMainTable->StoreData (pNode->GetNodeId() * GetRawNodeSize (),
                            GetRawNodeSize (),
                            pNode->GetRawData ());

  return;
}

void
TemplateTable::StoreItems (const D_UINT8* pSrcBuffer,
                     D_UINT64       firstItem,
                     D_UINT         itemsCount)
{
  m_apFixedFields.get()->StoreData( firstItem * m_RowSize, itemsCount * m_RowSize, pSrcBuffer);
}

void
TemplateTable::RetrieveItems (D_UINT8 *pDestBuffer,
                        D_UINT64 firstItem,
                        D_UINT   itemsCount)
{
  if (itemsCount + firstItem > m_RowsCount)
    itemsCount = m_RowsCount - firstItem;

  m_apFixedFields.get()->RetrieveData( firstItem * m_RowSize, itemsCount * m_RowSize, pDestBuffer);
}


void
TemplateTable::InitIndexedFields ()
{
  for (D_UINT fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex)
    {
      FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

      if (field.m_IndexNodeSizeKB == 0)
        {
          assert (field.m_IndexUnitsCount == 0);
          m_vIndexNodeMgrs.push_back (NULL);
          continue;
        }

      string containerNameBase = m_BaseFileName;

      containerNameBase += "_idf";
      containerNameBase += fieldIndex;
      containerNameBase += "bt";

      auto_ptr <I_DataContainer> apIndexContainer (new FileContainer (containerNameBase.c_str (),
                                                                      DBSGetMaxFileSize(),
                                                                      field.m_IndexUnitsCount ));

      m_vIndexNodeMgrs.push_back (new FieldIndexNodeManager (apIndexContainer,
                                                             field.m_IndexNodeSizeKB * 1024,
                                                             0x400000, //4MB
                                                             _SC (DBS_FIELD_TYPE, field.m_TypeDesc),
                                                             false));
    }
}

void
TemplateTable::InitVariableStorages ()
{
  const D_UINT64 maxFileSize = DBSGetMaxFileSize() - (DBSGetMaxFileSize() % m_RowSize);

  m_apFixedFields.reset (
      new FileContainer((m_BaseFileName + PS_TABLE_FIXFIELDS_EXT).c_str(),
                         maxFileSize,
                         ((m_RowSize * m_RowsCount) + maxFileSize - 1) / maxFileSize));

  for (D_UINT fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex)
    {
        DBSFieldDescriptor fieldDesc = GetFieldDescriptor (fieldIndex);

        assert ((fieldDesc.m_FieldType > T_UNKNOWN) && (fieldDesc.m_FieldType < T_UNDETERMINED));

        if (fieldDesc.isArray || (fieldDesc.m_FieldType == T_TEXT))
          {
            m_apVariableFields.reset (new VariableLengthStore ());
            m_apVariableFields->Init ((m_BaseFileName + PS_TABLE_VARFIELDS_EXT).c_str(),
                                      m_VariableStorageSize,
                                      maxFileSize);
            break; //We finished here!
          }
    }
}

void
TemplateTable::SyncToFile ()
{

  if (m_Removed)
    return ; //We were removed. We were removed.

  //TODO: You need to set the units correct after you add the part with container

  D_UINT8 aTableHdr[PS_HEADER_SIZE];

  memcpy (aTableHdr, PS_TABLE_SIGNATURE, sizeof PS_TABLE_SIGNATURE);

  *_RC (D_UINT32*, aTableHdr + PS_TABLE_FIELDS_COUNT_OFF)    = m_FieldsCount;
  *_RC (D_UINT32*, aTableHdr + PS_TABLE_ELEMS_SIZE_OFF)      = m_DescriptorsSize;
  *_RC (D_UINT64*, aTableHdr + PS_TABLE_RECORDS_COUNT_OFF)   = m_RowsCount;
  *_RC (D_UINT64*, aTableHdr + PS_TABLE_VARSTORAGE_SIZE_OFF) =
      (m_apVariableFields.get() != NULL) ? m_apVariableFields->GetRawSize() : 0;
  *_RC (D_UINT32*, aTableHdr + PS_TABLE_ROW_SIZE_OFF)        = m_RowSize;
  *_RC (NODE_INDEX*, aTableHdr + PS_TABLE_BT_ROOT_OFF)       = m_RootNode;
  *_RC (NODE_INDEX*, aTableHdr + PS_TABLE_BT_HEAD_OFF)       = m_FirstUnallocatedRoot;
  *_RC (D_UINT64*, aTableHdr + PS_TABLE_MAX_FILE_SIZE_OFF)   = m_MaxFileSize;
  *_RC (D_UINT64*, aTableHdr + PS_TABLE_MAINTABLE_SIZE_OFF)  = m_apMainTable->GetContainerSize ();

  memset(aTableHdr + PS_RESEVED_FOR_FUTURE_OFF, 0, PS_RESEVED_FOR_FUTURE_LEN);

  m_apMainTable->StoreData (0, sizeof aTableHdr, aTableHdr);
  m_apMainTable->StoreData (sizeof aTableHdr, m_DescriptorsSize, m_FieldsDescriptors.get ());
}

void
TemplateTable::InitFromFile ()
{
  D_UINT64   mainTableSize = 0;
  D_UINT8    aTableHdr[PS_HEADER_SIZE];


  WFile mainTableFile (m_BaseFileName.c_str(), WHC_FILEOPEN_EXISTING | WHC_FILEREAD);

  mainTableFile.Seek(0, WHC_SEEK_BEGIN);
  mainTableFile.Read(aTableHdr, PS_HEADER_SIZE);

  if (memcmp(aTableHdr, PS_TABLE_SIGNATURE, PS_TABLES_SIG_LEN) != 0)
    throw DBSException(NULL, _EXTRA (DBSException::TABLE_INVALID));

  //Retrieve the header information.
  m_FieldsCount          = *_RC (D_UINT32*, aTableHdr + PS_TABLE_FIELDS_COUNT_OFF);
  m_DescriptorsSize      = *_RC (D_UINT32*, aTableHdr + PS_TABLE_ELEMS_SIZE_OFF);
  m_RowsCount            = *_RC (D_UINT64*, aTableHdr + PS_TABLE_RECORDS_COUNT_OFF);
  m_VariableStorageSize  = *_RC (D_UINT64*, aTableHdr + PS_TABLE_VARSTORAGE_SIZE_OFF);
  m_RowSize              = *_RC (D_UINT32*, aTableHdr + PS_TABLE_ROW_SIZE_OFF);
  m_RootNode             = *_RC (NODE_INDEX*, aTableHdr + PS_TABLE_BT_ROOT_OFF);
  m_FirstUnallocatedRoot = *_RC (NODE_INDEX*, aTableHdr + PS_TABLE_BT_HEAD_OFF);
  m_MaxFileSize          = *_RC (D_UINT64*, aTableHdr + PS_TABLE_MAX_FILE_SIZE_OFF);
  mainTableSize          = *_RC (D_UINT64*, aTableHdr + PS_TABLE_MAINTABLE_SIZE_OFF);

  if ((m_FieldsCount == 0) ||
      (m_DescriptorsSize < (sizeof(FieldDescriptor) * m_FieldsCount)) ||
      (mainTableSize < PS_HEADER_SIZE))
    {
      throw DBSException(NULL, _EXTRA (DBSException::TABLE_INVALID));
    }

  //Cache the field descriptors in memory
  m_FieldsDescriptors.reset(new D_UINT8[m_DescriptorsSize]);
  mainTableFile.Read(_CC(D_UINT8 *, m_FieldsDescriptors.get ()), m_DescriptorsSize);

  m_apMainTable.reset (new FileContainer (m_BaseFileName.c_str(),
                                          m_MaxFileSize,
                                          (mainTableSize + m_MaxFileSize - 1) / m_MaxFileSize));
}

D_UINT64
TemplateTable::IncreaseRowCount ()
{
  return ++m_RowsCount;
}

void
TemplateTable::RemoveFromDatabase ()
{
  if (m_apFixedFields.get() != NULL)
    m_apFixedFields->MarkForRemoval();

  if (m_apVariableFields.get() != NULL)
    m_apVariableFields->MarkForRemoval();

  for (D_UINT fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex )
    if (m_vIndexNodeMgrs[fieldIndex]  != NULL)
      m_vIndexNodeMgrs[fieldIndex]->MarkForRemoval ();

  m_apMainTable->MarkForRemoval ();

  m_Removed = true;
}

void
TemplateTable::CheckRowToDelete (const D_UINT64 rowIndex)
{
  bool isRowDeleted = true;

  StoredItem cachedItem   = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

  for (D_UINT64 index = 0; index < m_FieldsCount; index += 8)
    {
      const FieldDescriptor&   fieldDesc = GetFieldDescriptorInternal (index);
      const D_UINT8            bitsSet   = ~0;

      if ( pRawData [fieldDesc.m_NullBitIndex / 8] != bitsSet)
        {
          isRowDeleted = false;
          break;
        }
    }

  if (isRowDeleted)
    {
      NODE_INDEX   dumyNode;
      KEY_INDEX    dummyKey;
      BTree        removedNodes (*this);
      TableRmKey    key (rowIndex);

      removedNodes.InsertKey (key, dumyNode, dummyKey);
    }
}

void
TemplateTable::CheckRowToReuse (const D_UINT64 rowIndex)
{
  bool wasRowDeleted = true;

  StoredItem     cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData   = cachedItem.GetDataForUpdate();

  for (D_UINT64 index = 0; index < m_FieldsCount; index += 8)
    {
      const FieldDescriptor& fieldDesc = GetFieldDescriptorInternal (index);
      const D_UINT8            bitsSet   = ~0;

      if ( pRawData [fieldDesc.m_NullBitIndex / 8] != bitsSet)
        {
          wasRowDeleted = false;
          break;
        }
    }

  if (wasRowDeleted)
    {
      BTree        removedNodes (*this);
      TableRmKey   key (rowIndex);

      removedNodes.RemoveKey (key);
    }
}

void
TemplateTable::AquireIndexField (FieldDescriptor* const pFieldDesc)
{
  bool aquired = false;
  while ( ! aquired)
    {
      WSynchronizerRAII syncHolder (m_IndexSync);

      if (pFieldDesc->m_Aquired == 0)
        {
          pFieldDesc->m_Aquired = 1;
          aquired = true;
        }

      syncHolder.Leave ();

      if ( ! aquired)
        wh_yield ();
    }
}

void
TemplateTable::ReleaseIndexField (FieldDescriptor* const pFieldDesc)
{
  WSynchronizerRAII syncHolder (m_IndexSync);

  assert (pFieldDesc->m_Aquired != 0);

  pFieldDesc->m_Aquired = 0;

}

void
TemplateTable::SetEntry (const DBSChar& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSBool& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSDate& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSDateTime& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSHiresTime& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSInt8& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSInt16& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSInt32& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSInt64& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSReal& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSRichReal& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSUInt8& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSUInt16& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSUInt32& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSUInt64& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
TemplateTable::SetEntry (const DBSText& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, T_TEXT)))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  D_UINT64                 newFirstEntry     = ~0;
  D_UINT64                 newFieldValueSize = 0;
  const D_UINT8            bitsSet           = ~0;
  bool                     fieldValueWasNull = false;

  if (rSource.IsNull() == false)
    {
      I_TextStrategy& text = rSource;
      if (text.IsRowValue ())
        {
          RowFieldText& value = text.GetRowValue();
          newFieldValueSize   = value.m_BytesSize;
          newFirstEntry       = m_apVariableFields->AddRecord (value.m_Storage,
                                                               value.m_FirstEntry,
                                                               0,
                                                               value.m_BytesSize);
        }
      else
        {
          TemporalText& value = text.GetTemporal();
          newFieldValueSize   = value.m_BytesSize;
          newFirstEntry       = m_apVariableFields->AddRecord (value.m_Storage,
                                                               0,
                                                               newFieldValueSize);
        }
    }

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem     cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8* const pRawData   = cachedItem.GetDataForUpdate();

  D_UINT64* const fieldFirstEntry = _RC (D_UINT64*, pRawData + field.m_StoreIndex + 0);
  D_UINT64* const fieldValueSize  = _RC (D_UINT64*,
                                         pRawData + field.m_StoreIndex + sizeof (D_UINT64));

  assert (field.m_NullBitIndex > 0);

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  if ((pRawData [byte_off] & (1 << bit_off)) != 0)
    fieldValueWasNull = true;

  if (fieldValueWasNull && rSource.IsNull ())
    return ;
  else if ( (fieldValueWasNull == false) && rSource.IsNull())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (rowIndex);

      return;
    }
  else if (rSource.IsNull() == false)
    {
      if (pRawData [byte_off] == bitsSet)
        {
          assert (fieldValueWasNull == true);
          CheckRowToReuse (rowIndex);
        }
      pRawData [byte_off] &= ~(1 << bit_off);
    }

  if (fieldValueWasNull == false)
    {
      //Postpone the removal of the actual record entries
      //to allow other threads gain access to 'm_Sync' faster.
      RowFieldText   oldEntryRAII (*m_apVariableFields.get (),
                                   *fieldFirstEntry,
                                   *fieldValueSize);

      m_apVariableFields->DecrementRecordRef (*fieldFirstEntry);
      *fieldFirstEntry = newFirstEntry;
      *fieldValueSize  = newFieldValueSize;

      syncHolder.Leave ();
    }
  else
    {
      *fieldFirstEntry = newFirstEntry;
      *fieldValueSize  = newFieldValueSize;
    }
}

void
TemplateTable::SetEntry (const DBSArray& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ( ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) == 0) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, rSource.GetElementsType())))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  D_UINT64      newFirstEntry     = ~0;
  D_UINT64      newFieldValueSize = 0;
  const D_UINT8 bitsSet           = ~0;
  bool          fieldValueWasNull = false;

  if (rSource.IsNull() == false)
    {
      I_ArrayStrategy &array = rSource;
      if (array.IsRowValue())
        {
          RowFieldArray& value = array.GetRowValue();
          newFieldValueSize    = value.GetRawDataSize();
          newFirstEntry        = m_apVariableFields->AddRecord (value.m_Storage,
                                                                value.m_FirstRecordEntry,
                                                                0,
                                                                newFieldValueSize);
        }
      else
        {
          TemporalArray& value = array.GetTemporal ();
          newFieldValueSize    = value.GetRawDataSize();

          const D_UINT64 elemsCount = value.GetElementsCount();
          newFirstEntry             = m_apVariableFields->AddRecord (_RC(const D_UINT8*, &elemsCount),
                                                                     sizeof elemsCount);
          m_apVariableFields->UpdateRecord (newFirstEntry,
                                            sizeof elemsCount,
                                            value.m_Storage,
                                            0,
                                            newFieldValueSize);
          newFieldValueSize += sizeof elemsCount;
        }
    }

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem cachedItem           = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData         = cachedItem.GetDataForUpdate();

  D_UINT64 *const fieldFirstEntry = _RC (D_UINT64*, pRawData + field.m_StoreIndex + 0);
  D_UINT64 *const fieldValueSize  = _RC (D_UINT64*,
                                         pRawData + field.m_StoreIndex + sizeof (D_UINT64));

  assert (field.m_NullBitIndex > 0);

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  if ((pRawData [byte_off] & (1 << bit_off)) != 0)
    fieldValueWasNull = true;

  if (fieldValueWasNull && rSource.IsNull ())
    return ;
  else if ( (fieldValueWasNull == false) && rSource.IsNull())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (rowIndex);

      return ;
    }
  else if (rSource.IsNull() == false)
    {
      if (pRawData [byte_off] == bitsSet)
        {
          assert (fieldValueWasNull == true);
          CheckRowToReuse (rowIndex);
        }
      pRawData [byte_off] &= ~(1 << bit_off);
    }

  if (fieldValueWasNull == false)
    {
      //Postpone the removal of the actual record entries
      //to allow other threads gain access to 'm_Sync' faster.
      RowFieldArray   oldEntryRAII (*m_apVariableFields.get (),
                                    *fieldFirstEntry,
                                    rSource.GetElementsType ());

      m_apVariableFields->DecrementRecordRef (*fieldFirstEntry);
      *fieldFirstEntry = newFirstEntry;
      *fieldValueSize  = newFieldValueSize;

      syncHolder.Leave ();
    }
  else
    {
      *fieldFirstEntry = newFirstEntry;
      *fieldValueSize  = newFieldValueSize;
    }
}

void
TemplateTable::GetEntry (DBSChar& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSBool& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSDate& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSDateTime& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSHiresTime& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSInt8& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSInt16& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSInt32& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSInt64& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSReal& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSRichReal& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSUInt8& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSUInt16& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSUInt32& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
TemplateTable::GetEntry (DBSUInt64& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

static RowFieldText*
allocate_row_field_text (VariableLengthStore& store,
                         const D_UINT64       firstRecordEntry,
                         const D_UINT64       valueSize)
{
  return new RowFieldText (store, firstRecordEntry, valueSize);
}

static RowFieldArray*
allocate_row_field_array (VariableLengthStore& store,
                          const D_UINT64       firstRecordEntry,
                          const DBS_FIELD_TYPE type)
{
  return new RowFieldArray (store, firstRecordEntry, type);
}

void
TemplateTable::GetEntry (DBSText& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, T_TEXT)))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem           cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8* const pRawData   = cachedItem.GetDataForRead();

  const D_UINT64& fieldFirstEntry = *_RC (const D_UINT64*, pRawData + field.m_StoreIndex + 0);
  const D_UINT64& fieldValueSize  = *_RC (const D_UINT64*,
                                          pRawData + field.m_StoreIndex + sizeof (D_UINT64));

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  rDestination.~DBSText ();
  if (pRawData[byte_off] & (1 << bit_off))
    _placement_new (&rDestination, DBSText (NULL));
  else
    {
      _placement_new (&rDestination,
                      DBSText (*allocate_row_field_text (*m_apVariableFields.get(),
                                                          fieldFirstEntry,
                                                          fieldValueSize)));
    }
}

void
TemplateTable::GetEntry (DBSArray& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{

  const FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ( ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) == 0) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, rDestination.GetElementsType())))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem           cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8 *const pRawData   = cachedItem.GetDataForRead();

  const D_UINT64& fieldFirstEntry = *_RC (const D_UINT64*,
                                          pRawData + field.m_StoreIndex + 0);

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  rDestination.~DBSArray ();
  if (pRawData[byte_off] & (1 << bit_off))
    {
      switch (field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK)
      {
      case T_BOOL:
        _placement_new (&rDestination, DBSArray(_SC(DBSBool *, NULL)));
        break;
      case T_CHAR:
        _placement_new (&rDestination, DBSArray(_SC(DBSChar *, NULL)));
        break;
      case T_DATE:
        _placement_new (&rDestination, DBSArray(_SC(DBSDate *, NULL)));
        break;
      case T_DATETIME:
        _placement_new (&rDestination, DBSArray(_SC(DBSDateTime *, NULL)));
        break;
      case T_HIRESTIME:
        _placement_new (&rDestination, DBSArray(_SC(DBSHiresTime *, NULL)));
        break;
      case T_UINT8:
        _placement_new (&rDestination, DBSArray(_SC(DBSUInt8 *, NULL)));
        break;
      case T_UINT16:
        _placement_new (&rDestination, DBSArray(_SC(DBSUInt16 *, NULL)));
        break;
      case T_UINT32:
        _placement_new (&rDestination, DBSArray(_SC(DBSUInt32 *, NULL)));
        break;
      case T_UINT64:
        _placement_new (&rDestination, DBSArray(_SC(DBSUInt64 *, NULL)));
        break;
      case T_REAL:
        _placement_new (&rDestination, DBSArray(_SC(DBSReal *, NULL)));
        break;
      case T_RICHREAL:
        _placement_new (&rDestination, DBSArray(_SC(DBSRichReal *, NULL)));
        break;
      case T_INT8:
        _placement_new (&rDestination, DBSArray(_SC(DBSInt8 *, NULL)));
        break;
      case T_INT16:
        _placement_new (&rDestination, DBSArray(_SC(DBSInt16 *, NULL)));
        break;
      case T_INT32:
        _placement_new (&rDestination, DBSArray(_SC(DBSInt32 *, NULL)));
        break;
      case T_INT64:
        _placement_new (&rDestination, DBSArray(_SC(DBSInt64 *, NULL)));
        break;
      default:
        assert (0);
      }
    }
  else
    {
      _placement_new (&rDestination,
                      DBSArray (*allocate_row_field_array (*m_apVariableFields.get(),
                                                            fieldFirstEntry,
                                                            _SC (DBS_FIELD_TYPE,
                                                                 field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK))));
    }
}


DBSArray
TemplateTable::GetMatchingRows (const DBSBool&  min,
                          const DBSBool&  max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSChar&  min,
                          const DBSChar&  max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSDate&  min,
                          const DBSDate&  max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSDateTime&  min,
                          const DBSDateTime&  max,
                          const D_UINT64      fromRow,
                          const D_UINT64      toRow,
                          const D_UINT64      ignoreFirst,
                          const D_UINT64      maxCount,
                          const D_UINT        fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSHiresTime& min,
                          const DBSHiresTime& max,
                          const D_UINT64      fromRow,
                          const D_UINT64      toRow,
                          const D_UINT64      ignoreFirst,
                          const D_UINT64      maxCount,
                          const D_UINT        fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSUInt8& min,
                          const DBSUInt8& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSUInt16& min,
                          const DBSUInt16& max,
                          const D_UINT64   fromRow,
                          const D_UINT64   toRow,
                          const D_UINT64   ignoreFirst,
                          const D_UINT64   maxCount,
                          const D_UINT     fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSUInt32& min,
                          const DBSUInt32& max,
                          const D_UINT64   fromRow,
                          const D_UINT64   toRow,
                          const D_UINT64   ignoreFirst,
                          const D_UINT64   maxCount,
                          const D_UINT     fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSUInt64& min,
                          const DBSUInt64& max,
                          const D_UINT64   fromRow,
                          const D_UINT64   toRow,
                          const D_UINT64   ignoreFirst,
                          const D_UINT64   maxCount,
                          const D_UINT     fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSInt8& min,
                          const DBSInt8& max,
                          const D_UINT64 fromRow,
                          const D_UINT64 toRow,
                          const D_UINT64 ignoreFirst,
                          const D_UINT64 maxCount,
                          const D_UINT   fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSInt16& min,
                          const DBSInt16& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSInt32& min,
                          const DBSInt32& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSInt64& min,
                          const DBSInt64& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSReal& min,
                          const DBSReal& max,
                          const D_UINT64 fromRow,
                          const D_UINT64 toRow,
                          const D_UINT64 ignoreFirst,
                          const D_UINT64 maxCount,
                          const D_UINT   fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
TemplateTable::GetMatchingRows (const DBSRichReal& min,
                          const DBSRichReal& max,
                          const D_UINT64     fromRow,
                          const D_UINT64     toRow,
                          const D_UINT64     ignoreFirst,
                          const D_UINT64     maxCount,
                          const D_UINT       fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

template <class T> void
TemplateTable::StoreEntry (const T& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{

  //Check if we are trying to write a different value
  T currentValue;
  RetrieveEntry (currentValue, rowIndex, fieldIndex);

  if (currentValue == rSource)
    return; //Nothing to change

  const D_UINT8      bitsSet  = ~0;
  FieldDescriptor& field    = GetFieldDescriptorInternal (fieldIndex);
  const D_UINT       byte_off = field.m_NullBitIndex / 8;
  const D_UINT8      bit_off  = field.m_NullBitIndex % 8;

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem     cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData   = cachedItem.GetDataForUpdate();

  assert (field.m_NullBitIndex > 0);

  if (rSource.IsNull ())
    {
      assert ((pRawData [byte_off] & (1 << bit_off)) == 0);

      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (rowIndex);
    }
  else
    {
      if (pRawData [byte_off] == bitsSet)
        CheckRowToReuse (rowIndex);

      pRawData [byte_off] &= ~(1 << bit_off);

      PSValInterp::Store (rSource, pRawData + field.m_StoreIndex);
    }

  syncHolder.Leave ();

  //Update the field index if it exists
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    {
      NODE_INDEX        dummyNode;
      KEY_INDEX         dummyKey;

      AquireIndexField (&field);

      try
        {
          BTree fieldIndexTree (*m_vIndexNodeMgrs[fieldIndex]);

          fieldIndexTree.RemoveKey (T_BTreeKey<T> (currentValue, rowIndex));
          fieldIndexTree.InsertKey (T_BTreeKey<T> (rSource, rowIndex), dummyNode, dummyKey);
        }
      catch (...)
        {
          ReleaseIndexField (&field);
          throw ;
        }
      ReleaseIndexField (&field);
    }
}

template <class T> void
TemplateTable::RetrieveEntry (T& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, _SC(DBS_FIELD_TYPE, rDestination))))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  T *const      pDest    = &rDestination;
  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem           cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8 *const pRawData   = cachedItem.GetDataForRead();

  if (pRawData[byte_off] & (1 << bit_off))
    {
      pDest->~T ();
      _placement_new (pDest, T ());
    }
  else
    PSValInterp::Retrieve (pDest, pRawData + field.m_StoreIndex);
}


template <class T> DBSArray
TemplateTable::MatchRowsWithIndex (const D_UINT   fieldIndex,
                             const T&       min,
                             const T&       max,
                             const D_UINT64 fromRow,
                             D_UINT64       toRow,
                             D_UINT64       ignoreFirst,
                             D_UINT64       maxCount)
{
  FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, _SC(DBS_FIELD_TYPE, min))))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  toRow = MIN (toRow, ((m_RowsCount > 0) ? m_RowsCount - 1 : 0));

  FieldIndexNodeManager *const pNodeMgr = m_vIndexNodeMgrs[fieldIndex];
  DBSArray                     result (_SC (DBSUInt64*, NULL));
  NODE_INDEX                   node;
  KEY_INDEX                    key;
  const T_BTreeKey<T>          firstKey (min, fromRow);
  const T_BTreeKey<T>          lastKey (max, MIN (toRow, m_RowsCount));

  assert (pNodeMgr != NULL);

  AquireIndexField (&field);

  try
    {
      BTree fieldIndexTree (*pNodeMgr);

      if ( (fieldIndexTree.FindBiggerOrEqual (firstKey, node, key) == false) ||
           (maxCount == 0))
        goto force_return;

      BTreeNodeHandler currentNode (pNodeMgr->RetrieveNode (node));

      while (ignoreFirst > 0)
        {
          if (ignoreFirst <= key)
            {
              key         -= ignoreFirst;
              ignoreFirst = 0;
            }
          else
            {
              NODE_INDEX nextNode = currentNode->GetNext ();

              if (nextNode == NIL_NODE)
                goto force_return;

              node        = nextNode;
              currentNode = pNodeMgr->RetrieveNode (node);
              assert (currentNode->GetKeysCount() > 0);

              ignoreFirst -= (key + 1);
              key          = currentNode->GetKeysCount () - 1;
            }
        }


      assert (key < currentNode->GetKeysCount ());

      while (true)
        {
          I_BTreeFieldIndexNode* pNode    = _SC (I_BTreeFieldIndexNode*, &*currentNode);
          KEY_INDEX              toKey    = ~0;;
          bool                   lastNode = false;

          if (pNode->FindBiggerOrEqual (lastKey, toKey) == false)
            toKey = 0;
          else
            {
              lastNode = true;
              toKey ++;
            }

          assert (key >= toKey);

          if (maxCount <= (key - toKey + 1))
            {
              toKey += maxCount;
              maxCount = 0;
            }
          else
            maxCount -= (key - toKey + 1);

          pNode->GetKeysRows (result, key, toKey);

          if (lastNode || (pNode->GetNext() == NIL_NODE) || (maxCount == 0))
            break;
          else
            {
              currentNode = pNodeMgr->RetrieveNode (pNode->GetNext ());

              assert (currentNode->GetKeysCount() > 0);
              key = currentNode->GetKeysCount () - 1;
            }
        }
    }
  catch (...)
    {
      ReleaseIndexField (&field);
      throw;
    }

force_return:
  ReleaseIndexField (&field);
  return result;
}


template <class T> DBSArray
TemplateTable::MatchRows (const T&       min,
                    const T&       max,
                    const D_UINT64 fromRow,
                    D_UINT64       toRow,
                    D_UINT64       ignoreFirst,
                    D_UINT64       maxCount,
                    const D_UINT   fieldIndex)
{
  toRow = MIN (toRow, ((m_RowsCount > 0) ? m_RowsCount - 1 : 0));

  DBSArray result (_SC (DBSUInt64*, NULL));
  T        rowValue;

  for (D_UINT64 rowIndex = fromRow; (rowIndex <= toRow) && (maxCount > 0); ++rowIndex)
    {
      GetEntry (rowValue, fieldIndex, rowIndex);

      if ( (rowValue < min) || (max < rowValue))
        continue;

      if (ignoreFirst > 0)
        {
          --ignoreFirst;
          continue;
        }

      --maxCount;

      result.AddElement (DBSUInt64 (rowIndex));
    }

  return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

TableRmNode::TableRmNode (TemplateTable &table, const NODE_INDEX nodeId) :
    I_BTreeNode (table),
    m_cpNodeData (new D_UINT8 [table.GetRawNodeSize ()])
{
  m_Header           = _RC ( NodeHeader*, m_cpNodeData.get ());
  m_Header->m_NodeId = nodeId;

  SetNullKeysCount (0);

  assert ((sizeof (NodeHeader) % ( 2 * sizeof  (D_UINT64)) == 0));
  assert (RAW_NODE_SIZE == m_NodesManager.GetRawNodeSize ());
}

TableRmNode::~TableRmNode ()
{
}

D_UINT
TableRmNode::GetKeysPerNode () const
{
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

  D_UINT result = m_NodesManager.GetRawNodeSize () - sizeof (NodeHeader);

  if (IsLeaf ())
    result /= sizeof (D_UINT64);
  else
    result /= sizeof (D_UINT64) + sizeof (NODE_INDEX);

  return result;
}

KEY_INDEX
TableRmNode::GetFirstKey (const I_BTreeNode& parent) const
{
  assert (GetKeysCount() > 0);

  KEY_INDEX               result;
  const NODE_INDEX *const pKeys = _RC ( const NODE_INDEX*, GetRawData () + sizeof (NodeHeader));

  const TableRmKey key (pKeys[0]);

  parent.FindBiggerOrEqual (key, result);

  assert (parent.IsEqual (key, result));
  assert (GetNodeId () == parent.GetChildNode (result));

  return result;
}

NODE_INDEX
TableRmNode::GetChildNode (const KEY_INDEX keyIndex) const
{
  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
  assert (IsLeaf () == false);

  const D_UINT firstKeyOff = sizeof (NodeHeader) + TableRmNode::GetKeysPerNode() * sizeof (D_UINT64);
  const NODE_INDEX *const cpFirstKey = _RC (const NODE_INDEX *, GetRawData () + firstKeyOff);

  return cpFirstKey [keyIndex];
}

void
TableRmNode::ResetKeyNode (const I_BTreeNode &childNode, const KEY_INDEX keyIndex)
{
  assert (childNode.GetNodeId () == GetChildNode (keyIndex));

  const TableRmNode &child       = _SC (const TableRmNode&, childNode);
  NODE_INDEX *const pKeys          = _RC (NODE_INDEX*, GetRawData () + sizeof (NodeHeader));
  const NODE_INDEX *const pSrcKeys = _RC ( const NODE_INDEX*,
                                           child.GetRawData () + sizeof (NodeHeader));

  pKeys [keyIndex] = pSrcKeys [0];
}

void
TableRmNode::SetChildNode (const KEY_INDEX keyIndex, const NODE_INDEX childNode)
{

  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
  assert (IsLeaf () == false);

  const D_UINT firstKeyOff = sizeof (NodeHeader) + TableRmNode::GetKeysPerNode() * sizeof (D_UINT64);
  NODE_INDEX *const cpFirstKey = _RC (NODE_INDEX *, GetRawData () + firstKeyOff);

  cpFirstKey [keyIndex] = childNode;
}

KEY_INDEX
TableRmNode::InsertKey (const I_BTreeKey& key)
{
  if (GetKeysCount () > 0)
    {
      KEY_INDEX keyIndex;
      KEY_INDEX lastKey = GetKeysCount () - 1;

      assert (lastKey < (GetKeysPerNode() - 1));

      if (FindBiggerOrEqual (key, keyIndex) == false)
        keyIndex = 0;
      else
        keyIndex ++;

      assert ((keyIndex == 0) || IsLess (key, keyIndex - 1));

      D_UINT64* const pRows = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));

      make_array_room (pRows, lastKey, keyIndex, 1);
      SetKeysCount (GetKeysCount() + 1);
      pRows[keyIndex] = *( _SC(const TableRmKey *, &key));

      if (IsLeaf () == false)
        {
          NODE_INDEX *const pNodes = _RC (NODE_INDEX*, pRows + TableRmNode::GetKeysPerNode());
          make_array_room (pNodes, lastKey, keyIndex, 1);
        }

      return keyIndex;
    }

  D_UINT64* const pRows = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  pRows [0]             = *( _SC(const TableRmKey *, &key));

  SetKeysCount (1);

  return 0;
}

void
TableRmNode::RemoveKey (const KEY_INDEX keyIndex)
{
  D_UINT lastKey = GetKeysCount () - 1;

  assert (lastKey < (GetKeysPerNode() - 1));

  D_UINT64* const pRows = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  remove_array_elemes (pRows, lastKey, keyIndex, 1);

  if (IsLeaf () == false)
    {
      NODE_INDEX *const pNodes = _RC (NODE_INDEX*, pRows + TableRmNode::GetKeysPerNode());
      remove_array_elemes (pNodes, lastKey, keyIndex, 1);
    }

  SetKeysCount (GetKeysCount () - 1);
}

void
TableRmNode::Split ( const NODE_INDEX parentId)
{
  assert (NeedsSpliting ());

  D_UINT64* const  pRows    = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const KEY_INDEX  splitKey = GetKeysCount() / 2;

  BTreeNodeHandler   parentNode (m_NodesManager.RetrieveNode (parentId));
  const TableRmKey key (pRows[splitKey]);
  const KEY_INDEX    insertionPos    = parentNode->InsertKey (key);
  const NODE_INDEX   allocatedNodeId = m_NodesManager.AllocateNode (parentId, insertionPos);
  BTreeNodeHandler   allocatedNode (m_NodesManager.RetrieveNode (allocatedNodeId));

  allocatedNode->SetLeaf (IsLeaf ());
  allocatedNode->MarkAsUsed();


  TableRmNode *const pRawAllocNode = _SC (TableRmNode*, &(*allocatedNode));
  D_UINT64* const pSplitRows         = _RC (D_UINT64*,
                                            pRawAllocNode->GetRawData () + sizeof (NodeHeader));

  for (D_UINT index = splitKey; index < GetKeysCount (); ++index)
    pSplitRows [index - splitKey] = pRows [index];

  if (IsLeaf () == false)
    {
      NODE_INDEX* const pNodes      = _RC (NODE_INDEX*, pRows + GetKeysPerNode ());
      NODE_INDEX* const pSplitNodes = _RC (NODE_INDEX*, pSplitRows + GetKeysPerNode ());

      for (D_UINT index = splitKey; index < GetKeysCount (); ++index)
        pSplitNodes [index - splitKey] = pNodes [index];
    }

  allocatedNode->SetKeysCount (GetKeysCount() - splitKey);
  SetKeysCount (splitKey);

  allocatedNode->SetNext (GetNodeId());
  allocatedNode->SetPrev (GetPrev());
  SetPrev (allocatedNodeId);
  if (allocatedNode->GetPrev() != NIL_NODE)
    {
      BTreeNodeHandler prevNode (m_NodesManager.RetrieveNode (allocatedNode->GetPrev()));
      prevNode->SetNext (allocatedNodeId);
    }
}

void
TableRmNode::Join (bool toRight)
{
  assert (NeedsJoining ());

  D_UINT64 *const   pRows  = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  NODE_INDEX *const pNodes = _RC (NODE_INDEX *, pRows + TableRmNode::GetKeysPerNode ());

  if (toRight)
    {
      assert (GetNext () != NIL_NODE);
      BTreeNodeHandler     nextNode (m_NodesManager.RetrieveNode (GetNext ()));
      TableRmNode *const pNextNode = _SC (TableRmNode*, &(*nextNode));
      D_UINT64 *const      pDestRows = _RC (D_UINT64 *, pNextNode->GetRawData () + sizeof (NodeHeader));

      for (D_UINT index = 0; index < GetKeysCount (); ++index)
        pDestRows [index + pNextNode->GetKeysCount ()] = pRows [index];

      if (IsLeaf () == false)
        {
          NODE_INDEX *const pDestNodes = _RC (NODE_INDEX*,
                                              pDestRows + TableRmNode::GetKeysPerNode ());

          for (D_UINT index = 0; index < GetKeysCount(); ++index)
            pDestNodes [index + pNextNode->GetKeysCount ()] = pNodes [index];
        }

      nextNode->SetKeysCount (nextNode->GetKeysCount () + GetKeysCount ());
      nextNode->SetPrev (GetPrev ());

      if (GetPrev () != NIL_NODE)
        {
          BTreeNodeHandler prevNode (m_NodesManager.RetrieveNode (GetPrev ()));
          prevNode->SetNext (GetNext ());
        }
    }
  else
    {
      assert (GetPrev () != NIL_NODE);

      BTreeNodeHandler     prevNode (m_NodesManager.RetrieveNode (GetPrev ()));
      TableRmNode *const pPrevNode = _SC (TableRmNode*, &(*prevNode));
      D_UINT64 *const      pSrcRows  = _RC (D_UINT64 *, pPrevNode->GetRawData () + sizeof (NodeHeader));

      for (D_UINT index = 0; index < prevNode->GetKeysCount(); ++index)
        pRows [index + GetKeysCount ()] = pSrcRows [index];

      if (IsLeaf () == false)
        {
          NODE_INDEX *const pSrcNodes = _RC (NODE_INDEX*,
                                             pSrcRows + TableRmNode::GetKeysPerNode ());

          for (D_UINT index = 0; index < prevNode->GetKeysCount (); ++index)
            pNodes [index + GetKeysCount ()] = pSrcNodes [index];
        }

      SetKeysCount (GetKeysCount () + prevNode->GetKeysCount());
      SetPrev (prevNode->GetPrev ());
      if (GetPrev () != NIL_NODE)
        {
          BTreeNodeHandler prevNode (m_NodesManager.RetrieveNode (GetPrev ()));
          prevNode->SetNext (GetNodeId ());
        }
    }
}

bool
TableRmNode::IsLess (const I_BTreeKey& key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const TableRmKey    tKey ( * _SC( const  TableRmKey*, &key));

  return tKey < pRows [keyIndex];
}

bool
TableRmNode::IsEqual (const I_BTreeKey& key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const TableRmKey    tKey ( * _SC( const  TableRmKey*, &key));

  return tKey == pRows [keyIndex];
}

bool
TableRmNode::IsBigger (const I_BTreeKey &key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const TableRmKey    tKey ( * _SC( const  TableRmKey*, &key));

  return tKey > pRows [keyIndex];
}

const I_BTreeKey&
TableRmNode::GetSentinelKey () const
{
  static TableRmKey _key (~0);

  return _key;
}





