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

#ifndef PS_BTREE_FIELDS_H_
#define PS_BTREE_FIELDS_H_

#include <assert.h>
#include <limits>

#include "whisper.h"

#include "ps_btree_index.h"
#include "ps_container.h"

namespace pastra

{

template <class DBS_T>
class T_BTreeKey : public I_BTreeKey
{
public:
  T_BTreeKey (const DBS_T& value, const ROW_INDEX row)
    : m_RowPart (row),
      m_ValuePart (value)
  {
  }

  bool operator< (const T_BTreeKey& key) const
  {
    if (m_ValuePart < key.m_ValuePart)
      return true;
    else if (m_ValuePart == key.m_ValuePart)
      return m_RowPart < key.m_RowPart;

    return false;
  }

  bool operator== (const T_BTreeKey& key) const
  {
    return (m_ValuePart == key.m_ValuePart) &&
           (m_RowPart == key.m_RowPart);
  }

  bool operator> (const T_BTreeKey  &key) const
  {
    return ! ((*this < key) || (*this == key));
  }

  const ROW_INDEX m_RowPart;
  const DBS_T     m_ValuePart;
};

typedef T_BTreeKey <DBSBool>      BoolBTreeKey;
typedef T_BTreeKey <DBSChar>      CharBTreeKey;
typedef T_BTreeKey <DBSUInt8>     UInt8BTreeKey;
typedef T_BTreeKey <DBSUInt16>    UInt16BTreeKey;
typedef T_BTreeKey <DBSUInt32>    UInt32BTreeKey;
typedef T_BTreeKey <DBSUInt64>    UInt64BTreeKey;
typedef T_BTreeKey <DBSInt8>      Int8BTreeKey;
typedef T_BTreeKey <DBSInt16>     Int16BTreeKey;
typedef T_BTreeKey <DBSInt32>     Int32BTreeKey;
typedef T_BTreeKey <DBSInt64>     Int64BTreeKey;
typedef T_BTreeKey <DBSDate>      DateBTreeKey;
typedef T_BTreeKey <DBSDateTime>  DateTimeBTreeKey;
typedef T_BTreeKey <DBSHiresTime> HiresTimeBTreeKey;
typedef T_BTreeKey <DBSReal>      RealBTreeKey;
typedef T_BTreeKey <DBSRichReal>  RichRealBTreeKey;

class I_BTreeFieldIndexNode : public I_BTreeNode
{
public:
   I_BTreeFieldIndexNode (I_BTreeNodeManager& nodesManager,
                         const NODE_INDEX     node)
     : I_BTreeNode (nodesManager, node)
   {
   }
   virtual ~I_BTreeFieldIndexNode () {}

   virtual void GetRows (KEY_INDEX fromPos, KEY_INDEX toPos, DBSArray& output) const = 0;
};

template <class T, class DBS_T, D_UINT typeSize = sizeof (T)>
class T_BTreeNode : public I_BTreeFieldIndexNode
{
public:
  T_BTreeNode (I_BTreeNodeManager& nodesManager, const NODE_INDEX node)
    : I_BTreeFieldIndexNode (nodesManager, node)
  {
  }
  virtual ~T_BTreeNode ()
  {
  }

  //Implementations of I_BTreeNode interface

  virtual D_UINT KeysPerNode () const
  {
    assert (sizeof (NodeHeader) % 16 == 0);

    D_UINT result = m_NodesMgr.RawNodeSize () - sizeof (NodeHeader);

    if (IsLeaf())
      {
        result /= sizeof (D_UINT64) + typeSize;
        result &= ~1ul;
      }
    else
      {
        result /= (sizeof (D_UINT64) + typeSize + sizeof (NODE_INDEX));
        result &= ~3ul;
      }

    return result;
  }

  virtual KEY_INDEX GetParentKeyIndex (const I_BTreeNode& parent) const
  {
    assert (GetKeysCount () > 0);

    KEY_INDEX result = ~0;

    parent.FindBiggerOrEqual (GetKey (0), result);

    assert (parent.IsEqual (GetKey (0), result));
    assert (NodeId () == parent.GetNodeOfKey (result));

    return result;
  }

  virtual NODE_INDEX GetNodeOfKey (const KEY_INDEX key) const
  {
    assert (key < GetKeysCount ());
    assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
    assert (IsLeaf () == false);

    const D_UINT64* const   pRowParts   = _RC (const D_UINT64*, DataToRead ());
    const D_UINT8* const    pValueParts = _RC (const D_UINT8*, pRowParts + KeysPerNode ());
    const NODE_INDEX* const pChildNodes = _RC (const NODE_INDEX*,
                                               pValueParts + KeysPerNode () * typeSize);

    return pChildNodes [key];
  }

  virtual void SetNodeOfKey (const KEY_INDEX keyIndex, const NODE_INDEX childNode)
  {
    assert (keyIndex < GetKeysCount ());
    assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
    assert (IsLeaf () == false);

    D_UINT64 *const   pRowParts   = _RC (D_UINT64*, DataToWrite ());
    D_UINT8 *const    pValueParts = _RC (D_UINT8*, pRowParts + KeysPerNode ());
    NODE_INDEX* const pChildNodes = _RC (NODE_INDEX*,
                                         pValueParts + KeysPerNode () * typeSize);

    pChildNodes [keyIndex] = childNode;
  }


  virtual void AdjustKeyNode (const I_BTreeNode& childNode, const KEY_INDEX keyIndex)
  {
    const T_BTreeNode& node = _SC (const T_BTreeNode&, childNode);

    const T_BTreeKey<DBS_T> zeroKey  = node.GetKey (0);

    if (zeroKey.m_ValuePart.IsNull () == true)
      SetNullKeysCount (GetNullKeysCount () + 1);

    SetKey (zeroKey, keyIndex);
  }

  virtual KEY_INDEX InsertKey (const I_BTreeKey& key)
  {
    assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

    const T_BTreeKey<DBS_T> &theKey  = _SC (const T_BTreeKey<DBS_T>&, key);
    KEY_INDEX               keyIndex = ~0;

    if (GetKeysCount () == 0)
      {
        SetKeysCount (1);
        if (theKey.m_ValuePart.IsNull ())
          SetNullKeysCount (1);

        SetKey (theKey, 0);
        return 0;
      }
    else if (FindBiggerOrEqual (key, keyIndex) == false)
      keyIndex = 0;
    else
      ++keyIndex;

    D_UINT64* const pRowParts   = _RC (D_UINT64*, DataToWrite ());
    T* const        pValueParts = _RC (T*, pRowParts + KeysPerNode ());
    const D_UINT    lastKey     = GetKeysCount () - 1;

    make_array_room (pRowParts, lastKey, keyIndex, 1);
    make_array_room (pValueParts, lastKey, keyIndex, 1);

    if (IsLeaf () == false)
      {
        NODE_INDEX* const pChildNodes = _RC (NODE_INDEX*, pValueParts + KeysPerNode ());
        make_array_room (pChildNodes, lastKey, keyIndex, 1);
      }

    if (theKey.m_ValuePart.IsNull ())
      SetNullKeysCount (GetNullKeysCount () + 1);

    SetKeysCount (GetKeysCount () + 1);
    SetKey (theKey, keyIndex);

    return keyIndex;
  }

  virtual void RemoveKey (const KEY_INDEX keyIndex)
  {
    assert (keyIndex < GetKeysCount ());
    assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

    const D_UINT      lastKey     = GetKeysCount () - 1;
    D_UINT64* const   pRowParts   = _RC (D_UINT64*, DataToWrite ());
    T* const          pValuePart  = _RC (T*, pRowParts + KeysPerNode ());
    NODE_INDEX* const pChildNodes = _RC (NODE_INDEX*, pValuePart + KeysPerNode ());

    remove_array_elemes (pRowParts, lastKey, keyIndex, 1);
    remove_array_elemes (pValuePart, lastKey, keyIndex, 1);

    if (IsLeaf () == false)
      remove_array_elemes (pChildNodes, lastKey, keyIndex, 1);

    if (keyIndex >= (GetKeysCount () - GetNullKeysCount ()))
      SetNullKeysCount (GetNullKeysCount () - 1);

    SetKeysCount (GetKeysCount () - 1);
  }

  virtual void Split (const NODE_INDEX parent)
  {
    assert (NeedsSpliting ());

    const KEY_INDEX          splitKeyIndex   = GetKeysCount () / 2;
    const T_BTreeKey<DBS_T>  splitKey        = GetKey (splitKeyIndex);
    BTreeNodeRAI             parentNode (m_NodesMgr.RetrieveNode (parent));
    const KEY_INDEX          insertPosition  = parentNode->InsertKey (splitKey);
    const NODE_INDEX         allocatedNodeId = m_NodesMgr.AllocateNode (parent,
                                                                        insertPosition);
    BTreeNodeRAI             allocatedNode (m_NodesMgr.RetrieveNode (allocatedNodeId));

    allocatedNode->SetLeaf (IsLeaf ());
    allocatedNode->MarkAsUsed ();
    allocatedNode->SetKeysCount (GetKeysCount () - splitKeyIndex);

    if (GetKeysCount () - GetNullKeysCount () < splitKeyIndex)
      allocatedNode->SetNullKeysCount (GetKeysCount () - splitKeyIndex);
    else
      allocatedNode->SetNullKeysCount (GetNullKeysCount ());

    assert (GetNullKeysCount () <= GetKeysCount ());
    assert (allocatedNode->GetNullKeysCount () <= allocatedNode->GetNullKeysCount ());

    for (KEY_INDEX index = splitKeyIndex; index < GetKeysCount (); ++index)
      _SC (T_BTreeNode*, &(*allocatedNode))->SetKey ( GetKey (index), index - splitKeyIndex);

    if (IsLeaf () == false)
      for (KEY_INDEX index = splitKeyIndex; index < GetKeysCount (); ++index)
        _SC (T_BTreeNode*, &(*allocatedNode))->T_BTreeNode::SetNodeOfKey (index - splitKeyIndex,
                                                                        T_BTreeNode::GetNodeOfKey (index));
    SetNullKeysCount (GetNullKeysCount () - allocatedNode->GetNullKeysCount ());
    SetKeysCount (splitKeyIndex);
    assert (GetNullKeysCount () <= GetKeysCount ());

    allocatedNode->SetNext (NodeId());
    allocatedNode->SetPrev (GetPrev());
    SetPrev (allocatedNodeId);
    if (allocatedNode->GetPrev() != NIL_NODE)
      {
        BTreeNodeRAI prevNode (m_NodesMgr.RetrieveNode (allocatedNode->GetPrev()));
        prevNode->SetNext (allocatedNodeId);
      }
  }

  virtual void Join (bool toRight)
  {
    if (toRight)
      {
        assert (GetNext() != NIL_NODE);
        BTreeNodeRAI       nextNode (m_NodesMgr.RetrieveNode (GetNext ()));
        T_BTreeNode* const pNextNode    = _SC (T_BTreeNode*,  &(*nextNode));
        const KEY_INDEX    oldKeysCount = pNextNode->GetKeysCount ();

        assert ((pNextNode->GetNullKeysCount() == 0) ||
            ( GetKeysCount () == GetNullKeysCount ()));

        pNextNode->SetKeysCount (oldKeysCount + GetKeysCount ());
        pNextNode->SetNullKeysCount (pNextNode->GetNullKeysCount () + GetNullKeysCount ());

        for (KEY_INDEX index = 0; index < GetKeysCount (); ++index)
          pNextNode->SetKey (GetKey (index), index + oldKeysCount);

        if (IsLeaf () == false)
          for (KEY_INDEX index = 0; index < GetKeysCount (); ++index)
            pNextNode->T_BTreeNode::SetNodeOfKey (index + oldKeysCount,
                                                  T_BTreeNode::GetNodeOfKey (index));

        nextNode->SetPrev (GetPrev ());
        if (GetPrev () != NIL_NODE)
          {
            BTreeNodeRAI prevNode (m_NodesMgr.RetrieveNode (GetPrev ()));
            prevNode->SetNext (GetNext ());
          }

        assert (pNextNode->GetNullKeysCount () <= pNextNode->GetKeysCount ());
        assert (pNextNode->GetKeysCount () <= pNextNode->KeysPerNode());
      }
    else
      {
        assert (GetPrev () != NIL_NODE);

        BTreeNodeRAI       prevNode (m_NodesMgr.RetrieveNode (GetPrev ()));
        T_BTreeNode* const pPrevNode    = _SC (T_BTreeNode*, &(*prevNode));
        const KEY_INDEX    oldKeysCount = GetKeysCount ();

        SetKeysCount (oldKeysCount + pPrevNode->GetKeysCount ());
        SetNullKeysCount (GetNullKeysCount () + pPrevNode->GetNullKeysCount());

        for (KEY_INDEX index = 0; index < pPrevNode->GetKeysCount (); ++index)
          SetKey (pPrevNode->GetKey (index), index + oldKeysCount);

        if (IsLeaf () == false)
          for (KEY_INDEX index = 0; index < oldKeysCount; ++index)
            SetNodeOfKey (index + oldKeysCount, pPrevNode->T_BTreeNode::GetNodeOfKey (index));

        SetPrev (prevNode->GetPrev ());
        if (GetPrev () != NIL_NODE)
          {
            BTreeNodeRAI prevNode (m_NodesMgr.RetrieveNode (GetPrev ()));
            prevNode->SetNext (NodeId ());
          }

        assert (GetNullKeysCount () <= GetKeysCount() );
        assert (GetKeysCount () <= KeysPerNode());
      }
  }

  virtual bool IsLess (const I_BTreeKey& key, KEY_INDEX keyIndex) const
  {
    assert (keyIndex  < GetKeysCount ());
    const T_BTreeKey<DBS_T> &theKey = _SC (const T_BTreeKey<DBS_T>&, key);

    return theKey < GetKey (keyIndex);
  }

  virtual bool IsEqual (const I_BTreeKey& key, KEY_INDEX keyIndex) const
  {
    assert (keyIndex  < GetKeysCount ());
    const T_BTreeKey<DBS_T> &theKey = _SC (const T_BTreeKey<DBS_T>&, key);

    return theKey == GetKey (keyIndex);
  }

  virtual bool IsBigger (const I_BTreeKey& key, KEY_INDEX keyIndex) const
  {
    assert (keyIndex  < GetKeysCount ());
    const T_BTreeKey<DBS_T> &theKey = _SC (const T_BTreeKey<DBS_T>&, key);

    return theKey > GetKey (keyIndex);
  }

  virtual const I_BTreeKey& SentinelKey () const
  {
    static T_BTreeKey<DBS_T> _sentinel (DBS_T (~0), ~0);

    return _sentinel;
  }

  virtual void GetRows (KEY_INDEX fromPos, KEY_INDEX toPos, DBSArray& output) const
  {
    assert (fromPos >= toPos);
    assert (fromPos < GetKeysCount ());
    assert (output.ElementsType() == T_UINT64);

    const D_UINT64* const pRowParts = _RC (const D_UINT64*, DataToRead ());

    if ((toPos == 0) && IsEqual (SentinelKey (), toPos))
      ++toPos;

    while (fromPos >= toPos)
      {
        output.AddElement (DBSUInt64 (pRowParts[fromPos]));
        if (fromPos == 0)
          break;
        fromPos--;
      }
  }

private:
  const T_BTreeKey<DBS_T> GetKey (const KEY_INDEX keyIndex) const
  {
    assert (keyIndex < GetKeysCount ());
    assert (GetKeysCount () <= GetKeysCount ());

    const D_UINT64 *const pRowParts = _RC (const D_UINT64*, DataToRead ());

    if (keyIndex >= GetKeysCount() - GetNullKeysCount ())
      return T_BTreeKey<DBS_T> (DBS_T (), pRowParts [keyIndex]);

    const T* const pValueParts = _RC (const T*, pRowParts + KeysPerNode ());

    return T_BTreeKey<DBS_T> (DBS_T (pValueParts[keyIndex]), pRowParts [keyIndex]);
  }

  void SetKey (const T_BTreeKey<DBS_T> &key,const KEY_INDEX keyIndex)
  {
    assert (keyIndex < GetKeysCount ());
    assert (GetKeysCount () <= GetKeysCount ());

    D_UINT64 *const pRowParts = _RC (D_UINT64*, DataToWrite ());

    if (keyIndex < GetKeysCount() - GetNullKeysCount ())
      {
        T* const pValueParts = _RC (T*, pRowParts + KeysPerNode ());
        pValueParts[keyIndex] = key.m_ValuePart.m_Value;
      }

    pRowParts[keyIndex] = key.m_RowPart;
  }
};


//Specialization for DBSBool
template <> inline const I_BTreeKey&
T_BTreeNode <bool, DBSBool, 1>::SentinelKey () const
{
  static BoolBTreeKey _sentinel (DBSBool (true), ~0);
  return _sentinel;
}

//Specializations for DBSDate
template <> inline const I_BTreeKey&
T_BTreeNode <void, DBSDate, 4>::SentinelKey () const
{
  static DateBTreeKey _sentinel (DBSDate (0x7FFF, 12, 31), ~0);
  return _sentinel;
}

template <> inline const DateBTreeKey
T_BTreeNode <void, DBSDate, 4>::GetKey (const KEY_INDEX keyIndex) const
{
  assert (keyIndex < GetKeysCount ());
  assert (GetKeysCount () <= GetKeysCount ());

  const D_UINT64 *const pRowParts = _RC (const D_UINT64*, DataToRead ());


  if (keyIndex >= GetKeysCount() - GetNullKeysCount ())
    return DateBTreeKey (DBSDate (), pRowParts [keyIndex]);

  const D_INT16 *const  pYearParts  = _RC (const D_INT16*, pRowParts + KeysPerNode ());
  const D_UINT8 *const  pMonthParts = _RC (const D_UINT8*, pYearParts + KeysPerNode ());
  const D_UINT8 *const  pDayParts   = _RC (const D_UINT8*, pMonthParts + KeysPerNode ());

  return DateBTreeKey (DBSDate (pYearParts [keyIndex], pMonthParts [keyIndex], pDayParts [keyIndex]),
                       pRowParts [keyIndex]);
}

template<> inline void
T_BTreeNode <void, DBSDate, 4>::SetKey (const DateBTreeKey& key,const KEY_INDEX keyIndex)
{
  assert (keyIndex < GetKeysCount ());
  assert (GetKeysCount () <= GetKeysCount ());

  D_UINT64 *const pRowParts = _RC (D_UINT64*, DataToWrite ());

  if (keyIndex < GetKeysCount() - GetNullKeysCount ())
    {
      D_INT16 *const  pYearParts  = _RC (D_INT16*, pRowParts + KeysPerNode ());
      D_UINT8 *const  pMonthParts = _RC (D_UINT8*, pYearParts + KeysPerNode ());
      D_UINT8 *const  pDayParts   = _RC (D_UINT8*, pMonthParts + KeysPerNode ());

      pYearParts[keyIndex]  = key.m_ValuePart.m_Year;
      pMonthParts[keyIndex] = key.m_ValuePart.m_Month;
      pDayParts[keyIndex]   = key.m_ValuePart.m_Day;
    }

  pRowParts[keyIndex] = key.m_RowPart;
}

template <> inline KEY_INDEX
T_BTreeNode <void, DBSDate, 4>::InsertKey (const I_BTreeKey& key)
{

  const DateBTreeKey& theKey   = _SC (const DateBTreeKey&, key);
  KEY_INDEX           keyIndex = ~0;

  if (GetKeysCount () == 0)
    {
      SetKeysCount (1);
      if (theKey.m_ValuePart.IsNull ())
        SetNullKeysCount (1);

      SetKey (theKey, 0);
      return 0;
    }
  else if (FindBiggerOrEqual (key, keyIndex) == false)
    keyIndex = 0;
  else
    ++keyIndex;

  D_UINT64 *const pRowParts   = _RC (D_UINT64*, DataToWrite ());
  D_INT16 *const  pYearParts  = _RC (D_INT16*, pRowParts + KeysPerNode ());
  D_UINT8 *const  pMonthParts = _RC (D_UINT8*, pYearParts + KeysPerNode ());
  D_UINT8 *const  pDayParts   = _RC (D_UINT8*, pMonthParts + KeysPerNode ());

  const D_UINT    lastKey     = GetKeysCount () - 1;

  make_array_room (pRowParts, lastKey, keyIndex, 1);
  make_array_room (pYearParts, lastKey, keyIndex, 1);
  make_array_room (pMonthParts, lastKey, keyIndex, 1);
  make_array_room (pDayParts, lastKey, keyIndex, 1);

  if (IsLeaf () == false)
    {
      NODE_INDEX* const pChildNodes = _RC (NODE_INDEX*, pDayParts + KeysPerNode ());
      make_array_room (pChildNodes, lastKey, keyIndex, 1);
    }

  if (theKey.m_ValuePart.IsNull ())
    SetNullKeysCount (GetNullKeysCount () + 1);

  SetKeysCount (GetKeysCount () + 1);
  SetKey (theKey, keyIndex);

  return keyIndex;
}

template <> inline void
T_BTreeNode <void, DBSDate, 4>::RemoveKey (const KEY_INDEX keyIndex)
{
  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

  const D_UINT    lastKey     = GetKeysCount () - 1;
  D_UINT64 *const pRowParts   = _RC (D_UINT64*, DataToWrite ());
  D_INT16 *const  pYearParts  = _RC (D_INT16*, pRowParts + KeysPerNode ());
  D_UINT8 *const  pMonthParts = _RC (D_UINT8*, pYearParts + KeysPerNode ());
  D_UINT8 *const  pDayParts   = _RC (D_UINT8*, pMonthParts + KeysPerNode ());

  remove_array_elemes (pRowParts, lastKey, keyIndex, 1);
  remove_array_elemes (pYearParts, lastKey, keyIndex, 1);
  remove_array_elemes (pMonthParts, lastKey, keyIndex, 1);
  remove_array_elemes (pDayParts, lastKey, keyIndex, 1);

  if (IsLeaf () == false)
    {
      NODE_INDEX* const pChildNodes = _RC (NODE_INDEX*, pDayParts + KeysPerNode ());
      remove_array_elemes (pChildNodes, lastKey, keyIndex, 1);
    }

  if (keyIndex >= (GetKeysCount () - GetNullKeysCount ()))
    SetNullKeysCount (GetNullKeysCount () - 1);

  SetKeysCount (GetKeysCount () - 1);
}

//Specialization for DateTime
template <> inline const I_BTreeKey&
T_BTreeNode <void, DBSDateTime, 7>::SentinelKey () const
{
  static DateTimeBTreeKey _sentinel (DBSDateTime (0x7FFF, 12, 31, 23, 59, 59), ~0);
  return _sentinel;
}

template <> inline const DateTimeBTreeKey
T_BTreeNode <void, DBSDateTime, 7>::GetKey (const KEY_INDEX keyIndex) const
{
  assert (keyIndex < GetKeysCount ());
  assert (GetKeysCount () <= GetKeysCount ());

  const D_UINT64 *const pRowParts = _RC (const D_UINT64*, DataToRead ());


  if (keyIndex >= GetKeysCount() - GetNullKeysCount ())
    return DateTimeBTreeKey (DBSDateTime (), pRowParts [keyIndex]);

  const D_INT16 *const  pYearParts  = _RC (const D_INT16*, pRowParts + KeysPerNode ());
  const D_UINT8 *const  pMonthParts = _RC (const D_UINT8*, pYearParts + KeysPerNode ());
  const D_UINT8 *const  pDayParts   = _RC (const D_UINT8*, pMonthParts + KeysPerNode ());
  const D_UINT8 *const  pHourParts  = _RC (const D_UINT8*, pDayParts + KeysPerNode ());
  const D_UINT8 *const  pMinParts   = _RC (const D_UINT8*, pHourParts + KeysPerNode ());
  const D_UINT8 *const  pSecParts   = _RC (const D_UINT8*, pMinParts + KeysPerNode ());

  return DateTimeBTreeKey (DBSDateTime (pYearParts[keyIndex],
                                        pMonthParts[keyIndex],
                                        pDayParts[keyIndex],
                                        pHourParts[keyIndex],
                                        pMinParts[keyIndex],
                                        pSecParts[keyIndex]),
                           pRowParts [keyIndex]);
}

template<> inline void
T_BTreeNode <void, DBSDateTime, 7>::SetKey (const DateTimeBTreeKey& key,const KEY_INDEX keyIndex)
{
  assert (keyIndex < GetKeysCount ());
  assert (GetKeysCount () <= GetKeysCount ());

  D_UINT64 *const pRowParts = _RC (D_UINT64*, DataToWrite ());

  if (keyIndex < GetKeysCount() - GetNullKeysCount ())
    {
      D_INT16 *const  pYearParts  = _RC (D_INT16*, pRowParts + KeysPerNode ());
      D_UINT8 *const  pMonthParts = _RC (D_UINT8*, pYearParts + KeysPerNode ());
      D_UINT8 *const  pDayParts   = _RC (D_UINT8*, pMonthParts + KeysPerNode ());
      D_UINT8 *const  pHourParts  = _RC (D_UINT8*, pDayParts + KeysPerNode ());
      D_UINT8 *const  pMinParts   = _RC (D_UINT8*, pHourParts + KeysPerNode ());
      D_UINT8 *const  pSecParts   = _RC (D_UINT8*, pMinParts + KeysPerNode ());

      pYearParts[keyIndex]  = key.m_ValuePart.m_Year;
      pMonthParts[keyIndex] = key.m_ValuePart.m_Month;
      pDayParts[keyIndex]   = key.m_ValuePart.m_Day;
      pHourParts[keyIndex]  = key.m_ValuePart.m_Hour;
      pMinParts[keyIndex]   = key.m_ValuePart.m_Minutes;
      pSecParts[keyIndex]   = key.m_ValuePart.m_Seconds;
    }

  pRowParts[keyIndex] = key.m_RowPart;
}

template <> inline KEY_INDEX
T_BTreeNode <void, DBSDateTime, 7>::InsertKey (const I_BTreeKey& key)
{
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

  const DateTimeBTreeKey& theKey  = _SC (const DateTimeBTreeKey&, key);
  KEY_INDEX              keyIndex = ~0;

  if (GetKeysCount () == 0)
    {
      SetKeysCount (1);
      if (theKey.m_ValuePart.IsNull ())
        SetNullKeysCount (1);

      SetKey (theKey, 0);
      return 0;
    }
  else if (FindBiggerOrEqual (key, keyIndex) == false)
    keyIndex = 0;
  else
    ++keyIndex;

  D_UINT64 *const pRowParts   = _RC (D_UINT64*, DataToWrite ());
  D_INT16 *const  pYearParts  = _RC (D_INT16*, pRowParts + KeysPerNode ());
  D_UINT8 *const  pMonthParts = _RC (D_UINT8*, pYearParts + KeysPerNode ());
  D_UINT8 *const  pDayParts   = _RC (D_UINT8*, pMonthParts + KeysPerNode ());
  D_UINT8 *const  pHourParts  = _RC (D_UINT8*, pDayParts + KeysPerNode ());
  D_UINT8 *const  pMinParts   = _RC (D_UINT8*, pHourParts + KeysPerNode ());
  D_UINT8 *const  pSecParts   = _RC (D_UINT8*, pMinParts + KeysPerNode ());

  const D_UINT    lastKey     = GetKeysCount () - 1;

  make_array_room (pRowParts, lastKey, keyIndex, 1);
  make_array_room (pYearParts, lastKey, keyIndex, 1);
  make_array_room (pMonthParts, lastKey, keyIndex, 1);
  make_array_room (pDayParts, lastKey, keyIndex, 1);
  make_array_room (pHourParts, lastKey, keyIndex, 1);
  make_array_room (pMinParts, lastKey, keyIndex, 1);
  make_array_room (pSecParts, lastKey, keyIndex, 1);

  if (IsLeaf () == false)
    {
      NODE_INDEX* const pChildNodes = _RC (NODE_INDEX*, pSecParts + KeysPerNode ());
      make_array_room (pChildNodes, lastKey, keyIndex, 1);
    }

  if (theKey.m_ValuePart.IsNull ())
    SetNullKeysCount (GetNullKeysCount () + 1);

  SetKeysCount (GetKeysCount () + 1);
  SetKey (theKey, keyIndex);

  return keyIndex;
}

template <> inline void
T_BTreeNode <void, DBSDateTime, 7>::RemoveKey (const KEY_INDEX keyIndex)
{
  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

  const D_UINT    lastKey     = GetKeysCount () - 1;
  D_UINT64 *const pRowParts   = _RC (D_UINT64*, DataToWrite ());
  D_INT16 *const  pYearParts  = _RC (D_INT16*, pRowParts + KeysPerNode ());
  D_UINT8 *const  pMonthParts = _RC (D_UINT8*, pYearParts + KeysPerNode ());
  D_UINT8 *const  pDayParts   = _RC (D_UINT8*, pMonthParts + KeysPerNode ());
  D_UINT8 *const  pHourParts  = _RC (D_UINT8*, pDayParts + KeysPerNode ());
  D_UINT8 *const  pMinParts   = _RC (D_UINT8*, pHourParts + KeysPerNode ());
  D_UINT8 *const  pSecParts   = _RC (D_UINT8*, pMinParts + KeysPerNode ());

  remove_array_elemes (pRowParts, lastKey, keyIndex, 1);
  remove_array_elemes (pYearParts, lastKey, keyIndex, 1);
  remove_array_elemes (pMonthParts, lastKey, keyIndex, 1);
  remove_array_elemes (pDayParts, lastKey, keyIndex, 1);
  remove_array_elemes (pHourParts, lastKey, keyIndex, 1);
  remove_array_elemes (pMinParts, lastKey, keyIndex, 1);
  remove_array_elemes (pSecParts, lastKey, keyIndex, 1);

  if (IsLeaf () == false)
    {
      NODE_INDEX* const pChildNodes = _RC (NODE_INDEX*, pSecParts + KeysPerNode ());
      remove_array_elemes (pChildNodes, lastKey, keyIndex, 1);
    }

  if (keyIndex >= (GetKeysCount () - GetNullKeysCount ()))
    SetNullKeysCount (GetNullKeysCount () - 1);

  SetKeysCount (GetKeysCount () - 1);
}

//Specialization of DBSHiresTime

template <> inline const I_BTreeKey&
T_BTreeNode <void, DBSHiresTime, 11>::SentinelKey () const
{
  static HiresTimeBTreeKey _sentinel (DBSHiresTime (0x7FFF, 12, 31, 23, 59, 59, 999999999), ~0);
  return _sentinel;
}

template <> inline const HiresTimeBTreeKey
T_BTreeNode <void, DBSHiresTime, 11>::GetKey (const KEY_INDEX keyIndex) const
{
  assert (keyIndex < GetKeysCount ());
  assert (GetKeysCount () <= GetKeysCount ());

  const D_UINT64 *const pRowParts = _RC (const D_UINT64*, DataToRead ());

  if (keyIndex >= GetKeysCount() - GetNullKeysCount ())
    return HiresTimeBTreeKey (DBSHiresTime (), pRowParts [keyIndex]);

  const D_UINT32 *const pMicroParts = _RC (const D_UINT32*, pRowParts + KeysPerNode ());
  const D_INT16 *const  pYearParts  = _RC (const D_INT16*, pMicroParts + KeysPerNode ());
  const D_UINT8 *const  pMonthParts = _RC (const D_UINT8*, pYearParts + KeysPerNode ());
  const D_UINT8 *const  pDayParts   = _RC (const D_UINT8*, pMonthParts + KeysPerNode ());
  const D_UINT8 *const  pHourParts  = _RC (const D_UINT8*, pDayParts + KeysPerNode ());
  const D_UINT8 *const  pMinParts   = _RC (const D_UINT8*, pHourParts + KeysPerNode ());
  const D_UINT8 *const  pSecParts   = _RC (const D_UINT8*, pMinParts + KeysPerNode ());

  return HiresTimeBTreeKey (DBSHiresTime (pYearParts[keyIndex],
                                          pMonthParts[keyIndex],
                                          pDayParts[keyIndex],
                                          pHourParts[keyIndex],
                                          pMinParts[keyIndex],
                                          pSecParts[keyIndex],
                                          pMicroParts [keyIndex]),
                            pRowParts [keyIndex]);
}

template<> inline void
T_BTreeNode <void, DBSHiresTime, 11>::SetKey (const HiresTimeBTreeKey& key, const KEY_INDEX keyIndex)
{
  assert (keyIndex < GetKeysCount ());
  assert (GetKeysCount () <= GetKeysCount ());

  D_UINT64 *const pRowParts = _RC (D_UINT64*, DataToWrite ());

  if (keyIndex < GetKeysCount() - GetNullKeysCount ())
    {
      D_UINT32 *const pMicroParts = _RC (D_UINT32*, pRowParts + KeysPerNode ());
      D_INT16 *const  pYearParts  = _RC (D_INT16*, pMicroParts + KeysPerNode ());
      D_UINT8 *const  pMonthParts = _RC (D_UINT8*, pYearParts + KeysPerNode ());
      D_UINT8 *const  pDayParts   = _RC (D_UINT8*, pMonthParts + KeysPerNode ());
      D_UINT8 *const  pHourParts  = _RC (D_UINT8*, pDayParts + KeysPerNode ());
      D_UINT8 *const  pMinParts   = _RC (D_UINT8*, pHourParts + KeysPerNode ());
      D_UINT8 *const  pSecParts   = _RC (D_UINT8*, pMinParts + KeysPerNode ());

      pMicroParts[keyIndex] = key.m_ValuePart.m_Microsec;
      pYearParts[keyIndex]  = key.m_ValuePart.m_Year;
      pMonthParts[keyIndex] = key.m_ValuePart.m_Month;
      pDayParts[keyIndex]   = key.m_ValuePart.m_Day;
      pHourParts[keyIndex]  = key.m_ValuePart.m_Hour;
      pMinParts[keyIndex]   = key.m_ValuePart.m_Minutes;
      pSecParts[keyIndex]   = key.m_ValuePart.m_Seconds;
    }

  pRowParts[keyIndex] = key.m_RowPart;
}

template <> inline KEY_INDEX
T_BTreeNode <void, DBSHiresTime, 11>::InsertKey (const I_BTreeKey& key)
{
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

  const HiresTimeBTreeKey& theKey  = _SC (const HiresTimeBTreeKey&, key);
  KEY_INDEX               keyIndex = ~0;

  if (GetKeysCount () == 0)
    {
      SetKeysCount (1);
      if (theKey.m_ValuePart.IsNull ())
        SetNullKeysCount (1);

      SetKey (theKey, 0);
      return 0;
    }
  else if (FindBiggerOrEqual (key, keyIndex) == false)
    keyIndex = 0;
  else
    ++keyIndex;

  D_UINT64 *const pRowParts   = _RC (D_UINT64*, DataToWrite ());
  D_UINT32 *const pMicroParts = _RC (D_UINT32*, pRowParts + KeysPerNode ());
  D_INT16 *const  pYearParts  = _RC (D_INT16*, pMicroParts + KeysPerNode ());
  D_UINT8 *const  pMonthParts = _RC (D_UINT8*, pYearParts + KeysPerNode ());
  D_UINT8 *const  pDayParts   = _RC (D_UINT8*, pMonthParts + KeysPerNode ());
  D_UINT8 *const  pHourParts  = _RC (D_UINT8*, pDayParts + KeysPerNode ());
  D_UINT8 *const  pMinParts   = _RC (D_UINT8*, pHourParts + KeysPerNode ());
  D_UINT8 *const  pSecParts   = _RC (D_UINT8*, pMinParts + KeysPerNode ());

  const D_UINT    lastKey     = GetKeysCount () - 1;

  make_array_room (pRowParts, lastKey, keyIndex, 1);
  make_array_room (pMicroParts, lastKey, keyIndex, 1);
  make_array_room (pYearParts, lastKey, keyIndex, 1);
  make_array_room (pMonthParts, lastKey, keyIndex, 1);
  make_array_room (pDayParts, lastKey, keyIndex, 1);
  make_array_room (pHourParts, lastKey, keyIndex, 1);
  make_array_room (pMinParts, lastKey, keyIndex, 1);
  make_array_room (pSecParts, lastKey, keyIndex, 1);

  if (IsLeaf () == false)
    {
      NODE_INDEX* const pChildNodes = _RC (NODE_INDEX*, pSecParts + KeysPerNode ());
      make_array_room (pChildNodes, lastKey, keyIndex, 1);
    }

  if (theKey.m_ValuePart.IsNull ())
    SetNullKeysCount (GetNullKeysCount () + 1);

  SetKeysCount (GetKeysCount () + 1);
  SetKey (theKey, keyIndex);

  return keyIndex;
}

template <> inline void
T_BTreeNode <void, DBSHiresTime, 11>::RemoveKey (const KEY_INDEX keyIndex)
{
  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

  const D_UINT    lastKey     = GetKeysCount () - 1;
  D_UINT64 *const pRowParts   = _RC (D_UINT64*, DataToWrite ());
  D_UINT32 *const pMicroParts = _RC (D_UINT32*, pRowParts + KeysPerNode ());
  D_INT16 *const  pYearParts  = _RC (D_INT16*, pMicroParts + KeysPerNode ());
  D_UINT8 *const  pMonthParts = _RC (D_UINT8*, pYearParts + KeysPerNode ());
  D_UINT8 *const  pDayParts   = _RC (D_UINT8*, pMonthParts + KeysPerNode ());
  D_UINT8 *const  pHourParts  = _RC (D_UINT8*, pDayParts + KeysPerNode ());
  D_UINT8 *const  pMinParts   = _RC (D_UINT8*, pHourParts + KeysPerNode ());
  D_UINT8 *const  pSecParts   = _RC (D_UINT8*, pMinParts + KeysPerNode ());

  remove_array_elemes (pRowParts, lastKey, keyIndex, 1);
  remove_array_elemes (pMicroParts, lastKey, keyIndex, 1);
  remove_array_elemes (pYearParts, lastKey, keyIndex, 1);
  remove_array_elemes (pMonthParts, lastKey, keyIndex, 1);
  remove_array_elemes (pDayParts, lastKey, keyIndex, 1);
  remove_array_elemes (pHourParts, lastKey, keyIndex, 1);
  remove_array_elemes (pMinParts, lastKey, keyIndex, 1);
  remove_array_elemes (pSecParts, lastKey, keyIndex, 1);

  if (IsLeaf () == false)
    {
      NODE_INDEX* const pChildNodes = _RC (NODE_INDEX*, pSecParts + KeysPerNode ());
      remove_array_elemes (pChildNodes, lastKey, keyIndex, 1);
    }

  if (keyIndex >= (GetKeysCount () - GetNullKeysCount ()))
    SetNullKeysCount (GetNullKeysCount () - 1);

  SetKeysCount (GetKeysCount () - 1);
}


template <> inline const I_BTreeKey&
T_BTreeNode <float, DBSReal, sizeof (float)>::SentinelKey () const
{
  static RealBTreeKey _sentinel (DBSReal (std::numeric_limits<float>::infinity ()), ~0);
  return _sentinel;
}

template <> inline const I_BTreeKey&
T_BTreeNode <long double, DBSRichReal, sizeof (long double)>::SentinelKey () const
{
  static RichRealBTreeKey _sentinel (DBSRichReal (std::numeric_limits<long double>::infinity ()),
                                     ~0);
  return _sentinel;
}

typedef T_BTreeNode <bool, DBSBool, 1>         BoolBTreeNode;
typedef T_BTreeNode <D_UINT32, DBSChar>        CharBTreeNode;
typedef T_BTreeNode <void, DBSDate, 4>         DateBTreeNode;
typedef T_BTreeNode <void, DBSDateTime, 7>     DateTimeBTreeNode;
typedef T_BTreeNode <void, DBSHiresTime, 11>   HiresTimeBTreeNode;
typedef T_BTreeNode <D_UINT8, DBSUInt8>        UInt8BTreeNode;
typedef T_BTreeNode <D_UINT16, DBSUInt16>      UInt16BTreeNode;
typedef T_BTreeNode <D_UINT32, DBSUInt32>      UInt32BTreeNode;
typedef T_BTreeNode <D_UINT64, DBSUInt64>      UInt64BTreeNode;
typedef T_BTreeNode <float, DBSReal>           RealBTreeNode;
typedef T_BTreeNode <long double, DBSRichReal> RichRealBTreeNode;
typedef T_BTreeNode <D_INT8, DBSInt8>          Int8BTreeNode;
typedef T_BTreeNode <D_INT16, DBSInt16>        Int16BTreeNode;
typedef T_BTreeNode <D_INT32, DBSInt32>        Int32BTreeNode;
typedef T_BTreeNode <D_INT64, DBSInt64>        Int64BTreeNode;

class FieldIndexNodeManager : public I_BTreeNodeManager
{
public:
  FieldIndexNodeManager (std::auto_ptr <I_DataContainer> &container,
                         const D_UINT nodeSize,
                         const D_UINT maxCacheMem,
                         const DBS_FIELD_TYPE nodeType,
                         const bool create);
  virtual ~FieldIndexNodeManager();


  void     MarkForRemoval ();
  D_UINT64 GetIndexRawSize () const;

  //Implementations of I_BTreeNodeManager
  virtual D_UINT      RawNodeSize () const;
  virtual NODE_INDEX  AllocateNode (const NODE_INDEX parent, KEY_INDEX parentKey);
  virtual void        FreeNode (const NODE_INDEX node);

  virtual NODE_INDEX  GetRootNodeId ();
  virtual void        SetRootNodeId (const NODE_INDEX node);

protected:
  virtual D_UINT       MaxCachedNodes ();
  virtual I_BTreeNode* LoadNode (const NODE_INDEX node);
  virtual void         SaveNode (I_BTreeNode* const node);

  void         InitContainer ();
  void         UpdateContainer ();
  void         InitFromContainer ();
  I_BTreeNode* NodeFactory (const NODE_INDEX nodeId);

  const D_UINT                          m_NodeSize;
  const D_UINT                          m_MaxCachedMem;
  NODE_INDEX                            m_RootNode;
  NODE_INDEX                            m_FirstFreeNode;
  std::auto_ptr<I_DataContainer>        m_Container;
  const DBS_FIELD_TYPE                  m_FieldType;

private:
  FieldIndexNodeManager (const FieldIndexNodeManager&);
  FieldIndexNodeManager& operator= (const FieldIndexNodeManager&);
};

}

#endif /* PS_BTREE_FIELDS_H_ */
