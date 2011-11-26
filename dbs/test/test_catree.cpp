/*
 * test_catree.cpp
 *
 *  Created on: Aug 21, 2011
 *      Author: iupo
 */


#include <vector>
#include <iostream>

#include "utils/include/catree.h"

typedef std::vector<D_UINT64> VectInt64;

class Int64NodeArray
{
public:
  Int64NodeArray (VectInt64 &vect) : mVect (vect) {};

  D_UINT64 GetSize ()
  {
    return mVect.size ();
  }

  void Exchange (D_UINT64 firstNode, D_UINT64 secondNode)
  {
    if (firstNode != secondNode)
      {
        D_UINT64 temp = mVect[firstNode];
        mVect[firstNode] = mVect[secondNode];
        mVect[secondNode] = temp;
      }
    ++changes;
  }

  void PushBack (D_UINT64 value)
  {
    mVect.push_back (value);
    changes = 0;
  }

  void DeleteLast ()
  {
    mVect.pop_back();
  }

  D_UINT64& operator[] (D_UINT64 index)
  {
    return mVect[index];
  }

public:
  int changes;
private:
  VectInt64 &mVect;
};


void print_tabs (const D_UINT count)
{
  for (D_UINT index = 0; index < count; ++index)
    std::cout << "\t";
}

void print_tree (VectInt64 &rInVect, D_UINT64 rootNode)
{
  Int64NodeArray array (rInVect);
  CATree <D_UINT64, D_UINT64, Int64NodeArray> tree (array);


  if (tree.GetRightChild (rootNode) != tree.Nil)
    print_tree (rInVect, tree.GetRightChild (rootNode));

  print_tabs (tree.GetNodeHeight (rootNode));
  std::cout << rInVect[rootNode] << "(" << rootNode << ")" <<std::endl;

  if (tree.GetLeftChild (rootNode) != tree.Nil)
      print_tree (rInVect, tree.GetLeftChild (rootNode));
}

bool create_tree (VectInt64 &rInVect, VectInt64 &rOutVect)
{

  Int64NodeArray array (rOutVect);
  CATree <D_UINT64, D_UINT64, Int64NodeArray> tree (array);

  for (unsigned int index = 0; index < rInVect.size(); ++index)
    tree.AddNode (rInVect[index]);

  if (rOutVect.size () != rInVect.size())
    return false;

  return true;
}

D_UINT64 check_tree_consistency (VectInt64 &rInVect, D_UINT64 rootNode)
{
  Int64NodeArray array (rInVect);
  CATree <D_UINT64, D_UINT64, Int64NodeArray> tree (array);

  D_UINT64 result = 0;

  if (tree.GetLeftChild (rootNode) != tree.Nil)
    {
      result = check_tree_consistency (rInVect, tree.GetLeftChild (rootNode));
      if (rInVect[result] > rInVect [rootNode])
        {
          assert (0);
          return 0;
        }
    }

  result = rootNode;
  std::cout << rInVect[rootNode] << std::endl;

  if (tree.GetRightChild (rootNode) != tree.Nil)
    {
      result = check_tree_consistency (rInVect, tree.GetRightChild (rootNode));
      if (rInVect [rootNode] > rInVect[result])
      {
        assert (0);
        return 0;
      }
    }

  return result;
}

int main ()
{
  VectInt64 temp;
  Int64NodeArray array (temp);
  CATree <D_UINT64, D_UINT64, Int64NodeArray> caTree (array);

  for (int i = 1; i < 1000; ++i)
    {
      caTree.AddNode (i);
      std::cout << i << " changes: " << array.changes << std::endl;
    }
#if 0
  caTree.AddNode (1); cout
  caTree.AddNode (2);
  caTree.AddNode (3);
  caTree.AddNode (4);
  caTree.AddNode (5);
  caTree.AddNode (6);
  caTree.AddNode (8);
  caTree.AddNode (9);
  caTree.AddNode (10);
  caTree.AddNode (11);
  caTree.AddNode (12);
  caTree.AddNode (13);
  caTree.AddNode (14);
  caTree.AddNode (15);
  caTree.AddNode (16);
  caTree.AddNode (17);
#endif

  std::cout << "\n\nArray Elements" << std::endl;
  print_tree (temp, 0);

#if 0

  std::cout << "Tree Walk consistency:" << std::endl;
  check_tree_consistency (temp, 0);
#endif

  return 0;
}
