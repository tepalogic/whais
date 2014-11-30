/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whais.
 Copyright (C) 2008  Iulian Popa

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

#include "utils/we_int128.h"
#include "utils/whash.h"

#include "base_types.h"



using namespace whais;



static const uint64_t NOT_FOUND_VALUE = ~_SC (uint64_t, 0);



WLIB_PROC_DESCRIPTION         gProcArrayCount;
WLIB_PROC_DESCRIPTION         gProcArraySort;

WLIB_PROC_DESCRIPTION         gProcArrayBinSearch;

WLIB_PROC_DESCRIPTION         gProcArraySearch;

WLIB_PROC_DESCRIPTION         gProcArrayMin;
WLIB_PROC_DESCRIPTION         gProcArrayMax;
WLIB_PROC_DESCRIPTION         gProcArrayAverage;

WLIB_PROC_DESCRIPTION         gProcArrayIntersect;
WLIB_PROC_DESCRIPTION         gProcArrayUnion;
WLIB_PROC_DESCRIPTION         gProcArrayDiff;

WLIB_PROC_DESCRIPTION         gProcArrayPushBack;
WLIB_PROC_DESCRIPTION         gProcArrayTruncate;

WLIB_PROC_DESCRIPTION         gProcArrayHash;



static WLIB_STATUS
array_elems_count( SessionStack& stack, ISession&)
{
  DArray array;

  stack[stack.Size() - 1].Operand().GetValue( array);
  stack.Pop (1);

  stack.Push( DUInt64 (array.Count()));

  return WOP_OK;
}


static WLIB_STATUS
array_sort( SessionStack& stack, ISession&)
{
  DArray array;
  DBool  reverse;

  stack[stack.Size() - 2].Operand().GetValue( array);
  stack[stack.Size() - 1].Operand().GetValue( reverse);
  stack.Pop (2);

  array.Sort( reverse == DBool( true));

  stack.Push( array);

  return WOP_OK;
}


template<typename T> uint64_t
binary_array_search( const DArray&     array,
                     const T&          value,
                     const DInt8&      searchType)
{
  if (value.IsNull())
    return NOT_FOUND_VALUE;

  const uint64_t arrayCount = array.Count();

  assert( arrayCount > 0);

  uint64_t i = 0, j = arrayCount;
  T        current;

  while( i < j)
    {
      const uint64_t c = (i + j) / 2;

      array.Get (c, current);

      if (value < current)
        j = c;

      else
        i = c;
    }

  assert( i == j);

  if (i < arrayCount)
    array.Get (i, current);

  else
    current = T ();

  assert( current.IsNull() || (value <= current));

  if (current == value)
    return i;

  if (searchType.IsNull() || (searchType.mValue == 0))
    return NOT_FOUND_VALUE;

  else if (searchType.mValue < 0)
    return( i == 0) ? NOT_FOUND_VALUE : i - 1;

  return i == array.Count() ? NOT_FOUND_VALUE : i;
}


static WLIB_STATUS
binary_array_search_proc( SessionStack& stack, ISession&)
{
  uint64_t foundPos;
  DArray   array;

  stack[stack.Size() - 3].Operand().GetValue( array);

  if (array.IsNull())
    {
      stack.Pop (3);
      stack.Push( DUInt64 ());

      return WOP_OK;
    }

  DInt8 searchType;
  stack[stack.Size() - 1].Operand().GetValue( searchType);

  switch( array.Type())
    {
    case T_BOOL:
        {
          DBool value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_CHAR:
        {
          DChar value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_DATE:
        {
          DDate value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_DATETIME:
        {
          DDate value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_HIRESTIME:
        {
          DDate value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_INT8:
        {
          DInt8 value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_INT16:
        {
          DInt16 value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_INT32:
        {
          DInt32 value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_INT64:
        {
          DInt64 value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_UINT8:
        {
          DUInt8 value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_UINT16:
        {
          DUInt16 value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_UINT32:
        {
          DUInt32 value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_UINT64:
        {
          DUInt64 value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_REAL:
        {
          DReal value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    case T_RICHREAL:
        {
          DReal value;

          stack[stack.Size() - 2].Operand().GetValue( value);

          foundPos = binary_array_search( array, value, searchType);
        }
      break;

    default:
      throw InterException( _EXTRA( InterException::INTERNAL_ERROR));
    }

  stack.Pop (3);
  stack.Push( foundPos == NOT_FOUND_VALUE ? DUInt64 () : DUInt64 (foundPos));

  return WOP_OK;
}


template<typename T> uint64_t
unsorted_array_search( const DArray&     array,
                       const T&          value)
{
  if (value.IsNull())
    return NOT_FOUND_VALUE;

  const uint64_t arrayCount = array.Count();

  assert( arrayCount > 0);

  uint64_t i = 0;
  T        current;

  while( i < arrayCount)
    {
      array.Get (i, current);

      if (current == value)
        return i;
    }

  return NOT_FOUND_VALUE;
}


static WLIB_STATUS
unsorted_array_search_proc( SessionStack& stack, ISession&)
{
  uint64_t foundPos;
  DArray   array;

  stack[stack.Size() - 2].Operand().GetValue( array);

  if (array.IsNull())
    {
      stack.Pop (2);
      stack.Push( DUInt64 ());

      return WOP_OK;
    }

  switch( array.Type())
    {
    case T_BOOL:
        {
          DBool value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_CHAR:
        {
          DChar value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_DATE:
        {
          DDate value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_DATETIME:
        {
          DDate value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_HIRESTIME:
        {
          DDate value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_INT8:
        {
          DInt8 value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_INT16:
        {
          DInt16 value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_INT32:
        {
          DInt32 value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_INT64:
        {
          DInt64 value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_UINT8:
        {
          DUInt8 value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_UINT16:
        {
          DUInt16 value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_UINT32:
        {
          DUInt32 value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_UINT64:
        {
          DUInt64 value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_REAL:
        {
          DReal value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    case T_RICHREAL:
        {
          DReal value;

          stack[stack.Size() - 1].Operand().GetValue( value);

          foundPos = unsorted_array_search( array, value);
        }
      break;

    default:
      throw InterException( _EXTRA( InterException::INTERNAL_ERROR));
    }

  stack.Pop (2);
  stack.Push( foundPos == NOT_FOUND_VALUE ? DUInt64 () : DUInt64 (foundPos));

  return WOP_OK;
}


template<typename T> uint64_t
retrieve_minim_value( const DArray& array)
{
  const uint64_t arrayCount = array.Count();

  uint64_t pos      = 0;
  uint64_t foundPos = NOT_FOUND_VALUE;
  T        minim    = T::Max ();

  while( pos < arrayCount)
    {
      T value;
      array.Get (pos, value);

      if (value < minim)
        {
          minim    = value;
          foundPos = pos;
        }
      ++pos;
    }

  return foundPos;
}


template<typename T> uint64_t
retrieve_maxim_value( const DArray& array)
{
  const uint64_t arrayCount = array.Count();

  uint64_t pos      = 0;
  uint64_t foundPos = NOT_FOUND_VALUE;
  T        maxim    = T::Min ();

  while( pos < arrayCount)
    {
      T value;
      array.Get (pos, value);

      if (maxim < value)
        {
          maxim    = value;
          foundPos = pos;
        }
      ++pos;
    }

  return foundPos;
}


template<bool minSearch> WLIB_STATUS
search_minmax( SessionStack& stack, ISession&)
{
  uint64_t foundPos;
  DArray   array;

  stack[stack.Size() - 1].Operand().GetValue( array);
  stack.Pop (1);

  if (array.IsNull())
    {
      stack.Push( DUInt64 ());
      return WOP_OK;
    }

   switch( array.Type())
    {
    case T_BOOL:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DBool> (array) :
                      retrieve_maxim_value<DBool> (array);
        }
      break;

    case T_CHAR:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DChar> (array) :
                      retrieve_maxim_value<DChar> (array);
        }
      break;

    case T_DATE:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DDate> (array) :
                      retrieve_maxim_value<DDate> (array);
        }
      break;

    case T_DATETIME:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DDateTime> (array) :
                      retrieve_maxim_value<DDateTime> (array);
        }
      break;

    case T_HIRESTIME:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DHiresTime> (array) :
                      retrieve_maxim_value<DHiresTime> (array);
        }
      break;

    case T_INT8:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DInt8> (array) :
                      retrieve_maxim_value<DInt8> (array);
        }
      break;

    case T_INT16:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DInt16> (array) :
                      retrieve_maxim_value<DInt16> (array);
        }
      break;

    case T_INT32:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DInt32> (array) :
                      retrieve_maxim_value<DInt32> (array);
        }
      break;

    case T_INT64:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DInt32> (array) :
                      retrieve_maxim_value<DInt32> (array);
        }
      break;

    case T_UINT8:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DUInt8> (array) :
                      retrieve_maxim_value<DUInt8> (array);
        }
      break;

    case T_UINT16:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DUInt16> (array) :
                      retrieve_maxim_value<DUInt16> (array);
        }
      break;

    case T_UINT32:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DUInt32> (array) :
                      retrieve_maxim_value<DUInt32> (array);
        }
      break;

    case T_UINT64:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DUInt64> (array) :
                      retrieve_maxim_value<DUInt64> (array);
        }
      break;

    case T_REAL:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DReal> (array) :
                      retrieve_maxim_value<DReal> (array);
        }
      break;

    case T_RICHREAL:
        {
          foundPos = minSearch ?
                      retrieve_minim_value<DRichReal> (array) :
                      retrieve_maxim_value<DRichReal> (array);
        }
      break;

    default:
      throw InterException( _EXTRA( InterException::INTERNAL_ERROR));
    }


   assert( foundPos != NOT_FOUND_VALUE);
   stack.Push( DUInt64 (foundPos));

   return WOP_OK;
}

template<typename T> DRichReal
compute_integer_array_average_value( const DArray& array)
{
  const uint64_t arrayCount = array.Count();

  assert( arrayCount > 0);

  WE_I128     sum = 0;
  T           currentValue;

  for (uint64_t offset = 0; offset < arrayCount; ++offset)
    {
      array.Get (offset, currentValue);

      sum += currentValue.mValue;
    }

  const RICHREAL_T quotient = sum / arrayCount;
  const RICHREAL_T reminder = sum % arrayCount;

  return DRichReal( quotient + reminder / arrayCount);
}


template<typename T> DRichReal
compute_real_array_average_value( const DArray& array)
{
  const uint64_t arrayCount = array.Count();

  assert( arrayCount > 0);

  WE_I128     integerSum = 0, fractionalSum = 0;
  T           currentValue;

  for (uint64_t offset = 0; offset < arrayCount; ++offset)
    {
      array.Get (offset, currentValue);

      const RICHREAL_T temp = currentValue.mValue;

      integerSum    += temp.Integer();
      fractionalSum += temp.Fractional();
    }

  integerSum    += fractionalSum / DBS_RICHREAL_PREC;
  fractionalSum %= DBS_RICHREAL_PREC;

  RICHREAL_T result = _SC (RICHREAL_T, integerSum) / arrayCount;
  result += RICHREAL_T( 0, toInt64 (fractionalSum), DBS_RICHREAL_PREC)
            / arrayCount;

  return DRichReal( result);
}


static WLIB_STATUS
compute_array_average( SessionStack& stack, ISession&)
{
  DArray    array;
  DRichReal result;

  stack[stack.Size() - 1].Operand().GetValue( array);
  stack.Pop (1);

  if (array.IsNull())
    {
      stack.Push( result);

      return WOP_OK;
    }

  switch( array.Type())
    {
    case T_INT8:
      result = compute_integer_array_average_value<DInt8> (array);
      break;

    case T_INT16:
      result = compute_integer_array_average_value<DInt16> (array);
      break;

    case T_INT32:
      result = compute_integer_array_average_value<DInt32> (array);
      break;

    case T_INT64:
      result = compute_integer_array_average_value<DInt64> (array);
      break;

    case T_UINT8:
      result = compute_integer_array_average_value<DUInt8> (array);
      break;

    case T_UINT16:
      result = compute_integer_array_average_value<DUInt16> (array);
      break;

    case T_UINT32:
      result = compute_integer_array_average_value<DUInt32> (array);
      break;

    case T_UINT64:
      result = compute_integer_array_average_value<DUInt64> (array);
      break;

    case T_REAL:
      result = compute_real_array_average_value<DReal> (array);
      break;

    case T_RICHREAL:
      result = compute_real_array_average_value<DRichReal> (array);
      break;

    default:
      throw InterException(
          _EXTRA( InterException::INVALID_PARAMETER_TYPE),
          "You need a numeric array type to compute the average."
                           );
    }

  stack.Push( result);
  return WOP_OK;
}


template<typename T> DArray
intersect_arrays( DArray& ar1, DArray ar2)
{
  DArray result;

  assert( ! ar1.IsNull());
  assert( ! ar2.IsNull());

  ar1.Sort();
  ar2.Sort();

  int64_t  i = ar1.Count() - 1, j = ar2.Count() - 1;
  while(  (0 <= i) && (0 <= j))
    {
      T ti, tj;

      ar1.Get (i, ti);
      ar2.Get (j, tj);


      if (ti == tj)
        {
          result.Add (ti);
          --i, --j;
        }

      else if (ti < tj)
        --j;

      else
        --i;
    }

  return result;
}


static WLIB_STATUS
proc_interesect_arrays( SessionStack& stack, ISession&)
{
  DArray  array1, array2;
  DArray  result;

  stack[stack.Size() - 2].Operand().GetValue( array1);
  stack[stack.Size() - 1].Operand().GetValue( array2);
  stack.Pop (2);

  if (array1.Type() != array2.Type())
    {
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                            "Intersection requires array of the same type.");
    }

  if (array1.IsNull())
    {
      stack.Push( result);

      return WOP_OK;
    }
  else if (array2.IsNull())
    {
      stack.Push( array1);

      return WOP_OK;
    }

  switch( array1.Type())
    {
    case T_BOOL:
      result = intersect_arrays<DBool> (array1, array2);
      break;

    case T_CHAR:
      result = intersect_arrays<DChar> (array1, array2);
      break;

    case T_DATE:
      result = intersect_arrays<DDate> (array1, array2);
      break;

    case T_HIRESTIME:
      result = intersect_arrays<DHiresTime> (array1, array2);
      break;

    case T_INT8:
      result = intersect_arrays<DInt8> (array1, array2);
      break;

    case T_INT16:
      result = intersect_arrays<DInt16> (array1, array2);
      break;

    case T_INT32:
      result = intersect_arrays<DInt32> (array1, array2);
      break;

    case T_INT64:
      result = intersect_arrays<DInt64> (array1, array2);
      break;

    case T_UINT8:
      result = intersect_arrays<DUInt8> (array1, array2);
      break;

    case T_UINT16:
      result = intersect_arrays<DUInt16> (array1, array2);
      break;

    case T_UINT32:
      result = intersect_arrays<DUInt32> (array1, array2);
      break;

    case T_UINT64:
      result = intersect_arrays<DUInt64> (array1, array2);
      break;

    case T_REAL:
      result = intersect_arrays<DReal> (array1, array2);
      break;

    case T_RICHREAL:
      result = intersect_arrays<DRichReal> (array1, array2);
      break;

    default:
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                            "Unexpected array type.");
    }

  stack.Push( result);

  return WOP_OK;
}


template<typename T> DArray
diff_arrays( DArray& ar1, DArray ar2)
{
  DArray result;

  assert( ! ar1.IsNull());
  assert( ! ar2.IsNull());

  ar1.Sort();
  ar2.Sort();

  int64_t  i = ar1.Count() - 1, j = ar2.Count() - 1;
  while(  (0 <= i) && (0 <= j))
    {
      T ti, tj;

      ar1.Get (i, ti);
      ar2.Get (j, tj);


      if (ti == tj)
          --i;

      else if (ti < tj)
          --j;

      else
        {
          result.Add (ti);
          --i;
        }
    }

  while( 0 <= i)
    {
      T t;

      ar1.Get (i, t);
      result.Add (t);

      --i;
    }

  return result;
}


static WLIB_STATUS
proc_diff_arrays( SessionStack& stack, ISession&)
{
  DArray  array1, array2;
  DArray  result;

  stack[stack.Size() - 2].Operand().GetValue( array1);
  stack[stack.Size() - 1].Operand().GetValue( array2);
  stack.Pop (2);

  if (array1.Type() != array2.Type())
    {
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                            "Differentiation requires arrays of the same type.");
    }

  if (array1.IsNull())
    {
      stack.Push( result);

      return WOP_OK;
    }
  else if (array2.IsNull())
    {
      stack.Push( array1);

      return WOP_OK;
    }

  switch( array1.Type())
    {
    case T_BOOL:
      result = diff_arrays<DBool> (array1, array2);
      break;

    case T_CHAR:
      result = diff_arrays<DChar> (array1, array2);
      break;

    case T_DATE:
      result = diff_arrays<DDate> (array1, array2);
      break;

    case T_HIRESTIME:
      result = diff_arrays<DHiresTime> (array1, array2);
      break;

    case T_INT8:
      result = diff_arrays<DInt8> (array1, array2);
      break;

    case T_INT16:
      result = diff_arrays<DInt16> (array1, array2);
      break;

    case T_INT32:
      result = diff_arrays<DInt32> (array1, array2);
      break;

    case T_INT64:
      result = diff_arrays<DInt64> (array1, array2);
      break;

    case T_UINT8:
      result = diff_arrays<DUInt8> (array1, array2);
      break;

    case T_UINT16:
      result = diff_arrays<DUInt16> (array1, array2);
      break;

    case T_UINT32:
      result = diff_arrays<DUInt32> (array1, array2);
      break;

    case T_UINT64:
      result = diff_arrays<DUInt64> (array1, array2);
      break;

    case T_REAL:
      result = diff_arrays<DReal> (array1, array2);
      break;

    case T_RICHREAL:
      result = diff_arrays<DRichReal> (array1, array2);
      break;

    default:
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                            "Unexpected array type.");
    }

  stack.Push( result);

  return WOP_OK;
}


template<typename T> DArray
union_arrays( DArray& ar1, DArray ar2)
{
  DArray result = ar1;

  const uint64_t ar2Count = ar2.Count();

  for (uint64_t i = 0; i < ar2Count; ++i)
    {
      T t;

      ar2.Get (i, t);
      result.Add (t);
    }

  return result;
}


static WLIB_STATUS
proc_union_arrays( SessionStack& stack, ISession&)
{
  DArray  array1, array2;
  DArray  result;

  stack[stack.Size() - 2].Operand().GetValue( array1);
  stack[stack.Size() - 1].Operand().GetValue( array2);
  stack.Pop (2);

  if (array1.Type() != array2.Type())
    {
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                            "Union requires arrays of the same type.");
    }

  if (array1.IsNull())
    {
      stack.Push( result);

      return WOP_OK;
    }
  else if (array2.IsNull())
    {
      stack.Push( array1);

      return WOP_OK;
    }

  switch( array1.Type())
    {
    case T_BOOL:
      result = union_arrays<DBool> (array1, array2);
      break;

    case T_CHAR:
      result = union_arrays<DChar> (array1, array2);
      break;

    case T_DATE:
      result = union_arrays<DDate> (array1, array2);
      break;

    case T_HIRESTIME:
      result = union_arrays<DHiresTime> (array1, array2);
      break;

    case T_INT8:
      result = union_arrays<DInt8> (array1, array2);
      break;

    case T_INT16:
      result = union_arrays<DInt16> (array1, array2);
      break;

    case T_INT32:
      result = union_arrays<DInt32> (array1, array2);
      break;

    case T_INT64:
      result = union_arrays<DInt64> (array1, array2);
      break;

    case T_UINT8:
      result = union_arrays<DUInt8> (array1, array2);
      break;

    case T_UINT16:
      result = union_arrays<DUInt16> (array1, array2);
      break;

    case T_UINT32:
      result = union_arrays<DUInt32> (array1, array2);
      break;

    case T_UINT64:
      result = union_arrays<DUInt64> (array1, array2);
      break;

    case T_REAL:
      result = union_arrays<DReal> (array1, array2);
      break;

    case T_RICHREAL:
      result = union_arrays<DRichReal> (array1, array2);
      break;

    default:
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                            "Unexpected array type.");
    }

  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
proc_array_pushback( SessionStack& stack, ISession&)
{
  DArray array;

  IOperand& opArray = stack[stack.Size() - 2].Operand();
  IOperand& opValue = stack[stack.Size() - 1].Operand();

  if (IS_TABLE( opValue.GetType())
      || IS_ARRAY( opValue.GetType()))
    {
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE));
    }

  opArray.GetValue( array);

  const uint_t type = (array.Type() != T_UNDETERMINED) ?
                        array.Type() :
                        GET_BASIC_TYPE( opValue.GetType());
  switch( type)
    {
    case T_BOOL:
        {
          DBool value;

          opValue.GetValue( value);
          array.Add(value);
        }
      break;

    case T_CHAR:
        {
          DChar value;

          opValue.GetValue( value);

          if (! value.IsNull())
            array.Add(value);
        }
      break;

    case T_DATE:
        {
          DDate value;

          opValue.GetValue( value);
          array.Add (value);
        }
      break;

    case T_DATETIME:
        {
          DDateTime value;

          opValue.GetValue( value);
          array.Add (value);
        }
      break;

    case T_HIRESTIME:
        {
          DHiresTime value;

          opValue.GetValue( value);

          array.Add (value);
        }
      break;

    case T_INT8:
        {
          DInt8 value;

          opValue.GetValue( value);
          array.Add (value);
        }
      break;

    case T_INT16:
        {
          DInt16 value;

          opValue.GetValue( value);
          array.Add (value);
        }
      break;

    case T_INT32:
        {
          DInt32 value;

          opValue.GetValue( value);
          array.Add(value);
        }
      break;

    case T_INT64:
        {
          DInt64 value;

          opValue.GetValue( value);
          array.Add(value);
        }
      break;

    case T_UINT8:
        {
          DUInt8 value;

          opValue.GetValue( value);
          array.Add(value);
        }
      break;

    case T_UINT16:
        {
          DUInt16 value;

          opValue.GetValue( value);
          array.Add(value);
        }
      break;

    case T_UINT32:
        {
          DUInt32 value;

          opValue.GetValue( value);
          array.Add(value);
        }
      break;

    case T_UINT64:
        {
          DUInt64 value;

          opValue.GetValue( value);
          array.Add(value);
        }
      break;

    case T_REAL:
        {
          DReal value;

          opValue.GetValue( value);
          array.Add(value);
        }
      break;

    case T_RICHREAL:
        {
          DRichReal value;

          opValue.GetValue( value);
          array.Add(value);
        }
      break;

    default:
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE));
    }

  opArray.SetValue( array);
  stack.Pop (2);
  stack.Push( array);

  return WOP_OK;
}


static WLIB_STATUS
proc_array_truncate( SessionStack& stack, ISession&)
{
  DArray      array;
  DInt64      newArraySize;

  IOperand&   refOp = stack[stack.Size() - 2].Operand();

  stack[stack.Size() - 2].Operand().GetValue( array);
  stack[stack.Size() - 1].Operand().GetValue( newArraySize);

  if (array.IsNull() || newArraySize.IsNull())
    {
      stack.Pop (1);
      return WOP_OK;
    }

  for (int64_t i = array.Count() - 1;
       MAX (newArraySize.mValue, 0) <= i;
       --i)
    {
      array.Remove( i);
    }

  refOp.SetValue( array);

  stack.Pop (2);
  stack.Push( array);

  return WOP_OK;
}


template<typename T> uint64_t
compute_array_hash( const DArray& array)
{
  uint8_t key[512];

  const uint64_t arrayCount = array.Count();

  assert( arrayCount > 0);

  uint_t   keyOff     = 0;
  uint_t   arrayOff   = 0;

  T temp;
  for (keyOff = 0, arrayOff = 0;
       (keyOff < sizeof( key) - sizeof( temp)) && (arrayOff < arrayCount);
       keyOff += sizeof( temp), ++arrayOff)
    {
      array.Get (arrayOff, temp);

      memcpy( key + keyOff, &temp, sizeof( temp));
    }

  return wh_hash( key, keyOff);
}

static WLIB_STATUS
proc_hash_array( SessionStack& stack, ISession&)
{
  DArray   array;
  DUInt64  result;

  stack[stack.Size() - 1].Operand().GetValue( array);
  stack.Pop (1);

  if (array.IsNull())
    {
      stack.Push( result);

      return WOP_OK;
    }

  switch( array.Type())
    {
    case T_BOOL:
      result = DUInt64 (compute_array_hash<DBool> (array));
      break;

    case T_CHAR:
      result = DUInt64 (compute_array_hash<DChar> (array));
      break;

    case T_DATE:
      result = DUInt64 (compute_array_hash<DDate> (array));
      break;

    case T_HIRESTIME:
      result = DUInt64 (compute_array_hash<DHiresTime> (array));
      break;

    case T_INT8:
      result = DUInt64 (compute_array_hash<DInt8> (array));
      break;

    case T_INT16:
      result = DUInt64 (compute_array_hash<DInt16> (array));
      break;

    case T_INT32:
      result = DUInt64 (compute_array_hash<DInt32> (array));
      break;

    case T_INT64:
      result = DUInt64 (compute_array_hash<DInt64> (array));
      break;

    case T_UINT8:
      result = DUInt64 (compute_array_hash<DUInt8> (array));
      break;

    case T_UINT16:
      result = DUInt64 (compute_array_hash<DUInt16> (array));
      break;

    case T_UINT32:
      result = DUInt64 (compute_array_hash<DUInt32> (array));
      break;

    case T_UINT64:
      result = DUInt64 (compute_array_hash<DUInt64> (array));
      break;

    case T_REAL:
      result = DUInt64 (compute_array_hash<DReal> (array));
      break;

    case T_RICHREAL:
      result = DUInt64 (compute_array_hash<DRichReal> (array));
      break;

    default:
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                            "Unexpected array type.");
    }

  stack.Push( result);

  return WOP_OK;
}


WLIB_STATUS
base_arrays_init()
{

  static const uint8_t* arrayCountLocals[] = {
                                                gUInt64Type,
                                                gGenericArrayType
                                             };
  gProcArrayCount.name        = "array_count";
  gProcArrayCount.localsCount = 2;
  gProcArrayCount.localsTypes = arrayCountLocals;
  gProcArrayCount.code        = array_elems_count;


  static const uint8_t* arraySortLocals[]  = {
                                                gGenericArrayType,
                                                gGenericArrayType,
                                                gBoolType
                                              };

  gProcArraySort.name        = "array_sort";
  gProcArraySort.localsCount = 3;
  gProcArraySort.localsTypes = arraySortLocals;
  gProcArraySort.code        = array_sort;


  static const uint8_t* binSearchLocals[] = {
                                               gUInt64Type,
                                               gGenericArrayType,
                                               gUndefinedType,
                                               gInt8Type
                                            };

  gProcArrayBinSearch.name         = "array_binary_search";
  gProcArrayBinSearch.localsCount  = 4;
  gProcArrayBinSearch.localsTypes  = binSearchLocals;
  gProcArrayBinSearch.code         = binary_array_search_proc;



  static const uint8_t* searchLocals[] = {
                                            gUInt64Type,
                                            gGenericArrayType,
                                            gUndefinedType
                                         };

  gProcArraySearch.name         = "array_search";
  gProcArraySearch.localsCount  = 3;
  gProcArraySearch.localsTypes  = searchLocals;
  gProcArraySearch.code         = unsorted_array_search_proc;


  gProcArrayMin.name         = "array_min";
  gProcArrayMin.localsCount  = 2;
  gProcArrayMin.localsTypes  = searchLocals; //reuse
  gProcArrayMin.code         = search_minmax<true>;

  gProcArrayMax.name         = "array_max";
  gProcArrayMax.localsCount  = 2;
  gProcArrayMax.localsTypes  = searchLocals; //reuse
  gProcArrayMax.code         = search_minmax<false>;



  static const uint8_t* computeAverageLocals[] = {
                                                    gRichRealType,
                                                    gGenericArrayType
                                                 };

  gProcArrayAverage.name        = "array_average";
  gProcArrayAverage.localsCount = 2;
  gProcArrayAverage.localsTypes = computeAverageLocals;
  gProcArrayAverage.code        = compute_array_average;


  static const uint8_t* arraySetOperationLocals[] = {
                                                      gGenericArrayType,
                                                      gGenericArrayType,
                                                      gGenericArrayType
                                                    };

  gProcArrayIntersect.name        = "array_intersection";
  gProcArrayIntersect.localsCount = 3;
  gProcArrayIntersect.localsTypes = arraySetOperationLocals;
  gProcArrayIntersect.code        = proc_interesect_arrays;

  gProcArrayUnion.name            = "array_union";
  gProcArrayUnion.localsCount     = 3;
  gProcArrayUnion.localsTypes     = arraySetOperationLocals;
  gProcArrayUnion.code            = proc_union_arrays;

  gProcArrayDiff.name             = "array_diff";
  gProcArrayDiff.localsCount      = 3;
  gProcArrayDiff.localsTypes      = arraySetOperationLocals;
  gProcArrayDiff.code             = proc_diff_arrays;


  static const uint8_t* arrayPushBackLocals[] = {
                                                  gGenericArrayType,
                                                  gGenericArrayType,
                                                  gUndefinedType
                                                };

  gProcArrayPushBack.name        = "array_pushback";
  gProcArrayPushBack.localsCount = 3;
  gProcArrayPushBack.localsTypes = arrayPushBackLocals;
  gProcArrayPushBack.code        = proc_array_pushback;

  static const uint8_t* arrayTruncateLocals[] = {
                                                  gGenericArrayType,
                                                  gGenericArrayType,
                                                  gUInt64Type
                                                };

  gProcArrayTruncate.name        = "array_truncate";
  gProcArrayTruncate.localsCount = 3;
  gProcArrayTruncate.localsTypes = arrayTruncateLocals;
  gProcArrayTruncate.code        = proc_array_truncate;


  gProcArrayHash.name         = "array_hash";
  gProcArrayHash.localsCount  = 2;
  gProcArrayHash.localsTypes  = arrayCountLocals; //Reusing!
  gProcArrayHash.code         = proc_hash_array;

  return WOP_OK;
}


