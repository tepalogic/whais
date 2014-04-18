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


namespace whisper {
namespace pastra {



class PersistentTable : public PrototypeTable
{
public:
  PersistentTable (DbsHandler& dbs, const std::string& name);
  PersistentTable (DbsHandler&                     dbs,
                   const std::string&              name,
                   const DBSFieldDescriptor* const fields,
                   const uint_t                    fieldsCount);
  PersistentTable (const PrototypeTable& prototype);
  virtual ~PersistentTable ();

  void RemoveFromDatabase ();

  virtual bool IsTemporal () const;

  virtual ITable& Spawn () const;

  virtual void FlushEpilog ();

protected:
  virtual void MakeHeaderPersistent ();

  virtual IDataContainer* CreateIndexContainer (const FIELD_INDEX field);

  virtual IDataContainer& RowsContainer ();

  virtual IDataContainer& TableContainer ();

  virtual VariableSizeStore& VSStore ();


  const DBSSettings&           mDbsSettings;
  uint64_t                     mMaxFileSize;
  uint64_t                     mVSDataSize;
  std::string                  mFileNamePrefix;
  std::auto_ptr<FileContainer> mTableData;
  std::auto_ptr<FileContainer> mRowsData;
  VariableSizeStore*           mVSData;
  bool                         mRemoved;

private:
  void InitFromFile (const std::string& tableName);

  void InitIndexedFields ();

  void InitVariableStorages();
};



class TemporalTable : public PrototypeTable
{
public:

  TemporalTable (DbsHandler&                     dbs,
                 const DBSFieldDescriptor* const fields,
                 const FIELD_INDEX               fieldsCount);
  TemporalTable (const PrototypeTable& protoype);
  virtual ~TemporalTable ();

  virtual bool IsTemporal () const;

  virtual ITable& Spawn () const;

  virtual void FlushEpilog ();

protected:

  virtual void MakeHeaderPersistent ();

  virtual IDataContainer* CreateIndexContainer (const FIELD_INDEX field);

  virtual IDataContainer& RowsContainer ();

  virtual IDataContainer& TableContainer ();

  virtual VariableSizeStore& VSStore ();

  std::auto_ptr<TemporalContainer>  mTableData;
  std::auto_ptr<TemporalContainer>  mRowsData;
  VariableSizeStore*                mVSData;
};

} //namespa pastra
} //namespa whisper

#endif  /* PS_TABLE_H_ */

