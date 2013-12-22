/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whisper.
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


#include "base_tables.h"
#include "base_types.h"



#define MAX_FIELD_NAME_LENGTH     256


using namespace whisper;


WLIB_PROC_DESCRIPTION       gProcTableIsPersistent;
WLIB_PROC_DESCRIPTION       gProcTableFieldsCount;
WLIB_PROC_DESCRIPTION       gProcTableFieldByIndex;
WLIB_PROC_DESCRIPTION       gProcTableFieldByName;
WLIB_PROC_DESCRIPTION       gProcTableRowsCount;
WLIB_PROC_DESCRIPTION       gProcTableAddRow;
WLIB_PROC_DESCRIPTION       gProcTableFindRemovedRow;
WLIB_PROC_DESCRIPTION       gProcTableRemoveRow;
WLIB_PROC_DESCRIPTION       gProcTableExchangeRows;


static WLIB_STATUS
proc_table_ispersistent (SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size () - 1].Operand ();

  if (op.IsNull ())
    {
      stack.Pop (1);
      stack.Push (DBool ());

      return WOP_OK;
    }

  ITable& table = op.GetTable ();

  DBool result ( ! table.IsTemporal ());

  stack.Pop (1);
  stack.Push (result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_fields_count (SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size () - 1].Operand ();

  ITable* const table = &op.GetTable ();

  if (table == NULL)
    throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));

  DUInt64 result (table->FieldsCount ());

  stack.Pop (1);
  stack.Push (result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_field_by_id (SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size () - 2].Operand ();

  DUInt64 fieldId;
  stack[stack.Size () - 1].Operand ().GetValue (fieldId);

  if (fieldId.IsNull ())
    {
      throw InterException ("A non null field index need to be specified.",
                            _EXTRA (InterException::INVALID_PARAMETER_VALUE));
    }
  StackValue result = op.GetFieldAt (fieldId.mValue);

  stack.Pop (2);
  stack.Push (result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_field_name (SessionStack& stack, ISession&)
{
  uint8_t fieldName[MAX_FIELD_NAME_LENGTH];

  DText       field;

  IOperand&     opTable = stack[stack.Size () - 2].Operand ();
  ITable* const table   = &opTable.GetTable ();

  stack[stack.Size () - 1].Operand ().GetValue (field);
  if (field.IsNull ())
    {
      throw InterException ("Name of the field could not be a null text.",
                            _EXTRA (InterException::INVALID_PARAMETER_VALUE));
    }
  else if (field.RawSize () >= sizeof fieldName)
    throw InterException (NULL, _EXTRA (InterException::FIELD_NAME_TOO_LONG));

  else if (table == NULL)
    throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));

  field.RawRead (0, field.RawSize (), fieldName);
  fieldName[field.RawSize ()] = 0;

  const FIELD_INDEX fieldId = table->RetrieveField (_RC (char*, fieldName));
  StackValue        result  = opTable.GetFieldAt (fieldId);

  stack.Pop (2);
  stack.Push (result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_rows_count (SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size () - 1].Operand ();

  if (op.IsNull ())
    {
      stack.Pop (1);
      stack.Push (DUInt64 (0));

      return WOP_OK;
    }

  ITable& table = op.GetTable ();

  DUInt64 result (table.AllocatedRows ());

  stack.Pop (1);
  stack.Push (result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_add_row (SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size () - 1].Operand ();

  ITable& table = op.GetTable ();

  DUInt64 result (table.AddRow ());

  stack.Pop (1);
  stack.Push (result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_reusable_row (SessionStack& stack, ISession&)
{
  IOperand& op = stack[stack.Size () - 1].Operand ();

  ITable& table = op.GetTable ();

  DUInt64 result (table.AddReusedRow ());

  stack.Pop (1);
  stack.Push (result);

  return WOP_OK;
}


static WLIB_STATUS
proc_table_reuse_row (SessionStack& stack, ISession&)
{
  DUInt64 row;
  DUInt64 result;

  ITable& table = stack[stack.Size () - 2].Operand ().GetTable ();
  stack[stack.Size () - 1].Operand ().GetValue (row);

  if (! row.IsNull ()
      && (row.mValue < table.AllocatedRows ()))
    {
      table.MarkRowForReuse (row.mValue);

      result = row;
    }

  stack.Pop (2);
  stack.Push (result);

  return WOP_OK;
}


static WLIB_STATUS
table_exchange_rows (SessionStack& stack, ISession&)
{
  DUInt64 row1, row2;

  IOperand& op = stack[stack.Size () - 3].Operand ();

  stack[stack.Size () - 2].Operand ().GetValue (row1);
  stack[stack.Size () - 1].Operand ().GetValue (row2);

  if (op.IsNull () || row1.IsNull () || row2.IsNull ())
    {
      stack.Pop (3);
      stack.Push (DBool ());

      return WOP_OK;
    }

  ITable& table = op.GetTable ();

  table.ExchangeRows (row1.mValue, row2.mValue);

  stack.Pop (3);
  stack.Push (DBool (true));

  return WOP_OK;

}

WLIB_STATUS
base_tables_init ()
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

  gProcTableAddRow.name        = "table_add_row";
  gProcTableAddRow.localsCount = 2;
  gProcTableAddRow.localsTypes = tableFieldsLocals; //reusing
  gProcTableAddRow.code        = proc_table_add_row;

  gProcTableFindRemovedRow.name        = "table_reusable_row";
  gProcTableFindRemovedRow.localsCount = 2;
  gProcTableFindRemovedRow.localsTypes = tableFieldsLocals; //reusing
  gProcTableFindRemovedRow.code        = proc_table_reusable_row;

  gProcTableRemoveRow.name        = "table_remove_row";
  gProcTableRemoveRow.localsCount = 3;
  gProcTableRemoveRow.localsTypes = tableFieldsLocals; //reusing
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

  return WOP_OK;
}

