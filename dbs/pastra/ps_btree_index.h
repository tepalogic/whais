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
#ifndef PS_BTREE_INDEX_H_
#define PS_BTREE_INDEX_H_

#include <vector>

#include "whisper.h"

#include "utils/include/wthread.h"

namespace pastra
{

class I_BTreeKey
{
};

class I_BTreeNodeManager;

typedef uint32_t NODE_INDEX;
typedef uint_t   KEY_INDEX;

static const NODE_INDEX NIL_NODE = ~0;

class I_BTreeNode
{
public:
  I_BTreeNode (I_BTreeNodeManager& nodesManager, const NODE_INDEX node);
  virtual ~I_BTreeNode ();

  bool       IsLeaf () const { return m_Header->m_Leaf != 0; }
  bool       IsDirty () const { return m_Header->m_Dirty != 0; }
  bool       IsRemoved() const { return m_Header->m_Removed != 0; };
  NODE_INDEX NodeId () const { return m_Header->m_NodeId; }
  NODE_INDEX GetNext () const { return m_Header->m_Right; }
  NODE_INDEX GetPrev () const { return m_Header->m_Left; }
  uint16_t   GetNullKeysCount () const { return m_Header->m_NullKeysCount; };

  const uint8_t*  DataToRead () const
  {
    return _RC (const uint8_t*, (m_Header.get () + 1));
  }
  uint8_t*        DataToWrite ()
  {
    MarkDirty ();
    return _RC (uint8_t*, (m_Header.get () + 1));
  }
  uint8_t*  RawData () const
  {
    return _RC (uint8_t*, m_Header.get ());
  }

  void MarkDirty () {  m_Header->m_Dirty = 1; }
  void MarkClean () {  m_Header->m_Dirty = 0; }
  void SetLeaf (bool leaf) { m_Header->m_Leaf = (leaf == false) ? 0 : 1; }
  void MarkAsRemoved() { m_Header->m_Removed = 1; MarkDirty (); }
  void MarkAsUsed() { m_Header->m_Removed = 0; MarkDirty (); };
  void SetNext (const NODE_INDEX next) { m_Header->m_Right = next; MarkDirty (); }
  void SetPrev (const NODE_INDEX prev) { m_Header->m_Left = prev; MarkDirty ();; }
  void SetNullKeysCount (const uint16_t count)
  {
    m_Header->m_NullKeysCount = count;
    MarkDirty ();
  };

  uint_t GetKeysCount () const { return m_Header->m_KeysCount; }
  void   SetKeysCount (uint_t count)
  {
    m_Header->m_KeysCount = count;
    MarkDirty ();
  }

  virtual uint_t KeysPerNode () const = 0;

  virtual bool NeedsSpliting () const;
  virtual bool NeedsJoining () const;


  virtual KEY_INDEX  GetParentKeyIndex (const I_BTreeNode& parent) const = 0;
  virtual NODE_INDEX GetChildNode (const I_BTreeKey& key) const;
  virtual NODE_INDEX GetNodeOfKey (const KEY_INDEX keyIndex) const = 0;
  virtual void       AdjustKeyNode (const I_BTreeNode& childNode,
                                    const KEY_INDEX    keyIndex) = 0;
  virtual void       SetNodeOfKey (const KEY_INDEX keyIndex, const NODE_INDEX childNode) = 0;
  virtual void       SetData (const KEY_INDEX keyIndex, const uint8_t *data);

  virtual KEY_INDEX InsertKey (const I_BTreeKey& key) = 0;
  void              RemoveKey (const I_BTreeKey& key);
  virtual void      RemoveKey (const KEY_INDEX keyIndex) = 0;

  virtual void Split (const NODE_INDEX parentId) = 0;
  virtual void Join (bool toRight) = 0;

  virtual bool IsLess (const I_BTreeKey& key, KEY_INDEX keyIndex) const = 0;
  virtual bool IsEqual (const I_BTreeKey& key, KEY_INDEX keyIndex) const = 0;
  virtual bool IsBigger (const I_BTreeKey& key, KEY_INDEX keyIndex) const = 0;

  virtual const I_BTreeKey& SentinelKey () const = 0;

  bool   FindBiggerOrEqual (const I_BTreeKey& key, KEY_INDEX& outIndex) const;
  void   Release ();

protected:
  struct NodeHeader
  {
    uint64_t  m_Left          : 32;
    uint64_t  m_Right         : 32;
    uint64_t  m_NodeId        : 32;
    uint64_t  m_KeysCount     : 16;
    uint64_t  m_NullKeysCount : 16;
    uint64_t  m_Leaf          : 1;
    uint64_t  m_Dirty         : 1;
    uint64_t  m_Removed       : 1;
    uint64_t  _reserved       : 61; //To make sure this is aligned well
    uint64_t  _unused;
  };

  I_BTreeNodeManager&       m_NodesMgr;

private:
  std::auto_ptr<NodeHeader> m_Header;
};

class BTreeNodeRAI
{
public:
  BTreeNodeRAI (I_BTreeNode* node) :
    m_pTreeNode (node)
  {}

  BTreeNodeRAI (I_BTreeNode& node) :
    m_pTreeNode (&node)
  {}

  ~BTreeNodeRAI ()
  {
    m_pTreeNode->Release ();
  }

  void operator= (I_BTreeNode& node)
  {
    m_pTreeNode->Release ();
    m_pTreeNode = &node;
  }

  void operator= (I_BTreeNode* node)
    {
      m_pTreeNode->Release ();
      m_pTreeNode = node;
    }

  I_BTreeNode* operator-> ()
    {
      return m_pTreeNode;
    }

  operator I_BTreeNode * ()
    {
      return m_pTreeNode;
    }

private:
  BTreeNodeRAI (const BTreeNodeRAI &);
  BTreeNodeRAI& operator= (const BTreeNodeRAI &);

  I_BTreeNode* m_pTreeNode;
};

class I_BTreeNodeManager
{
public:
  I_BTreeNodeManager ();
  virtual ~I_BTreeNodeManager ();

  void Split (NODE_INDEX parentId, const NODE_INDEX nodeId);
  void Join  (const NODE_INDEX parentId, const NODE_INDEX nodeId);

  I_BTreeNode* RetrieveNode (const NODE_INDEX node);
  void         ReleaseNode (const NODE_INDEX node);
  void         ReleaseNode (I_BTreeNode* pNode) { ReleaseNode (pNode->NodeId()); }
  void         FlushNodes ();

  virtual uint64_t   RawNodeSize () const = 0;
  virtual NODE_INDEX AllocateNode (const NODE_INDEX parent,
                                   const KEY_INDEX  parentKey) = 0;
  virtual void       FreeNode (const NODE_INDEX node) = 0;

  virtual NODE_INDEX  GetRootNodeId () = 0;
  virtual void        SetRootNodeId (const NODE_INDEX node) = 0;

protected:
  struct CachedData
  {
    CachedData (I_BTreeNode* pNode)
      : m_pNode (pNode),
        m_RefsCount (0)
    {
    }

    I_BTreeNode* m_pNode;
    uint_t       m_RefsCount;
  };

  virtual uint_t       MaxCachedNodes () = 0;
  virtual I_BTreeNode* LoadNode (const NODE_INDEX node) = 0;
  virtual void         SaveNode (I_BTreeNode* const pNode) = 0;

  WSynchronizer                     m_Sync;
  std::map <NODE_INDEX, CachedData> m_NodesKeeper;
};

class BTree
{
public:
  BTree (I_BTreeNodeManager& nodesManager);
  ~BTree ();

  bool FindBiggerOrEqual (const I_BTreeKey& key,
                          NODE_INDEX& outNode,
                          KEY_INDEX  &outKeyIndex);

  void InsertKey (const I_BTreeKey& key,
                  NODE_INDEX&       outNode,
                  KEY_INDEX&        outKeyIndex);
  void RemoveKey (const I_BTreeKey& key);

private:
  bool RecursiveInsertNodeKey (const NODE_INDEX parentId,
                               const NODE_INDEX nodeId,
                               const I_BTreeKey& key,
                               NODE_INDEX       &outNode,
                               KEY_INDEX        &outKeyIndex);
  bool RecursiveDeleteNodeKey (I_BTreeNode& node, const I_BTreeKey& key);

  I_BTreeNodeManager& m_NodesManager;
};


static inline void
make_array_room (uint8_t* const pArray,
                 const uint_t   lastIndex,
                 const uint_t   fromIndex,
                 const uint_t   elemSize)
{
  uint_t lastPos = lastIndex * elemSize + elemSize - 1;
  uint_t fromPos = fromIndex * elemSize;

  while (lastPos >= fromPos)
    {
      pArray[lastPos + elemSize] = pArray[lastPos];
      --lastPos;
    }
}

static inline void
remove_array_elemes (uint8_t* const pArray,
                     const uint_t   lastIndex,
                     const uint_t   fromIndex,
                     const uint_t   elemSize)
{
  uint_t lastPos = lastIndex  * elemSize + elemSize - 1;
  uint_t fromPos = fromIndex  * elemSize;

  while (fromPos + elemSize <= lastPos)
    {
      pArray[fromPos] = pArray[fromPos + elemSize];
      ++fromPos;
    }
}

}

#endif /* PS_BTREE_INDEX_H_ */
