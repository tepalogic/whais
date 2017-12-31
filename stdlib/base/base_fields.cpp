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
#include <utility>

#include "base_fields.h"
#include "base_types.h"

using namespace std;
using namespace whais;


WLIB_PROC_DESCRIPTION         gProcFieldTable;
WLIB_PROC_DESCRIPTION         gProcIsFielsIndexed;
WLIB_PROC_DESCRIPTION         gProcFieldName;
WLIB_PROC_DESCRIPTION         gProcFieldIndex;

WLIB_PROC_DESCRIPTION         gProcFindValueRange;

WLIB_PROC_DESCRIPTION         gProcFieldMinimum;
WLIB_PROC_DESCRIPTION         gProcFieldMaximum;
WLIB_PROC_DESCRIPTION         gProcFieldAverage;

WLIB_PROC_DESCRIPTION         gProcFieldSortTable;


static WLIB_STATUS
proc_field_table( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 1].Operand();
  StackValue temp = op.GetTableValue();

  stack.Pop(1);
  stack.Push(move(temp));

  return WOP_OK;
}


static WLIB_STATUS
proc_field_isindexed( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 1].Operand();

  if (op.IsNull())
  {
    stack.Pop(1);
    stack.Push(DBool());

    return WOP_OK;
  }

  ITable&           table = op.GetTable();
  const FIELD_INDEX field = op.GetField();

  DBool result( table.IsIndexed( field));

  stack.Pop(1);
  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
proc_field_name( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 1].Operand();

  if (op.IsNull())
  {
    stack.Pop(1);
    stack.Push(DText());

    return WOP_OK;
  }

  ITable& table = op.GetTable();
  const FIELD_INDEX field = op.GetField();

  DBSFieldDescriptor fd = table.DescribeField(field);
  DText result(fd.name);

  stack.Pop(1);
  stack.Push(result);

  return WOP_OK;
}


static WLIB_STATUS
proc_field_index( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 1].Operand();

  if (op.IsNull())
  {
    stack.Pop(1);
    stack.Push(DUInt64());

    return WOP_OK;
  }

  const uint64_t field = op.GetField();

  stack.Pop(1);
  stack.Push(DUInt64(field));

  return WOP_OK;
}


template<typename T> DArray
match_field_rows( ITable&               table,
                  const FIELD_INDEX     field,
                  const T&              from,
                  const T&              to,
                  const ROW_INDEX       fromRow,
                  const ROW_INDEX       toRow)
{
  if (from <= to)
    return table.MatchRows(from, to, fromRow, toRow, field);

  return table.MatchRows(to, from, fromRow, toRow, field);
}


static WLIB_STATUS
proc_field_find_range( SessionStack& stack, ISession&)
{
  IOperand& opField = stack[stack.Size() - 5].Operand();

  if (opField.IsNull())
  {
    stack.Pop(5);
    stack.Push(DArray());

    return WOP_OK;
  }

  const uint_t fieldType = opField.GetType();
  if (IS_ARRAY( fieldType)
      || (GET_BASIC_TYPE( fieldType) <= T_UNKNOWN)
      || (GET_BASIC_TYPE( fieldType) >= T_TEXT))
    {
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                            "Matching field values is available only for "
                              "basic types( e.g. reals, integers, dates, etc. "
                              "and not for arrays or text.");

    }

  IOperand& opFrom = stack[stack.Size() - 4].Operand();
  IOperand& opTo = stack[stack.Size() - 3].Operand();
  IOperand& opFromRow = stack[stack.Size() - 2].Operand();
  IOperand& opToRow = stack[stack.Size() - 1].Operand();
  ITable& table = opField.GetTable();
  DArray result;
  DUInt64 row;

  opFromRow.GetValue(row);
  const ROW_INDEX fromRow = row.IsNull() ? 0 : row.mValue;

  opToRow.GetValue(row);
  const ROW_INDEX toRow = row.IsNull() ? table.AllocatedRows() - 1 : row.mValue;

  const FIELD_INDEX field = opField.GetField();
  switch (GET_BASIC_TYPE(fieldType))
  {
  case T_BOOL:
  {
    DBool from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_DATE:
  {
    DDate from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_DATETIME:
  {
    DDateTime from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_HIRESTIME:
  {
    DHiresTime from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_INT8:
  {
    DInt8 from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_INT16:
  {
    DInt16 from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_INT32:
  {
    DInt32 from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_INT64:
  {
    DInt64 from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_UINT8:
  {
    DUInt8 from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_UINT16:
  {
    DUInt16 from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_UINT32:
  {
    DUInt32 from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_UINT64:
  {
    DUInt64 from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_REAL:
  {
    DReal from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  case T_RICHREAL:
  {
    DRichReal from, to;

    opFrom.GetValue(from);
    opTo.GetValue(to);

    result = match_field_rows(table, field, from, to, fromRow, toRow);
  }
    break;

  default:
    throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
  }

  stack.Pop(5);
  stack.Push(result);

  return WOP_OK;
}


template<typename T> uint64_t
retrieve_minim_value(ITable& table, const FIELD_INDEX field)
{
  const uint64_t rowsCount = table.AllocatedRows();
  T minim = T::Max();
  uint64_t foundRow = 0;

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
  {
    T value;
    table.Get(row, field, value);

    if ( !value.IsNull() && (value <= minim))
    {
      minim = value;
      foundRow = row;
    }
  }

  return foundRow;
}


template<typename T> uint64_t
retrieve_maxim_value( ITable&           table,
                      const FIELD_INDEX field)
{
  const uint64_t rowsCount = table.AllocatedRows();
  T maxim = T::Min();
  uint64_t foundRow = 0;

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
  {
    T value;
    table.Get(row, field, value);

    if ( !value.IsNull() && (maxim <= value))
    {
      maxim = value;
      foundRow = row;
    }
  }

  return foundRow;
}


template<bool minSearch> WLIB_STATUS
field_search_minmax( SessionStack& stack, ISession&)
{
  IOperand& opField = stack[stack.Size() - 1].Operand();
  uint64_t foundRow;

  if (opField.IsNull())
  {
    stack.Pop(1);
    stack.Push(DUInt64());

    return WOP_OK;
  }

  const uint_t fieldType = opField.GetType();
  if (IS_ARRAY(fieldType) || (GET_BASIC_TYPE( fieldType) <= T_UNKNOWN)
      || (GET_BASIC_TYPE( fieldType) >= T_TEXT))
  {
    throw InterException(_EXTRA(InterException::INVALID_PARAMETER_TYPE),
                         "Searching for min and max values is available "
                         "only for basic types( e.g. reals, integers, "
                         "dates, etc. and not for arrays or text.");
  }

  ITable& table = opField.GetTable();
  const FIELD_INDEX field = opField.GetField();

  switch (GET_BASIC_TYPE(fieldType))
  {
  case T_BOOL:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DBool>(table, field) : retrieve_maxim_value<DBool>(table, field);
  }
    break;

  case T_CHAR:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DChar>(table, field) : retrieve_maxim_value<DChar>(table, field);
  }
    break;

  case T_DATE:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DDate>(table, field) : retrieve_maxim_value<DDate>(table, field);
  }
    break;

  case T_DATETIME:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DDateTime>(table, field) :
            retrieve_maxim_value<DDateTime>(table, field);
  }
    break;

  case T_HIRESTIME:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DHiresTime>(table, field) :
            retrieve_maxim_value<DHiresTime>(table, field);
  }
    break;

  case T_INT8:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DInt8>(table, field) : retrieve_maxim_value<DInt8>(table, field);
  }
    break;

  case T_INT16:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DInt16>(table, field) : retrieve_maxim_value<DInt16>(table, field);
  }
    break;

  case T_INT32:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DInt32>(table, field) : retrieve_maxim_value<DInt32>(table, field);
  }
    break;

  case T_INT64:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DInt32>(table, field) : retrieve_maxim_value<DInt32>(table, field);
  }
    break;

  case T_UINT8:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DUInt8>(table, field) : retrieve_maxim_value<DUInt8>(table, field);
  }
    break;

  case T_UINT16:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DUInt16>(table, field) :
            retrieve_maxim_value<DUInt16>(table, field);
  }
    break;

  case T_UINT32:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DUInt32>(table, field) :
            retrieve_maxim_value<DUInt32>(table, field);
  }
    break;

  case T_UINT64:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DUInt64>(table, field) :
            retrieve_maxim_value<DUInt64>(table, field);
  }
    break;

  case T_REAL:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DReal>(table, field) : retrieve_maxim_value<DReal>(table, field);
  }
    break;

  case T_RICHREAL:
  {
    foundRow =
        minSearch ?
            retrieve_minim_value<DRichReal>(table, field) :
            retrieve_maxim_value<DRichReal>(table, field);
  }
    break;

  default:
    throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
  }

  stack.Pop(1);
  stack.Push(DUInt64(foundRow));

  return WOP_OK;
}


template<typename T> DRichReal
compute_integer_field_average_value(ITable& table, const FIELD_INDEX field)
{
  const uint64_t rowsCount = table.AllocatedRows();
  uint64_t rowsAdded = 0;
  WE_I128 sum = 0;

  T currentValue;
  for (uint64_t row = 0; row < rowsCount; ++row)
  {
    table.Get(row, field, currentValue);
    if ( !currentValue.IsNull())
    {
      sum += currentValue.mValue;
      ++rowsAdded;
    }
  }

  if (rowsAdded == 0)
    return DRichReal();

  const RICHREAL_T quotient = sum / rowsAdded;
  const RICHREAL_T reminder = sum % rowsAdded;

  return DRichReal(quotient + reminder / rowsAdded);
}


template<typename T> DRichReal
compute_real_field_average_value(ITable& table, const FIELD_INDEX field)
{
  const uint64_t rowsCount = table.AllocatedRows();
  uint64_t rowsAdded = 0;
  WE_I128 integerSum = 0, fractionalSum = 0;

  T currentValue;
  for (uint64_t row = 0; row < rowsCount; ++row)
  {
    table.Get(row, field, currentValue);

    if ( !currentValue.IsNull())
    {
      const RICHREAL_T temp = currentValue.mValue;

      integerSum += temp.Integer();
      fractionalSum += temp.Fractional();

      ++rowsAdded;
    }
  }

  if (rowsAdded == 0)
    return DRichReal();

  integerSum += fractionalSum / DBS_RICHREAL_PREC;
  fractionalSum %= DBS_RICHREAL_PREC;

  RICHREAL_T result = _SC(RICHREAL_T, integerSum) / rowsAdded;
  result += RICHREAL_T(0, toInt64(fractionalSum), DBS_RICHREAL_PREC) / rowsAdded;

  return DRichReal(result);
}


static WLIB_STATUS
compute_field_average( SessionStack& stack, ISession&)
{
  IOperand& opField = stack[stack.Size() - 1].Operand();

  if (opField.IsNull())
  {
    stack.Pop(1);
    stack.Push(DRichReal());

    return WOP_OK;
  }

  const uint_t fieldType = opField.GetType();
  if (IS_ARRAY( fieldType)
      || (GET_BASIC_TYPE( fieldType) <= T_UNKNOWN)
      || (GET_BASIC_TYPE( fieldType) >= T_TEXT))
  {
    throw InterException(_EXTRA(InterException::INVALID_PARAMETER_TYPE),
                         "Computing field average value is available only for basic types( e.g."
                         " reals, integers, dates, etc. and not for arrays or text.");
  }

  ITable&           table = opField.GetTable();
  const FIELD_INDEX field = opField.GetField();
  DRichReal         result;

  switch (fieldType)
  {
  case T_INT8:
    result = compute_integer_field_average_value<DInt8>(table, field);
    break;

  case T_INT16:
    result = compute_integer_field_average_value<DInt16>(table, field);
    break;

  case T_INT32:
    result = compute_integer_field_average_value<DInt32>(table, field);
    break;

  case T_INT64:
    result = compute_integer_field_average_value<DInt64>(table, field);
    break;

  case T_UINT8:
    result = compute_integer_field_average_value<DUInt8>(table, field);
    break;

  case T_UINT16:
    result = compute_integer_field_average_value<DUInt16>(table, field);
    break;

  case T_UINT32:
    result = compute_integer_field_average_value<DUInt32>(table, field);
    break;

  case T_UINT64:
    result = compute_integer_field_average_value<DUInt64>(table, field);
    break;

  case T_REAL:
    result = compute_real_field_average_value<DReal>(table, field);
    break;

  case T_RICHREAL:
    result = compute_real_field_average_value<DRichReal>(table, field);
    break;

  default:
    throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
  }

  stack.Pop(1);
  stack.Push(result);

  return WOP_OK;
}


static WLIB_STATUS
field_sort_table( SessionStack& stack, ISession&)
{
  DBool result, reverseSort;

  IOperand& opField = stack[stack.Size() - 2].Operand();
  stack[stack.Size() - 1].Operand().GetValue(reverseSort);

  if ( !opField.IsNull())
  {
    const FIELD_INDEX field = opField.GetField();
    ITable& table = opField.GetTable();

    const auto rowsCount = table.AllocatedRows();
    if (rowsCount != 0)
    {
      table.Sort(field, 0, rowsCount - 1, reverseSort == DBool(true));
      result = DBool(true);
    }
  }

  stack.Pop(1);
  return WOP_OK;
}


WLIB_STATUS
base_fields_init()
{
  static const uint8_t* fieldTableLocals[] = {
                                                gGenericTableType,
                                                gGenericFieldType
                                             };

  gProcFieldTable.name        = "table_of_field";
  gProcFieldTable.localsCount = 2;
  gProcFieldTable.localsTypes = fieldTableLocals;
  gProcFieldTable.code        = proc_field_table;


  static const uint8_t* fieldIsIndexedLocals[] = {
                                                    gBoolType,
                                                    gGenericFieldType
                                                 };

  gProcIsFielsIndexed.name        = "field_values_indexed";
  gProcIsFielsIndexed.localsCount = 2;
  gProcIsFielsIndexed.localsTypes = fieldIsIndexedLocals;
  gProcIsFielsIndexed.code        = proc_field_isindexed;


  static const uint8_t* fieldNameLocals[] = { gTextType, gGenericFieldType };

  gProcFieldName.name        = "field_name";
  gProcFieldName.localsCount = 2;
  gProcFieldName.localsTypes = fieldNameLocals;
  gProcFieldName.code        = proc_field_name;


  static const uint8_t* fieldIndexLocals[] = {
                                                gUInt64Type,
                                                gGenericFieldType
                                             };

  gProcFieldIndex.name        = "field_index";
  gProcFieldIndex.localsCount = 2;
  gProcFieldIndex.localsTypes = fieldIndexLocals;
  gProcFieldIndex.code        = proc_field_index;


  static const uint8_t* fieldMatchValuesLocals[] = {
                                                     gAUInt64Type,
                                                     gGenericFieldType,
                                                     gUndefinedType,
                                                     gUndefinedType,
                                                     gUInt64Type,
                                                     gUInt64Type
                                                   };

  gProcFindValueRange.name        = "match_rows";
  gProcFindValueRange.localsCount = 6;
  gProcFindValueRange.localsTypes = fieldMatchValuesLocals;
  gProcFindValueRange.code        = proc_field_find_range;


  static const uint8_t* fieldMinimumLocals[] = {
                                                 gUInt64Type,
                                                 gGenericFieldType,
                                                 gBoolType,
                                                 gUndefinedType
                                               };

  gProcFieldMinimum.name        = "field_biggest";
  gProcFieldMinimum.localsCount = 4;
  gProcFieldMinimum.localsTypes = fieldMinimumLocals;
  gProcFieldMinimum.code        = field_search_minmax<true>;

  gProcFieldMaximum.name        = "field_smallest";
  gProcFieldMaximum.localsCount = 4;
  gProcFieldMaximum.localsTypes = fieldMinimumLocals; //reusing
  gProcFieldMaximum.code        = field_search_minmax<false>;


  static const uint8_t* fieldAverageLocals[] = {
                                                 gRichRealType,
                                                 gGenericFieldType
                                               };

  gProcFieldAverage.name        = "field_average";
  gProcFieldAverage.localsCount = 2;
  gProcFieldAverage.localsTypes = fieldAverageLocals;
  gProcFieldAverage.code        = compute_field_average;


  static const uint8_t* fieldSortTableLocals[] = {
                                                   gGenericTableType,
                                                   gGenericFieldType,
                                                   gBoolType
                                                 };

  gProcFieldSortTable.name        = "field_sort_table";
  gProcFieldSortTable.localsCount = 3;
  gProcFieldSortTable.localsTypes = fieldSortTableLocals;
  gProcFieldSortTable.code        = field_sort_table;

  return WOP_OK;
}


