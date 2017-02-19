/******************************************************************************
 PASTRA - A light database one file system and more.
 Copyright(C) 2008  Iulian Popa

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
#ifndef PS_BTREE_INDEX_H_
#define PS_BTREE_INDEX_H_

#include <vector>

#include "whais.h"

#include "utils/wthread.h"

#include "ps_serializer.h"


namespace whais {
namespace pastra {



class IBTreeKey
{
  /* Empty class to be used for key access uniformization. */
};


class IBTreeNodeManager;


typedef uint_t KEY_INDEX;


static const NODE_INDEX NIL_NODE = ~0;



class IBTreeNode
{
public:
  IBTreeNode(IBTreeNodeManager&   nodesManager,
              const NODE_INDEX     nodeId);

  virtual ~IBTreeNode();

  bool IsLeaf() const
  {
    return mHeader->mLeaf != 0;
  }

  bool IsDirty() const
  {
    return mHeader->mDirty != 0;
  }

  bool IsRemoved() const
  {
    return mHeader->mRemoved != 0;
  }

  NODE_INDEX NodeId() const
  {
    return Serializer::LoadNode(&mHeader->mNodeId);
  }

  NODE_INDEX Next() const
  {
    return Serializer::LoadNode(&mHeader->mRight);
  }

  void Next(const NODE_INDEX next)
  {
    Serializer::StoreNode(next, &mHeader->mRight);
    MarkDirty();
  }

  NODE_INDEX Prev() const
  {
    return Serializer::LoadNode(&mHeader->mLeft);
  }

  void Prev(const NODE_INDEX prev)
  {
    Serializer::StoreNode(prev, &mHeader->mLeft);
    MarkDirty();
  }

  const uint8_t* DataForRead() const
  {
    return _RC(const uint8_t*, (mHeader.get() + 1));
  }

  uint8_t* DataForWrite()
  {
    MarkDirty();

    return _RC(uint8_t*, (mHeader.get() + 1));
  }

  uint8_t* RawData() const
  {
    return _RC(uint8_t*, mHeader.get());
  }

  void MarkDirty()
  {
    mHeader->mDirty = 1;
  }

  void MarkClean()
  {
    mHeader->mDirty = 0;
  }

  void Leaf(bool leaf)
  {
    mHeader->mLeaf = (leaf == false) ? 0 : 1;
  }

  void MarkAsRemoved()
  {
    mHeader->mRemoved = 1;
    MarkDirty();
  }

  void MarkAsUsed()
  {
    mHeader->mRemoved = 0;
    MarkDirty();
  }

  uint16_t NullKeysCount() const
  {
    return load_le_int16(mHeader->mNullKeysCount);
  }

  void NullKeysCount(const uint_t count)
  {
    store_le_int16(count, mHeader->mNullKeysCount);
    MarkDirty();
  }

  uint_t KeysCount() const
  {
    return load_le_int16(mHeader->mKeysCount);
  }

  void KeysCount(const uint_t count)
  {
    store_le_int16(count, mHeader->mKeysCount);
    MarkDirty();
  }

  virtual uint_t KeysPerNode() const = 0;

  virtual bool NeedsSpliting() const;

  virtual bool NeedsJoining() const;


  virtual KEY_INDEX GetParentKeyIndex(const IBTreeNode& parent) const = 0;

  virtual NODE_INDEX GetChildNode(const IBTreeKey& key) const;

  virtual NODE_INDEX NodeIdOfKey(const KEY_INDEX keyIndex) const = 0;

  virtual void AdjustKeyNode(const IBTreeNode& childNode,
                              const KEY_INDEX    keyIndex) = 0;

  virtual void SetNodeOfKey(const KEY_INDEX keyIndex,
                             const NODE_INDEX childNode) = 0;

  virtual void SetData(const KEY_INDEX keyIndex, const uint8_t *data);

  virtual KEY_INDEX InsertKey(const IBTreeKey& key) = 0;

  void RemoveKey(const IBTreeKey& key);

  virtual void RemoveKey(const KEY_INDEX keyIndex) = 0;

  virtual void Split(const NODE_INDEX parentId) = 0;

  virtual void Join(const bool toRight) = 0;

  virtual int CompareKey(const IBTreeKey&   key,
                          const KEY_INDEX    nodeKeyIndex) const = 0;

  virtual const IBTreeKey& SentinelKey() const = 0;

  bool   FindBiggerOrEqual(const IBTreeKey&  key,
                            KEY_INDEX* const  outIndex) const;

  void   Release();

protected:
  struct NodeHeader
  {
    NODE_INDEX   mLeft;
    NODE_INDEX   mRight;
    NODE_INDEX   mNodeId;
    uint8_t      mKeysCount[2];
    uint8_t      mNullKeysCount[2];

    uint8_t      mLeaf;
    uint8_t      mDirty;
    uint8_t      mRemoved;
    uint8_t      _unused[13]; //Leave space to align at 128 bit boundary.
  };

  IBTreeNodeManager&       mNodesMgr;
  const uint_t             mRawNodeSize;

private:
  std::unique_ptr<NodeHeader> mHeader;
};



class BTreeNodeRAII
{
public:
  BTreeNodeRAII(IBTreeNode* node)
    : mTreeNode(node)
  {
  }

  BTreeNodeRAII(IBTreeNode& node)
    : mTreeNode(&node)
  {
  }

  ~BTreeNodeRAII()
  {
    mTreeNode->Release();
  }

  void operator= (IBTreeNode& node)
  {
    mTreeNode->Release();
    mTreeNode = &node;
  }

  void operator= (IBTreeNode* node)
  {
    mTreeNode->Release();
    mTreeNode = node;
  }

  IBTreeNode* operator-> ()
  {
    return mTreeNode;
  }

  operator IBTreeNode* ()
  {
    return mTreeNode;
  }

private:
  BTreeNodeRAII(const BTreeNodeRAII &);
  BTreeNodeRAII& operator= (const BTreeNodeRAII &);


  IBTreeNode* mTreeNode;
};



class IBTreeNodeManager
{
public:
  IBTreeNodeManager();
  virtual ~IBTreeNodeManager();

  void Split(NODE_INDEX parentId, const NODE_INDEX nodeId);

  void Join(const NODE_INDEX parentId, const NODE_INDEX nodeId);

  IBTreeNode* RetrieveNode(const NODE_INDEX nodeId);

  void ReleaseNode(const NODE_INDEX nodeId);

  void ReleaseNode(IBTreeNode* const node)
  {
    ReleaseNode(node->NodeId());
  }

  void FlushNodes();

  virtual uint64_t NodeRawSize() const = 0;

  virtual NODE_INDEX AllocateNode(const NODE_INDEX parent,
                                   const KEY_INDEX  parentKey) = 0;

  virtual void FreeNode(const NODE_INDEX nodeId) = 0;

  virtual NODE_INDEX RootNodeId() = 0;

  virtual void RootNodeId(const NODE_INDEX nodeId) = 0;

protected:
  struct CachedData
  {
    CachedData(IBTreeNode* const node)
      : mNode(node),
        mRefsCount(0)
    {
    }

    IBTreeNode*  mNode;
    uint_t       mRefsCount;
  };


  virtual uint_t MaxCachedNodes() = 0;

  virtual IBTreeNode* LoadNode(const NODE_INDEX nodeId) = 0;

  virtual void SaveNode(IBTreeNode* const node) = 0;


  Lock                             mSync;
  std::map<NODE_INDEX, CachedData> mNodesKeeper;
};



class BTree
{
public:
  BTree(IBTreeNodeManager& nodesManager);

  bool FindBiggerOrEqual(const IBTreeKey&    key,
                          NODE_INDEX*         outNode,
                          KEY_INDEX*          outKeyIndex);

  void InsertKey(const IBTreeKey&  key,
                  NODE_INDEX*       outNode,
                  KEY_INDEX*        outKeyIndex);

  void RemoveKey(const IBTreeKey& key);

private:
  bool RecursiveInsertNodeKey(const NODE_INDEX parentId,
                               const NODE_INDEX nodeId,
                               const IBTreeKey& key,
                               NODE_INDEX*      outNode,
                               KEY_INDEX*       outKeyIndex);

  bool RecursiveDeleteNodeKey(IBTreeNode& nodeId, const IBTreeKey& key);


  IBTreeNodeManager& mNodesManager;
};


static inline void
make_array_room(const uint_t   lastIndex,
                 const uint_t   fromIndex,
                 const uint_t   elemSize,
                 uint8_t* const array)
{
  uint_t lastPos = lastIndex * elemSize + elemSize - 1;
  uint_t fromPos = fromIndex * elemSize;

  while (lastPos >= fromPos)
    {
      array[lastPos + elemSize] = array[lastPos];
      --lastPos;
    }
}


static inline void
remove_array_elemes(const uint_t   lastIndex,
                     const uint_t   fromIndex,
                     const uint_t   elemSize,
                     uint8_t* const array)
{
  uint_t lastPos = lastIndex  * elemSize + elemSize - 1;
  uint_t fromPos = fromIndex  * elemSize;

  while (fromPos + elemSize <= lastPos)
    {
      array[fromPos] = array[fromPos + elemSize];
      ++fromPos;
    }
}

} //namespace pastra
} //namespace whais

#endif /* PS_BTREE_INDEX_H_ */
