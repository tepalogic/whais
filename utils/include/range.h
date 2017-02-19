/*
 * range_set.h
 *
 *  Created on: Jul 7, 2013
 *      Author: ipopa
 */

#ifndef RANGE_H_
#define RANGE_H_

#include <cassert>
#include <limits>
#include <vector>

#include "whais.h"

#ifdef ARCH_WINDOWS_VC
  #undef min
  #undef max
#endif

namespace whais {

template<typename T> static inline
T Prev(const T& t)
{
  return t.Prev();
}

template<typename T> static inline
T Next(const T& t)
{
  return t.Next();
}


template<typename T> static inline
T Minimum()
{
  return T::Min();
}


template<typename T> static inline
T Maximum()
{
  return T::Max();
}

template<class T>
struct Interval
{
  Interval()
    : mFrom(),
      mTo()
  {
  }

  explicit Interval(const T& val)
    : mFrom(val),
      mTo   (val)
  {
  }

  Interval(const T& from, const T& to)
  {
    if (from <= to)
      mFrom = from, mTo = to;

    else
      mFrom = to, mTo = from;
  }


  bool operator== (const Interval& val) const
  {
    return(mFrom == val.mFrom) && (mTo == val.mTo);
  }


  bool operator!= (const Interval& val) const
  {
    return ! (*this == val);
  }

  T   mFrom;
  T   mTo;
};


template<> inline
int8_t Minimum<int8_t> ()
{
  return std::numeric_limits<int8_t>::min();
}


template<> inline
int8_t Maximum<int8_t> ()
{
  return std::numeric_limits<int8_t>::max();
}


template<> inline
int8_t Prev<int8_t> (const int8_t& t)
{
  if (t == Minimum<int8_t> ())
    return t;

  return t - 1;
}


template<> inline
int8_t Next<int8_t> (const int8_t& t)
{
  if (t == Maximum<int8_t> ())
    return t;

  return t + 1;
}


template<> inline
int16_t Minimum<int16_t> ()
{
  return std::numeric_limits<int16_t>::min();
}


template<> inline
int16_t Maximum<int16_t> ()
{
  return std::numeric_limits<int16_t>::max();
}


template<> inline
int16_t Prev<int16_t> (const int16_t& t)
{
  if (t == Minimum<int16_t> ())
    return t;

  return t - 1;
}


template<> inline
int16_t Next<int16_t> (const int16_t& t)
{
  if (t == Maximum<int16_t> ())
    return t;

  return t + 1;
}


template<> inline
int32_t Minimum<int32_t> ()
{
  return std::numeric_limits<int32_t>::min();
}


template<> inline
int32_t Maximum<int32_t> ()
{
  return std::numeric_limits<int32_t>::max();
}


template<> inline
int32_t Prev<int32_t> (const int32_t& t)
{
  if (t == Minimum<int32_t> ())
    return t;

  return t - 1;
}


template<> inline
int32_t Next<int32_t> (const int32_t& t)
{
  if (t == Maximum<int32_t> ())
    return t;

  return t + 1;
}


template<> inline
int64_t Minimum<int64_t> ()
{
  return std::numeric_limits<int64_t>::min();
}


template<> inline
int64_t Maximum<int64_t> ()
{
  return std::numeric_limits<int64_t>::max();
}


template<> inline
int64_t Prev<int64_t> (const int64_t& t)
{
  if (t == Minimum<int64_t> ())
    return t;

  return t - 1;
}


template<> inline
int64_t Next<int64_t> (const int64_t& t)
{
  if (t == Maximum<int64_t> ())
    return t;

  return t + 1;
}


template<> inline
uint8_t Minimum<uint8_t> ()
{
  return std::numeric_limits<uint8_t>::min();
}


template<> inline
uint8_t Maximum<uint8_t> ()
{
  return std::numeric_limits<uint8_t>::max();
}


template<> inline
uint8_t Prev<uint8_t> (const uint8_t& t)
{
  if (t == Minimum<uint8_t> ())
    return t;

  return t - 1;
}


template<> inline
uint8_t Next<uint8_t> (const uint8_t& t)
{
  if (t == Maximum<uint8_t> ())
    return t;

  return t + 1;
}


template<> inline
uint16_t Minimum<uint16_t> ()
{
  return std::numeric_limits<uint16_t>::min();
}


template<> inline
uint16_t Maximum<uint16_t> ()
{
  return std::numeric_limits<uint16_t>::max();
}


template<> inline
uint16_t Prev<uint16_t> (const uint16_t& t)
{
  if (t == Minimum<uint16_t> ())
    return t;

  return t - 1;
}


template<> inline
uint16_t Next<uint16_t> (const uint16_t& t)
{
  if (t == Maximum<uint16_t> ())
    return t;

  return t + 1;
}


template<> inline
uint32_t Minimum<uint32_t> ()
{
  return std::numeric_limits<uint32_t>::min();
}


template<> inline
uint32_t Maximum<uint32_t> ()
{
  return std::numeric_limits<uint32_t>::max();
}


template<> inline
uint32_t Prev<uint32_t> (const uint32_t& t)
{
  if (t == Minimum<uint32_t> ())
    return t;

  return t - 1;
}


template<> inline
uint32_t Next<uint32_t> (const uint32_t& t)
{
  if (t == Maximum<uint32_t> ())
    return t;

  return t + 1;
}



template<> inline
uint64_t Minimum<uint64_t> ()
{
  return std::numeric_limits<uint64_t>::min();
}


template<> inline
uint64_t Maximum<uint64_t> ()
{
  return std::numeric_limits<uint64_t>::max();
}


template<> inline
uint64_t Prev<uint64_t> (const uint64_t& t)
{
  if (t == Minimum<uint64_t> ())
    return t;

  return t - 1;
}


template<> inline
uint64_t Next<uint64_t> (const uint64_t& t)
{
  if (t == Maximum<uint64_t> ())
    return t;

  return t + 1;
}



template<class T>
struct Range
{
  void Join(const Interval<T>& v)
  {
    size_t bOffset = FindJoinInsertPlace(v.mFrom);

    if (bOffset == mIntervals.size())
      mIntervals.push_back(v);

    else if (v.mFrom < mIntervals[bOffset].mFrom)
      {
        if (v.mTo < Prev(mIntervals[bOffset].mFrom))
          mIntervals.insert(mIntervals.begin() + bOffset, v);

        else
          mIntervals[bOffset].mFrom = v.mFrom;
      }

    if (bOffset > 0)
      {
        if (mIntervals[bOffset - 1].mTo  == Prev(mIntervals[bOffset].mFrom))
          {
            mIntervals[bOffset - 1].mTo = mIntervals[bOffset].mTo;
            mIntervals.erase( mIntervals.begin() + bOffset--);
          }
      }


    size_t lOffset = FindJoinInsertPlace(v.mTo);

    if (lOffset == mIntervals.size())
      {
        mIntervals[bOffset].mTo = v.mTo;
        mIntervals.resize(bOffset + 1);
      }
    else if (Prev(mIntervals[lOffset].mFrom) <= v.mTo)
      {
        mIntervals[bOffset].mTo = mIntervals[lOffset].mTo;

        mIntervals.erase(mIntervals.begin() + (bOffset + 1),
                          mIntervals.begin() + (lOffset + 1));
      }
    else
      {
        mIntervals[bOffset].mTo = v.mTo;
        mIntervals.erase(mIntervals.begin() + (bOffset + 1),
                          mIntervals.begin() + lOffset);
      }
  }

  Range& Join(const Range& r)
  {
    const size_t count = r.mIntervals.size();

    for (size_t i = 0; i < count; ++i)
      Join(r.mIntervals[i]);

    return *this;
  }

  Range& Match(const Range& r)
  {
    size_t offset  = mIntervals.size();
    size_t offsetR = r.mIntervals.size();

    if (offset == 0)
      return *this;

    else if (offsetR == 0)
      {
        mIntervals.resize(0);

        return  *this;
      }

    while ((offsetR > 0) && (offset > 0))
      {
        --offsetR, --offset;

        const Interval<T>& curr = mIntervals[offset];
        const Interval<T>& v    = r.mIntervals[offsetR];

        if (curr.mTo < v.mFrom)
	  ++offset;

        else if (v.mTo < curr.mFrom)
          {
            mIntervals.erase(mIntervals.begin() + offset);
            ++offsetR;
          }
        else
          {
            Isolate(offset, v);
            ++offsetR;
          }
      }

     assert(offset <= mIntervals.size());

     mIntervals.erase(mIntervals.begin(), mIntervals.begin() + offset);

     return *this;
  }


  Range& Complement()
  {
    const T      min       = Minimum<T> ();
    const T      max       = Maximum<T> ();
    const size_t rangeSize = mIntervals.size();

    Interval<T> v(min, max);


    if (rangeSize == 0)
      {
        mIntervals.push_back(v);

        return *this;
      }
    else if ((mIntervals[0].mFrom == min) && (mIntervals[0].mTo == max))
      {
        assert(rangeSize == 1);

        mIntervals.resize(0);

        return *this;
      }
    else if (mIntervals[0].mFrom == min)
      {
        size_t offset = 0;

        while (offset < rangeSize)
          {
            if (mIntervals[offset].mTo == max)
              {
                assert(offset == rangeSize - 1);

                mIntervals.pop_back();

                break;
              }

            v.mFrom = Next(mIntervals[offset].mTo);

            if (offset < rangeSize - 1)
              v.mTo = Prev(mIntervals[offset + 1].mFrom);

            else
              v.mTo = max;

            mIntervals[offset++] = v;
          }
      }
    else
      {
        T      last;
        size_t offset   = 0;

        while (offset < rangeSize)
          {
            v.mTo = Prev(mIntervals[offset].mFrom);

            last = mIntervals[offset].mTo;

            mIntervals[offset] = v;
            if (last == max)
              {
                assert(offset == (rangeSize - 1));

                break;
              }
            else
              v.mFrom = Next(last);

            ++offset;
          }

        if (last != max)
          {
            assert(v.mFrom == Next(last));

            v.mTo   = max;
            mIntervals.push_back(v);
          }
      }

    return *this;
  }


  bool operator== (const Range& r) const
  {
    const size_t count = mIntervals.size();

    if (count != r.mIntervals.size())
      return false;

    for (size_t i = 0; i < count; ++i)
      {
        if (mIntervals[i] != r.mIntervals[i])
          return false;
      }

    return true;
  }


  bool operator!= (const Range& r) const
  {
    return ! (*this == r);
  }


  void Clear()
  {
    mIntervals.resize(0);
  }

  size_t FindJoinInsertPlace(const T& v)
  {
    size_t offset = mIntervals.size();

    if (offset == 0)
      return 0;

    size_t first = 0, last = offset;
    offset = (first + last) / 2;
    do
      {
        const Interval<T>& curr = mIntervals[offset];

        if ((curr.mFrom <= v) && (v <= curr.mTo))
          return offset; //Bang!

        else if (v < curr.mFrom)
          last = offset;

        else
          first = offset + 1;

        offset = (last + first) / 2;
      }
    while (first < last);

    assert(offset == mIntervals.size() ||
            (v <= mIntervals[offset].mTo));
    assert((offset == 0)
            || (mIntervals[offset - 1].mTo < v));

    return offset;
  }

  void Isolate(size_t& offset, const Interval<T>& v)
  {
    assert(v.mFrom <= v.mTo);
    assert(offset < mIntervals.size());

    Interval<T> curr = mIntervals[offset];

    assert(curr.mFrom <= curr.mTo);
    assert(v.mTo >= curr.mFrom);
    assert(v.mFrom <= curr.mTo);

    if (v.mFrom <= curr.mFrom)
      {
        if (v.mTo < curr.mTo)
          mIntervals[offset].mTo = v.mTo;
      }
    else
      {
        assert(v.mFrom <= curr.mTo);

        curr.mTo = Prev(v.mFrom);

        if (mIntervals[offset].mTo <= v.mTo)
            mIntervals[offset].mFrom = v.mFrom;

        else
          mIntervals[offset] = v;

        mIntervals.insert(mIntervals.begin() + offset, curr);
        ++offset;
      }
  }

  std::vector<Interval<T> >     mIntervals;
};

} //namespace whais

#endif //RANGE_H_

