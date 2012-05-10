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

#include <assert.h>
#include <string.h>
#include <vector>
#include <algorithm>

#include "utils/include/wfile.h"

#include "dbs_mgr.h"
#include "dbs_exception.h"

#include "ps_table.h"

using namespace pastra;
using namespace std;

static const D_CHAR PS_TEMP_TABLE_SUFFIX[] = "pttable_";

static string
get_temp_table_name ()
{
  static D_UINT64      countTemporalTables = 0;
  static WSynchronizer sync;
  string               result (PS_TEMP_TABLE_SUFFIX);

  sync.Enter ();
  D_UINT64 currentCount = countTemporalTables++;
  sync.Leave ();

  append_int_to_str (result, currentCount);

  return result;
}

////////////////////TemporalTable////////////////////////////////////////////////////////////


TemporalTable::TemporalTable (DbsHandler&               dbsHandler,
                              const DBSFieldDescriptor* pFields,
                              const D_UINT              fieldsCount) :
  TemplateTable (dbsHandler, get_temp_table_name (), pFields, fieldsCount, true)
{
}

TemporalTable::~TemporalTable ()
{
  FlushNodes ();
  RemoveFromDatabase ();
}

bool
TemporalTable::IsTemporal () const
{
  return true;
}



