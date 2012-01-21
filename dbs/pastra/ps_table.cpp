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

static const D_UINT PS_HEADER_SIZE = 128;

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
static const D_UINT PS_TABLE_BT_ROOT_OFF  = 32; //The root node of BTree holding the removed rows.
static const D_UINT PS_TABLE_BT_ROOT_LEN  = 4;
static const D_UINT PS_TABLE_BT_HEAD_OFF  = 36; //First node pointing to the removed BT nodes.
static const D_UINT PS_TABLE_BT_HEAD_LEN  = 4;
static const D_UINT PS_TABLE_ROW_SIZE_OFF = 40; //Size of a row.
static const D_UINT PS_TABLE_ROW_SIZE_LEN = 4;

static const D_UINT PS_RESEVED_FOR_FUTURE_OFF = 44;
static const D_UINT PS_RESEVED_FOR_FUTURE_LEN = PS_HEADER_SIZE - PS_RESEVED_FOR_FUTURE_OFF;

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
      result += strlen(pFields->m_pFieldName) + 1;
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
                                                      rvFields[schIndex].m_FieldType,
                                                      rvFields[schIndex].isArray);

	  D_INT currReqAlign = PSValInterp::GetAlignment(
                                                         rvFields[schIndex].m_FieldType,
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

      pFieldDesc[fieldIndex].m_NameOffset = get_strlens_till_index (&rvFields.front(), fieldIndex);
      pFieldDesc[fieldIndex].m_NameOffset += sizeof(PSFieldDescriptor) * rvFields.size();

      strcpy(_RC (D_CHAR *, pOutFieldsDescription + pFieldDesc[fieldIndex].m_NameOffset),
	     _RC (const D_CHAR *, temp.m_pFieldName));

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

static WFile
create_table_file(const string &baseFileName, const DBSFieldDescriptor *pFields, D_UINT fieldsCount)
{

  //Check the arguments
  if ((pFields == NULL) || (fieldsCount == 0) || (fieldsCount > 0xFFFFu))
    throw(DBSException(NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED)));

  //Compute the table header descriptor size
  const D_UINT32 descriptorsSize = sizeof(PSFieldDescriptor) * fieldsCount +
				   get_strlens_till_index(pFields, fieldsCount);

  //Validate the optimally rearrange the fields for minimum row size
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
  _RC (NODE_INDEX *, pBuffer + PS_TABLE_BT_ROOT_OFF)[0] = NIL_NODE;
  _RC (NODE_INDEX *, pBuffer + PS_TABLE_BT_HEAD_OFF)[0] = NIL_NODE;

  assert (sizeof (NODE_INDEX) == PS_TABLE_BT_HEAD_LEN);
  assert (sizeof (NODE_INDEX) == PS_TABLE_BT_ROOT_LEN);

  memset(pBuffer + PS_RESEVED_FOR_FUTURE_OFF, 0, PS_RESEVED_FOR_FUTURE_LEN);

  //Write the first header part!
  tableFile.Write(pBuffer, PS_HEADER_SIZE);

  //Write the field descriptors;
  tableFile.Write(apFieldDescription.get(), descriptorsSize);

  const D_UINT toFill = tableFile.Tell () % PSTableRmNode::RAW_NODE_SIZE;

  if (toFill != 0)
    {
      static const D_UINT8 dump [PSTableRmNode::RAW_NODE_SIZE] = {0,};
      assert ((tableFile.Tell() == tableFile.GetSize()));
      assert ((tableFile.Tell() < sizeof dump));

      tableFile.Write (dump, sizeof dump - toFill);


    }
  assert ((tableFile.Tell() == tableFile.GetSize()));

  return tableFile;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PSTableRmNode::PSTableRmNode (PSTable &table, const NODE_INDEX nodeId) :
    I_BTreeNode (table),
    m_cpNodeData (new D_UINT8 [table.GetRawNodeSize ()])
{
  m_Header           = _RC ( NodeHeader*, m_cpNodeData.get ());
  m_Header->m_NodeId = nodeId;

  SetNullKeysCount (0);

  assert ((sizeof (NodeHeader) % sizeof  (D_UINT64)) == 0);
  assert (RAW_NODE_SIZE == m_NodesManager.GetRawNodeSize ());
}

PSTableRmNode::~PSTableRmNode ()
{
}

D_UINT
PSTableRmNode::GetKeysPerNode () const
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
PSTableRmNode::GetFirstKey (const I_BTreeNode &parent) const
{
  assert (GetKeysCount() > 0);

  KEY_INDEX               result;
  const NODE_INDEX *const pKeys = _RC ( const NODE_INDEX*, GetRawData () + sizeof (NodeHeader));

  const PSTableRmKey key (pKeys[0]);

  parent.FindBiggerOrEqual (key, result);

  assert (parent.IsEqual (key, result));
  assert (GetNodeId () == parent.GetChildNode (result));

  return result;
}

NODE_INDEX
PSTableRmNode::GetChildNode (const KEY_INDEX keyIndex) const
{
  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
  assert (IsLeaf () == false);

  const D_UINT firstKeyOff = sizeof (NodeHeader) + PSTableRmNode::GetKeysPerNode() * sizeof (D_UINT64);
  const NODE_INDEX *const cpFirstKey = _RC (const NODE_INDEX *, GetRawData () + firstKeyOff);

  return cpFirstKey [keyIndex];
}

void
PSTableRmNode::ResetKeyNode (const I_BTreeNode &childNode, const KEY_INDEX keyIndex)
{
  assert (childNode.GetNodeId () == GetChildNode (keyIndex));

  const PSTableRmNode &child       = _SC (const PSTableRmNode&, childNode);
  NODE_INDEX *const pKeys          = _RC (NODE_INDEX*, GetRawData () + sizeof (NodeHeader));
  const NODE_INDEX *const pSrcKeys = _RC ( const NODE_INDEX*,
                                           child.GetRawData () + sizeof (NodeHeader));

  pKeys [keyIndex] = pSrcKeys [0];
}

void
PSTableRmNode::SetChildNode (const KEY_INDEX keyIndex, const NODE_INDEX childNode)
{

  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
  assert (IsLeaf () == false);

  const D_UINT firstKeyOff = sizeof (NodeHeader) + PSTableRmNode::GetKeysPerNode() * sizeof (D_UINT64);
  NODE_INDEX *const cpFirstKey = _RC (NODE_INDEX *, GetRawData () + firstKeyOff);

  cpFirstKey [keyIndex] = childNode;
}

KEY_INDEX
PSTableRmNode::InsertKey (const I_BTreeKey &key)
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
      pRows[keyIndex] = *( _SC(const PSTableRmKey *, &key));

      if (IsLeaf () == false)
        {
          NODE_INDEX *const pNodes = _RC (NODE_INDEX*, pRows + PSTableRmNode::GetKeysPerNode());
          make_array_room (pNodes, lastKey, keyIndex, 1);
        }

      return keyIndex;
    }

  D_UINT64* const pRows = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  pRows [0]             = *( _SC(const PSTableRmKey *, &key));

  SetKeysCount (1);

  return 0;
}

void
PSTableRmNode::RemoveKey (const KEY_INDEX keyIndex)
{
  D_UINT lastKey = GetKeysCount () - 1;

  assert (lastKey < (GetKeysPerNode() - 1));

  D_UINT64* const pRows = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  remove_array_elemes (pRows, lastKey, keyIndex, 1);

  if (IsLeaf () == false)
    {
      NODE_INDEX *const pNodes = _RC (NODE_INDEX*, pRows + PSTableRmNode::GetKeysPerNode());
      remove_array_elemes (pNodes, lastKey, keyIndex, 1);
    }

  SetKeysCount (GetKeysCount () - 1);
}

void
PSTableRmNode::Split ( const NODE_INDEX parentId)
{
  assert (NeedsSpliting ());

  D_UINT64* const  pRows    = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const KEY_INDEX  splitKey = GetKeysCount() / 2;

  BTreeNodeHandler   parentNode (m_NodesManager.RetrieveNode (parentId));
  const PSTableRmKey key (pRows[splitKey]);
  const KEY_INDEX    insertionPos    = parentNode->InsertKey (key);
  const NODE_INDEX   allocatedNodeId = m_NodesManager.AllocateNode (parentId, insertionPos);
  BTreeNodeHandler   allocatedNode (m_NodesManager.RetrieveNode (allocatedNodeId));

  allocatedNode->SetLeaf (IsLeaf ());
  allocatedNode->MarkAsUsed();


  PSTableRmNode *const pRawAllocNode = _SC (PSTableRmNode*, &(*allocatedNode));
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
PSTableRmNode::Join (bool toRight)
{
  assert (NeedsJoining ());

  D_UINT64 *const   pRows  = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  NODE_INDEX *const pNodes = _RC (NODE_INDEX *, pRows + PSTableRmNode::GetKeysPerNode ());

  if (toRight)
    {
      assert (GetNext () != NIL_NODE);
      BTreeNodeHandler     nextNode (m_NodesManager.RetrieveNode (GetNext ()));
      PSTableRmNode *const pNextNode = _SC (PSTableRmNode*, &(*nextNode));
      D_UINT64 *const      pDestRows = _RC (D_UINT64 *, pNextNode->GetRawData () + sizeof (NodeHeader));

      for (D_UINT index = 0; index < GetKeysCount (); ++index)
        pDestRows [index + pNextNode->GetKeysCount ()] = pRows [index];

      if (IsLeaf () == false)
        {
          NODE_INDEX *const pDestNodes = _RC (NODE_INDEX*,
                                              pDestRows + PSTableRmNode::GetKeysPerNode ());

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
      PSTableRmNode *const pPrevNode = _SC (PSTableRmNode*, &(*prevNode));
      D_UINT64 *const      pSrcRows  = _RC (D_UINT64 *, pPrevNode->GetRawData () + sizeof (NodeHeader));

      for (D_UINT index = 0; index < prevNode->GetKeysCount(); ++index)
        pRows [index + GetKeysCount ()] = pSrcRows [index];

      if (IsLeaf () == false)
        {
          NODE_INDEX *const pSrcNodes = _RC (NODE_INDEX*,
                                             pSrcRows + PSTableRmNode::GetKeysPerNode ());

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
PSTableRmNode::IsLess (const I_BTreeKey &key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const PSTableRmKey    tKey ( * _SC( const  PSTableRmKey*, &key));

  return tKey < pRows [keyIndex];
}

bool
PSTableRmNode::IsEqual (const I_BTreeKey &key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const PSTableRmKey    tKey ( * _SC( const  PSTableRmKey*, &key));

  return tKey == pRows [keyIndex];
}

bool
PSTableRmNode::IsBigger (const I_BTreeKey &key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const PSTableRmKey    tKey ( * _SC( const  PSTableRmKey*, &key));

  return tKey > pRows [keyIndex];
}

const I_BTreeKey&
PSTableRmNode::GetSentinelKey () const
{
  static PSTableRmKey _key (~0);

  return _key;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

PSTable::PSTable (DbsHandler & dbsHandler, const string & tableName) :
  m_RowsCount (0),
  m_RootNode (NIL_NODE),
  m_FirstUnallocatedRoot (NIL_NODE),
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

  assert (m_RootNode != NIL_NODE);

  m_RowCache.Init (m_RowSize, 4096, 1024);

  InitVariableStorages ();
  InitIndexedFields ();
}


PSTable::PSTable(DbsHandler & dbsHandler,
		 const string & tableName,
		 const DBSFieldDescriptor * pFields,
		 D_UINT fieldsCount) :
  m_RowsCount (0),
  m_RootNode (NIL_NODE),
  m_FirstUnallocatedRoot (NIL_NODE),
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

PSTable::~PSTable()
{
  if (m_ReferenceCount != 0)
    throw DBSException(NULL, _EXTRA (DBSException::TABLE_IN_USE));

  FlushNodes ();

  for (D_UINT fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex)
    if (m_vIndexNodeMgrs[fieldIndex] != NULL)
      {
        PSFieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

        D_UINT64 unitsCount = DBSGetMaxFileSize() - 1;

        unitsCount += m_vIndexNodeMgrs[fieldIndex]->GetIndexRawSize();
        unitsCount /= DBSGetMaxFileSize ();

        field.m_IndexUnitsCount = unitsCount;
        delete m_vIndexNodeMgrs [fieldIndex];
      }
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

  result.isArray      = (pDesc[fieldIndex].m_TypeDesc & PS_TABLE_ARRAY_MASK) != 0;
  result.m_FieldType  = _SC (DBS_FIELD_TYPE, pDesc[fieldIndex].m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK);
  result.m_pFieldName = _RC (const D_CHAR *, pDesc) + pDesc[fieldIndex].m_NameOffset;

  return result;
}

DBSFieldDescriptor
PSTable::GetFieldDescriptor(const D_CHAR * const pFieldName)
{

  const PSFieldDescriptor * const pDesc = _RC(const PSFieldDescriptor*, m_FieldsDescriptors.get ());
  D_UINT iterator = m_FieldsCount * sizeof(PSFieldDescriptor);

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
PSTable::GetAllocatedRows()
{
  return m_RowsCount;
}

D_UINT64
PSTable::AddRow()
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

  removedRows.InsertKey (PSTableRmKey (m_RowsCount), dummyNode, dummyKey);

  IncreaseRowCount ();

  assert (m_RowsCount > 0);

  m_RowCache.ForceItemUpdate (m_RowsCount - 1);

  return m_RowsCount - 1;
}

D_UINT64
PSTable::AddReusedRow ()
{
  D_UINT64     result = 0;
  NODE_INDEX   node;
  KEY_INDEX    keyIndex;
  PSTableRmKey key (0);
  BTree        removedRows (*this);

  if (removedRows.FindBiggerOrEqual(key, node, keyIndex) == false)
    result = PSTable::AddRow ();
  else
    {
      BTreeNodeHandler keyNode (RetrieveNode (node));
      PSTableRmNode *const pNode = _SC (PSTableRmNode *, &(*keyNode));

      assert (keyNode->IsLeaf() );
      assert (keyIndex < keyNode->GetKeysCount());

      const D_UINT64 *const pRows = _RC (D_UINT64*,
                                         pNode->GetRawData () + sizeof (PSTableRmNode::NodeHeader));
      result = pRows [keyIndex];
    }

  assert (result > 0);

  return result;
}

void
PSTable::MarkRowForReuse(D_UINT64 rowIndex)
{
  D_UINT fieldsCount = m_FieldsCount;

  StoredItem cachedItem (m_RowCache.RetriveItem (rowIndex));
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

    while (fieldsCount-- > 0)
      {
        const PSFieldDescriptor& field    = GetFieldDescriptorInternal (fieldsCount);
        D_UINT                   byte_off = field.m_NullBitIndex / 8;
        D_UINT8                  bit_off  = field.m_NullBitIndex % 8;

        pRawData[byte_off] |= (1 << bit_off);
      }

    NODE_INDEX   node;
    KEY_INDEX    keyIndex;
    PSTableRmKey key (rowIndex);
    BTree        removedRows (*this);

    removedRows.InsertKey (key, node, keyIndex);
}

template <class T> static void
insert_row_field (PSTable &table,
                  BTree &tree,
                  const D_UINT64 rowIndex,
                  const D_UINT fieldIndex)
{
  T rowValue (true);
  table.GetEntry (rowValue, rowIndex, fieldIndex);


  NODE_INDEX dummyNode;
  KEY_INDEX  dummyKey;

  T_BTreeKey<T> keyValue (rowValue, rowIndex);
  tree.InsertKey (keyValue, dummyNode, dummyKey);
}

void
PSTable::CreateFieldIndex (const D_UINT fieldIndex,
                           CREATE_INDEX_CALLBACK_FUNC *const cb_func,
                           CallBackIndexData *const pCbData)
{
  if (PSTable::IsFieldIndexed(fieldIndex))
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_INDEXED));

  if ((cb_func == NULL) && (pCbData != NULL))
    throw DBSException (NULL, _EXTRA (DBSException::INVALID_PARAMETERS));

  PSFieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc == T_TEXT) ||
      (field.m_TypeDesc == T_RICHREAL) ||
      (field.m_TypeDesc == T_REAL) ||
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
PSTable::RemoveFieldIndex (const D_UINT fieldIndex)
{
  if (PSTable::IsFieldIndexed(fieldIndex) == false)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_NOT_INDEXED));

  PSFieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

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
PSTable::IsFieldIndexed (const D_UINT fieldIndex) const
{
  if (fieldIndex >= m_FieldsCount)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  assert (m_vIndexNodeMgrs.size () == m_FieldsCount);

  return (m_vIndexNodeMgrs[fieldIndex] != NULL);
}

D_UINT
PSTable::GetRowSize() const
{
  return m_RowSize;
}

PSFieldDescriptor&
PSTable::GetFieldDescriptorInternal(D_UINT fieldIndex) const
{
  PSFieldDescriptor* const pDesc = _RC(PSFieldDescriptor*, m_FieldsDescriptors.get ());

  if (fieldIndex >= m_FieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  return pDesc[fieldIndex];
}

PSFieldDescriptor&
PSTable::GetFieldDescriptorInternal(const D_CHAR * const pFieldName) const
{

  PSFieldDescriptor* const pDesc    = _RC (PSFieldDescriptor*, m_FieldsDescriptors.get ());
  D_UINT                   iterator = m_FieldsCount * sizeof(PSFieldDescriptor);

  for (D_UINT index = 0; index < m_FieldsCount; ++index)
    {
      if (strcmp ( _RC(const D_CHAR *, m_FieldsDescriptors.get() + iterator), pFieldName) == 0)
	return pDesc[index];

      iterator += strlen( _RC(const D_CHAR*, pDesc) + iterator) + 1;
    }

  throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));
}

D_UINT
PSTable::GetRawNodeSize () const
{
  return PSTableRmNode::RAW_NODE_SIZE;
}

NODE_INDEX
PSTable::AllocateNode (const NODE_INDEX parent, KEY_INDEX parentKey)
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
      assert (m_MainTableFile.GetSize () % GetRawNodeSize () == 0);

      nodeIndex = m_MainTableFile.GetSize () / GetRawNodeSize ();
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
PSTable::FreeNode (const NODE_INDEX nodeId)
{
  BTreeNodeHandler node (RetrieveNode (nodeId));

  node->MarkAsRemoved();
  node->SetNext (m_FirstUnallocatedRoot);

  m_FirstUnallocatedRoot = node->GetNodeId();
  SyncToFile ();
}

NODE_INDEX
PSTable::GetRootNodeId () const
{
  return m_RootNode;
}

void
PSTable::SetRootNodeId (const NODE_INDEX node)
{
  m_RootNode = node;
  SyncToFile ();
}


D_UINT
PSTable::GetMaxCachedNodes ()
{
  return 128;
}

I_BTreeNode*
PSTable::GetNode (const NODE_INDEX node)
{
  std::auto_ptr <PSTableRmNode> apNode (new PSTableRmNode (*this, node));

  assert (m_MainTableFile.GetSize() % GetRawNodeSize () == 0);

  if (m_MainTableFile.GetSize() > node * GetRawNodeSize ())
    {
      m_MainTableFile.Seek (apNode->GetNodeId() * GetRawNodeSize (), WHC_SEEK_BEGIN);
      m_MainTableFile.Read (apNode->GetRawData (), GetRawNodeSize ());
    }
  else
    {
      //Reserve space for this node
      assert (m_MainTableFile.GetSize() == (node * GetRawNodeSize ()));
      m_MainTableFile.Seek (0, WHC_SEEK_END);
      m_MainTableFile.Write (apNode->GetRawData (), GetRawNodeSize ());
    }

  return apNode.release ();
}


void
PSTable::StoreNode (I_BTreeNode *const pNode)
{
  if (pNode->IsDirty() == false)
    return ;

  m_MainTableFile.Seek (pNode->GetNodeId() * GetRawNodeSize (), WHC_SEEK_BEGIN);
  m_MainTableFile.Write (pNode->GetRawData(), GetRawNodeSize ());

  return;
}

void
PSTable::StoreItems (const D_UINT8 *pSrcBuffer,
                     D_UINT64 firstItem,
                     D_UINT itemsCount)
{
  m_apFixedFields.get()->StoreData( firstItem * m_RowSize, itemsCount * m_RowSize, pSrcBuffer);
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
PSTable::InitIndexedFields ()
{
  for (D_UINT fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex)
    {
      PSFieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

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

        assert ((fieldDesc.m_FieldType > T_UNKNOWN) && (fieldDesc.m_FieldType < T_UNDETERMINED));

        if (fieldDesc.isArray || (fieldDesc.m_FieldType == T_TEXT))
          {
            m_apVariableFields.reset (new VaribaleLenghtStore ());
            m_apVariableFields->Init ((m_BaseFileName + PS_TABLE_VARFIELDS_EXT).c_str(),
                                      m_VariableStorageSize,
                                      maxFileSize);
            break; //We finished here!
          }
    }
}

void
PSTable::SyncToFile ()
{

  if (m_Removed)
    return ; //We were removed. We were removed.

  D_UINT8 aTableHdr[PS_HEADER_SIZE];

  memcpy (aTableHdr, PS_TABLE_SIGNATURE, sizeof PS_TABLE_SIGNATURE);

  _RC (D_UINT32 *, aTableHdr + PS_TABLE_FIELDS_COUNT_OFF)[0] = m_FieldsCount;
  _RC (D_UINT32 *, aTableHdr + PS_TABLE_ELEMS_SIZE_OFF)[0] = m_DescriptorsSize;
  _RC (D_UINT64 *, aTableHdr + PS_TABLE_RECORDS_COUNT_OFF)[0] = m_RowsCount;
  _RC (D_UINT64 *, aTableHdr + PS_TABLE_VARSTORAGE_SIZE_OFF)[0] =
      (m_apVariableFields.get() != NULL) ? m_apVariableFields->GetRawSize() : 0;
  _RC (D_UINT32 *, aTableHdr + PS_TABLE_ROW_SIZE_OFF)[0] = m_RowSize;
  _RC (NODE_INDEX *, aTableHdr + PS_TABLE_BT_ROOT_OFF)[0] = m_RootNode;
  _RC (NODE_INDEX *, aTableHdr + PS_TABLE_BT_HEAD_OFF)[0] = m_FirstUnallocatedRoot;


  memset(aTableHdr + PS_RESEVED_FOR_FUTURE_OFF, 0, PS_RESEVED_FOR_FUTURE_LEN);

  m_MainTableFile.Seek (0, WHC_SEEK_BEGIN);
  m_MainTableFile.Write (aTableHdr, sizeof aTableHdr);
  m_MainTableFile.Write (m_FieldsDescriptors.get (), m_DescriptorsSize);
}

void
PSTable::InitFromFile()
{
  D_UINT8 aTableHdr[PS_HEADER_SIZE];

  m_MainTableFile.Seek(0, WHC_SEEK_BEGIN);
  m_MainTableFile.Read(aTableHdr, PS_HEADER_SIZE);

  if (memcmp(aTableHdr, PS_TABLE_SIGNATURE, PS_TABLES_SIG_lEN) != 0)
    throw DBSException(NULL, _EXTRA (DBSException::TABLE_INVALID));

  //Retrieve the header information.
  m_FieldsCount = _RC (D_UINT32 *, aTableHdr + PS_TABLE_FIELDS_COUNT_OFF)[0];
  m_DescriptorsSize = _RC (D_UINT32 *, aTableHdr + PS_TABLE_ELEMS_SIZE_OFF)[0];
  m_RowsCount = _RC (D_UINT64 *, aTableHdr + PS_TABLE_RECORDS_COUNT_OFF)[0];
  m_VariableStorageSize = _RC (D_UINT64 *, aTableHdr + PS_TABLE_VARSTORAGE_SIZE_OFF)[0];
  m_RowSize = _RC (D_UINT32 *, aTableHdr + PS_TABLE_ROW_SIZE_OFF)[0];
  m_RootNode = _RC (NODE_INDEX *, aTableHdr + PS_TABLE_BT_ROOT_OFF)[0];
  m_FirstUnallocatedRoot = _RC (NODE_INDEX *, aTableHdr + PS_TABLE_BT_HEAD_OFF)[0];

  //TODO: You need to add code to check for a proper shutdown
  //TODO: You need extra check on removing nodes

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

  for (D_UINT fieldIndex = 0; fieldIndex < m_FieldsCount; ++fieldIndex )
    if (m_vIndexNodeMgrs[fieldIndex]  != NULL)
      m_vIndexNodeMgrs[fieldIndex]->MarkForRemoval ();

  m_MainTableFile.Close();
  whc_fremove(m_BaseFileName.c_str());

  m_Removed = true;
}

void
PSTable::CheckRowToDelete (const D_UINT64 rowIndex)
{
  bool isRowDeleted = true;

  StoredItem cachedItem   = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

  for (D_UINT64 index = 0; index < m_FieldsCount; index += 8)
    {
      const PSFieldDescriptor& fieldDesc = GetFieldDescriptorInternal (index);
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
      PSTableRmKey key (rowIndex);

      removedNodes.InsertKey (key, dumyNode, dummyKey);
    }
}

void
PSTable::CheckRowToReuse (const D_UINT64 rowIndex)
{
  bool wasRowDeleted = true;

  StoredItem cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

  for (D_UINT64 index = 0; index < m_FieldsCount; index += 8)
    {
      const PSFieldDescriptor& fieldDesc = GetFieldDescriptorInternal (index);
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
      PSTableRmKey key (rowIndex);

      removedNodes.RemoveKey (key);
    }
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
PSTable::SetEntry (const DBSHiresTime &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
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
  const PSFieldDescriptor& field             = GetFieldDescriptorInternal (fieldIndex);
  const D_UINT8            bitsSet           = ~0;
  bool                     fieldValueWasNull = false;


  StoredItem cachedItem   = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

  D_UINT64 *const fieldFirstEntry = _RC (D_UINT64*, pRawData + field.m_StoreIndex + 0);
  D_UINT64 *const fieldValueSize  = _RC (D_UINT64*,
                                         pRawData + field.m_StoreIndex + sizeof (D_UINT64));
  D_UINT64 newFirstEntry     = 0;
  D_UINT64 newFieldValueSize = 0;

  assert (field.m_NullBitIndex > 0);

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  if ((pRawData [byte_off] & (1 << bit_off)) != 0)
    fieldValueWasNull = true;

  if ( (fieldValueWasNull == false) && rSource.IsNull())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (rowIndex);
    }
  else if (rSource.IsNull() == false)
    {
      if (pRawData [byte_off] == bitsSet)
        {
          assert (fieldValueWasNull == true);
          CheckRowToReuse (rowIndex);
        }
      pRawData [byte_off] &= ~(1 << bit_off);

      I_TextStrategy &text = rSource;
      if (text.IsRowValue())
        {
          RowFieldText& value = text.GetRowValue();
          newFieldValueSize   = value.m_BytesSize;
          newFirstEntry       = m_apVariableFields->AddRecord (rowIndex,
                                                               value.m_Storage,
                                                               value.m_FirstEntry,
                                                               0,
                                                               value.m_BytesSize);
        }
      else
        {
          TemporalText& value = text.GetTemporal();
          newFieldValueSize   = value.m_BytesSize;
          newFirstEntry       = m_apVariableFields->AddRecord (rowIndex,
                                                               value.m_Storage,
                                                               0,
                                                               newFieldValueSize);
        }
    }

  if (fieldValueWasNull == false)
    m_apVariableFields->RemoveRecord (*fieldFirstEntry);

  *fieldFirstEntry = newFirstEntry;
  *fieldValueSize  = newFieldValueSize;
}

void
PSTable::SetEntry (const DBSArray &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const D_UINT8 bitsSet           = ~0;
  bool          fieldValueWasNull = false;

  const PSFieldDescriptor& field  = GetFieldDescriptorInternal (fieldIndex);

  StoredItem cachedItem           = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData         = cachedItem.GetDataForUpdate();

  D_UINT64 *const fieldFirstEntry = _RC (D_UINT64*, pRawData + field.m_StoreIndex + 0);
  D_UINT64 *const fieldValueSize  = _RC (D_UINT64*,
                                         pRawData + field.m_StoreIndex + sizeof (D_UINT64));
  D_UINT64 newFirstEntry     = 0;
  D_UINT64 newFieldValueSize = 0;

  assert (field.m_NullBitIndex > 0);

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  if ((pRawData [byte_off] & (1 << bit_off)) != 0)
    fieldValueWasNull = true;

  if ( (fieldValueWasNull == false) && rSource.IsNull())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (rowIndex);
    }
  else if (rSource.IsNull() == false)
    {
      if (pRawData [byte_off] == bitsSet)
        {
          assert (fieldValueWasNull == true);
          CheckRowToReuse (rowIndex);
        }
      pRawData [byte_off] &= ~(1 << bit_off);

      I_ArrayStrategy &array = rSource;
      if (array.IsRowValue())
        {
          RowFieldArray& value = array.GetRowValue();
          newFieldValueSize    = value.GetRawDataSize();
          newFirstEntry        = m_apVariableFields->AddRecord (rowIndex,
                                                                value.m_Storage,
                                                                value.m_FirstRecordEntry,
                                                                0,
                                                                newFieldValueSize);
        }
      else
        {
          TemporalArray& value = array.GetTemporal ();
          newFieldValueSize    = value.GetRawDataSize();

          const D_UINT64 elemsCount = value.GetElementsCount();
          newFirstEntry             = m_apVariableFields->AddRecord (rowIndex,
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
  *fieldValueSize  = newFieldValueSize;
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
PSTable::GetEntry (DBSHiresTime &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
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


DBSArray
PSTable::GetMatchingRows (const DBSBool&  min,
                          const DBSBool&  max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSChar&  min,
                          const DBSChar&  max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSDate&  min,
                          const DBSDate&  max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSDateTime&  min,
                          const DBSDateTime&  max,
                          const D_UINT64      fromRow,
                          const D_UINT64      toRow,
                          const D_UINT64      ignoreFirst,
                          const D_UINT64      maxCount,
                          const D_UINT        fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSHiresTime& min,
                          const DBSHiresTime& max,
                          const D_UINT64      fromRow,
                          const D_UINT64      toRow,
                          const D_UINT64      ignoreFirst,
                          const D_UINT64      maxCount,
                          const D_UINT        fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSUInt8& min,
                          const DBSUInt8& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSUInt16& min,
                          const DBSUInt16& max,
                          const D_UINT64   fromRow,
                          const D_UINT64   toRow,
                          const D_UINT64   ignoreFirst,
                          const D_UINT64   maxCount,
                          const D_UINT     fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSUInt32& min,
                          const DBSUInt32& max,
                          const D_UINT64   fromRow,
                          const D_UINT64   toRow,
                          const D_UINT64   ignoreFirst,
                          const D_UINT64   maxCount,
                          const D_UINT     fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSUInt64& min,
                          const DBSUInt64& max,
                          const D_UINT64   fromRow,
                          const D_UINT64   toRow,
                          const D_UINT64   ignoreFirst,
                          const D_UINT64   maxCount,
                          const D_UINT     fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSInt8& min,
                          const DBSInt8& max,
                          const D_UINT64 fromRow,
                          const D_UINT64 toRow,
                          const D_UINT64 ignoreFirst,
                          const D_UINT64 maxCount,
                          const D_UINT   fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSInt16& min,
                          const DBSInt16& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSInt32& min,
                          const DBSInt32& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PSTable::GetMatchingRows (const DBSInt64& min,
                          const DBSInt64& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (m_vIndexNodeMgrs[fieldIndex], min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}



template <class T> void
PSTable::StoreEntry (const T &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{

  //Check if we are trying to write a different value
  T currentValue (true);
  RetrieveEntry (currentValue, rowIndex, fieldIndex);

  if (currentValue == rSource)
    return; //Nothing to change

  const PSFieldDescriptor& field   = GetFieldDescriptorInternal (fieldIndex);
  const D_UINT8            bitsSet = ~0;

  StoredItem     cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData   = cachedItem.GetDataForUpdate();

  assert (field.m_NullBitIndex > 0);

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

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

  //Update the field index if it exists
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    {
      BTree      fieldIndexTree (*m_vIndexNodeMgrs[fieldIndex]);
      NODE_INDEX dummyNode;
      KEY_INDEX  dummyKey;

      fieldIndexTree.RemoveKey (T_BTreeKey<T> (currentValue, rowIndex));
      fieldIndexTree.InsertKey (T_BTreeKey<T> (rSource, rowIndex), dummyNode, dummyKey);
    }
}

template <class T> DBSArray
PSTable::MatchRowsWithIndex (FieldIndexNodeManager* const pNodeMgr,
                             const T&                     min,
                             const T&                     max,
                             const D_UINT64               fromRow,
                             D_UINT64                     toRow,
                             D_UINT64                     ignoreFirst,
                             D_UINT64                     maxCount)
{
  toRow = MIN (toRow, ((m_RowsCount > 0) ? m_RowsCount - 1 : 0));

  DBSArray            result (_SC (DBSUInt64*, NULL));
  BTree               fieldIndexTree (*pNodeMgr);
  NODE_INDEX          node;
  KEY_INDEX           key;
  const T_BTreeKey<T> firstKey (min, fromRow);
  const T_BTreeKey<T> lastKey (max, MIN (toRow, m_RowsCount));

  if ( (fieldIndexTree.FindBiggerOrEqual (firstKey, node, key) == false) ||
       (maxCount == 0))
    return result;

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
            return result;

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

  return result;
}


template <class T> DBSArray
PSTable::MatchRows (const T&       min,
                    const T&       max,
                    const D_UINT64 fromRow,
                    D_UINT64       toRow,
                    D_UINT64       ignoreFirst,
                    D_UINT64       maxCount,
                    const D_UINT   fieldIndex)
{
  toRow = MIN (toRow, ((m_RowsCount > 0) ? m_RowsCount - 1 : 0));

  DBSArray result (_SC (DBSUInt64*, NULL));
  T         rowValue (true);

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

      result.AddElement (DBSUInt64 (false, rowIndex));
    }

  return result;
}

//From this point this functions shall not allocated memory
//Other way will not be traceable.
#ifdef new
#undef new
#endif

void
PSTable::GetEntry (DBSText &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const PSFieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, T_TEXT)))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  StoredItem cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8 *const pRawData = cachedItem.GetDataForRead();

  const D_UINT64& fieldFirstEntry = *_RC (const D_UINT64*, pRawData + field.m_StoreIndex + 0);
  const D_UINT64& fieldValueSize  = *_RC (const D_UINT64*,
                                          pRawData + field.m_StoreIndex + sizeof (D_UINT64));

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

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

  const PSFieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ( ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) == 0) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, rDestination.GetElementsType())))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  StoredItem cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8 *const pRawData = cachedItem.GetDataForRead();

  const D_UINT64& fieldFirstEntry = *_RC (const D_UINT64*,
                                          pRawData + field.m_StoreIndex + 0);

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  if (pRawData[byte_off] & (1 << bit_off))
    {
      rDestination.~DBSArray ();

      switch (field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK)
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
        new (&rDestination) DBSArray(_SC(DBSHiresTime *, NULL));
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
                                _SC(DBS_FIELD_TYPE, field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK))));
    }
}

template <class T> void
PSTable::RetrieveEntry (T &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  T *const pDestination          = &rDestination;
  const PSFieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, _SC(DBS_FIELD_TYPE, rDestination))))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  StoredItem           cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8 *const pRawData   = cachedItem.GetDataForRead();

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  if (pRawData[byte_off] & (1 << bit_off))
    {
      pDestination->~T ();
      new (pDestination) T (true);
    }
  else
    {
      PSValInterp::Retrieve (pDestination, pRawData + field.m_StoreIndex);
    }
}
