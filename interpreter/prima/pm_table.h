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

#ifndef PM_TABLE_H_
#define PM_TABLE_H_

#include <assert.h>

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_table.h"
#include "pm_general_table.h"

namespace whisper {
namespace prima {

class TableReference
{
public:
  TableReference (I_DBSHandler& dbsHnd, I_DBSTable& table)
    : m_DbsHnd (dbsHnd),
      m_Table (table),
      m_RefCount (0)
  {
  }

  void IncrementRefCount () { ++m_RefCount; }
  void DecrementRefCount ()
  {
    assert (m_RefCount > 0);
    if (--m_RefCount == 0)
      delete this;
  }

  I_DBSTable&   GetTable () { assert (m_RefCount > 0); return m_Table; }
  I_DBSHandler& GetDBSHandler () { assert (m_RefCount > 0); return m_DbsHnd; }

private:
  ~TableReference ()
  {
    if (&m_Table != &GeneralTable::Instance ())
      m_DbsHnd.ReleaseTable (m_Table);
  }

  I_DBSHandler& m_DbsHnd;
  I_DBSTable&   m_Table;
  uint64_t      m_RefCount;
};

} //namespace whisper
} //namespace prima

#endif // PM_TABLE_H_

