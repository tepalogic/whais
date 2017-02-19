/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whais.
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

#ifndef BASE_TABLES_H_
#define BASE_TABLES_H_

#include "stdlib/interface.h"


extern whais::WLIB_PROC_DESCRIPTION       gProcTableIsPersistent;
extern whais::WLIB_PROC_DESCRIPTION       gProcTableFieldsCount;
extern whais::WLIB_PROC_DESCRIPTION       gProcTableFieldByIndex;
extern whais::WLIB_PROC_DESCRIPTION       gProcTableFieldByName;
extern whais::WLIB_PROC_DESCRIPTION       gProcTableRowsCount;
extern whais::WLIB_PROC_DESCRIPTION       gProcTableReusableRowsCount;
extern whais::WLIB_PROC_DESCRIPTION       gProcTableAddRow;
extern whais::WLIB_PROC_DESCRIPTION       gProcTableFindRemovedRow;
extern whais::WLIB_PROC_DESCRIPTION       gProcTableRemoveRow;
extern whais::WLIB_PROC_DESCRIPTION       gProcTableExchangeRows;
extern whais::WLIB_PROC_DESCRIPTION       gProcTableSort;

whais::WLIB_STATUS
base_tables_init();

#endif /* BASE_TABLES_H_ */
