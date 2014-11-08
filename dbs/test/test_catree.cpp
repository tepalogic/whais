/*
 * test_catree.cpp
 *
 *  Created on: Aug 21, 2011
 *      Author: iupo
 */


#include <vector>
#include <iostream>

#include "utils/catree.h"

typedef std::vector<uint64_t> VectInt64;

class Int64NodeArray
{
public:
  Int64NodeArray( VectInt64 &vect) : mVect( vect) {};

  uint64_t GetSize( )
  {
    return mVect.size( );
  }

  void Exchange( uint64_t firstNode, uint64_t secondNode)
  {
    if (firstNode != secondNode)
      {
        uint64_t temp = mVect[firstNode];
        mVect[firstNode] = mVect[secondNode];
        mVect[secondNode] = temp;
      }
    ++changes;
  }

  void PushBack( uint64_t value)
  {
    mVect.push_back( value);
    changes = 0;
  }

  void DeleteLast( )
  {
    mVect.pop_back( );
  }

  uint64_t& operator[] (uint64_t index)
  {
    return mVect[index];
  }

public:
  int changes;
private:
  VectInt64 &mVect;
};


void print_tabs( const uint_t count)
{
  for (uint_t index = 0; index < count; ++index)
    std::cout << "\t";
}

void print_tree( VectInt64 &rInVect, uint64_t rootNode)
{
  Int64NodeArray array( rInVect);
  CATree <uint64_t, uint64_t, Int64NodeArray> tree( array);


  if (tree.GetRightChild( rootNode) != tree.Nil)
    print_tree( rInVect, tree.GetRightChild( rootNode));

  print_tabs( tree.GetNodeHeight( rootNode));
  std::cout << rInVect[rootNode] << "(" << rootNode << ")" <<std::endl;

  if (tree.GetLeftChild( rootNode) != tree.Nil)
      print_tree( rInVect, tree.GetLeftChild( rootNode));
}

bool create_tree( VectInt64 &rInVect, VectInt64 &rOutVect)
{

  Int64NodeArray array( rOutVect);
  CATree <uint64_t, uint64_t, Int64NodeArray> tree( array);

  for (unsigned int index = 0; index < rInVect.size( ); ++index)
    tree.AddNode( rInVect[index]);

  if (rOutVect.size( ) != rInVect.size( ))
    return false;

  return true;
}

uint64_t check_tree_consistency( VectInt64 &rInVect, uint64_t rootNode)
{
  Int64NodeArray array( rInVect);
  CATree <uint64_t, uint64_t, Int64NodeArray> tree( array);

  uint64_t result = 0;

  if (tree.GetLeftChild( rootNode) != tree.Nil)
    {
      result = check_tree_consistency( rInVect, tree.GetLeftChild( rootNode));
      if (rInVect[result] > rInVect [rootNode])
        {
          assert( 0);
          return 0;
        }
    }

  result = rootNode;
  std::cout << rInVect[rootNode] << std::endl;

  if (tree.GetRightChild( rootNode) != tree.Nil)
    {
      result = check_tree_consistency( rInVect, tree.GetRightChild( rootNode));
      if (rInVect [rootNode] > rInVect[result])
      {
        assert( 0);
        return 0;
      }
    }

  return result;
}

int main( )
{
  VectInt64 temp;
  Int64NodeArray array( temp);
  CATree <uint64_t, uint64_t, Int64NodeArray> caTree( array);

  for (int i = 1; i < 1000; ++i)
    {
      caTree.AddNode( i);
      std::cout << i << " changes: " << array.changes << std::endl;
    }
#if 0
  caTree.AddNode( 1); cout
  caTree.AddNode( 2);
  caTree.AddNode( 3);
  caTree.AddNode( 4);
  caTree.AddNode( 5);
  caTree.AddNode( 6);
  caTree.AddNode( 8);
  caTree.AddNode( 9);
  caTree.AddNode( 10);
  caTree.AddNode( 11);
  caTree.AddNode( 12);
  caTree.AddNode( 13);
  caTree.AddNode( 14);
  caTree.AddNode( 15);
  caTree.AddNode( 16);
  caTree.AddNode( 17);
#endif

  std::cout << "\n\nArray Elements" << std::endl;
  print_tree( temp, 0);

#if 0

  std::cout << "Tree Walk consistency:" << std::endl;
  check_tree_consistency( temp, 0);
#endif

  return 0;
}
