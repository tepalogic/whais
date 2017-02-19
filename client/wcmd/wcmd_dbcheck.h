/******************************************************************************
  WCMD - An utility to manage whais database files.
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
******************************************************************************/

#ifndef WCMD_DBCHECK_H_
#define WCMD_DBCHECK_H_


bool
repair_database_erros();


int
check_database_for_errors(const bool allAnswersYes, const bool userRequest);


#endif /* WCMD_DBCHECK_H_ */
