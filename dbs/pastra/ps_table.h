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

#ifndef PS_TABLE_H_
#define PS_TABLE_H_

#include "ps_templatetable.h"


namespace whais {
namespace pastra {


bool
prototype_fix_table(DbsHandler&          dbs,
                    const std::string&   name,
                    FIX_ERROR_CALLBACK   fixCallback);


class PersistentTable : public PrototypeTable
{
public:
  PersistentTable(DbsHandler& dbs, const std::string& name);
  PersistentTable(DbsHandler&                       dbs,
                  const std::string&                name,
                  const DBSFieldDescriptor* const   inoutFields,
                  const uint_t                      fieldsCount);
  PersistentTable(const PrototypeTable& prototype);
  virtual ~PersistentTable() override;

  void RemoveFromDatabase();

  virtual bool IsTemporal() const override;
  virtual ITable& Spawn() const override;
  virtual void FlushEpilog() override;

public:
  static bool ValidateTable(const std::string& path, const std::string& name);
  static bool RepairTable(DbsHandler&          dbs,
                          const std::string&   name,
                          const std::string&   path,
                          FIX_ERROR_CALLBACK   fixCallback);

protected:
  virtual void MakeHeaderPersistent() override;
  virtual IDataContainer* CreateIndexContainer(const FIELD_INDEX field) override;
  virtual IDataContainer& RowsContainer() override;
  virtual IDataContainer& TableContainer() override;
  virtual VariableSizeStoreSPtr VSStore() override;

  const DBSSettings&               mDbsSettings;
  uint64_t                         mMaxFileSize;
  uint64_t                         mVSDataSize;
  std::string                      mFileNamePrefix;
  std::unique_ptr<FileContainer>   mTableData;
  std::unique_ptr<FileContainer>   mRowsData;
  VariableSizeStoreSPtr            mVSData;
  bool                             mRemoved;

private:
  void InitFromFile(const std::string& tableName);
  void InitIndexedFields();
  void InitVariableStorages();
  void CheckTableValues(FIX_ERROR_CALLBACK fixCallback);
};


class TemporalTable : public PrototypeTable
{
public:

  TemporalTable(DbsHandler&                       dbs,
                const DBSFieldDescriptor* const   inoutFields,
                const FIELD_INDEX                 fieldsCount);
  TemporalTable(const PrototypeTable& protoype);
  virtual ~TemporalTable() override;

  virtual bool IsTemporal() const override;
  virtual ITable& Spawn() const override;
  virtual void FlushEpilog() override;

protected:
  virtual void MakeHeaderPersistent() override;
  virtual IDataContainer* CreateIndexContainer(const FIELD_INDEX field) override;
  virtual IDataContainer& RowsContainer() override;
  virtual IDataContainer& TableContainer() override;
  virtual VariableSizeStoreSPtr VSStore() override;

  std::unique_ptr<TemporalContainer>   mTableData;
  std::unique_ptr<TemporalContainer>   mRowsData;
  VariableSizeStoreSPtr                mVSData;
};


} //namespa pastra
} //namespa whais


#endif  /* PS_TABLE_H_ */
