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

#include "wsync.h"

namespace pastra
{

class I_BTreeKey;
class I_BTreeNodeManager;

typedef D_UINT64 NODE_INDEX;
typedef D_UINT   KEY_INDEX;

static const NODE_INDEX NIL_NODE = ~0;

class I_BTreeNode
{
public:
  I_BTreeNode (I_BTreeNodeManager &nodesManager, const NODE_INDEX nodeId);
  virtual ~I_BTreeNode ();

  bool       IsLeaf () { return m_Header->m_Depth == 0; }
  bool       IsDirty () { return m_Header->m_Dirty != 0; }
  NODE_INDEX GetNodeId () { return m_NodeIndex; }
  NODE_INDEX GetParrent () { return m_Header->m_Parrent; }
  NODE_INDEX GetNext () { return m_Header->m_Right; }
  NODE_INDEX GetPrev () { return m_Header->m_Left; }

  void SetParrent (const NODE_INDEX parrent) { m_Header->m_Parrent = parrent, m_Header->m_Dirty = 1; }
  void SetNext (const NODE_INDEX next) { m_Header->m_Right = next, m_Header->m_Dirty = 1; }
  void SetPrev (const NODE_INDEX prev) { m_Header->m_Left = prev, m_Header->m_Dirty = 1; }

  virtual D_UINT GetKeysPerNode () const = 0;
  virtual D_UINT GetKeysCount () const = 0;

  virtual bool NeedsSpliting () const = 0;
  virtual bool NeedsJoining () const = 0;

  virtual NODE_INDEX GetKeyNode (I_BTreeKey &key) const = 0;
  virtual NODE_INDEX GetKeyNode (const KEY_INDEX keyIndex) const = 0;

  virtual void SetChild (const KEY_INDEX keyIndex, const NODE_INDEX child) = 0;
  virtual void SetData (const KEY_INDEX keyIndex, const D_UINT *data) = 0;

  virtual KEY_INDEX InsertKey (I_BTreeKey &key) = 0;
  virtual void      RemoveKey (const KEY_INDEX keyIndex) = 0;

  virtual NODE_INDEX Split (I_BTreeNodeManager &nodesManager) = 0;
  virtual NODE_INDEX Join (I_BTreeNodeManager &nodesManager) = 0;

  virtual bool IsLess (const I_BTreeKey &key, KEY_INDEX keyIndex) = 0;
  virtual bool IsEqual (const I_BTreeKey &key, KEY_INDEX keyIndex) = 0;
  virtual bool IsBigger (const I_BTreeKey &key, KEY_INDEX keyIndex) = 0;

  bool FindBiggerOrEqual (I_BTreeKey &key, KEY_INDEX &outIndex);
  void Release ();

protected:
  struct NodeHeader
  {
    NODE_INDEX    m_Parrent;
    NODE_INDEX    m_Left;
    NODE_INDEX    m_Right;
    D_UINT        m_Depth : 8;
    D_UINT        m_Dirty : 1;
  };

  NodeHeader         *m_Header;
  I_BTreeNodeManager &m_NodesManager;
  const NODE_INDEX    m_NodeIndex;
};

class BTreeNodeHandler
{
public:
  BTreeNodeHandler (I_BTreeNode *node) :
    m_pTreeNode (node)
  {}

  BTreeNodeHandler (I_BTreeNode &node) :
    m_pTreeNode (&node)
  {}

  ~BTreeNodeHandler ()
  {
    m_pTreeNode->Release ();
  }

  void operator= (I_BTreeNode &node)
  {
    m_pTreeNode->Release ();
    m_pTreeNode = &node;
  }

  void operator= (I_BTreeNode *node)
    {
      m_pTreeNode->Release ();
      m_pTreeNode = node;
    }

  I_BTreeNode* operator-> ()
  {
    return m_pTreeNode;
  }

private:
  BTreeNodeHandler (const BTreeNodeHandler &);
  BTreeNodeHandler &operator= (const BTreeNodeHandler &);

  I_BTreeNode *m_pTreeNode;
};

class I_BTreeNodeManager
{
public:
  I_BTreeNodeManager ();
  virtual ~I_BTreeNodeManager ();

  I_BTreeNode* RetrieveNode (const NODE_INDEX node);
  void         ReleaseNode (const NODE_INDEX node);
  void         ReleaseNode (I_BTreeNode *node) { ReleaseNode (node->GetNodeId()); }

  virtual NODE_INDEX  GetRootNodeId () = 0;
  virtual NODE_INDEX  AllocateNode (const NODE_INDEX parrent, KEY_INDEX parrentKey) = 0;
  virtual void        SetRoot (const NODE_INDEX node) = 0;

protected:
  struct CachedData
  {
    CachedData (I_BTreeNode *pNode, const D_UINT refCount) :
      m_pNode (pNode),
      m_ReferenceCount (refCount)
    {}

    I_BTreeNode *m_pNode;
    D_UINT       m_ReferenceCount;
  };

  virtual I_BTreeNode* GetNode (const NODE_INDEX node) = 0;
  virtual void         StoreNode (I_BTreeNode *const node) = 0;

  WSynchronizer                     m_Sync;
  std::map <NODE_INDEX, CachedData> m_NodesKeeper;
};

class BTree
{
public:
  BTree (I_BTreeNodeManager &nodesManager);
  ~BTree ();

  bool FindBiggerOrEqual (I_BTreeKey &key,
                          NODE_INDEX &outNode,
                          KEY_INDEX  &outKeyIndex);

  void InsertKey (I_BTreeKey &key, NODE_INDEX &outNode, KEY_INDEX &outKeyIndex);
  void RemoveKey (I_BTreeKey &key);

protected:
  I_BTreeNodeManager &m_NodesManager;
};

}

#endif /* PS_BTREE_INDEX_H_ */
