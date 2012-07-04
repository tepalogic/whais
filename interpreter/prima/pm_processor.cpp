/******************************************************************************
 PASTRA - A light database one file system and more.
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

#include <assert.h>

#include "compiler/include/whisperc/wopcodes.h"

#include "pm_processor.h"

using namespace prima;

inline static D_UINT16
load_le_uint16 (const D_UINT8* const pSrc)
{
  D_UINT16 result = pSrc[1];
  result <<= 8; result |= pSrc[0];

  return result;
}

inline static D_UINT32
load_le_uint32 (const D_UINT8* const pSrc)
{
  D_UINT32 result = pSrc[3];
  result <<= 8; result |= pSrc[2];
  result <<= 8; result |= pSrc[1];
  result <<= 8; result |= pSrc[0];

  return result;
}

inline static D_UINT64
load_le_uint64 (const D_UINT8* const pSrc)
{
  D_UINT32 result = pSrc[7];
  result <<= 8; result |= pSrc[6];
  result <<= 8; result |= pSrc[5];
  result <<= 8; result |= pSrc[4];
  result <<= 8; result |= pSrc[3];
  result <<= 8; result |= pSrc[2];
  result <<= 8; result |= pSrc[1];
  result <<= 8; result |= pSrc[0];

  return result;
}

static void
op_func_ldnull (Processor& processor, D_INT64&)
{
  processor.GetStack ().Push ();
}

static void
op_func_ldc (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  DBSChar value (load_le_uint32 (pData));

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT32);
}

static void
op_func_ldi8 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  DBSUInt8 value (*pData);

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT8);
}

static void
op_func_ldi16 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  DBSUInt16 value (load_le_uint16 (pData));

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT16);
}

static void
op_func_ldi32 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  DBSUInt32 value (load_le_uint32 (pData));

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT32);
}

static void
op_func_ldi64 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  DBSUInt64 value (load_le_uint64 (pData));

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT64);
}

static void
op_func_ldd (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT8 day   = pData[0];
  const D_UINT8 month = pData[1];
  const D_INT16 year  = load_le_uint16 (pData + 2);
  DBSDate value (year, month, day);

  processor.GetStack ().Push (value);

  offset += 2 + sizeof (D_UINT16);
}

static void
op_func_lddt (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT8 sec   = pData[0];
  const D_UINT8 min   = pData[1];
  const D_UINT8 hour  = pData[2];
  const D_UINT8 day   = pData[3];
  const D_UINT8 month = pData[4];
  const D_INT16 year  = load_le_uint16 (pData + 5);
  DBSDateTime value (year, month, day, hour, min, sec);

  processor.GetStack ().Push (value);

  offset += 5 + sizeof (D_UINT16);
}

static void
op_func_ldht (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT32 usec  = load_le_uint32 (pData);
  const D_UINT8  sec   = pData[4];
  const D_UINT8  min   = pData[5];
  const D_UINT8  hour  = pData[6];
  const D_UINT8  day   = pData[7];
  const D_UINT8  month = pData[8];
  const D_INT16  year  = load_le_uint16 (pData + 9);
  DBSHiresTime value (year, month, day, hour, min, sec, usec);

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT32) + 5 + sizeof (D_UINT16);
}

static void
op_func_ldr (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_INT64 intPart     = _SC (D_INT64, load_le_uint64 (pData));
  D_INT8        decExp      = *(pData + sizeof (D_UINT64));
  const bool    negativeExp = (decExp > 0) ? false : true;

  if (negativeExp)
    decExp *= -1;

  D_UINT64 fracPart = 1;
  while (decExp-- > 0)
    fracPart *= 10;

  RICHREAL_T basicValue = intPart;

  if (negativeExp)
    basicValue /= fracPart;
  else
    basicValue *= fracPart;

  DBSRichReal value (basicValue);
  processor.GetStack ().Push (value);

  offset += (sizeof (D_UINT8) + sizeof (D_UINT64));
}

static void
op_func_ldt (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT32 textOff  = load_le_uint32 (pData);
  const D_UINT8* pTextSrc = processor.GetUnit ().GetConstData (textOff);

  assert (pTextSrc != NULL);

  DBSText value (pTextSrc);
  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT32);
}

static void
op_func_ldbt (Processor& processor, D_INT64& offset)
{
  DBSBool value (true);
  processor.GetStack ().Push (value);
}

static void
op_func_ldbf (Processor& processor, D_INT64& offset)
{
  DBSBool value (false);
  processor.GetStack ().Push (value);
}

static void
op_func_ldlo8 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT8 localIndex = *pData;

  assert (localIndex < processor.LocalsCount ());

  StackValue& localValue = processor.GetStack ()[processor.StackBegin () +
                                                 localIndex];
  LocalOperand localOp (localValue);
  StackValue   value (localOp);

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT8);
}

static void
op_func_ldlo16 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT16 localIndex = load_le_uint16 (pData);

  assert (localIndex < processor.LocalsCount ());

  StackValue& localValue = processor.GetStack ()[processor.StackBegin () +
                                                 localIndex];
  LocalOperand localOp (localValue);
  StackValue   value (localOp);

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT16);
}

static void
op_func_ldlo32 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT32 localIndex = load_le_uint32 (pData);

  assert (localIndex < processor.LocalsCount ());

  StackValue& localValue = processor.GetStack ()[processor.StackBegin () +
                                                 localIndex];
  LocalOperand localOp (localValue);
  StackValue   value (localOp);

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT32);
}

static void
op_func_ldgb8 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;

  const D_UINT8  globalIndex = *pData;
  const D_UINT32 glbId       = processor.GetUnit ().GetGlobalId (globalIndex);
  StackValue     glbValue    = processor.GetSession ().GetGlobalValue (glbId);

  processor.GetStack ().Push (glbValue);

  offset += sizeof (D_UINT8);
}

static void
op_func_ldgb16 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;

  const D_UINT16 globalIndex = load_le_uint16 (pData);
  const D_UINT32 glbId       = processor.GetUnit ().GetGlobalId (globalIndex);
  StackValue     glbValue    = processor.GetSession ().GetGlobalValue (glbId);

  processor.GetStack ().Push (glbValue);

  offset += sizeof (D_UINT16);
}

static void
op_func_ldgb32 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;

  const D_UINT32 globalIndex = load_le_uint32 (pData);
  const D_UINT32 glbId       = processor.GetUnit ().GetGlobalId (globalIndex);
  StackValue     glbValue    = processor.GetSession ().GetGlobalValue (glbId);

  processor.GetStack ().Push (glbValue);

  offset += sizeof (D_UINT32);
}

static void
op_func_cts (Processor& processor, D_INT64& offset)
{
  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          processor.GetStack ().Size ());

  processor.GetStack ().Pop (1);
}

template <class T> static void
op_func_stXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 2));

  I_Operand& source = stack[stackSize - 1].GetOperand ();
  I_Operand& dest   = stack[stackSize - 2].GetOperand ();

  T value;
  source.GetValue (value);
  dest.SetValue (value);

  stack.Pop (1);
}

static void
op_func_stta (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 2));

  I_Operand& source = stack[stackSize - 1].GetOperand ();
  I_Operand& dest   = stack[stackSize - 2].GetOperand ();
  if (source.IsNull ())
    {
      if (dest.IsNull() != false)
        {
          TableOperand& tableOp = _SC (TableOperand&, dest);
          TableOperand  newTabOp (tableOp.GetTableRef ().GetDBSHandler (),
                                  tableOp.GetTableRef ().GetTable ().Spawn());
          tableOp = newTabOp;
        }
    }
  else
    {
      TableOperand& srcTableOp  = _SC (TableOperand&, source);
      TableOperand& destTableOp = _SC (TableOperand&, dest);

      srcTableOp = destTableOp;
    }

  stack.Pop (1);
}

static void
op_func_stf (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 2));

  FieldOperand& src  = _SC (FieldOperand&, stack[stackSize - 1].GetOperand ());
  FieldOperand& dest = _SC (FieldOperand&, stack[stackSize - 2].GetOperand ());

  src = dest;

  stack.Pop (1);
}

static void
op_func_inull (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& source = stack[stackSize - 1].GetOperand ();
  StackValue result ( BoolOperand (DBSBool (source.IsNull ())));

  stack[stackSize - 1] = result;
}

static void
op_func_nnull (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (processor.GetStack ().Size () - 2));

  I_Operand& source = stack[stackSize - 1].GetOperand ();
  StackValue result ( BoolOperand (DBSBool (source.IsNull () == false)));

  stack[stackSize - 1] = result;
}

static void
op_func_call (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;

  const D_UINT32 prcMgrId = load_le_uint32 (pData);
  offset += sizeof (D_UINT32);

  Session&      session         = processor.GetSession ();
  SessionStack& stack           = processor.GetStack ();
  const D_UINT  procLocalsCount = session.LocalsCount (prcMgrId);

  for (D_UINT32 localIndex = session.ArgsCount (prcMgrId) + 1;
       localIndex < procLocalsCount;
       ++localIndex)
    {
      StackValue localValue = session.ProcLocalValue (prcMgrId, localIndex);
      stack.Push (localValue);
    }

  Unit&                procUnit     = session.ProcUnit (prcMgrId);
  const D_UINT8* const pProcCode    = session.ProcCode (prcMgrId);
  const D_UINT32       procCodeSize = session.ProcCodeSize (prcMgrId);

  Processor procedure (session,
                       stack,
                       procUnit,
                       pProcCode,
                       procCodeSize,
                       procLocalsCount);
  try
  {
      procedure.Run ();
  }
  catch (...)
  {
      assert (false);
      //TODO: We need to implement this!
      throw;
  }
}

static void
op_func_ret (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) < stackSize);

  StackValue result = stack[stackSize - 1];

  stack.Pop (stackSize - processor.StackBegin());
  stack.Push (result);
}

typedef void (*OP_FUNC) (Processor& processor, D_INT64& ioOffset);

static OP_FUNC operations[] = {
                                NULL,
                                op_func_ldnull,
                                op_func_ldc,
                                op_func_ldi8,
                                op_func_ldi16,
                                op_func_ldi32,
                                op_func_ldi64,
                                op_func_ldd,
                                op_func_lddt,
                                op_func_ldht,
                                op_func_ldr,
                                op_func_ldt,
                                op_func_ldbt,
                                op_func_ldbf,
                                op_func_ldlo8,
                                op_func_ldlo16,
                                op_func_ldlo32,
                                op_func_ldgb8,
                                op_func_ldgb16,
                                op_func_ldgb32,
                                op_func_cts,

                                op_func_stXX<DBSBool>,
                                op_func_stXX<DBSChar>,
                                op_func_stXX<DBSDate>,
                                op_func_stXX<DBSDateTime>,
                                op_func_stXX<DBSHiresTime>,
                                op_func_stXX<DBSInt8>,
                                op_func_stXX<DBSInt16>,
                                op_func_stXX<DBSInt32>,
                                op_func_stXX<DBSInt64>,
                                op_func_stXX<DBSReal>,
                                op_func_stXX<DBSRichReal>,
                                op_func_stXX<DBSText>,
                                op_func_stXX<DBSUInt8>,
                                op_func_stXX<DBSUInt16>,
                                op_func_stXX<DBSUInt32>,
                                op_func_stXX<DBSUInt64>,
                                op_func_stta,
                                op_func_stf,
                                op_func_stXX<DBSArray>,

                                op_func_inull,
                                op_func_nnull,

                                op_func_call,
                                op_func_ret
                              };

/////////////////////////////Processor/////////////////////////////////////////

void
Processor::Run ()
{
  W_OPCODE opcode;

  while (m_CodePos < m_CodeSize)
    {

      D_INT64 offset = whc_decode_opcode (m_pCode + m_CodePos, &opcode);

      assert (opcode < (sizeof operations / sizeof operations[0]));
      assert (opcode != 0);
      assert ((offset > 0) && (offset < 3));

      operations[opcode] (*this, offset);
      m_CodePos += offset;

      assert (m_CodePos <= m_CodeSize);
    }
}

