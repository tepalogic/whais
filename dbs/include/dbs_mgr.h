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

#ifndef DBS_MGR_H_
#define DBS_MGR_H_

#include "dbs_types.h"
#include "dbs_table.h"


static D_UINT32 DEFAULT_MAX_FILE_SIZE = 0X80000000; /* 2GB */


struct DBSFieldDescriptor;
class I_DBSTable;

class I_DBSHandler
{
public:
  I_DBSHandler () {}
  virtual ~ I_DBSHandler () {}

  virtual D_UINT      GetPesistentTablesCount () = 0;
  virtual I_DBSTable& RetrievePersistentTable (D_UINT index) = 0;
  virtual I_DBSTable& RetrievePersistentTable (const D_CHAR* pTableName) = 0;
  virtual void        ReleaseTable (I_DBSTable &) = 0;
  virtual void        AddTable (const D_CHAR* const pTableName,
                                const DBSFieldDescriptor* pFields,
                                D_UINT fieldsCount) = 0;
  virtual void        DeleteTable (const D_CHAR * const pTableName) = 0;
};

void
DBSInit (const D_CHAR * const pDBSDirectory,
         const D_CHAR * const pTempDir,
         D_UINT64 maxFileSize = DEFAULT_MAX_FILE_SIZE);

void
DBSShoutdown ();

const D_CHAR*
DBSGetWorkingDir ();

const D_CHAR*
DBSGetTempDir ();

D_UINT64
DBSGetMaxFileSize ();

void
DBSCreateDatabase (const D_CHAR* const pName);

I_DBSHandler&
DBSRetrieveDatabase (const D_CHAR* const pName);

void
DBSReleaseDatabase (I_DBSHandler& hndDatabase);

void
DBSRemoveDatabase (const D_CHAR* const pName);

#endif /* DBS_MGR_H_ */
