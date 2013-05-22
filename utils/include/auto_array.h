/*
 * auto_array.h
 *
 *  Created on: Sep 18, 2012
 *      Author: ipopa
 */

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
