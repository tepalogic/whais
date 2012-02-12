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

#include "pm_operand.h"

using namespace std;
using namespace prima;

template <class T> static inline T
get_op_not_supp ()
{
  assert (false);
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_CONVERSION));
}

template <class T> static inline T
get_op_null ()
{
  return T ();
}

I_Operand::~I_Operand ()
{
}

NullOperand
I_Operand::GetNullOperand ()
{
  return get_op_not_supp<NullOperand> ();
}

BoolOperand
I_Operand::GetBoolOperand ()
{
  return get_op_not_supp<BoolOperand> ();
}

////////////////StackOperand////////////////////////////////////

StackedOperand::~StackedOperand ()
{
  I_Operand& operand = *_RC (I_Operand*, m_OperandPlacement);
  operand.~I_Operand ();
}

NullOperand
StackedOperand::GetNullOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetNullOperand ();
}

BoolOperand
StackedOperand::GetBoolOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetBoolOperand ();
}

////////////////////BoolOperand///////////////////////////////////

BoolOperand::~BoolOperand ()
{
}

////////////////////CharOperand////////////////////////////////////

CharOperand::~CharOperand ()
{
}




