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

#ifndef PS_TABLE_H_
#define PS_TABLE_H_

#include "ps_templatetable.h"

namespace pastra
{

class PersistentTable : public PrototypeTable
{
public:
  PersistentTable (DbsHandler& dbsHandler, const std::string& tableName);
  PersistentTable (DbsHandler&               dbsHandler,
                   const std::string&        tableName,
                   const DBSFieldDescriptor* pFields,
                   const D_UINT              fieldsCount,
                   const bool                temporal = false);

  PersistentTable (const PrototypeTable& prototype);
  virtual ~PersistentTable ();

  void RemoveFromDatabase ();

  //Implementations for I_DBSTable
  virtual bool        IsTemporal () const;
  virtual I_DBSTable& Spawn () const;

private:
  void InitFromFile ();
  void InitIndexedFields ();
  void InitVariableStorages();

protected:
  virtual void                 Flush ();
  virtual void                 MakeHeaderPersistent ();
  virtual I_DataContainer*     CreateIndexContainer (const D_UINT fieldIndex);
  virtual I_DataContainer&     FixedFieldsContainer ();
  virtual I_DataContainer&     MainTableContainer ();
  virtual VariableLengthStore& VariableFieldsStore ();

  //Data members
  D_UINT64                              m_MaxFileSize;
  D_UINT64                              m_VariableStorageSize;
  std::string                           m_BaseFileName;
  std::auto_ptr<FileContainer>          m_apMainTable;
  std::auto_ptr<FileContainer>          m_apFixedFields;
  std::auto_ptr<VariableLengthStore>    m_apVariableFields;
  bool                                  m_Removed;
};

class TemporalTable : public PrototypeTable
{
public:

  TemporalTable (DbsHandler&               dbsHandler,
                 const DBSFieldDescriptor* pFields,
                 const D_UINT              fieldsCount);
  TemporalTable (const PrototypeTable& protoype);

  virtual ~TemporalTable ();

  //Implementations for I_DBSTable
  virtual bool        IsTemporal () const;
  virtual I_DBSTable& Spawn () const;

protected:
  virtual void                 Flush ();
  virtual void                 MakeHeaderPersistent ();
  virtual I_DataContainer*     CreateIndexContainer (const D_UINT fieldIndex);
  virtual I_DataContainer&     FixedFieldsContainer ();
  virtual I_DataContainer&     MainTableContainer ();
  virtual VariableLengthStore& VariableFieldsStore ();

  //Data members
  std::auto_ptr<TempContainer>       m_apMainTable;
  std::auto_ptr<TempContainer>       m_apFixedFields;
  std::auto_ptr<VariableLengthStore> m_apVariableFields;
};

}
#endif  /* PS_TABLE_H_ */
