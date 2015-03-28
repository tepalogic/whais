/******************************************************************************
  WCMD - An utility to manage whais database files.
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
******************************************************************************/

#include <iostream>
#include <cstdio>


#include "dbs/dbs_mgr.h"

#include "wcmd_dbcheck.h"
#include "wcmd_optglbs.h"

using namespace std;
using namespace whais;

static bool sgAllAnswersYes = false;

static bool
ask_question (const char* question, const bool allowAllYes = false)
{
  if (sgAllAnswersYes)
    return true;

  while (true)
    {
      char c;

      cout << question;
      cin >> c;

      if ((c == 'y') || (c == 'Y'))
        return true;

      else if ((c == 'n') || (c == 'N'))
        return false;

      else if (allowAllYes && ((c=='a') || (c == 'A')))
        {
          sgAllAnswersYes = true;

          return true;
        }

      if (allowAllYes)
        cout << "Please choose 'y', 'n' or 'a'!\n";

      else
        cout << "Please choose 'y' or 'n'!\n";
    }

  assert (false);
  return false;
}


static bool
repair_callback (const FIX_ERROR_CALLBACK_TYPE type,
                 const char* const             format,
                 ... )
{
  const char* question = NULL;

  bool  result   = true;

  switch (type)
    {
  case INFORMATION:
    cout << "CHK INF: ";
    break;

  case FIX_QUESTION:
    cout << "CHK ERR: ";
    question = "Should I fix it? (y/n/a)";
    break;

  case FIX_INFO:
  case CONFIRMATION_QUESTION:
    cout << "CHK INF: ";
    question = "Is this OK? (y/n/a)";
    break;

  case OPTIMISE_QUESTION:
    cout << "CHK INF: ";
    question = "Should I proceed with it? (y/n/a)";
    break;

  case CRITICAL:
    cout << "CHK CRITICAL ERROR: ";
    result = false;
    break;

  default:

    assert (false);
    cout << "CRITICAL ERROR: Unexpected execution flow.\n";
    result = false;
    break;

    }

  va_list vl;

  va_start (vl, format);
  vprintf (format, vl);
  va_end (vl);

  printf ("\n");


  if ((type == CONFIRMATION_QUESTION)
      || (type == FIX_INFO)
      || (type == OPTIMISE_QUESTION)
      || (type == FIX_QUESTION))
    {
      result = ask_question (question, true);
    }

  return result;
}


bool
repair_database_erros()
{
  return ask_question ("Should I try to check and fix the database? (y/n)");
}


int
check_database_for_errors (const bool allAnswersYes, const bool userRequest)
{
  sgAllAnswersYes = allAnswersYes;

  if ( ! userRequest)
    {
      if (! ask_question ("--- WARNING ---\n"
                          " It is strongly recommended to backup the"
                          " database's files before continuing. Was the"
                          " back up made? (y/n)"))
        {
          return -1;
        }
    }

  const char* const dbName = GetWorkingDB().c_str ();

  if ( ! DBSRepairDatabase (dbName, NULL, repair_callback))
    return -1;

  return 0;
}

