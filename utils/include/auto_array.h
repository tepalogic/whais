/******************************************************************************
WHAISC - A compiler for whais programs
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

#ifndef AUTO_ARRAY_H_
#define AUTO_ARRAY_H_


#include <stdexcept>



template <class T>
class auto_array
{
public:
  auto_array ()
    : mpT (NULL),
      mSize (0)
  {
  }


  auto_array (const unsigned int size)
    : mpT (new T [size]),
      mSize (size)
  {
  }


  ~auto_array ()
  {
    delete [] mpT;
  }


  T& operator[] (const unsigned int i)
  {

    if (i >= mSize)
      throw std::out_of_range ("Index is bigger then the array size.");

    return mpT[i];
  }


  const T& operator[] (const unsigned int i) const
  {

    if (i >= mSize)
      throw std::out_of_range ("Index is bigger then the array size.");

    return mpT[i];
  }


  unsigned int Size () const
  {
    return mSize;
  }


  const T* Get () const
  {
    return mpT;
  }


  T* Get ()
  {
    return mpT;
  }


  T* Size (const unsigned int newSize)
  {
    if (newSize == 0)
      {
        delete [] mpT;

        mpT   = NULL;
        mSize = 0;

        return mpT;
      }

    const unsigned min = (newSize < mSize) ? newSize : mSize;
    T* const pNewArray = new T[newSize];

    try
    {
        for (unsigned int i = 0; i < min; ++i)
          pNewArray[i] = mpT[i];
    }
    catch (...)
    {
        delete [] pNewArray;
        throw;
    }

    mSize = newSize;
    mpT   = pNewArray;

    return mpT;
  }


  T* Reset (const unsigned int newSize)
  {
    T* const pNewArray = ((newSize == 0) ? NULL : new T[newSize]);
    try
    {
        delete [] mpT;
    }
    catch (...)
    {
        delete [] pNewArray;
        throw;
    }

    mSize = newSize;
    mpT   = pNewArray;

    return mpT;
  }

private:
  T*           mpT;
  unsigned int mSize;

  auto_array (const auto_array&);
  auto_array& operator= (const auto_array&);
};

#endif /* AUTO_ARRAY_H_ */
