/*
 * arrays_ops.cpp
 *
 *  Created on: Dec 31, 2018
 *      Author: ipopa
 */

#include "arrays_ops.h"

#include <cassert>

#include "ext_exception.h"

using namespace whais;

template<typename T>
DArray array_get_uniques_helper(const DArray& array)
{
  DArray result, temp = array;

  temp.Sort();
  const auto count = temp.Count();
  T lastValue;
  for (uint64_t i = 0; i < count; ++i)
  {
    T value;
    temp.Get(i, value);
    if (value == lastValue)
      continue;

    lastValue = value;
    result.Add(lastValue);
  }

  return result;
}

template<typename T>
DArray array_unite_helper(const DArray& ar1, const DArray& ar2)
{
  DArray result = ar1;

  const auto count = ar2.Count();
  for (uint64_t i = 0; i < count; ++i)
  {
    T value;
    ar2.Get(i, value);
    result.Add(value);
  }

  return result;
}


template<typename T>
DArray array_intersect_helper(const DArray& ar1, const DArray& ar2)
{

  DArray result, lessCount, second;
  if (ar1.Count() <= ar2.Count())
  {
    lessCount = ar1;
    second = ar2;
  }
  else
  {
    lessCount = ar2;
    second = ar1;
  }

  lessCount.Sort();
  const auto count = second.Count();
  for (uint64_t i = 0; i < count; ++i)
  {
    T value;
    second.Get(i, value);

    if (find_in_sorted_array(lessCount, value) >= 0)
      result.Add(value);
  }

  return result;
}


DArray
array_unite(const DArray& array1, const DArray& array2)
{
  const auto array1Count = array1.Count();
  const auto array2Count = array2.Count();

  if ((array1Count != 0) && (array2Count != 0))
  {
    if (array1.Type() != array2.Type())
      throw ExtException(_EXTRA(ExtException::FILTER_FIELD_NOT_EXISTENT),
                         "Array1 type is %d, array2 type is %s.",
                         array1.Type(), array2.Type());
  }
  else if (array1Count == 0)
    return array2;

  else if (array2Count == 0)
    return array1;

  switch (array1.Type())
  {
  case T_BOOL:
    return array_unite_helper<DBool>(array1, array2);

  case T_CHAR:
    return array_unite_helper<DChar>(array1, array2);

  case T_DATE:
    return array_unite_helper<DDate>(array1, array2);

  case T_DATETIME:
    return array_unite_helper<DDateTime>(array1, array2);

  case T_HIRESTIME:
    return array_unite_helper<DHiresTime>(array1, array2);

  case T_INT8:
    return array_unite_helper<DInt8>(array1, array2);

  case T_INT16:
    return array_unite_helper<DInt16>(array1, array2);

  case T_INT32:
    return array_unite_helper<DInt32>(array1, array2);

  case T_INT64:
    return array_unite_helper<DInt64>(array1, array2);

  case T_UINT8:
    return array_unite_helper<DUInt8>(array1, array2);

  case T_UINT16:
    return array_unite_helper<DUInt16>(array1, array2);

  case T_UINT32:
    return array_unite_helper<DUInt32>(array1, array2);

  case T_UINT64:
    return array_unite_helper<DUInt64>(array1, array2);

  case T_REAL:
    return array_unite_helper<DReal>(array1, array2);

  case T_RICHREAL:
    return array_unite_helper<DRichReal>(array1, array2);

  default:
    assert (false);
  }

  throw ExtException(_EXTRA(ExtException::GENERAL_CONTROL_ERROR),
                     "Unexpected array type %d",
                     array1.Type());
}

whais::DArray
array_intersect(const whais::DArray& array1, const whais::DArray& array2)
{
  const auto array1Count = array1.Count();
  const auto array2Count = array2.Count();

  if ((array1Count != 0) && (array2Count != 0))
  {
    if (array1.Type() != array2.Type())
      throw ExtException(_EXTRA(ExtException::FILTER_FIELD_NOT_EXISTENT),
                         "Array1 type is %d, array2 type is %s.",
                         array1.Type(), array2.Type());
  }
  else if (array1Count == 0)
    return array1;

  else if (array2Count == 0)
    return array2;

  switch (array1.Type())
  {
  case T_BOOL:
    return array_intersect_helper<DBool>(array1, array2);

  case T_CHAR:
    return array_intersect_helper<DChar>(array1, array2);

  case T_DATE:
    return array_intersect_helper<DDate>(array1, array2);

  case T_DATETIME:
    return array_intersect_helper<DDateTime>(array1, array2);

  case T_HIRESTIME:
    return array_intersect_helper<DHiresTime>(array1, array2);

  case T_INT8:
    return array_intersect_helper<DInt8>(array1, array2);

  case T_INT16:
    return array_intersect_helper<DInt16>(array1, array2);

  case T_INT32:
    return array_intersect_helper<DInt32>(array1, array2);

  case T_INT64:
    return array_intersect_helper<DInt64>(array1, array2);

  case T_UINT8:
    return array_intersect_helper<DUInt8>(array1, array2);

  case T_UINT16:
    return array_intersect_helper<DUInt16>(array1, array2);

  case T_UINT32:
    return array_intersect_helper<DUInt32>(array1, array2);

  case T_UINT64:
    return array_intersect_helper<DUInt64>(array1, array2);

  case T_REAL:
    return array_intersect_helper<DReal>(array1, array2);

  case T_RICHREAL:
    return array_intersect_helper<DRichReal>(array1, array2);

  default:
    assert (false);
  }

  throw ExtException(_EXTRA(ExtException::GENERAL_CONTROL_ERROR),
                     "Unexpected array type %d",
                     array1.Type());
}

whais::DArray
array_get_uniques(const whais::DArray& array)
{
  if (array.Count() == 0)
    return array;

  switch (array.Type())
  {
  case T_BOOL:
    return array_get_uniques_helper<DBool>(array);

  case T_CHAR:
    return array_get_uniques_helper<DChar>(array);

  case T_DATE:
    return array_get_uniques_helper<DDate>(array);

  case T_DATETIME:
    return array_get_uniques_helper<DDateTime>(array);

  case T_HIRESTIME:
    return array_get_uniques_helper<DHiresTime>(array);

  case T_INT8:
    return array_get_uniques_helper<DInt8>(array);

  case T_INT16:
    return array_get_uniques_helper<DInt16>(array);

  case T_INT32:
    return array_get_uniques_helper<DInt32>(array);

  case T_INT64:
    return array_get_uniques_helper<DInt64>(array);

  case T_UINT8:
    return array_get_uniques_helper<DUInt8>(array);

  case T_UINT16:
    return array_get_uniques_helper<DUInt16>(array);

  case T_UINT32:
    return array_get_uniques_helper<DUInt32>(array);

  case T_UINT64:
    return array_get_uniques_helper<DUInt64>(array);

  case T_REAL:
    return array_get_uniques_helper<DReal>(array);

  case T_RICHREAL:
    return array_get_uniques_helper<DRichReal>(array);

  default:
    assert (false);
  }

  throw ExtException(_EXTRA(ExtException::GENERAL_CONTROL_ERROR),
                     "Unexpected array type %d",
                     array.Type());
}
