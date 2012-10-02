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
  auto_array (const unsigned int size)
    : m_pT (new T [size]),
      m_Size (size)
  {
  }

  ~auto_array ()
  {
    delete [] m_pT;
  }

  T& operator[] (const unsigned int i)
  {

    if (i >= m_Size)
      throw std::out_of_range ("Index is bigger then the array size.");

    return m_pT[i];
  }

  const T& operator[] (const unsigned int i) const
  {

    if (i >= m_Size)
      throw std::out_of_range ("Index is bigger then the array size.");

    return m_pT[i];
  }

  unsigned int Size () const
  {
    return m_Size;
  }

  void Size (const unsigned int newSize)
  {
    if (newSize == 0)
      {
        delete m_pT;

        m_pT   = NULL;
        m_Size = 0;

        return;
      }

    const unsigned min = (newSize < m_Size) ? newSize : m_Size;
    T* const pNewArray = new T[newSize];

    try
    {
        for (unsigned int i = 0; i < min; ++i)
          pNewArray[i] = m_pT[i];
    }
    catch (...)
    {
        delete [] pNewArray;
        throw;
    }

    m_Size = newSize;
    m_pT   = pNewArray;
  }

  void Reset (const unsigned int newSize)
  {
    T* const pNewArray = ((newSize == 0) ? NULL : new T[newSize]);
    try
    {
        delete [] m_pT;
    }
    catch (...)
    {
        delete [] pNewArray;
        throw;
    }

    m_Size = newSize;
    m_pT   = pNewArray;
  }

private:
  T*           m_pT;
  unsigned int m_Size;

  auto_array (const auto_array&);
  auto_array& operator= (const auto_array&);
};

#endif /* AUTO_ARRAY_H_ */
