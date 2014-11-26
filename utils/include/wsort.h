/******************************************************************************
UTILS - Common routines used trough WHAIS project
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

#ifndef WSORT_H_
#define WSORT_H_

#include "whais.h"

#include <cassert>
#include <vector>



template<typename TE, typename TC> int64_t
partition( int64_t            from,
           int64_t            to,
           TC&                inoutContainer,
           bool* const        outAlreadySorted)
{
  assert( from < to);
  assert( to < _SC (int64_t, inoutContainer.Count()));

  const int64_t originalFrom = from;

  TE rightEl, leftEl, temp;

  *outAlreadySorted = true;
  while( from <= to)
    {
      temp = leftEl;
      leftEl = inoutContainer[from++];

      if (leftEl < temp)
        {
          *outAlreadySorted = false;

          break;
        }
    }

  if (*outAlreadySorted)
    {
      assert( from == to + 1);
      return 0;
    }

  from = originalFrom;
  inoutContainer.Pivot( from, to);
  while( from < to)
    {
      while( from <= to)
        {
          leftEl = inoutContainer[from];
          if (leftEl < inoutContainer.Pivot())
            ++from;

          else
            break;
        }

      if (leftEl == inoutContainer.Pivot())
        {
          while( from < to)
            {
              temp = inoutContainer[from + 1];
              if (temp == inoutContainer.Pivot())
                ++from;

              else
                break;
            }
        }

      assert( (from < to) || (leftEl == inoutContainer.Pivot()));

      while( from <= to)
        {
          rightEl = inoutContainer[to];
          if (inoutContainer.Pivot() < rightEl)
            --to;

          else
            break;
        }

      assert( (from < to) || (rightEl == inoutContainer.Pivot()));

      if (from < to)
        {
          if (leftEl == rightEl)
            {
              assert( leftEl == inoutContainer.Pivot());
              ++from;
            }
          else
            inoutContainer.Exchange( to, from);
        }
    }

  return from;
}


template<typename TE, typename TC> int64_t
partition_reverse( int64_t            from,
                   int64_t            to,
                   TC&                inoutContainer,
                   bool* const        outAlreadySorted)
{
  assert( from < to);
  assert( to < _SC (int64_t, inoutContainer.Count()));

  const int64_t originalFrom = from;

  TE rightEl, leftEl, temp;

  *outAlreadySorted = true;

  leftEl = inoutContainer[from++];
  while( from <= to)
    {
      temp   = leftEl;
      leftEl = inoutContainer[from++];

      if (temp < leftEl)
        {
          *outAlreadySorted = false;

          break;
        }
    }

  if (*outAlreadySorted)
    {
      assert( from == to + 1);
      return 0;
    }

  from  = originalFrom;
  inoutContainer.Pivot( from, to);
  while( from < to)
    {
      while( from <= to)
        {
          leftEl = inoutContainer[from];
          if (inoutContainer.Pivot() < leftEl)
            ++from;

          else
            break;
        }

      if (leftEl == inoutContainer.Pivot())
        {
          while( from < to)
            {
              temp = inoutContainer[from + 1];
              if (temp == inoutContainer.Pivot())
                ++from;

              else
                break;
            }
        }

      assert( (from < to) || (leftEl == inoutContainer.Pivot()));

      while( from <= to)
        {
         rightEl = inoutContainer[to];
          if (rightEl < inoutContainer.Pivot())
            --to;

          else
            break;
        }

      assert( (from < to) || (rightEl == inoutContainer.Pivot()));

      if (from < to)
        {
          if (leftEl == rightEl)
            {
              assert( leftEl == inoutContainer.Pivot());
              ++from;
            }
          else
            inoutContainer.Exchange( to, from);
        }
    }
  return from;
}


struct _partition_t
{
  _partition_t( int64_t from, int64_t to)
    : mFrom( from),
      mTo (to)
  {
  }
  int64_t mFrom;
  int64_t mTo;
};


template<typename TE, typename TC> void
quick_sort( int64_t           from,
            int64_t           to,
            const bool        reverse,
            TC&               inoutContainer)
{
  assert( from <= to);

  std::vector<_partition_t> partStack;

  partStack.push_back(  _partition_t( from, to));

  do
    {
      _partition_t current = partStack[partStack.size() - 1];
      partStack.pop_back();

      if (current.mFrom >= current.mTo)
        continue;

      int64_t pivot;
      bool    alreadySorted;

      if (reverse)
        {
          pivot = partition_reverse<TE, TC> (current.mFrom,
                                             current.mTo,
                                             inoutContainer,
                                             &alreadySorted);
        }
      else
        {
          pivot = partition<TE, TC> (current.mFrom,
                                     current.mTo,
                                     inoutContainer,
                                     &alreadySorted);
        }

      if (alreadySorted == false)
        {
          if ((pivot + 1) < current.mTo)
            partStack.push_back( _partition_t( pivot + 1, current.mTo));

          if (current.mFrom < (pivot - 1))
            partStack.push_back( _partition_t( current.mFrom, pivot - 1));
        }
    }
  while( partStack.size() > 0);
}



#endif /* WSORT_H_ */

