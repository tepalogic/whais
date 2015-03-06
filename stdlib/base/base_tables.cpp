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
#include <vector>

#include "whais.h"
#include "utils/wsort.h"
#include "utils/wunicode.h"
#include "utils/wutf.h"

#include "base_tables.h"
#include "base_types.h"


#define MAX_FIELD_NAME_LENGTH     256


using namespace whais;
using namespace std;


WLIB_PROC_DESCRIPTION       gProcTableIsPersistent;
WLIB_PROC_DESCRIPTION       gProcTableFieldsCount;
WLIB_PROC_DESCRIPTION       gProcTableFieldByIndex;
WLIB_PROC_DESCRIPTION       gProcTableFieldByName;
WLIB_PROC_DESCRIPTION       gProcTableRowsCount;
WLIB_PROC_DESCRIPTION       gProcTableReusableRowsCount;
WLIB_PROC_DESCRIPTION       gProcTableAddRow;
WLIB_PROC_DESCRIPTION       gProcTableFindRemovedRow;
WLIB_PROC_DESCRIPTION       gProcTableRemoveRow;
WLIB_PROC_DESCRIPTION       gProcTableExchangeRows;
WLIB_PROC_DESCRIPTION       gProcTableSort;

class TableSortContainer
{

public:

  typedef int
  (TableSortContainer::*field_comparator) (const ROW_INDEX    row1,
                                           const ROW_INDEX    row2,
                                           const FIELD_INDEX  row3) const;
  class Value
    {
      friend class TableSortContainer;

    public:
      Value()
        : mContainer( NULL),
          mRow( 0)
        {
        }

      Value( const TableSortContainer& container,
             const ROW_INDEX           row)
        : mContainer( &container),
          mRow( container.mRowsPermutation[row])
        {
        }

      bool operator< (const Value& val) const
        {
          if ((mContainer == NULL) && (val.mContainer != NULL))
            return true;

          else if (val.mContainer == NULL)
            return false;

          assert( val.mContainer == mContainer);

          for (FIELD_INDEX i = 0; i < mContainer->mFields.size(); ++i)
            {
              const field_comparator compare = mContainer->mComparators[i];

              const int r = (mContainer->*compare) (mRow,
                                                    val.mRow,
                                                    mContainer->mFields[i]);
              if (r < 0)
                return true;

              else if (r > 0)
                return false;
            }

          return false;
        }

      bool operator== (const Value& val) const
        {
          if ((mContainer == NULL) && (val.mContainer == NULL))
              return true;

          else if (mContainer != val.mContainer)
            return false;

          for (FIELD_INDEX i = 0; i < mContainer->mFields.size(); ++i)
            {
              const field_comparator compare = mContainer->mComparators[i];

              if ((mContainer->*compare) (mRow,
                                          val.mRow,
                                          mContainer->mFields[i]) != 0)
                return false;
            }

          return true;
        }

      Value& operator= (const Value& src)
        {
          _CC (const TableSortContainer*&, mContainer) = src.mContainer;
          _CC (ROW_INDEX&, mRow) = src.mRow;

          return *this;
        }

    private:
      const TableSortContainer* const mContainer;
      const ROW_INDEX                 mRow;
    };


  TableSortContainer( ITable&       table,
                      const DArray& fields,
                      const DArray& fieldsSortOrder)
    : mTable(table)
  {
    assert( fields.Count() > 0);
    assert( fields.Count() == fieldsSortOrder.Count());

    extract_fields_ids( fields);

    mTable.LockTable ();
    mRowsPermutation.reserve (mTable.AllocatedRows());
    for (ROW_INDEX i = 0; i < mRowsPermutation.size (); ++i)
      mRowsPermutation[i] = i;

    for (FIELD_INDEX field = 0; field < mFields.size(); ++field)
      {
        const DBSFieldDescriptor fd = mTable.DescribeField( mFields[field]);

        if (fd.isArray)
          {
            throw InterException(
                        _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                        "Cannot sort a table using an array field."
                                 );
          }

        DBool sortOrder;
        fieldsSortOrder.Get (field, sortOrder);

        switch( fd.type)
          {
          case T_BOOL:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DBool>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DBool>)
                                   );
            break;

          case T_CHAR:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DChar>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DChar>)
                                   );
            break;

          case T_DATE:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DDate>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DDate>)
                                   );
            break;

          case T_DATETIME:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DDateTime>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DDateTime>)
                                   );
            break;

          case T_HIRESTIME:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DHiresTime>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DHiresTime>)
                                   );
            break;

          case T_INT8:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DInt8>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DInt8>)
                                   );
            break;

          case T_INT16:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DInt16>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DInt16>)
                                   );
            break;


          case T_INT32:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DInt32>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DInt32>)
                                   );
            break;

          case T_INT64:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DInt64>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DInt64>)
                                   );
            break;

          case T_REAL:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DReal>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DReal>)
                                   );
            break;

          case T_RICHREAL:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DRichReal>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DRichReal>)
                                   );
            break;

          case T_UINT8:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DUInt8>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DUInt8>)
                                   );
            break;

          case T_UINT16:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DUInt16>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DUInt16>)
                                   );
            break;


          case T_UINT32:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DUInt32>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DUInt32>)
                                   );
            break;

          case T_UINT64:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DUInt64>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DUInt64>)
                                   );
            break;

          case T_TEXT:
            mComparators.push_back(
                          sortOrder.mValue ?
                            _SC (field_comparator, &TableSortContainer::compare_field_values_reverse<DText>) :
                            _SC (field_comparator, &TableSortContainer::compare_field_values<DText>)
                                   );
            break;

          default:
            throw InterException( _EXTRA( InterException::INTERNAL_ERROR));
          }
      }
  }

  const Value operator[] (const int64_t position) const
  {
    return Value( *this, position);
  }

  void Exchange( const int64_t pos1, const int64_t pos2)
  {
    const ROW_INDEX t = mRowsPermutation[pos2];

    mRowsPermutation[pos2] = mRowsPermutation[pos1];
    mRowsPermutation[pos1] = t;
  }

  uint64_t Count() const
  {
    return mRowsPermutation.size ();
  }

  void Pivot( const uint64_t index)
  {
    mPivot = Value( *this, index);
  }


  const Value& Pivot() const
  {
    return mPivot;
  }


  void Commit ()
  {
    ROW_INDEX row = 0;
    while (row < mRowsPermutation.size ())
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

            mTable.ExchangeRows (currentRow, correctRow, true);
            mRowsPermutation[currentRow] = currentRow;

            currentRow = correctRow;
          }
        while (true);
      }

    mTable.UnlockTable ();
  }

private:
  friend class TableSortContainer::Value;

  template<typename T> int
  compare_field_values( const ROW_INDEX     row1,
                        const ROW_INDEX     row2,
                        const FIELD_INDEX   field) const
  {
    T v1, v2;

    mTable.Get (row1, field, v1, true);
    mTable.Get (row2, field, v2, true);

    if (v1 == v2)
      return 0;

    else if  (v1 < v2)
      return -1;

    return 1;
  }

  template<typename T> int
  compare_field_values_reverse( const ROW_INDEX     row1,
                                const ROW_INDEX     row2,
                                const FIELD_INDEX   field) const
  {
    T v1, v2;

    mTable.Get (row1, field, v1, true);
    mTable.Get (row2, field, v2, true);

    if (v1 == v2)
      return 0;

    else if  (v2 < v1)
      return -1;

    return 1;
  }


  void extract_fields_ids( const DArray& fields)
  {
    const DBS_FIELD_TYPE arrayType   = fields.Type();
    const uint64_t       fieldsCount = fields.Count();

    assert( fieldsCount != 0);

    for (uint64_t field = 0; field < fieldsCount; ++field)
      {
        switch( arrayType)
          {
          case T_INT8:
              {
                DInt8 temp;
                fields.Get (field, temp);

                mFields.push_back( temp.mValue);
              }
            break;

          case T_INT16:
              {
                DInt16 temp;
                fields.Get (field, temp);

                mFields.push_back( temp.mValue);
              }
            break;

          case T_INT32:
              {
                DInt32 temp;
                fields.Get (field, temp);

                mFields.push_back( temp.mValue);
              }
            break;

          case T_INT64:
              {
                DInt64 temp;
                fields.Get (field, temp);

                mFields.push_back( temp.mValue);
              }
            break;

          case T_UINT8:
              {
                DUInt8 temp;
                fields.Get (field, temp);

                mFields.push_back( temp.mValue);
              }
            break;

          case T_UINT16:
              {
                DUInt16 temp;
                fields.Get (field, temp);

                mFields.push_back( temp.mValue);
              }
            break;

          case T_UINT32:
              {
                DUInt32 temp;
                fields.Get (field, temp);

                mFields.push_back( temp.mValue);
              }
            break;

          case T_UINT64:
              {
                DUInt64 temp;
                fields.Get (field, temp);

                mFields.push_back( temp.mValue);
              }
            break;

          default:
            throw InterException(
                        _EXTRA( InterException::INVALID_PARAMETER_TYPE),
                        "The fields list should be an array of integers."
                                 );
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

  if (op.IsNull())
    {
      stack.Pop (1);
      stack.Push( DBool());

      return WOP_OK;
    }

  ITable& table = op.GetTable();

  DBool result(  ! table.IsTemporal());

  stack.Pop (1);
  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_fields_count( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 1].Operand();

  ITable* const table = &op.GetTable();

  if (table == NULL)
    throw InterException( _EXTRA( InterException::INTERNAL_ERROR));

  DUInt64 result( table->FieldsCount());

  stack.Pop (1);
  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_field_by_id( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 2].Operand();

  DUInt64 fieldId;
  stack[stack.Size() - 1].Operand().GetValue( fieldId);

  if (fieldId.IsNull())
    {
      throw InterException(
                  _EXTRA( InterException::INVALID_PARAMETER_VALUE),
                  "A non null index is required for retrieving a table field."
                           );
    }
  StackValue result = op.GetFieldAt( fieldId.mValue);

  stack.Pop (2);
  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_field_name( SessionStack& stack, ISession&)
{
  uint8_t fieldName[MAX_FIELD_NAME_LENGTH];

  DText       field;

  IOperand&     opTable = stack[stack.Size() - 2].Operand();
  ITable* const table   = &opTable.GetTable();

  stack[stack.Size() - 1].Operand().GetValue( field);
  if (field.IsNull())
    {
      throw InterException( _EXTRA( InterException::INVALID_PARAMETER_VALUE),
                            "A non null name is required to retrieve a field.");
    }
  else if (field.RawSize() >= sizeof fieldName)
    {
      throw InterException(
                _EXTRA( InterException::FIELD_NAME_TOO_LONG),
                "This implementation support fields names up to %d characters.",
                sizeof fieldName
                            );
    }
  else if (table == NULL)
    throw InterException( _EXTRA( InterException::INTERNAL_ERROR));

  field.RawRead( 0, field.RawSize(), fieldName);
  fieldName[field.RawSize()] = 0;

  const FIELD_INDEX fieldId = table->RetrieveField( _RC (char*, fieldName));
  StackValue        result  = opTable.GetFieldAt( fieldId);

  stack.Pop (2);
  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_rows_count( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 1].Operand();

  if (op.IsNull())
    {
      stack.Pop (1);
      stack.Push( DUInt64 (0));

      return WOP_OK;
    }

  ITable& table = op.GetTable();

  DUInt64 result( table.AllocatedRows());

  stack.Pop (1);
  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_reusable_rows_count( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 1].Operand();

  if (op.IsNull())
    {
      stack.Pop (1);
      stack.Push( DUInt64 (0));

      return WOP_OK;
    }

  ITable& table = op.GetTable();

  DUInt64 result( table.AllocatedRows());

  stack.Pop (1);
  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_add_row( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 1].Operand();

  ITable& table = op.GetTable();

  DUInt64 result( table.AddRow());

  stack.Pop (1);
  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_reusable_row( SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size() - 1].Operand();

  ITable& table = op.GetTable();

  DUInt64 result( table.GetReusableRow( true));

  stack.Pop (1);
  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_reuse_row( SessionStack& stack, ISession&)
{
  DUInt64 row;
  DBool   result;

  ITable& table = stack[stack.Size() - 2].Operand().GetTable();
  stack[stack.Size() - 1].Operand().GetValue( row);

  if (! row.IsNull()
      && (row.mValue < table.AllocatedRows()))
    {
      table.MarkRowForReuse( row.mValue);

      result = DBool( true);
    }

  stack.Pop( 2);
  stack.Push( result);

  return WOP_OK;
}


static WLIB_STATUS
table_exchange_rows( SessionStack& stack, ISession&)
{
  DUInt64 row1, row2;

  IOperand& op = stack[stack.Size() - 3].Operand();

  stack[stack.Size() - 2].Operand().GetValue( row1);
  stack[stack.Size() - 1].Operand().GetValue( row2);

  if (op.IsNull() || row1.IsNull() || row2.IsNull())
    {
      stack.Pop (3);
      stack.Push( DBool());

      return WOP_OK;
    }

  ITable& table = op.GetTable();

  table.ExchangeRows( row1.mValue, row2.mValue);

  stack.Pop (3);
  stack.Push( DBool( true));

  return WOP_OK;

}


static WLIB_STATUS
proc_table_sort( SessionStack& stack, ISession&)
{
  DArray fields, sortOrder;

  IOperand& opTable = stack[stack.Size() - 3].Operand();
  stack[stack.Size() - 2].Operand().GetValue( fields);
  stack[stack.Size() - 1].Operand().GetValue( sortOrder);

  if (opTable.IsNull() || fields.IsNull())
    {
      stack.Pop (2);
      return WOP_OK;
    }
  else if (fields.Count() <sortOrder.Count())
    {
      throw InterException(
            _EXTRA( InterException::INVALID_PARAMETER_VALUE),
            "The field list and sort order arrays should have the same "
              "count of elements."
                           );
    }

  for (int i = fields.Count() - sortOrder.Count(); i > 0; --i)
    sortOrder.Add (DBool (false));

  ITable& table = opTable.GetTable();
  TableSortContainer container( table, fields, sortOrder);

  quick_sort<TableSortContainer::Value, TableSortContainer> (
                                                   0,
                                                   table.AllocatedRows() - 1,
                                                   false,
                                                   container
                                                             );
  container.Commit ();

  stack.Pop (2);
  return WOP_OK;
}


WLIB_STATUS
base_tables_init()
{
  static const uint8_t* isPersistentLocals[] = {
                                                 gBoolType,
                                                 gGenericTableType
                                               };
  gProcTableIsPersistent.name        = "table_is_persistent";
  gProcTableIsPersistent.localsCount = 2;
  gProcTableIsPersistent.localsTypes = isPersistentLocals;
  gProcTableIsPersistent.code        = proc_table_ispersistent;

  static const uint8_t* tableFieldsLocals[] = {
                                                gUInt64Type,
                                                gGenericTableType
                                              };

  gProcTableFieldsCount.name        = "table_fields_count";
  gProcTableFieldsCount.localsCount = 2;
  gProcTableFieldsCount.localsTypes = tableFieldsLocals;
  gProcTableFieldsCount.code        = proc_table_fields_count;


  static const uint8_t* tableFieldByIdLocals[] = {
                                                   gGenericFieldType,
                                                   gGenericTableType,
                                                   gUInt64Type
                                                  };

  gProcTableFieldByIndex.name        = "table_field_by_id";
  gProcTableFieldByIndex.localsCount = 3;
  gProcTableFieldByIndex.localsTypes = tableFieldByIdLocals;
  gProcTableFieldByIndex.code        = proc_table_field_by_id;

  static const uint8_t* tableFieldByNameLocals[] = {
                                                     gGenericFieldType,
                                                     gGenericTableType,
                                                     gTextType
                                                    };

  gProcTableFieldByName.name        = "table_field";
  gProcTableFieldByName.localsCount = 3;
  gProcTableFieldByName.localsTypes = tableFieldByNameLocals;
  gProcTableFieldByName.code        = proc_table_field_name;


  gProcTableRowsCount.name        = "table_rows";
  gProcTableRowsCount.localsCount = 2;
  gProcTableRowsCount.localsTypes = tableFieldsLocals; //reusing
  gProcTableRowsCount.code        = proc_table_rows_count;

  gProcTableReusableRowsCount.name        = "table_resuable_rows";
  gProcTableReusableRowsCount.localsCount = 2;
  gProcTableReusableRowsCount.localsTypes = tableFieldsLocals; //reusing
  gProcTableReusableRowsCount.code        = proc_table_reusable_rows_count;

  gProcTableAddRow.name        = "table_add_row";
  gProcTableAddRow.localsCount = 2;
  gProcTableAddRow.localsTypes = tableFieldsLocals; //reusing
  gProcTableAddRow.code        = proc_table_add_row;

  gProcTableFindRemovedRow.name        = "table_reusable_row";
  gProcTableFindRemovedRow.localsCount = 2;
  gProcTableFindRemovedRow.localsTypes = tableFieldsLocals; //reusing
  gProcTableFindRemovedRow.code        = proc_table_reusable_row;

  static const uint8_t* tableReuseRowLocals[] = {
                                                  gBoolType,
                                                  gGenericTableType,
                                                  gUInt64Type
                                                };

  gProcTableRemoveRow.name        = "table_reuse_row";
  gProcTableRemoveRow.localsCount = 3;
  gProcTableRemoveRow.localsTypes = tableReuseRowLocals;
  gProcTableRemoveRow.code        = proc_table_reuse_row;

  static const uint8_t* tableExchangeRows[] = {
                                                gBoolType,
                                                gGenericTableType,
                                                gUInt64Type,
                                                gUInt64Type
                                              };

  gProcTableExchangeRows.name        = "table_exchg_rows";
  gProcTableExchangeRows.localsCount = 4;
  gProcTableExchangeRows.localsTypes = tableExchangeRows;
  gProcTableExchangeRows.code        = table_exchange_rows;

  static const uint8_t* tableSortLocals[] = {
                                              gGenericTableType,
                                              gGenericTableType,
                                              gGenericArrayType,
                                              gABoolType
                                            };

  gProcTableSort.name        = "table_sort";
  gProcTableSort.localsCount = 4;
  gProcTableSort.localsTypes = tableSortLocals;
  gProcTableSort.code        = proc_table_sort;


  return WOP_OK;
}

