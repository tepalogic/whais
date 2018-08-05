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

#include "pm_operand_undefined.h"
#include "pm_operand.h"


namespace whais {
namespace prima {


UndefinedOperand::UndefinedOperand(INativeObject& object)
  : mNativeValue{&object}
{
  Initialise();
}

UndefinedOperand::UndefinedOperand(const UndefinedOperand& source)
{
  memcpy(_SC(void*,this), &source, sizeof(*this));
  Initialise();
}


UndefinedOperand::~UndefinedOperand()
{
  Cleanup();
}


UndefinedOperand&
UndefinedOperand::operator=(const UndefinedOperand& source)
{
  if (this == &source)
    return *this;

  Cleanup();
  memcpy(_SC(void*, this), &source, sizeof(*this));
  Initialise();
  return *this;
}


void
UndefinedOperand::Initialise()
{
  if (mNativeValue)
  {
    assert(IsNull() == false);
    mNativeValue->RegisterUser();
  }
  else
  {
    assert(IsNull());
  }
}


void
UndefinedOperand::Cleanup()
{
  if (mNativeValue)
  {
    assert(IsNull() == false);
    mNativeValue->ReleaseUser();
    mNativeValue = nullptr;
  }
  else
  {
    assert(IsNull());
  }
}


bool
UndefinedOperand::IsNull() const
{
  return mNativeValue == nullptr;
}

uint_t
UndefinedOperand::GetType()
{
  return T_UNDETERMINED;
}

void
UndefinedOperand::NativeObject(INativeObject* const object)
{
  Cleanup();
  mNativeValue = object;
  Initialise();
}

INativeObject&
UndefinedOperand::NativeObject()
{
  if (mNativeValue == nullptr)
    throw InterException(_EXTRA(InterException::NATIVE_NULL_DEREFERENCE));

  return *mNativeValue;
}


StackValue
UndefinedOperand::Clone() const
{
  return StackValue(*this);
}


bool
UndefinedOperand::CustomCopyIncomplete(void* const)
{
  //Just do a (re)initialization priory of a raw copy.
  //It should be ok because this objects only increases some usage counters.
  Initialise();

  return true;
}


} // namespace prima
} // namespace whais
