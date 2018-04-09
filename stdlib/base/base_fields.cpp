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

WLIB_PROC_DESCRIPTION         gProcFindValueRange;
WLIB_PROC_DESCRIPTION         gProcFilterRows;

WLIB_PROC_DESCRIPTION         gProcFieldMinimum;
WLIB_PROC_DESCRIPTION         gProcFieldMaximum;
WLIB_PROC_DESCRIPTION         gProcFieldAverage;



static WLIB_STATUS
proc_field_table( SessionStack& stack, ISession&)
{
  const auto stackTop = stack.Size() - 1;
  IOperand& op = stack[stackTop].Operand();
  stack[stackTop] = op.IsNull() ? StackValue() : op.GetTableValue();
  return WOP_OK;
}


static WLIB_STATUS
proc_field_isindexed( SessionStack& stack, ISession&)
{
  const auto stackTop = stack.Size() - 1;
  IOperand& op = stack[stackTop].Operand();

  if (op.IsNull())
  {
    stack[stackTop] = StackValue::Create(DBool());
    return WOP_OK;
  }

  ITable&           table = op.GetTable();
  const FIELD_INDEX field = op.GetField();

  DBool result(table.IsIndexed(field));
  stack[stackTop] = StackValue::Create(result);
  return WOP_OK;
}


static WLIB_STATUS
proc_field_name( SessionStack& stack, ISession&)
{
  const auto stackTop = stack.Size() - 1;
  IOperand& op = stack[stack.Size() - 1].Operand();

  if (op.IsNull())
  {
    stack[stackTop] = StackValue::Create(DText());
    return WOP_OK;
  }

  ITable& table = op.GetTable();
  const FIELD_INDEX field = op.GetField();

  DBSFieldDescriptor fd = table.DescribeField(field);
  DText result(fd.name);

  stack[stackTop] = StackValue::Create(result);
  return WOP_OK;
}


template<typename T> DArray
match_field_rows( ITable&               table,
                  const FIELD_INDEX     field,
                  T                     from,
                  T                     to,
                  ROW_INDEX             fromRow,
                  ROW_INDEX             toRow)
{
  if (to < from)
    swap(from, to);

  if (toRow < fromRow)
    swap(fromRow, toRow);

  return table.MatchRows(from, to, fromRow, toRow, field);

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
      || (GET_BASE_TYPE( fieldType) <= T_UNKNOWN)
      || (GET_BASE_TYPE( fieldType) >= T_TEXT))
  {
    throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                          "Matching field values is available only for "
                          "basic types( e.g. reals, integers, dates, etc. "
                          "and not for arrays or text.");

  }

  const auto stackTop = stack.Size() - 1;
  IOperand& opFrom = stack[stackTop - 3].Operand();
  IOperand& opTo = stack[stackTop - 2].Operand();
  IOperand& opFromRow = stack[stackTop - 1].Operand();
  IOperand& opToRow = stack[stackTop].Operand();
  ITable& table = opField.GetTable();
  DArray result;
  DUInt64 row;

  opFromRow.GetValue(row);
  const ROW_INDEX fromRow = row.IsNull() ? 0 : row.mValue;

  opToRow.GetValue(row);
  const ROW_INDEX toRow = row.IsNull() ? table.AllocatedRows() - 1 : row.mValue;

  const FIELD_INDEX field = opField.GetField();
  switch (GET_BASE_TYPE(fieldType))
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

template<typename T> bool
is_in_set(const DArray& set, const T e, const bool addNull)
{
  if (e.IsNull())
    return addNull;

  const uint_t to = set.Count();
  for (uint_t from = 0; from < to; ++from)
  {
    T current;
    set.Get(from, current);

    if (current == e)
      return true;
  }

  return false;
}

template<typename T> DArray
test_rows (ITable& table,
           const FIELD_INDEX f,
           DArray& rows,
           DArray& set,
           const bool addNull,
           const bool fiterOut)
{
  DArray result;

  const ROW_INDEX rowsCount = table.AllocatedRows();
  const ROW_INDEX count = rows.Count();
  for ( ROW_INDEX i = 0; i < count; ++i)
  {
    DROW_INDEX row;
    rows.Get(i, row);

    if (row.mValue >= rowsCount)
      continue ;

    T current;
    table.Get(row.mValue, f, current);

    const bool inSet = is_in_set(set, current, addNull);
    if (inSet ^ fiterOut)
      result.Add(row);
  }

  return result;
}



static WLIB_STATUS
proc_field_filter_rows(SessionStack& stack, ISession&)
{
  DArray result;

  const auto stackTop = stack.Size() - 1;
  IOperand& field = stack[stackTop - 4].Operand();

  if (field.IsNullExpression() || field.IsNull())
  {
    stack.Pop(5);
    stack.Push(result);

    return WOP_OK;
  }

  const uint_t fieldType = field.GetType();
  if (IS_ARRAY( fieldType)
      || (GET_BASE_TYPE( fieldType) <= T_UNKNOWN)
      || (GET_BASE_TYPE( fieldType) >= T_TEXT))
  {
    throw InterException( _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                          "Matching field values is available only for "
                          "basic types( e.g. reals, integers, dates, etc. "
                          "and not for arrays or text.");

  }

  DArray set, rows;
  DBool addNull, filterout;

  stack[stackTop - 0].Operand().GetValue(filterout);
  stack[stackTop - 1].Operand().GetValue(addNull);
  stack[stackTop - 2].Operand().GetValue(rows);
  stack[stackTop - 3].Operand().GetValue(set);

  if (filterout.IsNull())
    filterout = DBool(false);

  if (addNull.IsNull())
    addNull = DBool(false);

  if ( ! set.IsNull() && GET_BASE_TYPE(set.Type()) != GET_BASE_TYPE(fieldType))
    throw InterException(_EXTRA(InterException::FIELD_TYPE_MISMATCH));

  ITable& table = field.GetTable();
  const FIELD_INDEX f = field.GetField();
  switch (GET_BASE_TYPE(fieldType))
  {
  case T_BOOL:
    result = test_rows<DBool>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_CHAR:
    result = test_rows<DChar>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_DATE:
    result = test_rows<DDate>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_DATETIME:
    result = test_rows<DDateTime>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_HIRESTIME:
    result = test_rows<DHiresTime>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_INT8:
    result = test_rows<DInt8>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_INT16:
    result = test_rows<DInt16>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_INT32:
    result = test_rows<DInt32>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_INT64:
    result = test_rows<DInt64>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_UINT8:
    result = test_rows<DUInt8>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_UINT16:
    result = test_rows<DUInt16>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_UINT32:
    result = test_rows<DUInt32>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_UINT64:
    result = test_rows<DUInt64>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_REAL:
    result = test_rows<DUInt64>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  case T_RICHREAL:
    result = test_rows<DRichReal>(table, f, rows, set, addNull.mValue, filterout.mValue);
    break;

  default:
    throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
  }

  stack.Pop(5);
  stack.Push(result);
  return WOP_OK;
}


template<typename T> DArray
retrieve_minim_value(ITable& table, const FIELD_INDEX field, T margin)
{
  const uint64_t rowsCount = table.AllocatedRows();
  T minim = T::Max();
  DArray result;

  if (margin.IsNull())
    margin = T::Min();

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
  {
    T value;
    table.Get(row, field, value);

    if ( ! value.IsNull() && (value > margin))
    {
      if (value < minim)
      {
        minim = value;
        result = DArray();
        result.Add(DROW_INDEX(row));
      }
      else if (value == minim)
        result.Add(DROW_INDEX(row));
    }
  }

  return result;
}


template<typename T> DArray
retrieve_maxim_value( ITable& table, const FIELD_INDEX field, T margin)
{
  const uint64_t rowsCount = table.AllocatedRows();
  T maxim = T::Min();
  DArray result;

  if (margin.IsNull())
    margin = T::Max();

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
  {
    T value;
    table.Get(row, field, value);

    if ( ! value.IsNull() && (value < margin))
    {
      if (maxim < value)
      {
        maxim = value;
        result = DArray();
        result.Add(DROW_INDEX(row));
      }
      else if (maxim == value)
        result.Add(DROW_INDEX(row));
    }
  }

  return result;
}


template<bool minSearch> WLIB_STATUS
field_search_minmax( SessionStack& stack, ISession&)
{
  const auto stackTop = stack.Size() - 1;
  IOperand& opField = stack[stackTop - 1].Operand();
  DArray foundRows;

  if (opField.IsNull())
  {
    stack.Pop(1);
    stack[stackTop - 1] = StackValue::Create(DUInt64());

    return WOP_OK;
  }

  const uint_t fieldType = opField.GetType();
  if (IS_ARRAY(fieldType) || (GET_BASE_TYPE( fieldType) <= T_UNKNOWN)
      || (GET_BASE_TYPE( fieldType) >= T_TEXT))
  {
    throw InterException(_EXTRA(InterException::INVALID_PARAMETER_TYPE),
                         "Searching for min and max values is available "
                         "only for basic types( e.g. reals, integers, "
                         "dates, etc. and not for arrays or text.");
  }

  ITable& table = opField.GetTable();
  const FIELD_INDEX field = opField.GetField();

  switch (GET_BASE_TYPE(fieldType))
  {
  case T_BOOL:
  {
    DBool margin;
    stack[stackTop].Operand().GetValue(margin);

    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_CHAR:
  {
    DChar margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_DATE:
  {
    DDate margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_DATETIME:
  {
    DDateTime margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_HIRESTIME:
  {
    DHiresTime margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_INT8:
  {
    DInt8 margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_INT16:
  {
    DInt16 margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_INT32:
  {
    DInt32 margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_INT64:
  {
    DInt64 margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_UINT8:
  {
    DUInt8 margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_UINT16:
  {
    DUInt16 margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_UINT32:
  {
    DUInt32 margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_UINT64:
  {
    DUInt64 margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_REAL:
  {
    DReal margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  case T_RICHREAL:
  {
    DRichReal margin;
    stack[stackTop].Operand().GetValue(margin);
    foundRows = minSearch
                ? retrieve_minim_value(table, field, margin)
                : retrieve_maxim_value(table, field, margin);
  }
    break;

  default:
    throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
  }

  stack.Pop(1);
  stack[stackTop - 1] = StackValue::Create(foundRows);

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

WLIB_STATUS
base_fields_init()
{
  static const uint8_t* fieldTableLocals[] = {
                                                gGenericTableType,
                                                gGenericFieldType
                                             };

  gProcFieldTable.name        = "get_table";
  gProcFieldTable.localsCount = 2;
  gProcFieldTable.localsTypes = fieldTableLocals;
  gProcFieldTable.code        = proc_field_table;


  static const uint8_t* fieldIsIndexedLocals[] = {
                                                    gBoolType,
                                                    gGenericFieldType
                                                 };

  gProcIsFielsIndexed.name        = "is_indexed";
  gProcIsFielsIndexed.localsCount = 2;
  gProcIsFielsIndexed.localsTypes = fieldIsIndexedLocals;
  gProcIsFielsIndexed.code        = proc_field_isindexed;


  static const uint8_t* fieldNameLocals[] = { gTextType, gGenericFieldType };

  gProcFieldName.name        = "get_name";
  gProcFieldName.localsCount = 2;
  gProcFieldName.localsTypes = fieldNameLocals;
  gProcFieldName.code        = proc_field_name;


  static const uint8_t* fieldMatchValuesLocals[] = {
                                                     gAUInt32Type,
                                                     gGenericFieldType,
                                                     gUndefinedType,
                                                     gUndefinedType,
                                                     gUInt32Type,
                                                     gUInt32Type
                                                   };

  gProcFindValueRange.name        = "match_rows";
  gProcFindValueRange.localsCount = 6;
  gProcFindValueRange.localsTypes = fieldMatchValuesLocals;
  gProcFindValueRange.code        = proc_field_find_range;


  static const uint8_t* fieldFilterRowsLocals[] = {
                                                     gAUInt32Type,
                                                     gGenericFieldType,
                                                     gGenericArrayType,
                                                     gAUInt32Type,
                                                     gBoolType,
                                                     gBoolType
                                                    };

  gProcFilterRows.name        = "filter_rows";
  gProcFilterRows.localsCount = 6;
  gProcFilterRows.code        = proc_field_filter_rows;
  gProcFilterRows.localsTypes = fieldFilterRowsLocals;



  static const uint8_t* fieldMinimumLocals[] = {
                                                 gAUInt32Type,
                                                 gGenericFieldType,
                                                 gUndefinedType
                                               };

  gProcFieldMinimum.name        = "get_biggest";
  gProcFieldMinimum.localsCount = 3;
  gProcFieldMinimum.localsTypes = fieldMinimumLocals;
  gProcFieldMinimum.code        = field_search_minmax<false>;

  gProcFieldMaximum.name        = "get_smallest";
  gProcFieldMaximum.localsCount = 3;
  gProcFieldMaximum.localsTypes = fieldMinimumLocals; //reusing
  gProcFieldMaximum.code        = field_search_minmax<true>;

  return WOP_OK;
}


