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

private:
  T* const     m_pT;
  unsigned int m_Size;

  auto_array (const auto_array&);
  auto_array& operator= (const auto_array&);
};

#endif /* AUTO_ARRAY_H_ */
