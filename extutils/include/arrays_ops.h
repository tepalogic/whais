/*
 * arrays_ops.h
 *
 *  Created on: Dec 31, 2018
 *      Author: ipopa
 */

#ifndef ARRAYS_OPS_H_
#define ARRAYS_OPS_H_

#include "whais.h"
#include "dbs/dbs_values.h"


whais::DArray
array_unite(const whais::DArray& array1, const whais::DArray& array2);

whais::DArray
array_intersect(const whais::DArray& array1, const whais::DArray& array2);

whais::DArray
array_get_uniques(const whais::DArray& array);


template<typename T> int64_t
find_in_sorted_array(const whais::DArray& array, const T& value)
{
  const auto count = array.Count();
  if (count == 0)
    return -1;

  int64_t i = 0, j = count - 1;

  while (i <= j) {
    const int64_t t = (i + j) / 2;

    T test;
    array.Get(t, test);

    if (test == value)
      return t;

    else if (value < test)
      j = t - 1;

    else
      i = t + 1;
  };

  return -2;
}

#endif /* ARRAYS_OPS_H_ */
