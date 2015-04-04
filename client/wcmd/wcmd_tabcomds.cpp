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

#include <assert.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#include "whais.h"
#include "whais_time.h"

#include "utils/range.h"
#include "utils/tokenizer.h"
#include "dbs/dbs_mgr.h"

#include "wcmd_tabcomds.h"
#include "wcmd_cmdsmgr.h"
#include "wcmd_optglbs.h"
#include "wcmd_valparser.h"
#include "wcmd_altertable.h"


using namespace std;
using namespace whais;

static const string sArgsDelimiter (" \t");
static const string sListDelimiter (",");

static const char tableShowDesc[]    = "Describe the tables from the database.";
static const char tableShowDescExt[] =
  "Print information about a table of the database. If a table name is not\n"
  "provided it lists the tables from database.\n"
  "Usage:\n"
  "  table [table_name] ... ";

static const char tableRmDesc[]      = "Remove some tables from the database.";
static const char tableRmDescExt[ ]  =
  "Remove the specified tables from the database.\n"
  "Usage:\n"
  "  remove table_name [second_table_name ...]\n"
  "Example:\n"
  "  remove table other_table yet_another_table";

static const char tableAddDesc[]     = "Add a table to the database.";
static const char tableAddDescExt[]  =
  "Add a table to the database with the specified table name, field names\n"
  "and fields types.\n"
  "Type specifiers:\n"
  "\tbasic_type:\n"
  "\t\tBOOL\n"
  "\t\tCHAR\n"
  "\t\tDATE\n"
  "\t\tDATETIME\n"
  "\t\tHIRESTIME\n"
  "\t\tINT8\n"
  "\t\tINT16\n"
  "\t\tINT32\n"
  "\t\tINT64\n"
  "\t\tUINT8\n"
  "\t\tUINT16\n"
  "\t\tUINT32\n"
  "\t\tUINT64\n"
  "\t\tREAL\n"
  "\t\tRICHREAL\n"
  "\taggregate_types:\n"
  "\t\tTEXT\n"
  "\t\tARRAY basic_type\n"
  "\tfield_specifier:\n"
  "\t\tfield_name [basic_type]\n"
  "\t\tfield_name [aggregate_types]\n"
  "Usage:\n"
  "  add table_name field_specifier [second_field_specifier] ...\n"
  "Example:\n"
  "  add SomeTableName fieldName DATE otherFieldname ARRAY INT8";

static const char tableCopyDesc[]    = "Create a copy of a table.";
static const char tableCopeDescExt[] =
  "Creates a copy of persistent table. The user may supply a selection of\n"
  "rows based on the row numeber and/or some fields values from the\n"
  "that should be copied in the new table"
  "of the table.)\n"
  "Usage:\n"
  " copy srctab newtab [field_name[@new_field_name],...] {rows} {field_vals}\n"
  "Example:\n"
  " copy t1 t2\n"
  " copy t1 t2 name@username,pass\n"
  " copy t1 t2 name@username,pass@password * id=5 \n"
  " copy t1 t2 name,pass@password 1,3-100,1000- year=2001, \n"
  "Note:\n"
  "  The syntax used to provide the selected rows is the same as the one\n"
  "  to list the rows of a value. Have a look at 'help rows' for more\n"
  "  information and examples on how to select rows to be listed or updated.";

static const char tableAddIndDesc[]    = "Index the specified field tables.";
static const char tableAddIndDescExt[] =
  "Index the values of the specified table fields for faster searching.\n"
  "Currently it does not support to index array nor text field types.\n"
  "Usage:\n"
  "  index table_name field_name [second_field_name ...]\n"
  "Example:\n"
  "  index mytab password_hash";

static const char tableRmIndDesc[]    = "Remove the index associated with some"
                                        " table fields.";
static const char tableRmIndDescExt[] =
  "Remove the associated index of the specified table fields.\n"
  "Usage:\n"
  "  rmindex table_name field_name [second_field_name ...]\n"
  "Example:\n"
  "  rmindex mytab password_hash";

static const char rowsDesc[]    = "Manipulate table rows.";
static const char rowsDescExt[] =
  "This command is used to manage the rows of a table. It can be used to\n"
  "update, add, remove or retrieve tables rows content. It does so by means\n"
  "of different sub commands:\n"
  "  info    Used to retrieve general info about rows of a table.\n"
  "  list    Used to display the content of a set of row (it also allow\n"
  "          the selection of field subset to list). The selection is made\n"
  "          by using field and rows selection specifiers. Example:\n"
  "\n"
  "            rows TableName field1,field2 1,3,5,70-90,100-\n"
  "\n"
  "          List only field1 and field2 from rows 1,3,5, 70 to 90 included\n"
  "          and 100 to rows count. While\n"
  "\n"
  "            rows TableName * 10,20- field1=n,100@200 field2=min@max\n"
  "\n"
  "          List all fields from rows 10 and 20 to rows count, and further\n"
  "          only ones that have field1 set to null of from 100 to 200, and\n"
  "          field2 is not null.\n"
  "  add     Used to add a new table row. Example:\n"
  "\n"
  "             rows Table field1=\"Some text value\",field5='2001/2/3'\n"
  "\n"
  "          Add a new row to the table, set all fields by default to null,\n"
  "          except field1 that's set to 'Some text value' and field5\n"
  "          that's set to 3rd Feb 2001.\n"
  "  reuse   Reuse does the same thing as 'add' but it tries not to increase\n"
  "          the table. First it searches for an available row (e.g one that\n"
  "          has all fields set to null).\n"
  "  update  Updates a selected set of table rows. Example:\n"
  "             rows Table update * qnty=100,price=0.99 productId=190\n"
  "  remove  Remove a select set of rows from a table. By removing it's\n"
  "          understood that all of fields values of the corresponding rows\n"
  "          will be set to null such way the can be reused latter. Example:\n"
  "\n"
  "             rows Table remove * field1=n,field2=min,field3=max\n"
  "\n"
  "          Will remove all rows that have field1 is null, field2 is set to\n"
  "          minimum and field3 is set to maximum.\n"
  "Usage:\n"
  "  rows Table info\n"
  "  rows Table list [[field,...]|* [[rows_selector]|* [field_selector]]]\n"
  "  rows Table add {field_values}\n"
  "  rows Table reuse {field_values}\n"
  "  rows Table update {field_values} [[rows_selector]|* [field_selector]]\n"
  "  rows Table remove [[rows_selector]|* [field_selector]]\n"
  "Examples:\n"
  "  rows Products info\n"
  "  rows Products list * 1,2,40 qnty=5@max price=min@2.99,10.9,null\n"
  "  rows Products list Supplier,Received * qnty=0 price=null\n"
  "  rows Products reuse Supplier='Smart DBs',Recieved='2011/12/15',qnty=100\n"
  "  rows Products update price=10.99 * price=0.99,productId=891\n"
  "  rows Products remove * qnty=null,0\n";

static const char fieldsDesc[]    = "Manipulate table fields.";
static const char fieldsDescExt[] = ""
  "This commands is used to the manage the fields of a table. It can be\n"
  "used to add fields to add fields into a table, remove fields from a\n"
  "table, rename or change a table's fields. The following sub commands are\n"
  "supported:\n"
  "  add     Add a new field into a table.\n"
  "\n"
  "             fields Table add field1 TEXT field2 ARRAY DATE\n"
  "\n"
  "          This commands adds two fields to table Table, field1 as a\n"
  "          text and field2 as an array of dates.\n"
  "\n"
  "  remove  Remove a field from a table.\n"
  "\n"
  "             fields Table remove field1 field2\n"
  "\n"
  "          Drops the fields name field1 and field2 from table Table.\n"
  "\n"
  "  rename  Rename fields of a table.\n"
  "\n"
  "             fields Table rename field1 new_field1 field2 new_field2\n"
  "\n"
  "          Renames the fields name field1 and field2 from table Table, to\n"
  "\n"
  "          new_field1 and respectively to new_filed2.\n"
  "\n"
  "  retype  Change the type of the fields of a table.\n"
  "\n"
  "             fields Table retype field1 TEXT field2 ARRAY DATETIME\n"
  "\n"
  "          Attempts to change the types of table Table field field1 to\n"
  "          TEXT and of field2 to an array of date and times. Depending on\n"
  "          the old fields types this operations might fail (e.g. the new\n"
  "          and old field's types are incompatible and there is at least\n"
  "          one non null value that has to be converted).\n"
  "          Hint: If you want to change the type of a field but skip its\n"
  "          value conversion, it is better to remove the field first and\n"
  "          after add it again with the new type.\n"
  "Usage:\n"
  "  alter Table {subcommand}[,{subcommand}[,{subcommand}]]\n"
  "Where {subcommands}:\n"
  "  add {field specfier} [{field specifier} [{field specifier}]]\n"
  "  retype {field specfier} [{field specifier} [{field specifier}]]\n"
  "  rename {old name} {new name} [{old name} {new name}]\n"
  "  remove {name} [{name} [{name}]]\n"
  "Examples:\n"
  "  alter Table add name TEXT surename TEXT, retype age INT8\n"
  "  alter Table retype age UINT8 hash UINT64, remove email email_hash\n"
  "  alter Table add username TEXT\n"
  "  alter Table rename hash username_hash\n"
  "Note:\n"
  "1. Due to performance reasons it is better to group as much of this sub \n"
  "   commands as much as possible. Also because if one of the sub commands\n"
  "   fails then all group of them will be canceled.\n"
  "2. The exact syntax for the fields specified in care of add or retype\n"
  "   sub commands it is the same as the one specified in the case of\n"
  "   'table_add' commands. See its help description for more details.\n"
  "3. Any indexed fields associated with the table will be discarded. So\n"
  "   they have to be rebuild after a successful completion of such a\n"
  "   command.\n";



static void
print_field_desc (const DBSFieldDescriptor& desc, const bool indexed)
{
  cout << desc.name << " : ";

  if (desc.isArray)
    cout << "ARRAY OF ";

  if (desc.type == T_BOOL)
    cout << "BOOL";

  else if (desc.type == T_CHAR)
    cout << "CHAR";

  else if (desc.type == T_DATE)
    cout << "DATE";

  else if (desc.type == T_DATETIME)
    cout << "DATETIME";

  else if (desc.type == T_HIRESTIME)
    cout << "HIRESTIME";

  else if (desc.type == T_INT8)
    cout << "INT8";

  else if (desc.type == T_INT16)
    cout << "INT16";

  else if (desc.type == T_INT32)
    cout << "INT32";

  else if (desc.type == T_INT64)
    cout << "INT64";

  else if (desc.type == T_UINT8)
    cout << "UINT8";

  else if (desc.type == T_UINT16)
    cout << "UINT16";

  else if (desc.type == T_UINT32)
    cout << "UINT32";

  else if (desc.type == T_UINT64)
    cout << "UINT64";

  else if (desc.type == T_REAL)
    cout << "REAL";

  else if (desc.type == T_RICHREAL)
    cout << "RICHREAL";

  else if (desc.type == T_RICHREAL)
    cout << "RICHREAL";

  else if (desc.type == T_TEXT)
    cout << "TEXT";

  else
    {
      assert (false);
      cout << "??";
    }

  if (indexed)
    cout << " (indexed)";

  cout << endl;
}


static bool
parse_field_type (const string&   cmdLine,
                  size_t*         ioLinePos,
                  DBS_FIELD_TYPE* oType,
                  bool_t*         oIsArray)
{
  string token = CmdLineNextToken (cmdLine, *ioLinePos);
  transform (token.begin (), token.end (), token.begin (), ::toupper);
  if (token == "ARRAY")
    {
      *oIsArray = TRUE;

      token = CmdLineNextToken (cmdLine, *ioLinePos);
      transform (token.begin (), token.end (), token.begin (), ::toupper);
    }
  else
    *oIsArray = FALSE;

  if (token == "BOOL")
    *oType = T_BOOL;

  else if (token == "CHAR")
    *oType = T_CHAR;

  else if (token == "DATE")
    *oType = T_DATE;

  else if (token == "DATETIME")
    *oType = T_DATETIME;

  else if (token == "HIRESTIME")
    *oType = T_HIRESTIME;

  else if (token == "INT8")
    *oType = T_INT8;

  else if (token == "INT16")
    *oType = T_INT16;

  else if (token == "INT32")
    *oType = T_INT32;

  else if (token == "INT64")
    *oType = T_INT64;

  else if (token == "UINT8")
    *oType = T_UINT8;

  else if (token == "UINT16")
    *oType = T_UINT16;

  else if (token == "UINT32")
    *oType = T_UINT32;

  else if (token == "UINT64")
    *oType = T_UINT64;

  else if (token == "REAL")
    *oType = T_REAL;

  else if (token == "RICHREAL")
    *oType = T_RICHREAL;

  else if (token == "RICHREAL")
    *oType = T_RICHREAL;

  else if (token == "TEXT")
    *oType = T_TEXT;

  else
    {
      cerr << "Token '" << token << "' is not a valid type.\n";
      return false;
    }

  return true;
}


static void
print_match_statistic (ostream&                os,
                       ITable&                 table,
                       const Range<ROW_INDEX>& rows,
                       const WTICKS            startTicks,
                       const WTICKS            endTicks)
{
  const WTICKS ticks = endTicks - startTicks;

  const uint_t  secs  = ticks / 1000;
  const uint_t  msecs = ticks % 1000;

  ROW_INDEX matchedRows = 0;
  for (size_t r = 0; r < rows.mIntervals.size(); ++r)
    {
      const Interval<ROW_INDEX> intv = rows.mIntervals[r];

      matchedRows += intv.mTo - intv.mFrom + 1;
    }

  os << "Matched "<< matchedRows;
  os << '(' << table.AllocatedRows() << ") rows in ";
  os << secs << '.' << setw (3) << setfill ('0') << msecs;
  os << setw (1) << setfill (' ') << "s.\n";
}


static bool
cmdTableAdd (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  IDBSHandler&               dbs     = *_RC (IDBSHandler*, context);
  size_t                     linePos = 0;
  string                     token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL       level   = GetVerbosityLevel();
  bool                       result  = true;
  vector<DBSFieldDescriptor> fields;
  vector<string>             fieldsNames;
  string                     tableName;



  assert (token == "add");

  if (linePos >= cmdLine.length())
    goto invalid_args;

  tableName = CmdLineNextToken (cmdLine, linePos);

  if (level > VL_INFO)
    cout << "Adding table " << tableName << "...\n";

  if (linePos >= cmdLine.length())
    goto invalid_args;

  do
    {
      DBSFieldDescriptor desc = {NULL, };

      //The field name

      token = CmdLineNextToken (cmdLine, linePos);
      assert (token.length() > 0);

      fieldsNames.push_back (token);
      desc.name = fieldsNames.back().c_str ();

      if ( ! parse_field_type (cmdLine, &linePos, &desc.type, &desc.isArray))
        goto invalid_args;

      fields.push_back (desc);

      if (level >= VL_INFO)
        {
          cout << " ... ";
          print_field_desc (desc, false);
        }

      assert (fields.size() == fieldsNames.size());
    }
  while (linePos < cmdLine.length());

  try
  {
    dbs.AddTable (tableName.c_str (), fields.size(), &(fields.front()));
  }
  catch (const Exception& e)
  {
    printException (cerr, e);

    result = false;
  }

  return result;

invalid_args:
  cerr << "Invalid command arguments.\n";

  return false;
}


static bool
cmdTableCopy (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  IDBSHandler&               dbs      = *_RC (IDBSHandler*, context);
  size_t                     linePos  = 0;
  string                     token    = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL       level    = GetVerbosityLevel();
  ITable*                    srcTable = NULL;
  bool                       result   = true;
  RowsSelection              rows;
  string                     tableName;
  string                     newTableName;
  string                     fieldSpecifier;
  vector<string>             selectedFields;
  vector<string>             newTableFields;

  assert (token == "copy");

  if (linePos < cmdLine.length ())
    tableName = CmdLineNextToken (cmdLine, linePos);

  if (linePos < cmdLine.length())
    newTableName = CmdLineNextToken (cmdLine, linePos);

  if ((newTableName.length () == 0) || (tableName.length () == 0))
    goto invalid_args;

  fieldSpecifier = CmdLineNextToken (cmdLine, linePos);
  if (fieldSpecifier != "*")
    {
      size_t fieldsPos = 0;
      while (fieldsPos < fieldSpecifier.length ())
        {
          string fieldEntry = NextToken (fieldSpecifier, fieldsPos, ",");
          if (fieldEntry.length() == 0)
            goto invalid_args;

          const size_t sepOff = fieldEntry.find ('@');

          if (sepOff == string::npos)
            {
              selectedFields.push_back (fieldEntry);
              newTableFields.push_back (fieldEntry);
            }
          else
            {
              selectedFields.push_back (fieldEntry.substr (0, sepOff));
              newTableFields.push_back (fieldEntry.substr (sepOff + 1));
            }
        }
    }

  assert (selectedFields.size () == newTableFields.size ());

  try
  {
    assert (linePos <= cmdLine.length());

    srcTable = &dbs.RetrievePersistentTable (tableName.c_str ());

    if (! ParseRowsSelectionClause (&cerr,
                                    *srcTable,
                                    cmdLine.c_str () + linePos,
                                    rows))
      {
        dbs.ReleaseTable (*srcTable);
        return false;
      }

    if (level > VL_INFO)
      {
        cout << "Copy table " << tableName << " into " << newTableName << ".\n";

        if (selectedFields.size () > 0)
          cout << "Selected fields to copy:\n";

        for (size_t f = 0; f < selectedFields.size (); ++f)
          cout << selectedFields[f] << " --> " << newTableFields[f] << endl;
      }


    const WTICKS matchBegin = wh_msec_ticks();

    MatchSelectedRows (*srcTable, rows);

    const WTICKS matchEnd = wh_msec_ticks();

    print_match_statistic (cout, *srcTable, rows.mRows, matchBegin, matchEnd);

    dbs.ReleaseTable (*srcTable);
    srcTable = NULL;

    TableAlterRules copyRules (dbs, tableName, selectedFields);
    for (size_t i = 0; i < selectedFields.size (); ++i)
      copyRules.RenameField (selectedFields[i], newTableFields[i]);

    copyRules.CommitToTable (newTableName, rows.mRows);
  }
  catch (const Exception& e)
  {
      printException (cerr, e);
      result = false;
  }

  if (srcTable != NULL)
    dbs.ReleaseTable (*srcTable);

  return result;

invalid_args:

  cerr << "Invalid command arguments.\n";

  if (srcTable != NULL)
    dbs.ReleaseTable (*srcTable);

  return false;
}



static bool
cmdTableRemove (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  IDBSHandler&         dbs     = *_RC (IDBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel();
  bool                 result  = false;

  assert (token == "remove");

  if (linePos >= cmdLine.length())
    goto invalid_args;

  if (level > VL_INFO)
    cout << "Removing table (s):\n";

  do
    {
      try
      {
        token = CmdLineNextToken (cmdLine, linePos);
        dbs.DeleteTable (token.c_str ());

        if (level >= VL_INFO)
          cout << " ... " << token << endl;

        result = true;
      }
      catch (const Exception& e)
      {
        printException (cerr, e);
      }
    }
  while (linePos < cmdLine.length());

  return result;

invalid_args:
  if (level >= VL_ERROR)
    cerr << "Invalid commands arguments.\n";

  return false;
}


static bool
cmdTablePrint (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  IDBSHandler&         dbs     = *_RC (IDBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel();
  bool                 result  = true;

  assert (token == "table");

  try
  {
    if (linePos >= cmdLine.length())
      {
        const TABLE_INDEX tablesCount = dbs.PersistentTablesCount();

        if (level >= VL_INFO)
          cout << "Retrieved " << tablesCount << " persistent tables.\n";

        for (TABLE_INDEX index = 0; index < tablesCount; ++index)
          cout << dbs.TableName (index) << endl;
      }
    else
      {
        do
          {
            token = CmdLineNextToken (cmdLine, linePos);

            ITable& table = dbs.RetrievePersistentTable (token.c_str ());

            const FIELD_INDEX fieldCount = table.FieldsCount();

            cout << "Table '"<< token << "' fields description:\n";

            for (FIELD_INDEX index = 0; index < fieldCount; ++index)
              {
                DBSFieldDescriptor desc = table.DescribeField (index);

                print_field_desc (desc, table.IsIndexed (index));
              }

            dbs.ReleaseTable (table);
          }
        while (linePos < cmdLine.length());
      }
  }
  catch (const Exception& e)
  {
    printException (cerr, e);

    result = false;
  }

  return result;
}


static void
create_index_call_back (CreateIndexCallbackContext* cbContext)
{
  if (cbContext->mRowsCount == 0)
    {
      cout << "100%";
      return;
    }

  if (((cbContext->mRowIndex * 100) % cbContext->mRowsCount) == 0)
    {
      cout << '\r';
      cout <<(cbContext->mRowIndex * 100) / cbContext->mRowsCount << '%';

      cout.flush();
    }
}


static bool
cmdTableAddIndex (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  IDBSHandler&         dbs     = *_RC (IDBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel();
  ITable*              table   = NULL;
  bool                 result  = true;

  assert (token == "index");

  if (linePos >= cmdLine.length())
    goto invalid_args;

  try
  {
    token = CmdLineNextToken (cmdLine, linePos);
    table = &dbs.RetrievePersistentTable (token.c_str ());
  }
  catch (const Exception& e)
  {
    if (level >= VL_INFO)
      cerr << "Failed to open table '" << token << "'.\n";

    printException (cerr, e);

    return false;
  }


  if (linePos >= cmdLine.length())
    goto invalid_args;

  do
    {
      try
      {
        token = CmdLineNextToken (cmdLine, linePos);

        const FIELD_INDEX field = table->RetrieveField (token.c_str ());

        if ( ! table->IsIndexed (field))
          {
            if (level >= VL_INFO)
              {
                CreateIndexCallbackContext context;

                table->CreateIndex (field, create_index_call_back, &context);

                cout << endl;
              }
            else
              table->CreateIndex (field, NULL, NULL);
          }
      }
      catch (const Exception& e)
      {
        if (level >= VL_INFO)
          cerr << "Failed to create index for field '" << token << "'.\n";

        printException (cerr, e);

        result = false;
        break;
      }
    }
  while (linePos < cmdLine.length());

  dbs.ReleaseTable (*table);

  return result;

invalid_args:

  if (table)
    dbs.ReleaseTable (*table);

  if (level >= VL_ERROR)
    cerr << "Invalid commands arguments.\n";

  return false;
}


static bool
cmdTableRmIndex (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  IDBSHandler&         dbs     = *_RC (IDBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel();
  ITable*              table   = NULL;
  bool                 result  = true;

  assert (token == "rmindex");

  if (linePos >= cmdLine.length())
    goto invalid_args;

  try
  {
    token = CmdLineNextToken (cmdLine, linePos);
    table = &dbs.RetrievePersistentTable (token.c_str ());
  }
  catch (const Exception& e)
  {
    if (level >= VL_INFO)
      cerr << "Failed to open table '" << token << "'.\n";

    printException (cout, e);

    return false;
  }

  if (linePos >= cmdLine.length())
    goto invalid_args;

  do
    {
      try
      {
        token = CmdLineNextToken (cmdLine, linePos);

        const FIELD_INDEX field = table->RetrieveField (token.c_str ());

        if (table->IsIndexed (field))
          {
            if (level >= VL_INFO)
              cout << "Removing index for '" << token << "'.\n";

            table->RemoveIndex (field);
          }
        else
          if (level > VL_INFO)
            cout << "Ignoring field '" << token << "'.\n";
      }
      catch (const Exception& e)
      {
        if (level >= VL_INFO)
          cerr << "Failed to remove index for field '" << token << "'.\n";

        printException (cerr, e);

        result = false;
        break;
      }
    }
  while (linePos < cmdLine.length());

  dbs.ReleaseTable (*table);

  return result;

invalid_args:

  if (table)
    dbs.ReleaseTable (*table);

  if (level >= VL_ERROR)
    cerr << "Invalid commands arguments.\n";

  return false;
}



struct
TableFieldParameter
{
  DBSFieldDescriptor mDesc;
  FIELD_INDEX        mField;
};



static bool
parse_list_table_fields (ITable&                      table,
                         const string&                cmdLine,
                         size_t&                      inoutLinePos,
                         vector<TableFieldParameter>& outParams)
{
  const string fieldSpec = NextToken (cmdLine, inoutLinePos, sArgsDelimiter);

  outParams.clear ();
  if ((fieldSpec.length() == 0) || (fieldSpec == "*"))
    {

      for (FIELD_INDEX i = 0; i < table.FieldsCount(); ++i)
        {
          TableFieldParameter field;

          field.mField = i;
          field.mDesc  = table.DescribeField (i);

          outParams.push_back (field);
        }
    }
  else
    {
      size_t fieldSpecPos = 0;

      while (fieldSpecPos < fieldSpec.length())
        {
          const string fieldName = NextToken (fieldSpec,
                                              fieldSpecPos,
                                              sListDelimiter);
          if (fieldName.length() == 0)
            break;

          TableFieldParameter field;

          field.mField = table.RetrieveField (fieldName.c_str ());
          field.mDesc  = table.DescribeField (field.mField);

          outParams.push_back (field);
        }
    }

    assert (outParams.size()  > 0);

    return true;
}


static bool
cmdFieldsMgm (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  IDBSHandler&         dbs     = *_RC (IDBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel();

  assert (token == "alter");

  if (linePos >= cmdLine.length())
    goto invalid_args;

  try
  {
    const string tableName = CmdLineNextToken (cmdLine, linePos);
    if ((tableName.length () == 0) || (linePos >= cmdLine.length ()))
      goto invalid_args;

    TableAlterRules rules (dbs, tableName, vector<string> ());

    string subCommand;

    if (level >= VL_INFO)
      cout << "Alter table '" << tableName << "' fields:";

    while ((subCommand = NextToken (cmdLine, linePos, ",")).length () > 0)
      {
        size_t subCmdPos = 0;

        while (subCmdPos < subCommand.length ())
          {
            token = CmdLineNextToken (subCommand, subCmdPos);
            if (token == "add")
              {
                if (level >= VL_INFO)
                  cout << "\n..add fields: ";

                DBS_FIELD_TYPE type;
                bool_t         isArray;

                do
                  {
                    token = CmdLineNextToken (subCommand, subCmdPos);
                    if (token.length () == 0)
                      break;

                    if ( ! parse_field_type (subCommand,
                                             &subCmdPos,
                                             &type,
                                             &isArray))
                      {
                        goto invalid_args;
                      }

                    if (level >= VL_INFO)
                      cout << token << ' ';

                    rules.AddField (token, type, isArray);
                  }
                while (subCmdPos < subCommand.length ());
              }
            else if (token == "retype")
              {
                if (level >= VL_INFO)
                  cout << "\n...retype fields: ";

                DBS_FIELD_TYPE type;
                bool_t         isArray;

                do
                  {
                    token = CmdLineNextToken (subCommand, subCmdPos);
                    if (token.length () == 0)
                      break;

                    if ( ! parse_field_type (subCommand,
                                             &subCmdPos,
                                             &type,
                                             &isArray))
                      {
                        goto invalid_args;
                      }

                    if (level >= VL_INFO)
                      cout << token << ' ';

                    rules.RetypeField (token, type, isArray);
                  }
                while (subCmdPos < subCommand.length ());
              }
            else if (token == "rename")
              {
                if (level >= VL_INFO)
                  cout << "\n...rename fields: ";

                do
                  {
                    const string oldName = CmdLineNextToken (subCommand,
                                                             subCmdPos);
                    if (oldName.length() == 0)
                      break;

                    const string newName = CmdLineNextToken (subCommand,
                                                             subCmdPos);

                    if (level >= VL_INFO)
                      cout << oldName << " --> " << newName << ' ';

                    rules.RenameField (oldName, newName);
                  }
                while (subCmdPos < subCommand.length ());
              }
            else if (token == "remove")
              {
                if (level >= VL_INFO)
                  cout << "\n...remove fields: ";

                do
                  {
                    token = CmdLineNextToken (subCommand, subCmdPos);
                    if (token.length() == 0)
                      break ;

                    if (level >= VL_INFO)
                      cout << token << ' ';

                    rules.DropField (token);
                  }
                while (subCmdPos < subCommand.length ());
              }
            else
              {
                cerr << "Unknown fields sub command '" << token << "'\n";
                return false;
              }
          }
      }

    rules.Commit ();

    if (level >= VL_INFO)
      cout << "\nDone!\n";
  }
  catch (const Exception& e)
  {
      printException (cerr, e);
      return false;
  }

  return true;

invalid_args:

  cerr << "Invalid commands arguments.\n";

  return false;
}



static bool
cmdRowsMgm (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  IDBSHandler&         dbs     = *_RC (IDBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel();
  ITable*              table   = NULL;

  assert (token == "rows");

  if (linePos >= cmdLine.length())
    goto invalid_args;

  try
  {
    token = CmdLineNextToken (cmdLine, linePos);
    table = &dbs.RetrievePersistentTable (token.c_str ());
  }
  catch (const Exception& e)
  {
    assert (table == NULL);

    if (level >= VL_INFO)
      cerr << "Failed to open table: " << token << endl;

    printException (cerr, e);

    return false;
  }

  try
  {
    token = CmdLineNextToken (cmdLine, linePos);
    if ((token == "info") || (token.length() == 0))
      {
        const ROW_INDEX firstReusableRow  = table->GetReusableRow (false);
        const ROW_INDEX reusableRowsCount = table->ReusableRowsCount();
        const ROW_INDEX rowsCount         = table->AllocatedRows();

        cout << "Allocated rows    : " << rowsCount << endl;
        cout << "Reusable rows     : " << reusableRowsCount << endl;
        cout << "First reusable row: "
             << ((firstReusableRow == INVALID_ROW_INDEX) ?
                   rowsCount :
                   firstReusableRow)
            << endl;
      }
    else if (token == "list")
      {
        vector<TableFieldParameter> fields;
        RowsSelection               rows;

        if (! parse_list_table_fields (*table, cmdLine, linePos, fields))
          goto invalid_args;

        assert (linePos <= cmdLine.length());
        if (! ParseRowsSelectionClause (&cerr,
                                        *table,
                                        cmdLine.c_str () + linePos,
                                        rows))
          {
            dbs.ReleaseTable (*table);
            return false;
          }

        const WTICKS matchBegin = wh_msec_ticks();

        MatchSelectedRows (*table, rows);

        const WTICKS matchEnd = wh_msec_ticks();

        for (size_t r = 0; r < rows.mRows.mIntervals.size(); ++r)
          {
            const Interval<ROW_INDEX>& intv = rows.mRows.mIntervals[r];

            for (ROW_INDEX row = intv.mFrom; row <= intv.mTo; ++row)
              {
                for (size_t f = 0; f < fields.size(); ++f)
                  {
                    cout << row << ':' << fields[f].mDesc.name << ':';
                    PrintFieldValue (cout, *table, row, fields[f].mField);
                    cout << endl;
                  }
                cout << endl;
              }
          }

        print_match_statistic (cout, *table, rows.mRows, matchBegin, matchEnd);
      }
    else if (token == "update")
      {
        size_t                    temp;
        vector<FieldValuesUpdate> fieldVals;
        RowsSelection             rows;

        if (! ParseFieldUpdateValues (&cerr,
                                      *table,
                                      cmdLine.c_str () + linePos,
                                      &temp,
                                      fieldVals))
          {
            goto invalid_args;
          }

        linePos += temp;
        assert (linePos <= cmdLine.length());
        if (! ParseRowsSelectionClause (&cerr,
                                        *table,
                                        cmdLine.c_str () + linePos,
                                        rows))
          {
            dbs.ReleaseTable (*table);
            return false;
          }

        const WTICKS matchBegin = wh_msec_ticks();

        MatchSelectedRows (*table, rows);

        const WTICKS matchEnd = wh_msec_ticks();

        print_match_statistic (cout, *table, rows.mRows, matchBegin, matchEnd);

        for (size_t r = 0; r < rows.mRows.mIntervals.size(); ++r)
          {
            const Interval<ROW_INDEX>& intv = rows.mRows.mIntervals[r];

            for (ROW_INDEX row = intv.mFrom; row <= intv.mTo; ++row)
              {
                for (size_t f = 0; f < fieldVals.size(); ++f)
                  {
                    if (! UpdateTableRow (&cerr, *table, row, fieldVals))
                      goto invalid_args;
                  }
              }
          }
      }
    else if (token == "add")
      {
        size_t                    temp;
        vector<FieldValuesUpdate> fieldVals;

        if (! ParseFieldUpdateValues (&cerr,
                                      *table,
                                      cmdLine.c_str () + linePos,
                                      &temp,
                                      fieldVals))
          {
            goto invalid_args;
          }

        const ROW_INDEX row = table->AllocatedRows();
        if (! UpdateTableRow (&cerr, *table, row, fieldVals))
          return false;

        if (level >= VL_INFO)
          cout << "Row added at index " << row << ".\n";
      }
    else if (token == "reuse")
      {
        size_t                    temp;
        vector<FieldValuesUpdate> fieldVals;

        if (! ParseFieldUpdateValues (&cerr,
                                      *table,
                                      cmdLine.c_str () + linePos,
                                      &temp,
                                      fieldVals))
          {
            goto invalid_args;
          }

        const ROW_INDEX row = table->GetReusableRow (true);
        if (! UpdateTableRow (&cerr, *table, row, fieldVals))
          return false;

        if (level >= VL_INFO)
          cout << "Row added at index " << row << ".\n";
      }
    else if (token == "remove")
      {
        RowsSelection rows;

        if ( ! ParseRowsSelectionClause (&cerr,
                                         *table,
                                         cmdLine.c_str () + linePos,
                                         rows))
          {
            dbs.ReleaseTable (*table);
            return false;
          }

        const WTICKS matchBegin = wh_msec_ticks();

        MatchSelectedRows (*table, rows);

        const WTICKS matchEnd = wh_msec_ticks();

        print_match_statistic (cout, *table, rows.mRows, matchBegin, matchEnd);


        const size_t rowIntervals = rows.mRows.mIntervals.size();
        for (size_t rowI = 0; rowI < rowIntervals; ++rowI)
          {
            const Interval<ROW_INDEX>& r = rows.mRows.mIntervals[rowI];

            for (ROW_INDEX row = r.mFrom; row <= r.mTo; ++row)
              table->MarkRowForReuse (row);
          }
      }
    else
      {
        if (table)
          dbs.ReleaseTable (*table);

        cerr << "Unexpected token '" << token << "'.\n";

        return false;
      }
  }
  catch (const Exception& e)
  {
    if (level >= VL_INFO)
      cerr << "Failed to open table: " << token << endl;

    printException (cerr, e);

    if (table)
      dbs.ReleaseTable (*table);

    return false;
  }

  if (table)
    dbs.ReleaseTable (*table);

  return true;

invalid_args:

  if (table)
    dbs.ReleaseTable (*table);

  cerr << "Invalid commands arguments.\n";

  return false;

}


void
AddOfflineTableCommands()
{
  CmdEntry entry;

  entry.mContext = &(GetDBSHandler ());

  entry.mShowStatus   = false;
  entry.mName         = "table";
  entry.mDesc         = tableShowDesc;
  entry.mExtendedDesc = tableShowDescExt;
  entry.mCmd          = cmdTablePrint;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "add";
  entry.mDesc         = tableAddDesc;
  entry.mExtendedDesc = tableAddDescExt;
  entry.mCmd          = cmdTableAdd;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "copy";
  entry.mDesc         = tableCopyDesc;
  entry.mExtendedDesc = tableCopeDescExt;
  entry.mCmd          = cmdTableCopy;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "remove";
  entry.mDesc         = tableRmDesc;
  entry.mExtendedDesc = tableRmDescExt;
  entry.mCmd          = cmdTableRemove;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "index";
  entry.mDesc         = tableAddIndDesc;
  entry.mExtendedDesc = tableAddIndDescExt;
  entry.mCmd          = cmdTableAddIndex;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "rmindex";
  entry.mDesc         = tableRmIndDesc;
  entry.mExtendedDesc = tableRmIndDescExt;
  entry.mCmd          = cmdTableRmIndex;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "alter";
  entry.mDesc         = fieldsDesc;
  entry.mExtendedDesc = fieldsDescExt;
  entry.mCmd          = cmdFieldsMgm;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "rows";
  entry.mDesc         = rowsDesc;
  entry.mExtendedDesc = rowsDescExt;
  entry.mCmd          = cmdRowsMgm;

  RegisterCommand (entry);
}

