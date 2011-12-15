/******************************************************************************
UTILS - Common routines used trough WHISPER project
Copyright (C) 2009  Iulian Popa

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
******************************************************************************/

#ifndef CATREE_H_
#define CATREE_H_

#ifndef __cplusplus
#error "This file shall be included only in C++ code units!"
#endif

#include <assert.h>

#include "whisper.h"

template <class TKey, class TNode, class TNodeArray> class CATree
{
public:
  explicit CATree (TNodeArray &array) :
    mNodeArray (array)
  {
  }
  ~CATree ()
  {
  }

  D_UINT64 GetLeftChild (D_UINT64 nodeIndex)
  {
    assert (nodeIndex < mNodeArray.GetSize ());

    const D_UINT64 left = (nodeIndex * 2) + 1;

    if (left >= mNodeArray.GetSize ())
      return Nil;

    return left;
  }

  D_UINT64 GetRightChild (D_UINT64 nodeIndex)
  {
    assert (nodeIndex < mNodeArray.GetSize ());

    const D_UINT64 right = (nodeIndex * 2) + 2;

    if (right >= mNodeArray.GetSize ())
      return Nil;

    return right;
  }

  D_UINT64 GetParent (D_UINT64 nodeIndex)
  {
    assert (nodeIndex < mNodeArray.GetSize ());

    if (nodeIndex == 0)
      return Nil;

    return (nodeIndex - 1) / 2;
  }

  bool IsLeftChild (D_UINT64 nodeIndex)
  {
    assert (nodeIndex < mNodeArray.GetSize ());

    return (nodeIndex > 0 ) && ((nodeIndex & 1) != 0);
  }

  bool IsRightChild (D_UINT64 nodeIndex)
  {
    assert (nodeIndex < mNodeArray.GetSize ());

    return (nodeIndex > 0 ) && ((nodeIndex & 1) == 0);
  }

  D_UINT64 AddNode (const TNode& node)
  {
    mNodeArray.PushBack (node);
    D_UINT64 addedNode = mNodeArray.GetSize() - 1;

    FixTree (addedNode);

    return addedNode;
  }

  D_UINT64 GetNext (D_UINT64 nodeIndex)
  {
    D_UINT64 rightChild = GetRightChild (nodeIndex);

    if (rightChild != Nil)
      return GetSubtreeSmallest (rightChild);

    if (IsRightChild (nodeIndex))
      {
        while (IsRightChild (nodeIndex))
          nodeIndex = GetParent (nodeIndex);

        return GetParent (nodeIndex);
      }
    else if (IsLeftChild (nodeIndex))
      return GetParent (nodeIndex);

    return Nil;
  }

  void DeleteNode (D_UINT64 nodeIndex)
  {
    D_UINT64 lastNode = mNodeArray.GetSize() - 1;
    D_UINT64 replacement = GetLeftChild (nodeIndex);

    if (replacement != Nil)
      {
        replacement = GetSubtreeBiggest (replacement);
        mNodeArray.Exchange (replacement, nodeIndex);
        nodeIndex = replacement;
      }

    assert (GetLeftChild (nodeIndex) == Nil);
    assert (GetRightChild (nodeIndex) == Nil);

    if (lastNode != nodeIndex)
      {
        mNodeArray.Exchange (lastNode, nodeIndex);
        mNodeArray.DeleteLast ();
        FixTree (nodeIndex);
      }

    mNodeArray.DeleteLast ();
  }

  D_UINT64 GetPrev (D_UINT64 nodeIndex)
  {
    D_UINT64 leftChild = GetLeftChild (nodeIndex);

    if (leftChild != Nil)
      return GetSubtreeBiggest (leftChild);

    if (IsLeftChild (nodeIndex))
      {
        while (IsLeftChild (nodeIndex))
          nodeIndex = GetParent (nodeIndex);

        return GetParent (nodeIndex);
      }
    else if (IsRightChild (nodeIndex))
      return GetParent (nodeIndex);

    return Nil;
  }

  D_UINT64 GetLastSmalleer (const TKey& key, D_UINT64 rootNode = 0)
  {
    D_UINT64 lastSmalleer = Nil;
    const TNode searchedNode (key);

    while ((rootNode != Nil) && (mNodeArray[rootNode] != searchedNode))
      {
        if (mNodeArray[rootNode] < searchedNode)
          {
            lastSmalleer = rootNode;
            rootNode = GetRightChild (rootNode);
          }
        else
          rootNode = GetLeftChild (rootNode);
      }

    return lastSmalleer;
  }

  D_UINT64 GetLastBigger (const TKey& key, D_UINT64 rootNode = 0)
  {
    D_UINT64 lastBigger = Nil;
    const TNode searchedNode (key);

    while ((rootNode != Nil) && (mNodeArray[rootNode] != searchedNode))
      {
        if (mNodeArray[rootNode] < searchedNode)
          rootNode = GetRightChild (rootNode);
        else
          {
            lastBigger = rootNode;
            rootNode = GetLeftChild (rootNode);
          }
      }

    return lastBigger;
  }


  D_UINT64 FindNode (const TKey& key, D_UINT64 rootNode = 0)
  {
    D_UINT64 iterator = rootNode;
    const TNode searchedNode (key);

    while ((iterator != Nil) && (iterator < mNodeArray.GetSize ()))
      {
        if (mNodeArray[iterator] == searchedNode)
          return iterator;
        else if (mNodeArray[iterator] < searchedNode)
          iterator = GetLeftChild (iterator);
        else
          iterator = GetRightChild (iterator);
      }

    return Nil;
  }

  D_UINT GetHeight () const
  {
    D_UINT64 lastNode = mNodeArray.GetSize ()  - 1;

    return GetNodeHeight (lastNode);
  }

  D_UINT GetNodeHeight (D_UINT64 nodeIndex) const
  {
    D_UINT result = 0;

    ++nodeIndex;
    while (nodeIndex)
      {
        ++result, nodeIndex /= 2;
      }

    return result;
  }

protected:

  D_UINT64 GetSubtreeBiggest (D_UINT64 rootNode)
  {
    assert (rootNode < mNodeArray.GetSize ());

    D_UINT64 rightChild = rootNode;

    do
      {
        rootNode = rightChild;
        rightChild = GetRightChild (rootNode);
      }
    while (rightChild != Nil);

    return rootNode;
  }

  D_UINT64 GetSubtreeSmallest (D_UINT64 rootNode)
  {
    assert (rootNode < mNodeArray.GetSize ());

    D_UINT64 leftChild = rootNode;
    do
      {
        rootNode = leftChild;
        leftChild = GetLeftChild (rootNode);
      }
    while (leftChild != Nil);

    return rootNode;
  }


  bool FixTreeNode (D_UINT64& addedNode)
  {
    D_UINT64 parentNode = GetParent (addedNode);
    D_UINT64 leftNode = GetLeftChild (addedNode);
    D_UINT64 rightNode = GetRightChild (addedNode);

    bool fixComplete = true;

    if ((leftNode != Nil) &&
        (mNodeArray[addedNode] < mNodeArray[leftNode]))
      {
        D_UINT64 replacement = GetSubtreeBiggest (leftNode);
        mNodeArray.Exchange (addedNode, replacement);
        addedNode = replacement;
        fixComplete = false; //It shall be checked again
      }

    else if ((rightNode != Nil) &&
        (mNodeArray[rightNode] < mNodeArray[addedNode]))
      {
        D_UINT64 replacement = GetSubtreeSmallest (rightNode);
        mNodeArray.Exchange (addedNode, replacement);
        addedNode = replacement;
        fixComplete = false;
      }
    else if (IsLeftChild (addedNode) &&
        (mNodeArray[parentNode] < mNodeArray[addedNode]))
      {
        D_UINT64 replacement = GetSubtreeBiggest (addedNode);
        mNodeArray.Exchange (parentNode, replacement);

        if (replacement == addedNode)
          addedNode = parentNode;

        fixComplete = false;
      }
    else if (IsRightChild (addedNode) &&
        (mNodeArray[addedNode] < mNodeArray[parentNode]))
      {
        D_UINT64 replacement = GetSubtreeSmallest (addedNode);
        mNodeArray.Exchange (parentNode, replacement);

        if (replacement == addedNode)
          addedNode = parentNode;

        fixComplete = false;
      }

    return fixComplete;
  }

  bool FixNodePath (D_UINT64& addedNode)
  {
    bool fixComplete = true;

    D_UINT64 parentNode = GetParent (addedNode);
    D_UINT64 leftNode = GetLeftChild (addedNode);
    D_UINT64 rightNode = GetRightChild (addedNode);

    if (IsLeftChild (addedNode) &&
        (leftNode == Nil))
      {
        while (IsLeftChild (parentNode))
          {
            assert (mNodeArray[addedNode] <= mNodeArray[parentNode]);
            parentNode = GetParent (parentNode);
          }

        if (parentNode != 0)
          {
            parentNode = GetParent (parentNode);
            if (mNodeArray[addedNode] < mNodeArray [parentNode])
              {
                D_UINT64 prev = GetPrev (parentNode);

                if ( (prev == Nil) ||
                    (mNodeArray [prev] < mNodeArray [addedNode]))
                  {
                    mNodeArray.Exchange (addedNode, parentNode);
                    addedNode = parentNode;
                  }
                else
                  {
                    mNodeArray.Exchange (addedNode, parentNode);
                    mNodeArray.Exchange (parentNode, prev);
                    addedNode = prev;
                  }
                fixComplete = false;
              }
          }
      }
    else if (IsRightChild (addedNode) &&
        (rightNode == Nil))
      {
        while (IsRightChild (parentNode))
          {
            assert (mNodeArray[parentNode] <= mNodeArray[addedNode]);
            parentNode = GetParent (parentNode);
          }

        if (parentNode != 0)
          {
            parentNode = GetParent (parentNode);
            if (mNodeArray[parentNode] < mNodeArray[addedNode])
              {
                D_UINT64 next = GetNext (parentNode);

                if ((next == Nil) ||
                    (mNodeArray[addedNode] < mNodeArray[next]))
                  {
                    mNodeArray.Exchange (addedNode, parentNode);
                    addedNode = parentNode;
                  }
                else
                  {
                    mNodeArray.Exchange (addedNode, parentNode);
                    mNodeArray.Exchange (parentNode, next);
                    addedNode = next;
                  }
                fixComplete = false;
              }
          }
      }

    return fixComplete;
  }

  void FixTree (D_UINT64 &addedNode)
  {
    while (true)
      {
        if ( ! FixTreeNode (addedNode))
          continue;

        if ( ! FixNodePath (addedNode))
          continue;

        break;
      }
  }

public:
  static const D_UINT64 Nil = ~0;

protected:
  TNodeArray &mNodeArray;
};

#endif /* CATREE_H_ */
