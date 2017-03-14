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

#ifndef PM_INTERPRETER_H_
#define PM_INTERPRETER_H_

#include "interpreter.h"

#include "pm_typemanager.h"
#include "pm_globals.h"
#include "pm_procedures.h"
#include "pm_units.h"


namespace whais {
namespace prima {


class NameSpace
{
public:
  NameSpace(IDBSHandler& dbsHandler);

  IDBSHandler& GetDBSHandler() { return mDbsHandler; }
  TypeManager& GetTypeManager() { return mTypeManager; }
  GlobalsManager& GetGlobalsManager() { return mGlbsManager; }
  ProcedureManager& GetProcedureManager() { return mProcsManager; }
  UnitsManager& GetUnitsManager() { return mUnitsManager; }

private:
  IDBSHandler&                  mDbsHandler;
  TypeManager                   mTypeManager;
  GlobalsManager                mGlbsManager;
  ProcedureManager              mProcsManager;
  UnitsManager                  mUnitsManager;
};


using NameSpaceHolder=std::shared_ptr<NameSpace>;

class Session : public ISession
{
public:
  Session(Logger& log, NameSpaceHolder& globalNames, NameSpaceHolder& privateNames);
  virtual ~Session();

  virtual void LoadCompiledUnit(WIFunctionalUnit& unit);
  virtual bool LoadSharedLib(WH_SHLIB shl);

  virtual void ExecuteProcedure(const char* const   procedure,
                                 SessionStack&       stack);

  virtual uint_t GlobalValuesCount() const;

  virtual uint_t ProceduresCount() const;

  virtual const char* GlobalValueName(const uint_t index) const;

  virtual const char* ProcedureName(const uint_t index) const;

  virtual uint_t GlobalValueRawType(const uint32_t index);
  virtual uint_t GlobalValueRawType(const char* const name);

  virtual uint_t GlobalValueFieldsCount(const uint32_t index);
  virtual uint_t GlobalValueFieldsCount(const char* const name);

  virtual const char* GlobalValueFieldName(const uint32_t index, const uint32_t field);
  virtual const char* GlobalValueFieldName(const char* const name, const uint32_t field);

  virtual uint_t GlobalValueFieldType(const uint32_t index, const uint32_t field);
  virtual uint_t GlobalValueFieldType(const char* const name, const uint32_t field);

  virtual uint_t ProcedureParametersCount(const uint_t id) const;
  virtual uint_t ProcedureParametersCount(const char* const name) const;

  virtual uint_t ProcedurePameterRawType(const uint_t id, const uint_t param);
  virtual uint_t ProcedurePameterRawType(const char* const name, const uint_t param);

  virtual uint_t ProcedurePameterFieldsCount(const uint_t id, const uint_t param);
  virtual uint_t ProcedurePameterFieldsCount(const char* const name, const uint_t param);

  virtual const char* ProcedurePameterFieldName(const uint_t id,
                                                const uint_t param,
                                                const uint_t field);
  virtual const char* ProcedurePameterFieldName(const char* const name,
                                                const uint_t param,
                                                const uint_t field);

  virtual uint_t ProcedurePameterFieldType(const uint_t id, const uint_t param, const uint_t field);
  virtual uint_t ProcedurePameterFieldType(const char* const name,
                                           const uint_t param,
                                           const uint_t field);

  virtual bool NotifyEvent(const uint_t event, uint64_t* const extra);

  uint32_t FindGlobal(const uint8_t* name, const uint_t nameLength);

  StackValue GetGlobalValue(const uint32_t globalId);

  const uint8_t* FindGlobalTI(const uint32_t globalId);

  uint32_t FindProcedure(const uint8_t* name, const uint_t nameLength);

  uint32_t ArgsCount(const uint32_t procId);

  const uint8_t* FindLocalTI(const uint32_t procId, const uint32_t local);

  IDBSHandler& DBSHandler();

  const Procedure& GetProcedure(const uint32_t procId);

  bool IsServerShoutdowing() const { return mServerStopped; }
  uint_t MaxStackCount() const { return mMaxStackCount; }

private:
  void DefineTablesGlobalValues();

  uint32_t DefineGlobalValue(const uint8_t* const name,
                             const uint_t nameLength,
                             const uint8_t* const typeDesc,
                             const bool external,
                             ITable* const persitentTable);

  uint32_t DefineProcedure(const uint8_t* const name,
                           const uint_t nameLength,
                           const uint32_t localsCount,
                           const uint32_t argsCount,
                           const uint32_t syncCount,
                           std::vector<StackValue>& localValues,
                           const uint32_t* const typesOffset,
                           const uint8_t* const code,
                           const uint32_t codeSize,
                           const bool external,
                           Unit* const unit);

  NameSpaceHolder            mGlobalNames;
  NameSpaceHolder            mPrivateNames;
  std::vector<WH_SHLIB>      mNativeLibs;
  volatile uint_t            mMaxStackCount;
  volatile bool              mServerStopped;
};


} //namespace prima
} //namespace whais


#endif /* PM_INTERPRETER_H_ */
