/******************************************************************************
 PASTRA - A light database one file system and more.
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

#ifndef PM_OPERAND_UNDEFINED_H_
#define PM_OPERAND_UNDEFINED_H_

#include "interpreter.h"
#include "operands.h"

#include "pm_table.h"
#include "pm_operand.h"


namespace whais {
namespace prima {


class UndefinedOperand final : public BaseOperand
{
public:
  UndefinedOperand() = default;
  explicit UndefinedOperand(INativeObject& object);
  UndefinedOperand(const UndefinedOperand& source);
  virtual ~UndefinedOperand() override;

  UndefinedOperand& operator=(const UndefinedOperand& source);

  virtual bool IsNull() const override;
  virtual uint_t GetType() override;
  virtual StackValue Clone() const override;

  virtual void NativeObject(INativeObject* const value) override;
  virtual INativeObject& NativeObject() override;

  virtual bool CustomCopyIncomplete(void* const dest) override;

  void Initialise();
  void Cleanup();

  INativeObject* mNativeValue{nullptr};
};


} // namespace prima
} // namespace whais

#endif /* PM_OPERAND_UNDEFINED_H_ */

