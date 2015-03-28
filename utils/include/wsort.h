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
partition (int64_t      from,
           int64_t      to,
           TC&          inoutContainer,
           bool* const  outAlreadySorted)
{
  assert (from < to);
  assert (to < _SC (int64_t, inoutContainer.Count()));

  *outAlreadySorted = true;

  int64_t pivotIndex = from++;
  inoutContainer.Pivot (pivotIndex);
  do
  {
    if (inoutContainer.Pivot () < inoutContainer[from])
      {
        pivotIndex = from++;
        inoutContainer.Pivot (pivotIndex);
        continue;
      }
    else if (inoutContainer[from] < inoutContainer[from - 1])
      {
        *outAlreadySorted = false;
        break;
      }
    ++from;
  }
  while (from <= to);

  if (*outAlreadySorted)
    return to;

  inoutContainer.Pivot(pivotIndex);
  from = pivotIndex;
  do
    {
      if (inoutContainer[from] < inoutContainer.Pivot ())
        {
          ++from;
          continue;
        }
      if (! (inoutContainer[to] < inoutContainer.Pivot ()))
        {
          to--;
          continue;
        }

      assert (from != to);
      assert (inoutContainer[to] < inoutContainer[from]);

      inoutContainer.Exchange (from, to);
    }
  while (from < to);

  assert (from == to);

  if (inoutContainer[from] < inoutContainer.Pivot ())
    from++;

  return from;
}




template<typename TE, typename TC> int64_t
partition_reverse (int64_t      from,
                   int64_t      to,
                   TC&          inoutContainer,
                   bool* const  outAlreadySorted)
{
  assert (from < to);
  assert (to < _SC (int64_t, inoutContainer.Count()));

  *outAlreadySorted = true;

  int64_t pivotIndex = to--;
  inoutContainer.Pivot (pivotIndex);
  do
  {
    if (inoutContainer.Pivot () < inoutContainer[to] )
      {
        pivotIndex = to--;
        inoutContainer.Pivot (pivotIndex);
        continue;
      }
    else if (inoutContainer[to] < inoutContainer[to + 1])
      {
        *outAlreadySorted = false;
        break;
      }
    --to;
  }
  while (from <= to);

  if (*outAlreadySorted)
    return to;

  inoutContainer.Pivot(pivotIndex);
  to = pivotIndex;
  do
    {
      if (inoutContainer[to] < inoutContainer.Pivot ())
        {
          --to;
          continue;
        }

      if (! (inoutContainer[from] < inoutContainer.Pivot ()))
        {
          ++from;
          continue;
        }

      assert (from != to);
      assert (inoutContainer[from] < inoutContainer[to]);

      inoutContainer.Exchange (from, to);
    }
  while (from < to);

  assert (from == to);

  if (! (inoutContainer[to] < inoutContainer.Pivot ()))
    return ++to;

  return to;
}


struct _partition_t
{
  _partition_t (int64_t from, int64_t to)
    : mFrom (from),
      mTo (to)
  {
    assert (from < to);
  }
  int64_t mFrom;
  int64_t mTo;
};


template<typename TE, typename TC> void
quick_sort (int64_t           from,
            int64_t           to,
            const bool        reverse,
            TC&               inoutContainer)
{
  assert (from <= to);

  if (from == to)
    return ;

  std::vector<_partition_t> partStack;
  partStack.push_back ( _partition_t (from, to));

  do
    {
      _partition_t current = partStack[partStack.size() - 1];
      partStack.pop_back();

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

      if ( ! alreadySorted)
        {
          if (pivot < current.mTo)
            partStack.push_back (_partition_t (pivot, current.mTo));

          if (current.mFrom < pivot - 1)
            partStack.push_back (_partition_t (current.mFrom, pivot - 1));
        }
    }
  while (partStack.size() > 0);
}



#endif /* WSORT_H_ */

