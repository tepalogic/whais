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

#ifndef PS_DBSMGR_H_
#define PS_DBSMGR_H_

#include <map>
#include <string.h>

#include "dbs_mgr.h"
#include "ps_table.h"


namespace pastra
{

class DbsHandler : public I_DBSHandler
{
public:
  DbsHandler (const std::string & name);
  virtual ~ DbsHandler ();

  virtual D_UINT      GetPesistentTablesCount ();
  virtual I_DBSTable& RetrievePersistentTable (D_UINT index);
  virtual I_DBSTable& RetrievePersistentTable (const D_CHAR * pTableName);
  virtual void        ReleaseTable (I_DBSTable&);
  virtual void        AddTable (const D_CHAR* const       pTableName,
                                const DBSFieldDescriptor* pFields,
                                const D_UINT              fieldsCount);
  virtual void        DeleteTable (const D_CHAR * const pTableName);

  virtual I_DBSTable* CreateTempTable (const DBSFieldDescriptor* pFields,
                                       const D_UINT              fieldsCount);

  void Discard ();
  void RemoveFromStorage ();
protected:
  typedef std::map < std::string, PSTable * >TABLES;

  void SyncToFile ();

  const std::string mName;
  TABLES mTables;
};
}

#endif				/* PS_DBSMGR_H_ */
