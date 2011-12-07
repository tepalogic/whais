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

#include "whisper.h"

namespace pastra
{

class I_BTreeKey;
class I_BTreeNodeManager;

typedef D_UINT32 NODE_INDEX;

static const NODE_INDEX NIL_NODE = ~0;

class I_BTreeNode
{
public:
  I_BTreeNode (const D_UINT32 nodeId);
  virtual ~I_BTreeNode ();

  virtual bool  IsLeaf () const = 0;
  virtual bool  NeedsSpliting () const = 0;
  virtual bool  NeedsJoining () const = 0;

  virtual D_UINT  GetKeysPerNode () const = 0;
  virtual D_UINT  GetKeysCount () const = 0;

  virtual NODE_INDEX GetParrent () const = 0;
  virtual NODE_INDEX GetNext () const = 0;
  virtual NODE_INDEX GetPrev () const = 0;
  virtual NODE_INDEX GetKeyNode (I_BTreeKey &key) const = 0;
  virtual NODE_INDEX GetKeyNode (const D_UINT keyIndex) const = 0;

  virtual void SetLeaf (bool leaf) = 0;
  virtual void SetParrent (const NODE_INDEX parrent) = 0;
  virtual void SetNext (const NODE_INDEX next) = 0;
  virtual void SetPrev (const NODE_INDEX next) = 0;
  virtual void SetChild (const D_UINT keyIndex, const NODE_INDEX child) = 0;
  virtual void SetData (const D_UINT keyIndex, const D_UINT *data) = 0;

  virtual D_UINT InsertKey (I_BTreeKey &key) = 0;
  virtual void   RemoveKey (const D_UINT keyIndex) = 0;

  virtual NODE_INDEX Split (I_BTreeNodeManager &nodesManager) = 0;
  virtual NODE_INDEX Join (I_BTreeNodeManager &nodesManager) = 0;

  virtual bool IsLess (const I_BTreeKey &key, D_UINT keyIndex) = 0;
  virtual bool IsEqual (const I_BTreeKey &key, D_UINT keyIndex) = 0;
  virtual bool IsBigger (const I_BTreeKey &key, D_UINT keyIndex) = 0;
};

class I_BTreeNodeManager
{
public:
  I_BTreeNodeManager ();
  virtual ~I_BTreeNodeManager ();

  virtual I_BTreeNode&  GetRoot () = 0;
  virtual I_BTreeNode&  GetNode (const NODE_INDEX node) = 0;

  virtual NODE_INDEX    GetRootNodeId () = 0;
  virtual NODE_INDEX    AllocateNode (const NODE_INDEX parrent, D_UINT parrentKey) = 0;
  virtual void          ReleaseNode (const NODE_INDEX node) = 0;

  virtual void          SetRoot (const NODE_INDEX node) = 0;
};

class BTree
{
public:
  BTree (I_BTreeNodeManager &nodesManager);
  ~BTree ();



  void InsertKey (I_BTreeKey &key, NODE_INDEX &outNode, D_UINT &outKeyIndex);
  void RemoveKey (I_BTreeKey &key);

protected:
  bool FindBiggerOrEqual (I_BTreeKey &key,
                          NODE_INDEX &outNode,
                          D_UINT &outKeyIndex,
                          const bool splitCheck);
  I_BTreeNodeManager &m_NodesManager;
};

}

#endif /* PS_BTREE_INDEX_H_ */
