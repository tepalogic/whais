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

typedef D_UINT32 NODE_INDEX;
typedef D_UINT   KEY_INDEX;

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
  D_UINT16   GetNullKeysCount () const { return m_Header->m_NullKeysCount; };

  const D_UINT8*  DataToRead () const
  {
    return _RC (const D_UINT8*, (m_Header.get () + 1));
  }
  D_UINT8*        DataToWrite ()
  {
    MarkDirty ();
    return _RC (D_UINT8*, (m_Header.get () + 1));
  }
  D_UINT8*  RawData () const
  {
    return _RC (D_UINT8*, m_Header.get ());
  }

  void MarkDirty () {  m_Header->m_Dirty = 1; }
  void MarkClean () {  m_Header->m_Dirty = 0; }
  void SetLeaf (bool leaf) { m_Header->m_Leaf = (leaf == false) ? 0 : 1; }
  void MarkAsRemoved() { m_Header->m_Removed = 1; MarkDirty (); }
  void MarkAsUsed() { m_Header->m_Removed = 0; MarkDirty (); };
  void SetNext (const NODE_INDEX next) { m_Header->m_Right = next; MarkDirty (); }
  void SetPrev (const NODE_INDEX prev) { m_Header->m_Left = prev; MarkDirty ();; }
  void SetNullKeysCount (const D_UINT16 count)
  {
    m_Header->m_NullKeysCount = count;
    MarkDirty ();
  };

  D_UINT GetKeysCount () const { return m_Header->m_KeysCount; }
  void   SetKeysCount (D_UINT count)
  {
    m_Header->m_KeysCount = count;
    MarkDirty ();
  }

  virtual D_UINT KeysPerNode () const = 0;

  virtual bool NeedsSpliting () const;
  virtual bool NeedsJoining () const;


  virtual KEY_INDEX  GetParentKeyIndex (const I_BTreeNode& parent) const = 0;
  virtual NODE_INDEX GetChildNode (const I_BTreeKey& key) const;
  virtual NODE_INDEX GetNodeOfKey (const KEY_INDEX keyIndex) const = 0;
  virtual void       AdjustKeyNode (const I_BTreeNode& childNode,
                                    const KEY_INDEX    keyIndex) = 0;
  virtual void       SetNodeOfKey (const KEY_INDEX keyIndex, const NODE_INDEX childNode) = 0;
  virtual void       SetData (const KEY_INDEX keyIndex, const D_UINT8 *data);

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
    D_UINT64  m_Left          : 32;
    D_UINT64  m_Right         : 32;
    D_UINT64  m_NodeId        : 32;
    D_UINT64  m_KeysCount     : 16;
    D_UINT64  m_NullKeysCount : 16;
    D_UINT64  m_Leaf          : 1;
    D_UINT64  m_Dirty         : 1;
    D_UINT64  m_Removed       : 1;
    D_UINT64  _reserved       : 61; //To make sure this is aligned well
    D_UINT64  _unused;
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

  virtual D_UINT64   RawNodeSize () const = 0;
  virtual NODE_INDEX AllocateNode (const NODE_INDEX parent, KEY_INDEX parentKey) = 0;
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
    D_UINT       m_RefsCount;
  };

  virtual D_UINT       MaxCachedNodes () = 0;
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


template <typename T> void
make_array_room (T* const pArray,
                 const D_UINT lastIndex,
                 const D_UINT fromIndex,
                 const D_UINT elemsCount)
{
  D_INT iterator = lastIndex; // Unsigned as we need to check against underflow.

  while (iterator >= _SC(D_INT, fromIndex))
    {
      pArray [iterator + elemsCount] = pArray [iterator];
      --iterator;
    }
}

template <typename T> void
remove_array_elemes (T* const pArray,
                     const D_UINT lastIndex,
                     const D_UINT fromIndex,
                     const D_UINT elemsCount)
{
  D_UINT iterator = fromIndex;

  while (iterator < lastIndex)
    {
      pArray [iterator] = pArray [iterator + elemsCount];
      ++iterator;
    }
}

}

#endif /* PS_BTREE_INDEX_H_ */
