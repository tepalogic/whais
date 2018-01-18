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
#include <vector>

#include "whais.h"
#include "utils/wsort.h"
#include "utils/wunicode.h"
#include "utils/wutf.h"
#include "base_tables.h"
#include "base_types.h"


using namespace whais;
using namespace std;


#define MAX_FIELD_NAME_LENGTH     256

WLIB_PROC_DESCRIPTION       gProcTableIsPersistent;
WLIB_PROC_DESCRIPTION       gProcTableFieldsCount;
WLIB_PROC_DESCRIPTION       gProcTableFieldByIndex;
WLIB_PROC_DESCRIPTION       gProcTableFieldByName;
WLIB_PROC_DESCRIPTION       gProcTableRowsCount;
WLIB_PROC_DESCRIPTION       gProcTableAddRow;
WLIB_PROC_DESCRIPTION       gProcTableFindRemovedRow;
WLIB_PROC_DESCRIPTION       gProcTableRemoveRow;
WLIB_PROC_DESCRIPTION       gProcTableExchangeRows;
WLIB_PROC_DESCRIPTION       gProcTableSort;


class TableSortContainer
{
public:

  typedef int
  (TableSortContainer::*field_comparator)(const ROW_INDEX row1,
                                          const ROW_INDEX row2,
                                          const FIELD_INDEX row3) const;
  class Value
  {
    friend class TableSortContainer;

  public:
    Value()
        : mContainer(nullptr),
          mRow(0)
    {}

    Value(const TableSortContainer& container, const ROW_INDEX row)
        : mContainer( &container),
          mRow(container.mRowsPermutation[row])
    {}

    Value& operator=(const Value& src)
    {
      _CC(const TableSortContainer*&, mContainer) = src.mContainer;
      _CC(ROW_INDEX&, mRow) = src.mRow;

      return *this;
    }

    bool operator<(const Value& val) const
    {
      if ((mContainer == nullptr) && (val.mContainer != nullptr))
        return true;

      else if (val.mContainer == nullptr)
        return false;

      assert(val.mContainer == mContainer);

      for (FIELD_INDEX i = 0; i < mContainer->mFields.size(); ++i)
      {
        const field_comparator compare = mContainer->mComparators[i];

        const int r = (mContainer->*compare)(mRow, val.mRow, mContainer->mFields[i]);
        if (r < 0)
          return true;

        else if (r > 0)
          return false;
      }

      return false;
    }

    bool operator==(const Value& val) const
    {
      if ((mContainer == nullptr) && (val.mContainer == nullptr))
        return true;

      else if (mContainer != val.mContainer)
        return false;

      for (FIELD_INDEX i = 0; i < mContainer->mFields.size(); ++i)
      {
        const field_comparator compare = mContainer->mComparators[i];

        if ((mContainer->*compare)(mRow, val.mRow, mContainer->mFields[i]) != 0)
          return false;
      }

      return true;
    }

  private:
    const TableSortContainer* const mContainer;
    const ROW_INDEX mRow;
  };


  TableSortContainer(ITable& table, const DArray& fields, const DArray& fieldsSortOrder)
      : mTable(table)
  {
    assert(fields.Count() > 0);
    assert(fields.Count() == fieldsSortOrder.Count());

    extract_fields_ids(fields);

    mTable.LockTable();
    mRowsPermutation.resize(mTable.AllocatedRows());
    for (ROW_INDEX i = 0; i < mRowsPermutation.size(); ++i)
      mRowsPermutation[i] = i;

    for (FIELD_INDEX field = 0; field < mFields.size(); ++field)
    {
      const DBSFieldDescriptor fd = mTable.DescribeField(mFields[field]);

      if (fd.isArray)
      {
        throw InterException(_EXTRA(InterException::INVALID_PARAMETER_TYPE),
                             "Cannot sort a table using an array field.");
      }

      DBool sortOrder;
      fieldsSortOrder.Get(field, sortOrder);

      switch (fd.type)
      {
      case T_BOOL:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DBool>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DBool>));
        break;

      case T_CHAR:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DChar>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DChar>));
        break;

      case T_DATE:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DDate>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DDate>));
        break;

      case T_DATETIME:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator,
                    &TableSortContainer::compare_field_values_reverse<DDateTime>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DDateTime>));
        break;

      case T_HIRESTIME:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator,
                    &TableSortContainer::compare_field_values_reverse<DHiresTime>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DHiresTime>));
        break;

      case T_INT8:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DInt8>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DInt8>));
        break;

      case T_INT16:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DInt16>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DInt16>));
        break;

      case T_INT32:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DInt32>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DInt32>));
        break;

      case T_INT64:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DInt64>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DInt64>));
        break;

      case T_REAL:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DReal>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DReal>));
        break;

      case T_RICHREAL:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator,
                    &TableSortContainer::compare_field_values_reverse<DRichReal>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DRichReal>));
        break;

      case T_UINT8:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DUInt8>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DUInt8>));
        break;

      case T_UINT16:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DUInt16>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DUInt16>));
        break;

      case T_UINT32:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DUInt32>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DUInt32>));
        break;

      case T_UINT64:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DUInt64>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DUInt64>));
        break;

      case T_TEXT:
        mComparators.push_back(
            sortOrder.mValue ?
                _SC(field_comparator, &TableSortContainer::compare_field_values_reverse<DText>) :
                _SC(field_comparator, &TableSortContainer::compare_field_values<DText>));
        break;

      default:
        throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
      }
    }
  }

  TableSortContainer(const TableSortContainer&) = delete;
  TableSortContainer& operator=(const TableSortContainer&) = delete;

  const Value operator[] (const int64_t position) const { return Value(*this, position); }
  void Exchange( const int64_t pos1, const int64_t pos2)
  {
    const ROW_INDEX t = mRowsPermutation[pos2];

    mRowsPermutation[pos2] = mRowsPermutation[pos1];
    mRowsPermutation[pos1] = t;
  }
  uint64_t Count() const { return mRowsPermutation.size(); }
  void Pivot( const uint64_t index) { mPivot = Value( *this, index); }
  const Value& Pivot() const { return mPivot; }

  void Commit()
  {
    ROW_INDEX row = 0;
    while (row < mRowsPermutation.size())
    {
      if (mRowsPermutation[row] == row)
      {
        ++row;
        continue;
      }

      ROW_INDEX currentRow = row;
      do
      {
        const ROW_INDEX correctRow = mRowsPermutation[currentRow];
        if (correctRow == row)
        {
          mRowsPermutation[currentRow] = currentRow;
          break;
        }

        mTable.ExchangeRows(currentRow, correctRow, true);
        mRowsPermutation[currentRow] = currentRow;

        currentRow = correctRow;
      } while (true);
    }

    mTable.UnlockTable();
  }

private:
  friend class TableSortContainer::Value;

  template<typename T> int
  compare_field_values(const ROW_INDEX row1, const ROW_INDEX row2, const FIELD_INDEX field) const
  {
    T v1, v2;

    mTable.Get(row1, field, v1, true);
    mTable.Get(row2, field, v2, true);

    if (v1 == v2)
      return 0;

    else if (v1 < v2)
      return -1;

    return 1;
  }

  template<typename T> int
  compare_field_values_reverse(const ROW_INDEX row1,
                               const ROW_INDEX row2,
                               const FIELD_INDEX field) const
  {
    T v1, v2;

    mTable.Get(row1, field, v1, true);
    mTable.Get(row2, field, v2, true);

    if (v1 == v2)
      return 0;

    else if  (v2 < v1)
      return -1;

    return 1;
  }


  void extract_fields_ids(const DArray& fields)
  {
    const DBS_FIELD_TYPE arrayType = fields.Type();
    const uint64_t fieldsCount = fields.Count();

    assert(fieldsCount != 0);

    for (uint64_t field = 0; field < fieldsCount; ++field)
    {
      switch (arrayType)
      {
      case T_INT8:
      {
        DInt8 temp;
        fields.Get(field, temp);

        mFields.push_back(temp.mValue);
      }
        break;

      case T_INT16:
      {
        DInt16 temp;
        fields.Get(field, temp);

        mFields.push_back(temp.mValue);
      }
        break;

      case T_INT32:
      {
        DInt32 temp;
        fields.Get(field, temp);

        mFields.push_back(temp.mValue);
      }
        break;

      case T_INT64:
      {
        DInt64 temp;
        fields.Get(field, temp);

        mFields.push_back(temp.mValue);
      }
        break;

      case T_UINT8:
      {
        DUInt8 temp;
        fields.Get(field, temp);

        mFields.push_back(temp.mValue);
      }
        break;

      case T_UINT16:
      {
        DUInt16 temp;
        fields.Get(field, temp);

        mFields.push_back(temp.mValue);
      }
        break;

      case T_UINT32:
      {
        DUInt32 temp;
        fields.Get(field, temp);

        mFields.push_back(temp.mValue);
      }
        break;

      case T_UINT64:
      {
        DUInt64 temp;
        fields.Get(field, temp);

        mFields.push_back(temp.mValue);
      }
        break;

      default:
        throw InterException(_EXTRA(InterException::INVALID_PARAMETER_TYPE),
                             "The fields list should be an array of integers.");
      }
    }
  }

  ITable&                       mTable;
  vector<ROW_INDEX>             mRowsPermutation;
  vector<uint16_t>              mFields;
  vector<field_comparator>      mComparators;
  uint16_t                      mFieldsCount;
  ROW_INDEX                     mRowsCount;
  Value                         mPivot;
};


static WLIB_STATUS
proc_table_ispersistent( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 1].Operand();
  if (op.IsNullExpression())
  {
    stack.Pop(1);
    stack.Push(DBool());

    return WOP_OK;
  }

  DBool result( ! op.GetTable().IsTemporal());

  stack.Pop(1);
  stack.Push(result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_fields_count( SessionStack& stack, ISession&)
{
  DUInt16 result(0);

  IOperand& op = stack[stack.Size() - 1].Operand();
  if ( ! op.IsNullExpression())
    result = DUInt16(op.GetTable().FieldsCount());

  stack.Pop(1);
  stack.Push(result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_field_by_id( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 2].Operand();
  if (op.IsNullExpression())
  {
    stack.Pop(2);
    stack.Push();

    return WOP_OK;
  }

  StackValue result;
  DUInt16 fieldId;
  stack[stack.Size() - 1].Operand().GetValue(fieldId);
  if ( ! fieldId.IsNull() && (fieldId.mValue < op.GetTable().FieldsCount()))
    result = op.GetFieldAt(fieldId.mValue);

  stack.Pop(2);
  stack.Push(move(result));

  return WOP_OK;
}


static WLIB_STATUS
proc_table_field_name( SessionStack& stack, ISession&)
{
  uint8_t fieldName[MAX_FIELD_NAME_LENGTH];
  DText field;

  IOperand& op = stack[stack.Size() - 2].Operand();
  if (op.IsNullExpression())
  {
    stack.Pop(2);
    stack.Push();
    return WOP_OK;
  }

  stack[stack.Size() - 1].Operand().GetValue(field);
  if (field.IsNull() || field.RawSize() >= sizeof fieldName)
  {
    stack.Pop(2);
    stack.Push();
    return WOP_OK;
  }

  field.RawRead(0, field.RawSize(), fieldName);
  fieldName[field.RawSize()] = 0;

  StackValue result;
  try
  {
    const FIELD_INDEX fieldId = op.GetTable().RetrieveField(_RC(char*, fieldName));
    result = op.GetFieldAt(fieldId);
  }
  catch (DBSException& e)
  {
    if (e.Code() != DBSException::FIELD_NOT_FOUND)
      throw ;
  }

  stack.Pop(2);
  stack.Push(move(result));

  return WOP_OK;
}


static WLIB_STATUS
proc_table_rows_count( SessionStack& stack, ISession&)
{
  DBool empties;

  stack[stack.Size() - 1].Operand().GetValue(empties);
  if (empties.IsNull())
    empties = DBool(false);

  IOperand& op = stack[stack.Size() - 2].Operand();
  if (op.IsNullExpression())
  {
    stack.Pop(2);
    stack.Push(DUInt32());
    return WOP_OK;
  }
  else if (op.IsNull())
  {
    stack.Pop(2);
    stack.Push(DUInt32(0));

    return WOP_OK;
  }

  ITable& table = op.GetTable();
  DUInt32 result(empties.mValue ? table.ReusableRowsCount() : table.AllocatedRows());

  stack.Pop(2);
  stack.Push(result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_add_row( SessionStack& stack, ISession&)
{
  DUInt32 result;
  IOperand& op = stack[stack.Size() - 1].Operand();
  if ( ! op.IsNullExpression())
    result = DUInt32(op.GetTable().AddRow());

  stack.Pop(1);
  stack.Push(result);
  return WOP_OK;
}


static WLIB_STATUS
proc_table_reusable_row( SessionStack& stack, ISession&)
{
  DUInt32 result;
  IOperand& op = stack[stack.Size() - 1].Operand();
  if ( ! op.IsNullExpression())
    result = DUInt32(op.GetTable().GetReusableRow(true));

  stack.Pop(1);
  stack.Push(result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_reuse_row( SessionStack& stack, ISession&)
{
  DUInt32 row;
  DBool   result;

  IOperand& op = stack[stack.Size() - 2].Operand();
  if (op.IsNullExpression())
  {
    stack.Pop(2);
    stack.Push(result);

    return WOP_OK;
  }

  ITable& table = op.GetTable();
  stack[stack.Size() - 1].Operand().GetValue(row);

  if (! row.IsNull()
      && (row.mValue < table.AllocatedRows()))
  {
    table.MarkRowForReuse(row.mValue);

    result = DBool(true);
  }
  else
    result = DBool(false);

  stack.Pop(2);
  stack.Push(result);

  return WOP_OK;
}


static WLIB_STATUS
table_exchange_rows( SessionStack& stack, ISession&)
{
  DUInt32 row1, row2;

  IOperand& op = stack[stack.Size() - 3].Operand();
  if (op.IsNullExpression())
  {
    stack.Pop(3);
    stack.Push(DBool());

    return WOP_OK;
  }
  stack[stack.Size() - 2].Operand().GetValue(row1);
  stack[stack.Size() - 1].Operand().GetValue(row2);

  ITable& table = op.GetTable();
  const auto rowsCount = table.AllocatedRows();
  if (row1.IsNull() || row1.mValue >= rowsCount
      || row2.IsNull() || row2.mValue >= rowsCount)
  {
    stack.Pop(3);
    stack.Push(DBool(false));

    return WOP_OK;
  }

  table.ExchangeRows(row1.mValue, row2.mValue);
  stack.Pop(3);
  stack.Push(DBool(true));

  return WOP_OK;
}


static WLIB_STATUS
proc_table_sort( SessionStack& stack, ISession&)
{
  DArray fields(_SC(DUInt16*, nullptr));
  DArray sortOrder(_SC(DBool*, nullptr));

  IOperand& opTable = stack[stack.Size() - 5].Operand();
  if (opTable.IsNullExpression() || opTable.IsNull())
  {
    stack.Pop(5);
    stack.Push(DBool());
    return WOP_OK;
  }

  IOperand& opFields = stack[stack.Size() - 4].Operand();
  if (! opFields.IsNullExpression())
    opFields.GetValue(fields);

  const uint_t fieldsCount = fields.Count();
  ITable& table = opTable.GetTable();
  const FIELD_INDEX tableFieldsCount = table.FieldsCount();
  for (auto f = 0u; f < fieldsCount; ++f)
  {
    DUInt16 fieldId;

    fields.Get(f, fieldId);
    if (fieldId.mValue >= tableFieldsCount)
    {
      stack.Pop(5);
      stack.Push(DBool(false));
      return WOP_OK;
    }
  }

  IOperand& opDirs = stack[stack.Size() - 3].Operand();
  if (! opDirs.IsNullExpression())
    opDirs.GetValue(sortOrder);

  const uint32_t directionsCount = sortOrder.Count();
  if (directionsCount > fieldsCount)
  {
    stack.Pop(5);
    stack.Push(DBool(false));
    return WOP_OK;
  }

  for (int i = fieldsCount - directionsCount; i > 0; --i)
    sortOrder.Add(DBool(false));

  DInt32 from, to;
  stack[stack.Size() - 2].Operand().GetValue(from);
  if (from.IsNull())
    from = DInt32(0);

  stack[stack.Size() - 1].Operand().GetValue(to);
  if (to.IsNull())
    to = DInt32(table.AllocatedRows() - 1);

  if (to < from)
    swap(to, from);

  if ((from.mValue < 0)
      || (to.mValue >= _SC(decltype(to.mValue), table.AllocatedRows())))
  {
    stack.Pop(5);
    stack.Push(DBool(false));
    return WOP_OK;
  }

  TableSortContainer container(table, fields, sortOrder);
  quick_sort<TableSortContainer::Value>(from.mValue, to.mValue, false, container);
  container.Commit();

  stack.Pop(5);
  stack.Push(DBool(true));
  return WOP_OK;
}


WLIB_STATUS
base_tables_init()
{
  static const uint8_t* isPersistentLocals[] = {
                                                 gBoolType,
                                                 gGenericTableType
                                               };
  gProcTableIsPersistent.name        = "is_persistent";
  gProcTableIsPersistent.localsCount = 2;
  gProcTableIsPersistent.localsTypes = isPersistentLocals;
  gProcTableIsPersistent.code        = proc_table_ispersistent;

  static const uint8_t* tableFieldsLocals[] = {
                                                gUInt16Type,
                                                gGenericTableType
                                              };

  gProcTableFieldsCount.name        = "count_fields";
  gProcTableFieldsCount.localsCount = 2;
  gProcTableFieldsCount.localsTypes = tableFieldsLocals;
  gProcTableFieldsCount.code        = proc_table_fields_count;


  static const uint8_t* tableFieldByIdLocals[] = {
                                                   gGenericFieldType,
                                                   gGenericTableType,
                                                   gUInt16Type
                                                  };

  gProcTableFieldByIndex.name        = "get_fieldth";
  gProcTableFieldByIndex.localsCount = 3;
  gProcTableFieldByIndex.localsTypes = tableFieldByIdLocals;
  gProcTableFieldByIndex.code        = proc_table_field_by_id;

  static const uint8_t* tableFieldByNameLocals[] = {
                                                     gGenericFieldType,
                                                     gGenericTableType,
                                                     gTextType
                                                    };

  gProcTableFieldByName.name        = "get_field";
  gProcTableFieldByName.localsCount = 3;
  gProcTableFieldByName.localsTypes = tableFieldByNameLocals;
  gProcTableFieldByName.code        = proc_table_field_name;

  static const uint8_t* tableCountRows[] = {
                                             gUInt32Type,
                                             gGenericTableType,
                                             gBoolType
                                           };

  gProcTableRowsCount.name        = "count_rows";
  gProcTableRowsCount.localsCount = 3;
  gProcTableRowsCount.localsTypes = tableCountRows;
  gProcTableRowsCount.code        = proc_table_rows_count;

  static const uint8_t* tableAddRowLocals[] = {
                                                gUInt32Type,
                                                gGenericTableType
                                              };
  gProcTableAddRow.name        = "add_row";
  gProcTableAddRow.localsCount = 2;
  gProcTableAddRow.localsTypes = tableAddRowLocals;
  gProcTableAddRow.code        = proc_table_add_row;

  gProcTableFindRemovedRow.name        = "get_empty";
  gProcTableFindRemovedRow.localsCount = 2;
  gProcTableFindRemovedRow.localsTypes = tableAddRowLocals; //reusing
  gProcTableFindRemovedRow.code        = proc_table_reusable_row;

  static const uint8_t* tableReuseRowLocals[] = {
                                                  gBoolType,
                                                  gGenericTableType,
                                                  gUInt32Type
                                                };

  gProcTableRemoveRow.name        = "empty_row";
  gProcTableRemoveRow.localsCount = 3;
  gProcTableRemoveRow.localsTypes = tableReuseRowLocals;
  gProcTableRemoveRow.code        = proc_table_reuse_row;

  static const uint8_t* tableExchangeRows[] = {
                                                gBoolType,
                                                gGenericTableType,
                                                gUInt32Type,
                                                gUInt32Type
                                              };

  gProcTableExchangeRows.name        = "exchg_rows";
  gProcTableExchangeRows.localsCount = 4;
  gProcTableExchangeRows.localsTypes = tableExchangeRows;
  gProcTableExchangeRows.code        = table_exchange_rows;

  static const uint8_t* tableSortLocals[] = {
                                              gBoolType,
                                              gGenericTableType,
                                              gAUInt16Type,
                                              gABoolType,
                                              gUInt32Type,
                                              gUInt32Type
                                            };

  gProcTableSort.name        = "sort_table";
  gProcTableSort.localsCount = 6;
  gProcTableSort.localsTypes = tableSortLocals;
  gProcTableSort.code        = proc_table_sort;


  return WOP_OK;
}

