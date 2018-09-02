/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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
******************************************************************************/
#include "dev_prima_accessors.h"

#include "whais.h"

#include <cassert>
#include <vector>
#include <algorithm>

#include "interpreter/prima/pm_interpreter.h"
#include "interpreter/prima/pm_globals.h"

using namespace std;
using namespace whais;


static const DInt32                  gVariableUpdated(0);
static const DInt32                  gNoVariable(-1);
static const DInt32                  gNullVariableName(-2);
static const DInt32                  gVariableIsTable(-3);
static const DInt32                  gVariableIsField(-4);
static const DInt32                  gVariableArrayUnspp(-5);

static const DInt32                  gVariableNotTable(-6);
static const DInt32                  gVariableInvalidRow(-7);
static const DInt32                  gVariableInvalidField(-8);

static const DInt32                  gVariableUnkErr(-1000);


WLIB_PROC_DESCRIPTION                gGlbVarRead;
WLIB_PROC_DESCRIPTION                gGlbTableVarRead;
WLIB_PROC_DESCRIPTION                gGlbVarUpdate;
WLIB_PROC_DESCRIPTION                gGlbTableVarUpdate;


template<typename T> void
transfer_value (const IOperand& from, IOperand& to)
{
  T temp;
  from.GetValue(temp);
  to.SetValue(temp);
}


static void
update_result_table_field(IOperand& op, ITable& t, ROW_INDEX r, FIELD_INDEX f)
{
  uint_t type = op.GetType();

  assert (!(IS_ARRAY(type) || IS_TABLE(type) || IS_TABLE_FIELD(type)));

  type = GET_BASE_TYPE(type);

  assert ((type >= T_BOOL) && (type <= T_TEXT));
  switch (type)
  {
  case T_BOOL:
    {
      DBool val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_CHAR:
    {
      DChar val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_DATE:
    {
      DDate val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_DATETIME:
    {
      DDateTime val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_HIRESTIME:
    {
      DHiresTime val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_INT8:
    {
      DInt8 val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_INT16:
    {
      DInt16 val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_INT32:
    {
      DInt32 val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_INT64:
    {
      DInt64 val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_UINT8:
    {
      DUInt8 val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_UINT16:
    {
      DUInt16 val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_UINT32:
    {
      DUInt32 val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_UINT64:
    {
      DUInt64 val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_REAL:
    {
      DReal val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_RICHREAL:
    {
      DRichReal val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  case T_TEXT:
    {
      DText val;
      op.GetValue(val);
      t.Set(r, f, val);
    }
    break;

  default:
    assert(false);
  }
}

static void
copy_table_value (ITable& from,
                  ITable& to,
                  const ROW_INDEX fromRow,
                  const ROW_INDEX toRow,
                  const FIELD_INDEX fromField,
                  const FIELD_INDEX toField,
                  const uint_t type)
{
  if (IS_ARRAY(type))
  {
    DArray val;
    from.Get(fromRow, fromField, val);
    to.Set(toRow, toField, val);
    return ;
  }

  switch (GET_BASE_TYPE(type))
  {
  case T_BOOL:
    {
      DBool val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_CHAR:
    {
      DChar val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_DATE:
    {
      DDate val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_DATETIME:
    {
      DDate val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_HIRESTIME:
    {
      DDate val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_INT8:
    {
      DInt8 val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_INT16:
    {
      DInt16 val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_INT32:
    {
      DInt32 val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_INT64:
    {
      DInt64 val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_UINT8:
    {
      DUInt8 val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_UINT16:
    {
      DUInt16 val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_UINT32:
    {
      DUInt32 val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_UINT64:
    {
      DUInt64 val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_REAL:
    {
      DReal val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_RICHREAL:
    {
      DRichReal val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  case T_TEXT:
    {
      DText val;
      from.Get(fromRow, fromField, val);
      to.Set(toRow, toField, val);
    }
    break;

  default:
    assert(false);
  }
}

static WLIB_STATUS
read_glb_variable( SessionStack& stack, ISession& session)
{
  DText variableName;

  stack[stack.Size() - 1].Operand().GetValue(variableName);
  stack.Pop(1);

  if (variableName.IsNull())
  {
    stack.Push(gNullVariableName);
    return WOP_OK;
  }

  const string name = variableName;
  prima::Session& s = _SC(prima::Session&, session);
  uint_t globalId = s.FindGlobal(_RC(const uint8_t*, name.c_str()), name.length());
  if (! prima::GlobalsManager::IsValid(globalId))
  {
    stack.Push(gNoVariable);
    return WOP_OK;
  }

  StackValue sv = s.GetGlobalValue(globalId);
  IOperand& op = sv.Operand();
  const uint_t type = op.GetType();
  if (IS_TABLE(type) || IS_TABLE_FIELD(type))
  {
    stack.Push(IS_TABLE(type) ? gVariableIsTable : gVariableIsField);
    return WOP_OK;
  }
  else if (IS_ARRAY(type))
  {
    if ((GET_BASE_TYPE(type) < T_BOOL) || (T_TEXT < GET_BASE_TYPE(type)))
    {
      stack.Push(gVariableArrayUnspp);
      return WOP_OK;
    }
  }

  DBSFieldDescriptor resultFieldsp[] = {
      {"Name", T_TEXT, false},
      {"Type", T_UINT32, false},
      {"Value", GET_BASE_TYPE(type), IS_ARRAY(type)}
  };

  ITable& result = s.DBSHandler().CreateTempTable(
                                    sizeof (resultFieldsp) / sizeof (resultFieldsp[0]),
                                    resultFieldsp
                                                 );

  const FIELD_INDEX nameField = result.RetrieveField("Name");
  const FIELD_INDEX typeField = result.RetrieveField("Type");
  const FIELD_INDEX valueField = result.RetrieveField("Value");

  result.Set(0, nameField, variableName);
  result.Set(0, typeField, DUInt32(type));
  if (IS_ARRAY(type))
  {
    DArray val;
    op.GetValue(val);

    result.Set(0, valueField, val);
  }
  else
    update_result_table_field(op, result, 0, valueField);

  stack.Push(result);
  return WOP_OK;
}


static WLIB_STATUS
udpate_glb_variable( SessionStack& stack, ISession& session)
{
  DText variableName;

  stack[stack.Size() - 2].Operand().GetValue(variableName);
  StackValue newValue = stack[stack.Size() - 1];
  stack.Pop(2);

  if (variableName.IsNull())
  {
    stack.Push(gNullVariableName);
    return WOP_OK;
  }

  const string name = variableName;
  prima::Session& s = _SC(prima::Session&, session);
  uint_t globalId = s.FindGlobal(_RC(const uint8_t*, name.c_str()), name.length());
  if (! prima::GlobalsManager::IsValid(globalId))
  {
    stack.Push(gNoVariable);
    return WOP_OK;
  }

  StackValue sv = s.GetGlobalValue(globalId);
  IOperand& op = sv.Operand();
  const uint_t type = op.GetType();
  if (IS_TABLE(type) || IS_TABLE_FIELD(type))
  {
    stack.Push(IS_TABLE(type) ? gVariableIsTable : gVariableIsField);
    return WOP_OK;
  }
  else if (IS_ARRAY(type))
  {
    if ((GET_BASE_TYPE(type) < T_BOOL) || (T_TEXT < GET_BASE_TYPE(type)))
    {
      stack.Push(gVariableArrayUnspp);
      return WOP_OK;
    }

    if (GET_BASE_TYPE(op.GetType()) != T_UNDETERMINED
        && GET_BASE_TYPE(newValue.Operand().GetType()) != GET_BASE_TYPE(op.GetType()))
    {
      stack.Push(gVariableArrayUnspp);
      return WOP_OK;
    }

    transfer_value<DArray>(newValue.Operand(), op);
    stack.Push(gVariableUpdated);
    return WOP_OK;
  }

  switch (GET_BASE_TYPE(type))
  {
    case T_BOOL:
      transfer_value<DBool>(newValue.Operand(), op);
      break;

    case T_CHAR:
      transfer_value<DChar>(newValue.Operand(), op);
      break;

    case T_DATE:
    case T_DATETIME:
    case T_HIRESTIME:
      transfer_value<DHiresTime>(newValue.Operand(), op);
      break;

    case T_INT8:
    case T_INT16:
    case T_INT32:
    case T_INT64:
      transfer_value<DInt64>(newValue.Operand(), op);
      break;

    case T_UINT8:
    case T_UINT16:
    case T_UINT32:
    case T_UINT64:
      transfer_value<DUInt64>(newValue.Operand(), op);
      break;

    case T_REAL:
    case T_RICHREAL:
      transfer_value<DRichReal>(newValue.Operand(), op);
      break;

    case T_TEXT:
      transfer_value<DText>(newValue.Operand(), op);
      break;

    default:
      assert(false);

      stack.Push(gVariableUnkErr);
      return WOP_OK;
  }

  stack.Push(gVariableUpdated);
  return WOP_OK;
}

static WLIB_STATUS
get_glb_table_variable( SessionStack& stack, ISession& session)
{
  DText variableName;
  DText fieldsNames;
  DUInt64 fromRow;
  DUInt64 rowsCount;

  stack[stack.Size() - 1].Operand().GetValue(rowsCount);
  stack[stack.Size() - 2].Operand().GetValue(fromRow);
  stack[stack.Size() - 3].Operand().GetValue(fieldsNames);
  stack[stack.Size() - 4].Operand().GetValue(variableName);
  stack.Pop(4);

  if (variableName.IsNull())
  {
    stack.Push(gNullVariableName);
    return WOP_OK;
  }

  const string name = variableName;
  prima::Session& s = _SC(prima::Session&, session);
  uint_t globalId = s.FindGlobal(_RC(const uint8_t*, name.c_str()), name.length());
  if (! prima::GlobalsManager::IsValid(globalId))
  {
    stack.Push(gNoVariable);
    return WOP_OK;
  }

  StackValue sv = s.GetGlobalValue(globalId);
  IOperand& op = sv.Operand();
  const uint_t type = op.GetType();
  if (!IS_TABLE(type))
  {
    stack.Push(gVariableNotTable);
    return WOP_OK;
  }

  vector<DBSFieldDescriptor> resultFields;
  vector<FIELD_INDEX> resultFieldsIdx;
  ITable& table = op.GetTable();
  if (fieldsNames.IsNull())
  {
    for (FIELD_INDEX f = 0; f < table.FieldsCount(); ++f)
      resultFieldsIdx.push_back(f);
  }
  else
  {
    const string fields = _SC(string, fieldsNames);
    vector<string> fieldsArray;

    size_t startPos = fields.find_first_not_of(' ');
    while (startPos != string::npos && startPos < fields.length())
    {
      size_t lastPos = fields.find_first_of(' ', startPos + 1);
      if (lastPos == string::npos)
      {
        fieldsArray.push_back(fields.substr(startPos, lastPos));
        break;
      }

      fieldsArray.push_back(fields.substr(startPos, lastPos - startPos));
      startPos = fields.find_first_not_of(' ', lastPos);
    }

    for (const string& field : fieldsArray)
    {
      try {
        const FIELD_INDEX f = table.RetrieveField(field.c_str());
        resultFieldsIdx.push_back(f);
      }
      catch (DBSException& e)
      {
        if (e.Code() != DBSException::FIELD_NOT_FOUND)
          throw;

        stack.Push(gVariableInvalidField);
        return WOP_OK;
      }

      for (auto i = resultFieldsIdx.size() - 1; i > 0; --i)
      {
        if (resultFieldsIdx[i] < resultFieldsIdx[i - 1])
          swap(resultFieldsIdx[i], resultFieldsIdx[i - 1]);

        else
          break;
      }
    }
  }

  for (const auto& f : resultFieldsIdx)
    resultFields.push_back(table.DescribeField(f));

  if (fromRow.IsNull())
    fromRow = DUInt64(0);

  if (rowsCount.IsNull())
    rowsCount = DUInt64(table.AllocatedRows() - fromRow.mValue);

  if ((fromRow.mValue + rowsCount.mValue > table.AllocatedRows()))
  {
    stack.Push(gVariableInvalidRow);
    return WOP_OK;
  }

  if (resultFieldsIdx.empty())
  {
    stack.Push(gVariableInvalidField);
    return WOP_OK;
  }

  ITable& result = s.DBSHandler().CreateTempTable(resultFields.size(), &resultFields[0]);
  for (ROW_INDEX r = fromRow.mValue, cr = 0;
      r < fromRow.mValue + rowsCount.mValue;
      ++r, ++cr)
  {
    for (size_t f = 0; f < resultFieldsIdx.size(); ++f)
    {
      uint_t type = resultFields[f].type;

      if (resultFields[f].isArray)
        MARK_ARRAY(type);

      copy_table_value(table, result, r, cr, resultFieldsIdx[f], f, type);
    }
  }

  stack.Push(result);
  return WOP_OK;
}


static WLIB_STATUS
udpate_glb_table_variable_field( SessionStack& stack, ISession& session)
{
  DText   variableName;
  DText   fieldName;
  DUInt64 rowIndex;

  stack[stack.Size() - 4].Operand().GetValue(variableName);
  stack[stack.Size() - 3].Operand().GetValue(fieldName);
  stack[stack.Size() - 2].Operand().GetValue(rowIndex);
  StackValue newValue = stack[stack.Size() - 1];
  stack.Pop(4);

  if (variableName.IsNull())
  {
    stack.Push(gNullVariableName);
    return WOP_OK;
  }
  else if (fieldName.IsNull())
  {
    stack.Push(gVariableInvalidField);
    return WOP_OK;
  }

  const string name = variableName;
  prima::Session& s = _SC(prima::Session&, session);
  uint_t globalId = s.FindGlobal(_RC(const uint8_t*, name.c_str()), name.length());
  if (! prima::GlobalsManager::IsValid(globalId))
  {
    stack.Push(gNoVariable);
    return WOP_OK;
  }

  StackValue sv = s.GetGlobalValue(globalId);
  IOperand& op = sv.Operand();
  uint_t type = op.GetType();
  if (! IS_TABLE(type))
  {
    stack.Push(gVariableNotTable);
    return WOP_OK;
  }

  ITable& table = op.GetTable();
  if (rowIndex.IsNull() || (rowIndex.mValue > table.AllocatedRows()))
  {
    stack.Push(gVariableInvalidRow);
    return WOP_OK;
  }

  StackValue field;
  try {
    const string fname= fieldName;
    field = op.GetFieldAt(table.RetrieveField(fname.c_str()));
  }
  catch (DBSException& e)
  {
    if (e.Code() != DBSException::FIELD_NOT_FOUND)
      throw;

    stack.Push(gVariableInvalidField);
    return WOP_OK;
  }

  StackValue rowValue = field.Operand().GetValueAt(rowIndex.mValue);
  type = rowValue.Operand().GetType();

  if (IS_ARRAY(type))
  {
    transfer_value<DArray>(newValue.Operand(), rowValue.Operand());
    stack.Push(gVariableUpdated);
    return WOP_OK;
  }

  switch (GET_BASE_TYPE(type))
  {
    case T_BOOL:
      transfer_value<DBool>(newValue.Operand(), op);
      break;

    case T_CHAR:
      transfer_value<DChar>(newValue.Operand(), rowValue.Operand());
      break;

    case T_DATE:
    case T_DATETIME:
    case T_HIRESTIME:
      transfer_value<DHiresTime>(newValue.Operand(), rowValue.Operand());
      break;

    case T_INT8:
    case T_INT16:
    case T_INT32:
    case T_INT64:
      transfer_value<DInt64>(newValue.Operand(), rowValue.Operand());
      break;

    case T_UINT8:
    case T_UINT16:
    case T_UINT32:
    case T_UINT64:
      transfer_value<DUInt64>(newValue.Operand(), rowValue.Operand());
      break;

    case T_REAL:
    case T_RICHREAL:
      transfer_value<DRichReal>(newValue.Operand(), rowValue.Operand());
      break;

    case T_TEXT:
      transfer_value<DText>(newValue.Operand(), rowValue.Operand());
      break;

    default:
      assert(false);

      stack.Push(gVariableUnkErr);
      return WOP_OK;
  }

  stack.Push(gVariableUpdated);
  return WOP_OK;
}

static uint8_t gUndefinedType[sizeof(TypeSpec)];
static uint8_t gTextType[sizeof(TypeSpec)];
static uint8_t gInt64Type[sizeof(TypeSpec)];
static uint8_t gUInt64Type[sizeof(TypeSpec)];

WLIB_STATUS
dev_prima_accessors_init()
{
  if ((wh_define_basic_type(T_UNDETERMINED, _RC(TypeSpec*, gUndefinedType)) <= 0)
     || (wh_define_basic_type(T_TEXT, _RC(TypeSpec*, gTextType)) <= 0)
     || (wh_define_basic_type(T_INT64, _RC(TypeSpec*, gInt64Type)) <= 0)
     || (wh_define_basic_type(T_UINT64, _RC(TypeSpec*, gUInt64Type)) <= 0))
  {
    return WOP_UNKNOW;
  }

  static const uint8_t* glbLocals[]= {
                                            gUndefinedType,
                                            gTextType
                                     };

  gGlbVarRead.name = "__w_get_glb";
  gGlbVarRead.localsCount = 2;
  gGlbVarRead.code = read_glb_variable;
  gGlbVarRead.localsTypes = glbLocals;


  static const uint8_t* glbUpdateLocals[]= {
                                               gInt64Type,
                                               gTextType,
                                               gUndefinedType
                                            };

  gGlbVarUpdate.name = "__w_upd_glb";
  gGlbVarUpdate.localsCount = 3;
  gGlbVarUpdate.code = udpate_glb_variable;
  gGlbVarUpdate.localsTypes = glbUpdateLocals;


  static const uint8_t* glbTableLocals[]= {
                                            gUndefinedType,
                                            gTextType,
                                            gTextType,
                                            gUInt64Type,
                                            gUInt64Type
                                         };

  gGlbTableVarRead.name = "__w_get_glb_tab";
  gGlbTableVarRead.localsCount = 5;
  gGlbTableVarRead.code = get_glb_table_variable;
  gGlbTableVarRead.localsTypes = glbTableLocals;

  static const uint8_t* glbTableUpdateLocals[]= {
                                                  gInt64Type,
                                                  gTextType,
                                                  gTextType,
                                                  gUInt64Type,
                                                  gUndefinedType
                                               };

  gGlbTableVarUpdate.name = "__w_upd_glb_tab";
  gGlbTableVarUpdate.localsCount = 5;
  gGlbTableVarUpdate.code = udpate_glb_table_variable_field;
  gGlbTableVarUpdate.localsTypes = glbTableUpdateLocals;

  return WOP_OK;
}
