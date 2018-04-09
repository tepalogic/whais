/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whais.
 Copyright(C) 2008  Iulian Popa

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
 *****************************************************************************/

#include <cassert>
#include <algorithm>

#include "utils/we_int128.h"
#include "utils/whash.h"

#include "base_types.h"


using namespace whais;
using namespace std;



static const uint64_t NOT_FOUND_VALUE = ~_SC(uint64_t, 0);



WLIB_PROC_DESCRIPTION         gProcArrayCount;
WLIB_PROC_DESCRIPTION         gProcArraySort;

WLIB_PROC_DESCRIPTION         gProcArrayMin;
WLIB_PROC_DESCRIPTION         gProcArrayMax;

WLIB_PROC_DESCRIPTION         gProcArrayTruncate;
WLIB_PROC_DESCRIPTION         gProcArrayHash;



static WLIB_STATUS
array_elems_count( SessionStack& stack, ISession&)
{
  DArray array;

  const auto firstParam = stack.Size() - 1;
  IOperand& op = stack[firstParam].Operand();

  if (op.IsNull())
  {
    stack[firstParam] = StackValue::Create(DUInt64(0));
    return WOP_OK;
  }

  op.GetValue( array);
  stack[firstParam] = StackValue::Create(DUInt64(array.Count()));

  return WOP_OK;
}

template<typename T> DArray
remove_array_duplicates(DArray& source)
{
  T value;
  DArray result;

  source.Get(0, value);
  result.Add(value);

  const auto count = source.Count();
  assert (count > 0);

  for (uint64_t i = 1; i < count; ++i)
  {
    T elem;
    source.Get(i, elem);
    if (elem == value)
      continue;

    value = elem;
    result.Add(value);
  }

  return result;
}


static WLIB_STATUS
array_sort( SessionStack& stack, ISession&)
{
  DArray array;
  DBool  reverse, removeDuplicates;

  const auto paramsCount = 3;
  const auto firstParam = stack.Size() - paramsCount;

  if (stack[firstParam].Operand().IsNull())
  {
    stack.Pop(paramsCount - 1);
    stack[firstParam] = StackValue::Create(DUInt64(0));

    return WOP_OK;
  }

  stack[firstParam].Operand().GetValue( array);
  stack[firstParam + 1].Operand().GetValue(reverse);
  stack[firstParam + 2].Operand().GetValue(removeDuplicates);

  if (reverse.IsNull())
    reverse = DBool(false);

  if (removeDuplicates.IsNull())
    removeDuplicates = DBool(false);

  array.Sort(reverse.mValue);
  if (removeDuplicates.mValue && (array.Count() > 1))
  {
    switch (GET_BASE_TYPE(array.Type()))
    {
    case T_BOOL:
      array = remove_array_duplicates<DBool>(array);
      break;

    case T_CHAR:
      array = remove_array_duplicates<DChar>(array);
      break;

    case T_DATE:
      array = remove_array_duplicates<DDate>(array);
      break;

    case T_DATETIME:
      array = remove_array_duplicates<DDateTime>(array);
      break;

    case T_HIRESTIME:
      array = remove_array_duplicates<DHiresTime>(array);
      break;

    case T_INT8:
      array = remove_array_duplicates<DInt8>(array);
      break;

    case T_INT16:
      array = remove_array_duplicates<DInt16>(array);
      break;

    case T_INT32:
      array = remove_array_duplicates<DInt32>(array);
      break;

    case T_INT64:
      array = remove_array_duplicates<DInt64>(array);
      break;

    case T_UINT8:
      array = remove_array_duplicates<DUInt8>(array);
      break;

    case T_UINT16:
      array = remove_array_duplicates<DUInt16>(array);
      break;

    case T_UINT32:
      array = remove_array_duplicates<DUInt32>(array);
      break;

    case T_UINT64:
      array = remove_array_duplicates<DUInt64>(array);
      break;

    case T_REAL:
      array = remove_array_duplicates<DReal>(array);
      break;

    case T_RICHREAL:
      array = remove_array_duplicates<DRichReal>(array);
      break;

    default:
        throw InterException(_EXTRA(InterException::INVALID_PARAMETER_TYPE),
                             "Unexpected array type.");

    }
  }

  stack[firstParam].Operand().SetValue(array);
  stack.Pop(paramsCount - 1);
  stack[firstParam] = StackValue::Create(DUInt64(array.Count()));

  return WOP_OK;
}


template<typename T> DUInt64
retrieve_minim_value( const DArray& array, T margin, const uint64_t from)
{
  const uint64_t count = array.Count();

  DUInt64 result;
  T minim = T::Max();

  if (margin.IsNull())
    margin = T::Min();

  for (uint64_t pos = from; pos < count; ++pos)
  {
    T elem;
    array.Get(pos, elem);

    if (elem < minim && elem >= margin)
    {
      result = DUInt64(pos);
      if (elem == margin)
        break;

      minim = elem;
    }
  }

  return result;
}


template<typename T> DUInt64
retrieve_maxim_value( const DArray& array, T margin, const uint64_t from)
{
  const uint64_t count = array.Count();

  DUInt64 result;

  T maxim = T::Min();
  if (margin.IsNull())
    margin = T::Max();

  for (uint64_t pos = from; pos < count; ++pos)
  {
    T elem;
    array.Get(pos, elem);

    if (elem > maxim && elem <= margin)
    {
      result = DUInt64(pos);
      if (elem == margin)
        break;

      maxim = elem;
    }
  }

  return result;
}


template<bool minSearch> WLIB_STATUS
search_minmax( SessionStack& stack, ISession&)
{
  DUInt64 result, from;
  DArray array;

  const auto paramsCount = 3;
  const auto firstParam = stack.Size() - paramsCount;

  if (stack[firstParam].Operand().IsNull())
  {
    stack.Pop(paramsCount - 1);
    stack[firstParam] = StackValue();
    return WOP_OK;
  }

  stack[firstParam].Operand().GetValue(array);
  stack[firstParam + 2].Operand().GetValue(from);

  if (from.IsNull())
    from = DUInt64(0);

  switch (array.Type())
  {
  case T_BOOL:
  {
    DBool margin;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_CHAR:
  {
    DChar margin;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_DATE:
  {
    DDate margin;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_DATETIME:
  {
    DDateTime margin;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_HIRESTIME:
  {
    DHiresTime margin;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_INT8:
  {
    DInt8 margin ;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_INT16:
  {
    DInt16 margin ;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_INT32:
  {
    DInt32 margin ;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_INT64:
  {
    DInt64 margin ;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_UINT8:
  {
    DUInt8 margin ;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_UINT16:
  {
    DUInt16 margin ;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_UINT32:
  {
    DUInt32 margin ;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_UINT64:
  {
    DUInt64 margin ;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_REAL:
  {
    DReal margin ;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  case T_RICHREAL:
  {
    DRichReal margin ;
    stack[firstParam + 1].Operand().GetValue(margin);
    result = minSearch
               ? retrieve_minim_value(array, margin, from.mValue)
               : retrieve_maxim_value(array, margin, from.mValue);
  }
    break;

  default:
    throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
  }

  stack.Pop(paramsCount - 1);
  stack[firstParam] = StackValue::Create(result);

  return WOP_OK;
}


static WLIB_STATUS
proc_array_truncate( SessionStack& stack, ISession&)
{
  DArray array;
  DUInt64 newArraySize;

  const auto paramsCount = 2;
  const auto firstParam = stack.Size() - paramsCount;

  IOperand& refOp = stack[firstParam].Operand();
  if (stack[firstParam].Operand().IsNull())
  {
    stack.Pop(paramsCount - 1);
    stack[firstParam] = StackValue::Create(DUInt64(0));

    return WOP_OK;
  }

  refOp.GetValue(array);
  stack[firstParam + 1].Operand().GetValue(newArraySize);
  if (newArraySize.IsNull())
    newArraySize = DUInt64(0);

  for (int64_t i = array.Count() - 1; max<decltype(i)>(newArraySize.mValue, 0) <= i; --i)
    array.Remove(i);

  refOp.SetValue(array);
  stack.Pop(paramsCount - 1);
  stack[firstParam] = StackValue::Create(DUInt64(array.Count()));

  return WOP_OK;
}


template<typename T> uint64_t
compute_array_hash( const DArray& array)
{
  uint8_t key[512];

  const uint64_t arrayCount = array.Count();

  assert(arrayCount > 0);

  uint_t keyOff = 0;
  uint_t arrayOff = 0;

  T temp;
  for (keyOff = 0, arrayOff = 0;
       (keyOff < sizeof(key) - sizeof(temp)) && (arrayOff < arrayCount);
       keyOff += sizeof(temp), ++arrayOff)
  {
    array.Get(arrayOff, temp);
    memcpy(key + keyOff, &temp, sizeof(temp));
  }

  return wh_hash(key, keyOff);
}

static WLIB_STATUS
proc_hash_array( SessionStack& stack, ISession&)
{
  DArray array;
  DUInt64 result;

  const uint64_t firstParameter = stack.Size() - 1;

  IOperand& op = stack[firstParameter].Operand();
  if (op.IsNull())
  {
    stack[firstParameter] = StackValue();
    return WOP_OK;
  }
  stack[firstParameter].Operand().GetValue(array);

  if (array.IsNull())
  {
    stack[firstParameter] = StackValue::Create(DUInt64(0));
    return WOP_OK;
  }

  switch (array.Type())
  {
  case T_BOOL:
    result = DUInt64(compute_array_hash<DBool>(array));
    break;

  case T_CHAR:
    result = DUInt64(compute_array_hash<DChar>(array));
    break;

  case T_DATE:
    result = DUInt64(compute_array_hash<DDate>(array));
    break;

  case T_HIRESTIME:
    result = DUInt64(compute_array_hash<DHiresTime>(array));
    break;

  case T_INT8:
    result = DUInt64(compute_array_hash<DInt8>(array));
    break;

  case T_INT16:
    result = DUInt64(compute_array_hash<DInt16>(array));
    break;

  case T_INT32:
    result = DUInt64(compute_array_hash<DInt32>(array));
    break;

  case T_INT64:
    result = DUInt64(compute_array_hash<DInt64>(array));
    break;

  case T_UINT8:
    result = DUInt64(compute_array_hash<DUInt8>(array));
    break;

  case T_UINT16:
    result = DUInt64(compute_array_hash<DUInt16>(array));
    break;

  case T_UINT32:
    result = DUInt64(compute_array_hash<DUInt32>(array));
    break;

  case T_UINT64:
    result = DUInt64(compute_array_hash<DUInt64>(array));
    break;

  case T_REAL:
    result = DUInt64(compute_array_hash<DReal>(array));
    break;

  case T_RICHREAL:
    result = DUInt64(compute_array_hash<DRichReal>(array));
    break;

  default:
    throw InterException(_EXTRA(InterException::INVALID_PARAMETER_TYPE), "Unexpected array type.");
  }

  stack[firstParameter] = StackValue::Create(DUInt64(result));
  return WOP_OK;
}


WLIB_STATUS
base_arrays_init()
{

  static const uint8_t* arrayCountLocals[] = {
                                                gUInt64Type,
                                                gGenericArrayType
                                             };
  gProcArrayCount.name        = "count";
  gProcArrayCount.localsCount = 2;
  gProcArrayCount.localsTypes = arrayCountLocals;
  gProcArrayCount.code        = array_elems_count;


  static const uint8_t* arraySortLocals[]  = {
                                                gUInt64Type,
                                                gGenericArrayType,
                                                gBoolType,
                                                gBoolType
                                              };

  gProcArraySort.name        = "sort";
  gProcArraySort.localsCount = 4;
  gProcArraySort.localsTypes = arraySortLocals;
  gProcArraySort.code        = array_sort;


  static const uint8_t* minMaxLocals[] = {
                                            gUInt64Type,
                                            gGenericArrayType,
                                            gUndefinedType,
                                            gUInt64Type
                                         };


  gProcArrayMin.name         = "get_min";
  gProcArrayMin.localsCount  = 4;
  gProcArrayMin.localsTypes  = minMaxLocals;
  gProcArrayMin.code         = search_minmax<true>;

  gProcArrayMax.name         = "get_max";
  gProcArrayMax.localsCount  = 4;
  gProcArrayMax.localsTypes  = minMaxLocals;
  gProcArrayMax.code         = search_minmax<false>;


  static const uint8_t* arrayTruncateLocals[] = {
                                                  gUInt64Type,
                                                  gGenericArrayType,
                                                  gUInt64Type
                                                };

  gProcArrayTruncate.name        = "truncate";
  gProcArrayTruncate.localsCount = 3;
  gProcArrayTruncate.localsTypes = arrayTruncateLocals;
  gProcArrayTruncate.code        = proc_array_truncate;


  gProcArrayHash.name         = "hash_array";
  gProcArrayHash.localsCount  = 2;
  gProcArrayHash.localsTypes  = arrayCountLocals; //Reusing!
  gProcArrayHash.code         = proc_hash_array;

  return WOP_OK;
}


