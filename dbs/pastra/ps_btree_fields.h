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
#include "ps_serializer.h"

namespace whisper {
namespace pastra {



template <class DBS_T>
class T_BTreeKey : public IBTreeKey
{
public:
  T_BTreeKey (const DBS_T& value, const ROW_INDEX row)
    : mRowPart (row),
      mValuePart (value)
  {
  }

  bool operator< (const T_BTreeKey& key) const
  {
    if (mValuePart < key.mValuePart)
      return true;

    else if (mValuePart == key.mValuePart)
      return mRowPart < key.mRowPart;

    return false;
  }

  bool operator== (const T_BTreeKey& key) const
  {
    return (mRowPart == key.mRowPart)
           && (mValuePart == key.mValuePart);
  }

  bool operator> (const T_BTreeKey& key) const
  {
    return ! ((*this < key) || (*this == key));
  }

  const ROW_INDEX mRowPart;
  const DBS_T     mValuePart;
};



typedef T_BTreeKey<DBool>      BoolBTreeKey;
typedef T_BTreeKey<DChar>      CharBTreeKey;
typedef T_BTreeKey<DUInt8>     UInt8BTreeKey;
typedef T_BTreeKey<DUInt16>    UInt16BTreeKey;
typedef T_BTreeKey<DUInt32>    UInt32BTreeKey;
typedef T_BTreeKey<DUInt64>    UInt64BTreeKey;
typedef T_BTreeKey<DInt8>      Int8BTreeKey;
typedef T_BTreeKey<DInt16>     Int16BTreeKey;
typedef T_BTreeKey<DInt32>     Int32BTreeKey;
typedef T_BTreeKey<DInt64>     Int64BTreeKey;
typedef T_BTreeKey<DDate>      DateBTreeKey;
typedef T_BTreeKey<DDateTime>  DateTimeBTreeKey;
typedef T_BTreeKey<DHiresTime> HiresTimeBTreeKey;
typedef T_BTreeKey<DReal>      RealBTreeKey;
typedef T_BTreeKey<DRichReal>  RichRealBTreeKey;



class IBTreeFieldIndexNode : public IBTreeNode
{
public:
   IBTreeFieldIndexNode (IBTreeNodeManager&   nodesManager,
                         const NODE_INDEX     node)
     : IBTreeNode (nodesManager, node)
   {
   }

   virtual void GetRows (KEY_INDEX    fromPos,
                         KEY_INDEX    toPos,
                         DArray&      output) const = 0;
};


template <class DBS_T, class T, uint_t T_SIZE>
class DBS_BTreeNode : public IBTreeFieldIndexNode
{
public:
  DBS_BTreeNode (IBTreeNodeManager& nodesManager, const NODE_INDEX node)
    : IBTreeFieldIndexNode (nodesManager, node)
  {
  }

  virtual uint_t KeysPerNode () const
  {
    assert (sizeof (NodeHeader) % 16 == 0);

    uint_t result = mNodesMgr.NodeRawSize () - sizeof (NodeHeader);

    if (IsLeaf())
      result /= sizeof (ROW_INDEX) + T_SIZE;

    else
      result /= sizeof (ROW_INDEX) + sizeof (NODE_INDEX) + T_SIZE;

    return result;
  }

  virtual KEY_INDEX GetParentKeyIndex (const IBTreeNode& parent) const
  {
    assert (KeysCount () > 0);

    KEY_INDEX result = ~0;

    parent.FindBiggerOrEqual (GetKey (0), &result);

    assert (parent.IsEqual (GetKey (0), result));
    assert (NodeId () == parent.NodeIdOfKey (result));

    return result;
  }

  virtual NODE_INDEX NodeIdOfKey (const KEY_INDEX keyIndex) const
  {
    assert (keyIndex < KeysCount ());
    assert (sizeof (NodeHeader) % sizeof (uint64_t) == 0);
    assert (IsLeaf () == false);

    const ROW_INDEX* const  rows       = _RC (const ROW_INDEX*, DataForRead ());
    const NODE_INDEX* const childNodes = _RC (const NODE_INDEX*,
                                              rows + KeysPerNode ());

    return Serializer::LoadNode (childNodes + keyIndex);
  }

  virtual void SetNodeOfKey (const KEY_INDEX  keyIndex,
                             const NODE_INDEX childNode)
  {
    assert (keyIndex < KeysCount ());
    assert (sizeof (NodeHeader) % sizeof (uint64_t) == 0);
    assert (IsLeaf () == false);

    ROW_INDEX* const  rows       = _RC (ROW_INDEX*, DataForWrite ());
    NODE_INDEX* const childNodes = _RC (NODE_INDEX*, rows + KeysPerNode ());

    Serializer::StoreNode (childNode, childNodes + keyIndex);
  }

  virtual void AdjustKeyNode (const IBTreeNode&  childNode,
                              const KEY_INDEX    keyIndex)
  {
    const DBS_BTreeNode& node = _SC (const DBS_BTreeNode&, childNode);

    const T_BTreeKey<DBS_T> zeroKey  = node.GetKey (0);

    if (zeroKey.mValuePart.IsNull ())
      {
        if (GetKey (keyIndex).mValuePart.IsNull () == false)
          NullKeysCount (NullKeysCount () + 1);
      }
    else
      {
        if (GetKey (keyIndex).mValuePart.IsNull ())
          NullKeysCount (NullKeysCount () - 1);
      }

    SetKey (zeroKey, keyIndex);
  }

  virtual KEY_INDEX InsertKey (const IBTreeKey& key)
  {
    assert (sizeof (NodeHeader) % sizeof (uint64_t) == 0);

    const T_BTreeKey<DBS_T>& theKey   = _SC (const T_BTreeKey<DBS_T>&, key);
    KEY_INDEX                keyIndex = ~0;

    if (KeysCount () == 0)
      {
        KeysCount (1);

        if (theKey.mValuePart.IsNull ())
          NullKeysCount (1);

        SetKey (theKey, 0);

        return 0;
      }
    else if (FindBiggerOrEqual (key, &keyIndex) == false)
      keyIndex = 0;

    else
      ++keyIndex;

    ROW_INDEX* const rows    = _RC (ROW_INDEX*, DataForWrite ());
    const KEY_INDEX  lastKey = KeysCount () - 1;

    if (IsLeaf () == false)
      {
        NODE_INDEX* const childNodes = _RC (NODE_INDEX*, rows + KeysPerNode ());

        uint8_t* const values = _RC (uint8_t*, childNodes + KeysPerNode ());

        make_array_room (lastKey,
                         keyIndex,
                         sizeof (ROW_INDEX),
                         _RC (uint8_t*, rows));

        make_array_room (lastKey,
                         keyIndex,
                         sizeof (NODE_INDEX),
                         _RC (uint8_t*, childNodes));

        make_array_room (lastKey, keyIndex, T_SIZE, values);
      }
    else
      {
        uint8_t* const values = _RC (uint8_t*, rows + KeysPerNode ());

        make_array_room (lastKey,
                         keyIndex,
                         sizeof (ROW_INDEX),
                         _RC (uint8_t*, rows));

        make_array_room (lastKey, keyIndex, T_SIZE, values);
      }

    if (theKey.mValuePart.IsNull ())
      NullKeysCount (NullKeysCount () + 1);

    KeysCount (KeysCount () + 1);
    SetKey (theKey, keyIndex);

    return keyIndex;
  }

  virtual void RemoveKey (const KEY_INDEX keyIndex)
  {
    assert (keyIndex < KeysCount ());
    assert (sizeof (NodeHeader) % sizeof (uint64_t) == 0);

    const uint_t     lastKey = KeysCount () - 1;
    ROW_INDEX* const rows    = _RC (ROW_INDEX*, DataForWrite ());

    if (IsLeaf () == false)
      {
        NODE_INDEX* const childNodes = _RC (NODE_INDEX*, rows + KeysPerNode ());

        uint8_t* const values = _RC (uint8_t*, childNodes + KeysPerNode ());

        remove_array_elemes (lastKey,
                             keyIndex,
                             sizeof (ROW_INDEX),
                             _RC (uint8_t*, rows));

        remove_array_elemes (lastKey,
                             keyIndex,
                             sizeof (NODE_INDEX),
                             _RC (uint8_t*, childNodes));

        remove_array_elemes (lastKey, keyIndex, T_SIZE, values);
      }
    else
      {
         uint8_t* const values = _RC (uint8_t*, rows + KeysPerNode ());

         remove_array_elemes (lastKey,
                              keyIndex,
                              sizeof (ROW_INDEX),
                              _RC (uint8_t*, rows));

         remove_array_elemes (lastKey, keyIndex, T_SIZE, values);
      }

    if (keyIndex >= (KeysCount () - NullKeysCount ()))
      NullKeysCount (NullKeysCount () - 1);

    KeysCount (KeysCount () - 1);
  }

  virtual void Split (const NODE_INDEX parent)
  {
    assert (NeedsSpliting ());

    const KEY_INDEX         splitKeyIndex = KeysCount () / 2;
    const T_BTreeKey<DBS_T> splitKey      = GetKey (splitKeyIndex);

    BTreeNodeRAII parentNode (mNodesMgr.RetrieveNode (parent));

    const KEY_INDEX  insertPosition  = parentNode->InsertKey (splitKey);
    const NODE_INDEX allocatedNodeId = mNodesMgr.AllocateNode (parent,
                                                               insertPosition);
    BTreeNodeRAII allocatedNode (mNodesMgr.RetrieveNode (allocatedNodeId));

    allocatedNode->Leaf (IsLeaf ());
    allocatedNode->MarkAsUsed ();
    allocatedNode->KeysCount (KeysCount () - splitKeyIndex);

    if (KeysCount () - NullKeysCount () < splitKeyIndex)
      allocatedNode->NullKeysCount (KeysCount () - splitKeyIndex);

    else
      allocatedNode->NullKeysCount (NullKeysCount ());

    assert (NullKeysCount () <= KeysCount ());
    assert (allocatedNode->NullKeysCount () <= allocatedNode->NullKeysCount ());

    for (KEY_INDEX index = splitKeyIndex; index < KeysCount (); ++index)
      {
        _SC (DBS_BTreeNode*, &(*allocatedNode))->SetKey (GetKey (index),
                                                         index - splitKeyIndex);
      }

    if ( ! IsLeaf ())
      {
        for (KEY_INDEX index = splitKeyIndex; index < KeysCount (); ++index)
          {
            _SC (DBS_BTreeNode*, &(*allocatedNode))->SetNodeOfKey (
                                                        index - splitKeyIndex,
                                                        NodeIdOfKey (index)
                                                                  );
          }
      }

    NullKeysCount (NullKeysCount () - allocatedNode->NullKeysCount ());
    KeysCount (splitKeyIndex);

    assert (NullKeysCount () <= KeysCount ());

    allocatedNode->Next (NodeId());
    allocatedNode->Prev (Prev());
    Prev (allocatedNodeId);
    if (allocatedNode->Prev() != NIL_NODE)
      {
        BTreeNodeRAII prevNode (
            mNodesMgr.RetrieveNode (allocatedNode->Prev())
                              );
        prevNode->Next (allocatedNodeId);
      }
  }

  virtual void Join (const bool toRight)
  {
    if (toRight)
      {
        assert (Next () != NIL_NODE);

        BTreeNodeRAII next (mNodesMgr.RetrieveNode (Next ()));

        DBS_BTreeNode* const nextNode     = _SC (DBS_BTreeNode*, &(*next));
        const KEY_INDEX      oldKeysCount = nextNode->KeysCount ();

        assert ((nextNode->NullKeysCount() == 0) ||
            ( KeysCount () == NullKeysCount ()));

        nextNode->KeysCount (oldKeysCount + KeysCount ());
        nextNode->NullKeysCount (nextNode->NullKeysCount () + NullKeysCount ());

        for (KEY_INDEX index = 0; index < KeysCount (); ++index)
          nextNode->SetKey (GetKey (index), index + oldKeysCount);

        if (IsLeaf () == false)
          {
            for (KEY_INDEX index = 0; index < KeysCount (); ++index)
              {
                nextNode->SetNodeOfKey (index + oldKeysCount,
                                        NodeIdOfKey (index));
              }
          }

        nextNode->Prev (Prev ());
        if (Prev () != NIL_NODE)
          {
            BTreeNodeRAII prev (mNodesMgr.RetrieveNode (Prev ()));
            prev->Next (Next ());
          }

        assert (nextNode->NullKeysCount () <= nextNode->KeysCount ());
        assert (nextNode->KeysCount () <= nextNode->KeysPerNode());
      }
    else
      {
        assert (Prev () != NIL_NODE);

        BTreeNodeRAII prev (mNodesMgr.RetrieveNode (Prev ()));

        DBS_BTreeNode* const prevNode     = _SC (DBS_BTreeNode*, &(*prev));
        const KEY_INDEX      oldKeysCount = KeysCount ();

        KeysCount (oldKeysCount + prevNode->KeysCount ());
        NullKeysCount (NullKeysCount () + prevNode->NullKeysCount());

        for (KEY_INDEX index = 0; index < prevNode->KeysCount (); ++index)
          SetKey (prevNode->GetKey (index), index + oldKeysCount);

        if (IsLeaf () == false)
          {
            for (KEY_INDEX index = 0; index < prevNode->KeysCount (); ++index)
              {
                SetNodeOfKey (index + oldKeysCount,
                              prevNode->NodeIdOfKey (index));
              }
          }

        Prev (prev->Prev ());
        if (Prev () != NIL_NODE)
          {
            BTreeNodeRAII prev (mNodesMgr.RetrieveNode (Prev ()));

            prev->Next (NodeId ());
          }

        assert (NullKeysCount () <= KeysCount() );
        assert (KeysCount () <= KeysPerNode());
      }
  }

  virtual bool IsLess (const IBTreeKey& key, const KEY_INDEX keyIndex) const
  {
    assert (keyIndex  < KeysCount ());

    const T_BTreeKey<DBS_T> &theKey = _SC (const T_BTreeKey<DBS_T>&, key);

    return theKey < GetKey (keyIndex);
  }

  virtual bool IsEqual (const IBTreeKey& key, const KEY_INDEX keyIndex) const
  {
    assert (keyIndex  < KeysCount ());

    const T_BTreeKey<DBS_T> &theKey = _SC (const T_BTreeKey<DBS_T>&, key);

    return theKey == GetKey (keyIndex);
  }

  virtual bool IsBigger (const IBTreeKey& key, const KEY_INDEX keyIndex) const
  {
    assert (keyIndex  < KeysCount ());

    const T_BTreeKey<DBS_T> &theKey = _SC (const T_BTreeKey<DBS_T>&, key);

    return theKey > GetKey (keyIndex);
  }

  virtual const IBTreeKey& SentinelKey () const
  {
    static T_BTreeKey<DBS_T> _sentinel (DBS_T::Max (), ~0ull);

    return _sentinel;
  }

  virtual void GetRows (KEY_INDEX fromPos,
                        KEY_INDEX toPos,
                        DArray&   output) const
  {
    assert (fromPos >= toPos);
    assert (fromPos < KeysCount ());

    const ROW_INDEX* const rows = _RC (const ROW_INDEX*, DataForRead ());

    if ((toPos == 0) && IsEqual (SentinelKey (), toPos))
      ++toPos;

    while (fromPos >= toPos)
      {
        if (sizeof (ROW_INDEX) == 8)
          output.Add (DUInt64 (Serializer::LoadRow (rows + fromPos)));

        else if (sizeof (ROW_INDEX) == 4)
          output.Add (DUInt32 (Serializer::LoadRow (rows + fromPos)));

        else
          {
            assert (false);
          }

        if (fromPos == 0)
          break;

        fromPos--;
      }
  }

private:
  const T_BTreeKey<DBS_T> GetKey (const KEY_INDEX keyIndex) const
  {
    assert (keyIndex < KeysCount ());
    assert (NullKeysCount () <= KeysCount ());

    const ROW_INDEX *const rows = _RC (const ROW_INDEX*, DataForRead ());

    if (keyIndex >= KeysCount() - NullKeysCount ())
      {
        return T_BTreeKey<DBS_T> (DBS_T (),
                                  Serializer::LoadRow (rows + keyIndex));
      }

    DBS_T value;
    if (IsLeaf())
      {
        const uint8_t* const values = _RC (const uint8_t*,
                                           rows + KeysPerNode ());
        Serializer::Load (values + keyIndex * T_SIZE, &value);
      }
    else
      {
        const NODE_INDEX* const childNodes = _RC (const NODE_INDEX*,
                                                  rows + KeysPerNode ());

        const uint8_t* const values = _RC (const uint8_t*,
                                           childNodes + KeysPerNode ());

        Serializer::Load (values + keyIndex * T_SIZE, &value);
      }


    return T_BTreeKey<DBS_T> (value, Serializer::LoadRow (rows + keyIndex));
  }

  void SetKey (const T_BTreeKey<DBS_T> &key, const KEY_INDEX keyIndex)
  {
    assert (keyIndex < KeysCount ());
    assert (NullKeysCount () <= KeysCount ());

    ROW_INDEX *const rows = _RC (ROW_INDEX*, DataForWrite ());

    if (keyIndex < KeysCount() - NullKeysCount ())
      {
        if (IsLeaf ())
          {
            uint8_t* const values = _RC (uint8_t*, rows + KeysPerNode ());

            Serializer::Store (values + keyIndex * T_SIZE, key.mValuePart);
          }
        else
          {
            NODE_INDEX* const childNodes = _RC (NODE_INDEX*,
                                                rows + KeysPerNode ());

            uint8_t* const values = _RC (uint8_t*, childNodes + KeysPerNode ());

            Serializer::Store (values + keyIndex * T_SIZE, key.mValuePart);
          }
      }

    Serializer::StoreRow (key.mRowPart, rows + keyIndex);
  }
};



typedef DBS_BTreeNode<DBool, bool, 1>             BoolBTreeNode;
typedef DBS_BTreeNode<DChar, uint32_t, 4>         CharBTreeNode;
typedef DBS_BTreeNode<DDate, void, 4>             DateBTreeNode;
typedef DBS_BTreeNode<DDateTime, void, 7>         DateTimeBTreeNode;
typedef DBS_BTreeNode<DHiresTime, void, 11>       HiresTimeBTreeNode;
typedef DBS_BTreeNode<DUInt8, uint8_t, 1>         UInt8BTreeNode;
typedef DBS_BTreeNode<DUInt16, uint16_t, 2>       UInt16BTreeNode;
typedef DBS_BTreeNode<DUInt32, uint32_t, 4>       UInt32BTreeNode;
typedef DBS_BTreeNode<DUInt64, uint64_t, 8>       UInt64BTreeNode;
typedef DBS_BTreeNode<DInt8, int8_t, 1>           Int8BTreeNode;
typedef DBS_BTreeNode<DInt16, int16_t, 2>         Int16BTreeNode;
typedef DBS_BTreeNode<DInt32, int32_t, 4>         Int32BTreeNode;
typedef DBS_BTreeNode<DInt64, int64_t, 8>         Int64BTreeNode;
typedef DBS_BTreeNode<DReal, REAL_T, 8>           RealBTreeNode;
typedef DBS_BTreeNode<DRichReal, RICHREAL_T, 14>  RichRealBTreeNode;



class FieldIndexNodeManager : public IBTreeNodeManager
{
public:
  FieldIndexNodeManager (std::auto_ptr<IDataContainer>& container,
                         const uint_t                   nodeSize,
                         const uint_t                   maxCacheMem,
                         const DBS_FIELD_TYPE           nodeType,
                         const bool                     create);
  virtual ~FieldIndexNodeManager();

  void MarkForRemoval ();

  uint64_t IndexRawSize () const;

  virtual uint64_t NodeRawSize () const;

  virtual NODE_INDEX AllocateNode (const NODE_INDEX parent,
                                   const KEY_INDEX  parentKey);

  virtual void FreeNode (const NODE_INDEX nodeId);

  virtual NODE_INDEX RootNodeId ();

  virtual void RootNodeId (const NODE_INDEX nodeId);

protected:
  virtual uint_t MaxCachedNodes ();

  virtual IBTreeNode* LoadNode (const NODE_INDEX nodeId);

  virtual void SaveNode (IBTreeNode* const nodeId);

  void InitContainer ();

  void UpdateContainer ();

  void InitFromContainer ();

  IBTreeNode* NodeFactory (const NODE_INDEX nodeId);


  const uint_t                          mNodeSize;
  const uint_t                          mMaxCachedMem;
  NODE_INDEX                            mRootNode;
  NODE_INDEX                            mFirstFreeNode;
  std::auto_ptr<IDataContainer>         mContainer;
  const DBS_FIELD_TYPE                  mFieldType;

private:
  FieldIndexNodeManager (const FieldIndexNodeManager&);
  FieldIndexNodeManager& operator= (const FieldIndexNodeManager&);
};

} //namespace pastra
} //namespace whisper

#endif /* PS_BTREE_FIELDS_H_ */

