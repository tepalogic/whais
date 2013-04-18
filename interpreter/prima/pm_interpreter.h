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

#ifndef PM_INTERPRETER_H_
#define PM_INTERPRETER_H_

#include "interpreter.h"

#include "pm_typemanager.h"
#include "pm_globals.h"
#include "pm_procedures.h"
#include "pm_units.h"

namespace whisper {
namespace prima {

class NameSpace
{
public:
  NameSpace (I_DBSHandler& dbsHandler);
  ~NameSpace ();

  I_DBSHandler&     GetDBSHandler () { return m_DbsHandler; }
  TypeManager&      GetTypeManager () { return m_TypeManager; }
  GlobalsManager&   GetGlobalsManager () { return m_GlbsManager; }
  ProcedureManager& GetProcedureManager () { return m_ProcsManager; }
  UnitsManager&     GetUnitsManager () { return m_UnitsManager; }

private:
  I_DBSHandler&    m_DbsHandler;
  TypeManager      m_TypeManager;
  GlobalsManager   m_GlbsManager;
  ProcedureManager m_ProcsManager;
  UnitsManager     m_UnitsManager;
};

class NameSpaceHolder
{
public:
  explicit NameSpaceHolder (NameSpace* pSpace = NULL)
    : m_pSpace (pSpace),
      m_RefsCount (0)
  {
    assert (m_pSpace != NULL);
  }

  NameSpaceHolder (const NameSpaceHolder& source)
    : m_pSpace (source.m_pSpace),
      m_RefsCount (source.m_RefsCount)
  {
    _CC (NameSpace*&, source.m_pSpace) = NULL;
    _CC (uint64_t&, source.m_RefsCount)  = 0;
  }

  ~NameSpaceHolder ()
  {
    assert (m_RefsCount == 0);
    if (m_pSpace != NULL)
      {
        I_DBSHandler& dbsHandler = m_pSpace->GetDBSHandler ();
        delete m_pSpace;
        DBSReleaseDatabase (dbsHandler);
      }
  }

  NameSpace& Get () { assert (m_RefsCount > 0); return *m_pSpace; }
  uint64_t   RefsCount () { return m_RefsCount; }
  void       IncRefsCount () { ++m_RefsCount; }
  void       DecRefsCount () { assert (m_RefsCount > 0); --m_RefsCount; }
  void       ForceRelease () { m_RefsCount = 0; }

private:
  const NameSpaceHolder& operator= (const NameSpaceHolder& source);

  NameSpace* m_pSpace;
  uint64_t   m_RefsCount;
};

class Session : public I_Session
{
public:
  Session (Logger&         log,
          NameSpaceHolder&   globalNames,
          NameSpaceHolder&   privateNames);
  virtual ~Session ();

  virtual void LoadCompiledUnit (WICompiledUnit& unit);
  virtual void ExecuteProcedure (const char* const pProcName,
                                 SessionStack&       stack);

  virtual uint_t GlobalValuesCount () const;
  virtual uint_t ProceduresCount () const;

  virtual const char* GlobalValueName (const uint_t index) const;
  virtual const char* ProcedureName (const uint_t index) const;

  virtual uint_t GlobalValueRawType (const uint32_t index);
  virtual uint_t GlobalValueRawType (const char* const name);
  virtual uint_t GlobalValueFieldsCount (const uint32_t index);
  virtual uint_t GlobalValueFieldsCount (const char* const name);

  virtual const char* GlobalValueFieldName (const uint32_t index,
                                              const uint32_t field);
  virtual const char* GlobalValueFieldName (const char* const name,
                                              const uint32_t      field);

  virtual uint_t GlobalValueFieldType (const uint32_t index,
                                       const uint32_t field);
  virtual uint_t GlobalValueFieldType (const char* const name,
                                       const uint32_t      field);

  virtual uint_t ProcedureParametersCount (const uint_t id) const;
  virtual uint_t ProcedureParametersCount (const char* const name) const;
  virtual uint_t ProcedurePameterRawType (const uint_t id,
                                          const uint_t param);
  virtual uint_t ProcedurePameterRawType (const char* const name,
                                          const uint_t        param);
  virtual uint_t ProcedurePameterFieldsCount (const uint_t id,
                                              const uint_t param);
  virtual uint_t ProcedurePameterFieldsCount (const char* const name,
                                              const uint_t        param);
  virtual const char* ProcedurePameterFieldName (const uint_t id,
                                                   const uint_t param,
                                                   const uint_t field);
  virtual const char* ProcedurePameterFieldName (const char* const name,
                                                   const uint_t        param,
                                                   const uint_t        field);
  virtual uint_t ProcedurePameterFieldType (const uint_t id,
                                            const uint_t param,
                                            const uint_t field);
  virtual uint_t ProcedurePameterFieldType (const char* const name,
                                            const uint_t        param,
                                            const uint_t        field);

  uint32_t       FindGlobal (const uint8_t* pName, const uint_t nameLength);
  StackValue     GetGlobalValue (const uint32_t globalId);
  const uint8_t* FindGlobalTI (const uint32_t globalId);

  uint32_t       FindProcedure (const uint8_t* pName, const uint_t nameLength);
  uint32_t       ArgsCount (const uint32_t procId);
  uint32_t       LocalsCount (const uint32_t procId);
  const uint8_t* FindLocalTI (const uint32_t procId, const uint32_t local);
  Unit&          ProcUnit (const uint32_t procId);
  const uint8_t* ProcCode (const uint32_t procId);
  uint64_t       ProcCodeSize (const uint32_t procId);
  StackValue     ProcLocalValue (const uint32_t procId, const uint32_t local);

  void AquireProcSync (const uint32_t procId, const uint32_t sync);
  void ReleaseProcSync (const uint32_t procId, const uint32_t sync);

  I_DBSHandler& DBSHandler ();

private:
  void     DefineTablesGlobalValues ();
  uint32_t DefineGlobalValue (const uint8_t*    pName,
                              const uint_t      nameLength,
                              const uint8_t*    pTI,
                              const bool        external,
                              I_DBSTable* const pPersistentTable);
  uint32_t DefineProcedure (const uint8_t*           pName,
                            const uint_t             nameLength,
                            const uint32_t           localsCount,
                            const uint32_t           argsCount,
                            const uint32_t           syncCount,
                            std::vector<StackValue>& localValues,
                            const uint32_t*          pTypesOffset,
                            const uint8_t*           pCode,
                            const uint32_t           codeSize,
                            const bool               external,
                            Unit&                    unit);

  NameSpaceHolder& m_GlobalNames;
  NameSpaceHolder& m_PrivateNames;
};

} //namespace prima
} //namespace whisper

#endif /* PM_INTERPRETER_H_ */

