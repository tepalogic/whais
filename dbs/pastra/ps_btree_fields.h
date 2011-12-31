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

#include "ps_btree_index.h"

namespace pastra

{

template <class DBS_T>
class T_BTreeKey : public I_BTreeKey
{
public:
  T_BTreeKey (const DBS_T &value, const D_UINT64 index) :
      m_RowPart (index),
      m_ValuePart (value)
      {
      }

  bool operator< (const T_BTreeKey &key) const
  {
    if (m_ValuePart < key.m_ValuePart)
      return true;
    else if (m_ValuePart == key.m_ValuePart)
      return m_RowPart < key.m_RowPart;

    return false;
  }

  bool operator== (const T_BTreeKey  &key) const
  {
    return (m_ValuePart == key.m_ValuePart) &&
           (m_RowPart == key.m_RowPart);
  }

  bool operator> (const T_BTreeKey  &key) const
  {
    return ! ((*this < key) || (*this == key));
  }

  const D_UINT64 m_RowPart;
  const DBS_T    m_ValuePart;
};

template <class T, class DBS_T>
class T_BTreeNode : public I_BTreeNode
{
public:
  T_BTreeNode (I_BTreeNodeManager &nodesManager, const NODE_INDEX nodeId) :
    I_BTreeNode (nodesManager, nodeId),
    m_cpNodeData (new D_UINT8 [RAW_NODE_SIZE])
  {
    m_Header = _RC (NodeHeader*, m_cpNodeData);
    SetNullKeysCount (0);
  }
  virtual ~T_BTreeNode ()
  {
  }

  //Implementations of I_BTreeNode interface

  virtual D_UINT GetKeysPerNode () const
  {
    assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

    D_UINT result = RAW_NODE_SIZE - sizeof (NodeHeader);

    if (IsLeaf())
      result /= sizeof (D_UINT64) + sizeof (T);
    else
      {
        result /= (sizeof (D_UINT64) + sizeof (T) + sizeof (NODE_INDEX));

        //Ensure the right alignment for NODE_INDEX by dropping rows.
        if (sizeof (T) < sizeof (NODE_INDEX))
          result -= (((sizeof (T) * result) % sizeof (NODE_INDEX)) / sizeof (T));
      }

    return result;
  }

  virtual KEY_INDEX GetFirstKey (const I_BTreeNode &parent) const
  {

    assert (GetKeysCount () > 0);

    KEY_INDEX             result     = ~0;
    const D_UINT64 *const pRowParts = _RC (const D_UINT64*, m_cpNodeData + sizeof (NodeHeader));

    if (GetNullKeysCount () == 0)
      {
        T_BTreeKey <DBS_T> key (DBS_T(true), pRowParts [0]);

        parent.FindBiggerOrEqual (key, result);

        assert (parent.IsEqual (key, result));
      }
    else
      {
        const T *const     pValuePart = _RC (const T*, pRowParts + GetKeysPerNode ());
        T_BTreeKey<DBS_T> key (DBS_T(false, pValuePart[0]), pRowParts [0]);

        parent.FindBiggerOrEqual (key, result);

        assert (parent.IsEqual (key, result));
      }

      assert (GetNodeId () == parent.GetChildNode (result));
      return result;
  }

  virtual NODE_INDEX GetChildNode (const KEY_INDEX keyIndex) const
  {
    assert (keyIndex < GetKeysCount ());
    assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
    assert (IsLeaf () == false);

    const D_UINT64 *const   pRowParts   = _RC (const D_UINT64*, m_cpNodeData + sizeof (NodeHeader));
    const T *const          pValueParts = _RC (const T*, pRowParts + GetKeysPerNode ());
    const NODE_INDEX *const pChildNodes = _RC (const NODE_INDEX*, pValueParts + GetKeysPerNode ());

    return pChildNodes [keyIndex];
  }

  virtual void ResetKeyNode (const I_BTreeNode &childNode, const KEY_INDEX keyIndex)
  {
    D_UINT64 *const       pRowParts      = _RC (D_UINT64*, m_cpNodeData + sizeof (NodeHeader));
    T *const              pValueParts    = _RC (T*, pRowParts + GetKeysPerNode ());
    const T_BTreeNode    &node          = _SC (const T_BTreeNode&, childNode);
    const D_UINT64 *const pSrcRowParts   = _RC (const D_UINT64*, node.m_cpNodeData + sizeof (NodeHeader));
    const T *const        pSrcValueParts = _RC (const T*, pSrcRowParts + GetKeysPerNode ());

    pRowParts [keyIndex]   = pSrcRowParts [0];
    pValueParts [keyIndex] = pSrcValueParts [0];
  }

  virtual void SetChildNode (const KEY_INDEX keyIndex, const NODE_INDEX childNode)
  {
    assert (keyIndex < GetKeysCount ());
    assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
    assert (IsLeaf () == false);

    D_UINT64 *const   pRowParts   = _RC (D_UINT64*, m_cpNodeData + sizeof (NodeHeader));
    T *const          pValueParts = _RC (T*, pRowParts + GetKeysPerNode ());
    NODE_INDEX *const pChildNodes = _RC (NODE_INDEX*, pValueParts + GetKeysPerNode ());

    pChildNodes [keyIndex] = childNode;
  }

  virtual KEY_INDEX InsertKey (const I_BTreeKey &key)
  {
    assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

    const T_BTreeKey<DBS_T> &theKey     = _SC (const T_BTreeKey<DBS_T>&, key);
    D_UINT64 *const          pRowParts   = _RC (D_UINT64*, m_cpNodeData + sizeof (NodeHeader));
    T *const                 pValueParts = _RC (T*, pRowParts + GetKeysPerNode ());
    KEY_INDEX                keyIndex    = ~0;

    if (GetKeysCount () == 0)
      {
        SetKeysCount (1);
        if (theKey.m_ValuePart.IsNull () == false)
          SetNullKeysCount (1);
        else
          pValueParts[0] = theKey.m_ValuePart.m_Value;

        pRowParts[0] = theKey.m_RowPart;
        return 0;
      }
    else if (FindBiggerOrEqual (key, keyIndex) == false)
      keyIndex = 0;
    else
      ++keyIndex;

    const D_UINT lastKey = GetKeysCount () - 1;
    make_array_room (pRowParts, lastKey, keyIndex, 1);
    make_array_room (pValueParts, lastKey, keyIndex, 1);

    if (IsLeaf () == false)
      {
        NODE_INDEX *const pChildNodes = _RC (NODE_INDEX*, pValueParts + GetKeysPerNode ());
        make_array_room (pChildNodes, lastKey, keyIndex, 1);
      }

    if (theKey.m_ValuePart.IsNull ())
      SetNullKeysCount (GetNullKeysCount () + 1);

    SetKeysCount (GetKeysCount () + 1);

    SetKey (_SC (const T_BTreeKey<DBS_T>&, key), keyIndex);

    return keyIndex;
  }

  virtual void RemoveKey (const KEY_INDEX keyIndex)
  {
    assert (keyIndex < GetKeysCount ());
    assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

    const D_UINT      lastKey     = GetKeysCount () - 1;
    D_UINT64 *const   pRowParts   = _RC (D_UINT64*, m_cpNodeData + sizeof (NodeHeader));
    T *const          pValuePart  = _RC (T*, pRowParts + GetKeysPerNode ());
    NODE_INDEX *const pChildNodes = _RC (NODE_INDEX*, pValuePart + GetKeysPerNode ());

    remove_array_elemes (pRowParts, lastKey, keyIndex, 1);
    remove_array_elemes (pValuePart, lastKey, keyIndex, 1);

    if (IsLeaf () == false)
      remove_array_elemes (pChildNodes, lastKey, keyIndex, 1);

    if (keyIndex <= (GetKeysCount () - GetNullKeysCount ()))
      SetNullKeysCount (GetNullKeysCount () - 1);

    SetKeysCount (GetKeysCount () - 1);
  }

  virtual void Split (const NODE_INDEX parentId)
  {
    assert (NeedsSpliting ());

    const KEY_INDEX          splitKeyIndex   = GetKeysCount () / 2;
    const T_BTreeKey<DBS_T> splitKey        = GetKey (splitKeyIndex);
    BTreeNodeHandler         parentNode (m_NodesManager.RetrieveNode (parentId));
    const KEY_INDEX          insertPosition  = parentNode->InsertKey (splitKey);
    const NODE_INDEX         allocatedNodeId = m_NodesManager.AllocateNode (parentId,
                                                                            insertPosition);
    BTreeNodeHandler         allocatedNode (m_NodesManager.RetrieveNode (allocatedNodeId));

    allocatedNode->SetLeaf (IsLeaf ());
    allocatedNode->MarkAsUsed ();
    allocatedNode->SetKeysCount (GetKeysCount () - splitKeyIndex);

    if (GetKeysCount () - GetNullKeysCount () < splitKeyIndex)
      allocatedNode->SetNullKeysCount (GetKeysCount () - splitKeyIndex);
    else
      allocatedNode->SetNullKeysCount (GetNullKeysCount ());

    SetNullKeysCount (GetNullKeysCount () - allocatedNode->GetNullKeysCount ());
    assert (GetNullKeysCount () <= GetKeysCount ());
    assert (allocatedNode->GetNullKeysCount () <= allocatedNode->GetNullKeysCount ());

    for (KEY_INDEX index = splitKeyIndex; index < GetKeysCount (); ++index)
      _SC (T_BTreeNode*, &(*allocatedNode))->SetKey ( GetKey (index), index - splitKeyIndex);

    if (IsLeaf () == false)
      for (KEY_INDEX index = splitKeyIndex; index < GetKeysCount (); ++index)
        _SC (T_BTreeNode*, &(*allocatedNode))->T_BTreeNode::SetChildNode (
                                                  T_BTreeNode::GetChildNode (index),
                                                  index - splitKeyIndex);

    SetKeysCount (splitKeyIndex);
    assert (GetNullKeysCount () <= GetKeysCount ());

    allocatedNode->SetNext (GetNodeId());
    allocatedNode->SetPrev (GetPrev());
    SetPrev (allocatedNodeId);
    if (allocatedNode->GetPrev() != NIL_NODE)
      {
        BTreeNodeHandler prevNode (m_NodesManager.RetrieveNode (allocatedNode->GetPrev()));
        prevNode->SetNext (allocatedNodeId);
      }
  }

  virtual void Join (bool toRight)
  {
    if (toRight)
      {
        assert (GetNext() != NIL_NODE);
        BTreeNodeHandler    nextNode (m_NodesManager.RetrieveNode (GetNext ()));
        T_BTreeNode *const pNextNode    = _SC (T_BTreeNode*,  &(*nextNode));
        const KEY_INDEX     oldKeysCount = pNextNode->GetKeysCount ();

        pNextNode->SetKeysCount (oldKeysCount + GetKeysCount ());
        pNextNode->SetNullKeysCount (pNextNode->GetNullKeysCount () + GetNullKeysCount ());

        for (KEY_INDEX index = 0; index < oldKeysCount; ++index)
          pNextNode->SetKey (GetKey (index), index + oldKeysCount);

        if (IsLeaf () == false)
          for (KEY_INDEX index = 0; index < oldKeysCount; ++index)
            pNextNode->T_BTreeNode::SetChildNode (T_BTreeNode::GetChildNode (index),
                                                   index + oldKeysCount);

        assert ((pNextNode->GetNullKeysCount() == 0) ||
            ( GetKeysCount () == GetNullKeysCount ()));

        nextNode->SetPrev (GetPrev ());
        if (GetPrev () != NIL_NODE)
          {
            BTreeNodeHandler prevNode (m_NodesManager.RetrieveNode (GetPrev ()));
            prevNode->SetNext (GetNext ());
          }

        assert (pNextNode->GetNullKeysCount () <= pNextNode->GetKeysCount ());
        assert (pNextNode->GetKeysCount () <= pNextNode->GetKeysPerNode());
      }
    else
      {
        assert (GetPrev () != NIL_NODE);

        BTreeNodeHandler    prevNode (m_NodesManager.RetrieveNode (GetNext ()));
        T_BTreeNode *const pPrevNode    = _SC (T_BTreeNode*, &(*prevNode));
        const KEY_INDEX     oldKeysCount = GetKeysCount ();

        SetKeysCount (oldKeysCount + pPrevNode->GetKeysCount ());
        SetNullKeysCount (GetNullKeysCount () + pPrevNode->GetKeysCount());

        for (KEY_INDEX index =0; index < pPrevNode->GetKeysCount (); ++index)
          SetKey (pPrevNode->GetKey (index), index + oldKeysCount);

        if (IsLeaf () == false)
          for (KEY_INDEX index = 0; index < oldKeysCount; ++index)
            SetChildNode (pPrevNode->T_BTreeNode::GetChildNode (index),
                          index + oldKeysCount);

        SetPrev (prevNode->GetPrev ());
        if (GetPrev () != NIL_NODE)
          {
            BTreeNodeHandler prevNode (m_NodesManager.RetrieveNode (GetPrev ()));
            prevNode->SetNext (GetNodeId ());
          }

        assert (GetNullKeysCount () <= GetKeysCount() );
        assert (GetKeysCount () <= GetKeysPerNode());
      }
  }

  virtual bool IsLess (const I_BTreeKey &key, KEY_INDEX keyIndex) const
  {
    assert (keyIndex  < GetKeysCount ());
    const T_BTreeKey<DBS_T> &theKey = _SC (const T_BTreeKey<DBS_T>&, key);

    return theKey < GetKey (keyIndex);
  }

  virtual bool IsEqual (const I_BTreeKey &key, KEY_INDEX keyIndex) const
  {
    assert (keyIndex  < GetKeysCount ());
    const T_BTreeKey<DBS_T> &theKey = _SC (const T_BTreeKey<DBS_T>&, key);

    return theKey == GetKey (keyIndex);
  }

  virtual bool IsBigger (const I_BTreeKey &key, KEY_INDEX keyIndex) const
  {
    assert (keyIndex  < GetKeysCount ());
    const T_BTreeKey<DBS_T> &theKey = _SC (const T_BTreeKey<DBS_T>&, key);

    return theKey > GetKey (keyIndex);
  }

  virtual const I_BTreeKey& GetSentinelKey () const
  {
    static T_BTreeKey<DBS_T> _sentinel (DBS_T (false, ~0), ~0);

    return _sentinel;
  }

  static const D_UINT RAW_NODE_SIZE = 16384;

protected:
  const T_BTreeKey<DBS_T> GetKey (const KEY_INDEX keyIndex) const
  {
    assert (keyIndex < GetKeysCount ());
    assert (GetKeysCount () <= GetKeysCount ());

    const D_UINT64 *const pRowParts = _RC (const D_UINT64*, m_cpNodeData + sizeof (NodeHeader));

    if (keyIndex <= GetKeysCount() - GetNullKeysCount ())
      return T_BTreeKey<DBS_T> (DBS_T(true), pRowParts [keyIndex]);

    const T *const pValueParts = _RC (const T*, pRowParts + GetKeysPerNode ());

    return T_BTreeKey<DBS_T> (DBS_T (false, pValueParts[keyIndex]), pRowParts [keyIndex]);
  }

  void SetKey (const T_BTreeKey<DBS_T> &key,const KEY_INDEX keyIndex)
  {
    assert (keyIndex < GetKeysCount ());
    assert (GetKeysCount () <= GetKeysCount ());

    D_UINT64 *const pRowParts = _RC (D_UINT64*, m_cpNodeData + sizeof (NodeHeader));

    if (keyIndex <= GetKeysCount() - GetNullKeysCount ())
      SetNullKeysCount (GetNullKeysCount () + 1);
    else
      {
        T *const pValueParts = _RC (T*, pRowParts + GetKeysPerNode ());
        pValueParts[keyIndex] = key.m_ValuePart.m_Value;
      }

    pRowParts[keyIndex] = key.m_RowPart;
  }


  D_UINT8 * const m_cpNodeData;
};

typedef T_BTreeKey <DBSBool>                BoolBTreeKey;
typedef T_BTreeKey <DBSChar>                CharBTreeKey;
typedef T_BTreeKey <DBSUInt8>               UInt8BTreeKey;
typedef T_BTreeKey <DBSUInt16>              UInt16BTreeKey;
typedef T_BTreeKey <DBSUInt32>              UInt32BTreeKey;
typedef T_BTreeKey <DBSUInt64>              UInt64BTreeKey;
typedef T_BTreeKey <DBSInt8>                Int8BTreeKey;
typedef T_BTreeKey <DBSInt16>               Int16BTreeKey;
typedef T_BTreeKey <DBSInt32>               Int32BTreeKey;
typedef T_BTreeKey <DBSInt64>               Int64BTreeKey;
typedef T_BTreeKey <DBSDate>                DateBTreeKey;
typedef T_BTreeKey <DBSDateTime>            DateTimeBTreeKey;
typedef T_BTreeKey <DBSHiresTime>           HiresDateBTreeKey;
typedef T_BTreeKey <DBSReal>                RealBTreeKey;
typedef T_BTreeKey <DBSRichReal>            RichRealBTreeKey;

typedef T_BTreeNode <D_UINT32, DBSChar>     CharBTreeNode;
typedef T_BTreeNode <D_BOOL, DBSBool>       BoolBTreeNode;
typedef T_BTreeNode <D_UINT8, DBSUInt8>     UInt8BTreeNode;
typedef T_BTreeNode <D_UINT16, DBSUInt16>   UInt16BTreeNode;
typedef T_BTreeNode <D_UINT32, DBSUInt32>   UInt32BTreeNode;
typedef T_BTreeNode <D_UINT64, DBSUInt64>   UInt64BTreeNode;
typedef T_BTreeNode <D_INT8, DBSInt8>       Int8BTreeNode;
typedef T_BTreeNode <D_INT16, DBSInt16>     Int16BTreeNode;
typedef T_BTreeNode <D_INT32, DBSInt32>     Int32BTreeNode;
typedef T_BTreeNode <D_INT64, DBSInt64>     Int64BTreeNode;

}

#endif /* PS_BTREE_FIELDS_H_ */
