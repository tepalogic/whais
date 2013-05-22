/******************************************************************************
  WCMD - An utility to manage whisper database files.
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
#include <vector>

#include "whisper.h"

#include "dbs/dbs_mgr.h"

#include "wcmd_tabcomds.h"
#include "wcmd_cmdsmgr.h"
#include "wcmd_optglbs.h"

using namespace std;
using namespace whisper;

static const char tableShowDesc[]    = "Describe database's tables.";
static const char tableShowDescEXt[] =
"Show table information. If no table name is provided it prints the list of\n"
"tables from the current database.\n"
"If a table name is provided, it prints the table's fields information.\n"
"Usage:\n"
"  table [table_name] ... ";

static const char tableRmDesc[]      = "Remove tables from the database.";
static const char tableRmDescExt[ ]  =
"Remove the specified tables from the database.\n"
"Usage:\n"
"  table_remove table_name ... ";

static const char tableAddDesc[]     = "Add table to the database.";
static const char tableAddDescExt[]  =
"Add a table to database with by specifying table name,\n"
"fields names and fields types.\n"
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
"  table_add table_name [field_specifier] ...\n"
"Example:\n"
"  table_add SomeTableName fieldName DATE otherFieldname ARRAY INT8";

static const char tableAddIndDesc[]    = "Create table field index.";
static const char tableAddIndDescExt[] =
"For the specified fields creates indexes.\n"
"Usage:\n"
"  table_index table_name field_name ...";

static const char tableRmIndDesc[]    = "Remove table field index.";
static const char tableRmIndDescExt[] =
"For the specified fields remove their indexes.\n"
"Usage:\n"
"  table_rmindex table_name field_name ...";


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
cmdTableAdd (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  I_DBSHandler&              dbsHnd  = *_RC (I_DBSHandler*, context);
  size_t                     linePos = 0;
  string                     token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL       level   = GetVerbosityLevel ();
  vector<DBSFieldDescriptor> fields;
  vector<string>             fieldsNames;
  string                     tableName;
  bool                       result   = true;


  assert (token == "table_add");

  if (linePos >= cmdLine.length ())
    goto invalid_args;

  tableName = CmdLineNextToken (cmdLine, linePos);

  if (level > VL_INFO)
    cout << "Adding table " << tableName << "...\n";

  if (linePos >= cmdLine.length ())
    goto invalid_args;

  do
    {
      DBSFieldDescriptor desc = {NULL, };

      //The field name
      token = CmdLineNextToken (cmdLine, linePos);
      assert (token.length () > 0);

      fieldsNames.push_back (token);
      desc.name = fieldsNames.back ().c_str ();

      if (linePos >= cmdLine.length ())
        goto invalid_args;

      //The filed type
      token = CmdLineNextToken (cmdLine, linePos);

      if (token == "ARRAY")
        {
          if (linePos >= cmdLine.length ())
            goto invalid_args;

          desc.isArray = true;
          token        = CmdLineNextToken (cmdLine, linePos);
        }

      if (token == "BOOL")
        desc.type = T_BOOL;
      else if (token == "CHAR")
        desc.type = T_CHAR;
      else if (token == "DATE")
        desc.type = T_DATE;
      else if (token == "DATETIME")
        desc.type = T_DATETIME;
      else if (token == "HIRESTIME")
        desc.type = T_HIRESTIME;
      else if (token == "INT8")
        desc.type = T_INT8;
      else if (token == "INT16")
        desc.type = T_INT16;
      else if (token == "INT32")
        desc.type = T_INT32;
      else if (token == "INT64")
        desc.type = T_INT64;
      else if (token == "UINT8")
        desc.type = T_UINT8;
      else if (token == "UINT16")
        desc.type = T_UINT16;
      else if (token == "UINT32")
        desc.type = T_UINT32;
      else if (token == "UINT64")
        desc.type = T_UINT64;
      else if (token == "REAL")
        desc.type = T_REAL;
      else if (token == "RICHREAL")
        desc.type = T_RICHREAL;
      else if (token == "RICHREAL")
        desc.type = T_RICHREAL;
      else if (token == "TEXT")
        desc.type = T_TEXT;
      else
        goto invalid_args;

      fields.push_back (desc);

      if (level >= VL_INFO)
        {
          cout << " ... ";
          print_field_desc (desc, false);
        }

      assert (fields.size () == fieldsNames.size ());
    }
  while (linePos < cmdLine.length ());

  try
  {
    dbsHnd.AddTable (tableName.c_str (), fields.size (), &(fields.front ()));
  }
  catch (const Exception& e)
  {
    if (level >= VL_ERROR)
      cout << "Failed to add table: " << tableName << endl;

    printException (cout, e);

    result =  false;
  }

  return result;

invalid_args:
  if (level >= VL_ERROR)
    cout << "Invalid commands arguments.\n";

  return false;
}

static bool
cmdTableRemove (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  I_DBSHandler&        dbsHnd  = *_RC (I_DBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel ();
  bool                 result  = true;

  assert (token == "table_remove");
  if (linePos >= cmdLine.length ())
    goto invalid_args;

  if (level > VL_INFO)
    cout << "Removing tables\n";

  do
    {
      try
      {
        token = CmdLineNextToken (cmdLine, linePos);
        dbsHnd.DeleteTable (token.c_str ());

        if (level >= VL_INFO)
          cout << "  ... " << token << endl;
      }
      catch (const Exception& e)
      {
        if (level >= VL_ERROR)
          cout << "Failed to remove table: " << token << endl;

        printException (cout, e);

        result =  false;
        break;
      }
    }
  while (linePos < cmdLine.length ());

  return result;

invalid_args:
  if (level >= VL_ERROR)
    cout << "Invalid commands arguments.\n";

  return false;
}


static bool
cmdTablePrint (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  I_DBSHandler&        dbsHnd  = *_RC (I_DBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel ();
  bool                 result  = true;

  assert (token == "table");

  try
  {
    if (linePos >= cmdLine.length ())
      {
        const TABLE_INDEX tablesCount = dbsHnd.PersistentTablesCount ();

        if (level >= VL_INFO)
          cout << "Retrieved " << tablesCount << " persistent tables.\n";

        for (TABLE_INDEX index = 0; index < tablesCount; ++index)
          cout << dbsHnd.TableName (index) << endl;
      }
    else
      {
        do
          {
            token = CmdLineNextToken (cmdLine, linePos);

            ITable&       table      = dbsHnd.RetrievePersistentTable (
                                                                token.c_str ());
            const FIELD_INDEX fieldCount = table.FieldsCount ();

            cout << "Table '"<< token << "' fields description:\n";

            for (FIELD_INDEX index = 0; index < fieldCount; ++index)
              {
                DBSFieldDescriptor desc = table.DescribeField (index);
                print_field_desc (desc, table.IsIndexed (index));
              }

            dbsHnd.ReleaseTable (table);

          }
        while (linePos < cmdLine.length ());
      }
  }
  catch (const Exception& e)
  {
    printException (cout, e);

    result = false;
  }

  return result;
}

static void
create_index_call_back (CreateIndexCallbackContext* cbData)
{
  if (cbData->mRowsCount == 0)
    {
      cout << "100%";
      return;
    }

  if (((cbData->mRowIndex * 100) % cbData->mRowsCount) == 0)
    {
      cout << '\r' << (cbData->mRowIndex * 100) / cbData->mRowsCount << '%';
      cout.flush ();
    }
}

static bool
cmdTableAddIndex (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  I_DBSHandler&        dbsHnd  = *_RC (I_DBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel ();
  ITable*          pTable  = NULL;
  bool                 result  = true;

  assert (token == "table_index");

  if (linePos >= cmdLine.length ())
    goto invalid_args;

  try
  {
    token  = CmdLineNextToken (cmdLine, linePos);
    pTable = &dbsHnd.RetrievePersistentTable (token.c_str ());
  }
  catch (const Exception& e)
  {
    if (level >= VL_INFO)
      cout << "Failed to open table: " << token << endl;

    printException (cout, e);

    return false;
  }


  if (linePos >= cmdLine.length ())
    goto invalid_args;

  do
    {
      try
      {
        token = CmdLineNextToken (cmdLine, linePos);

        const FIELD_INDEX field = pTable->RetrieveField (token.c_str ());

        if ( ! pTable->IsIndexed (field))
          {
            if (level >= VL_INFO)
              {
                CreateIndexCallbackContext data;
                pTable->CreateIndex (field,
                                          create_index_call_back,
                                          &data);
                cout << endl;
              }
            else
              pTable->CreateIndex (field, NULL, NULL);
          }
      }
      catch (const Exception& e)
      {
        if (level >= VL_INFO)
          cout << "Failed to create index for field: " << token << endl;

        printException (cout, e);

        result = false;
        break;
      }
    }
  while (linePos < cmdLine.length ());

  dbsHnd.ReleaseTable (*pTable);

  return result;

invalid_args:

  if (pTable)
    dbsHnd.ReleaseTable (*pTable);

  if (level >= VL_ERROR)
    cout << "Invalid commands arguments.\n";

  return false;
}

static bool
cmdTableRmIndex (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  I_DBSHandler&        dbsHnd  = *_RC (I_DBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel ();
  ITable*          pTable  = NULL;
  bool                 result  = true;

  assert (token == "table_rmindex");

  if (linePos >= cmdLine.length ())
    goto invalid_args;

  try
  {
    token  = CmdLineNextToken (cmdLine, linePos);
    pTable = &dbsHnd.RetrievePersistentTable (token.c_str ());
  }
  catch (const Exception& e)
  {
    if (level >= VL_INFO)
      cout << "Failed to open table: " << token << endl;

    printException (cout, e);

    return false;
  }

  if (linePos >= cmdLine.length ())
    goto invalid_args;

  do
    {
      try
      {
        token = CmdLineNextToken (cmdLine, linePos);

        const FIELD_INDEX field = pTable->RetrieveField (token.c_str ());

        if (pTable->IsIndexed (field))
          {
            if (level >= VL_INFO)
              cout << "Removing index for " << token << ".\n";

            pTable->RemoveIndex (field);
          }
        else
          if (level > VL_INFO)
            cout << "Ignoring field " << token << ".\n";
      }
      catch (const Exception& e)
      {
        if (level >= VL_INFO)
          cout << "Failed to renove index for field: " << token << endl;

        printException (cout, e);

        result = false;
        break;
      }
    }
  while (linePos < cmdLine.length ());

  dbsHnd.ReleaseTable (*pTable);

  return result;

invalid_args:

  if (pTable)
    dbsHnd.ReleaseTable (*pTable);

  if (level >= VL_ERROR)
    cout << "Invalid commands arguments.\n";

  return false;
}

void
AddOfflineTableCommands ()
{
  CmdEntry entry;

  entry.mcontext = &(GetDBSHandler ());

  entry.mshowStatus   = false;
  entry.mpCmdText     = "table";
  entry.mpCmdDesc     = tableShowDesc;
  entry.mpExtHelpDesc = tableShowDescEXt;
  entry.mcmd          = cmdTablePrint;

  RegisterCommand (entry);

  entry.mshowStatus   = true;
  entry.mpCmdText     = "table_add";
  entry.mpCmdDesc     = tableAddDesc;
  entry.mpExtHelpDesc = tableAddDescExt;
  entry.mcmd          = cmdTableAdd;

  RegisterCommand (entry);

  entry.mshowStatus   = true;
  entry.mpCmdText     = "table_remove";
  entry.mpCmdDesc     = tableRmDesc;
  entry.mpExtHelpDesc = tableRmDescExt;
  entry.mcmd          = cmdTableRemove;

  RegisterCommand (entry);

  entry.mshowStatus   = true;
  entry.mpCmdText     = "table_index";
  entry.mpCmdDesc     = tableAddIndDesc;
  entry.mpExtHelpDesc = tableAddIndDescExt;
  entry.mcmd          = cmdTableAddIndex;

  RegisterCommand (entry);

  entry.mshowStatus   = true;
  entry.mpCmdText     = "table_rmindex";
  entry.mpCmdDesc     = tableRmIndDesc;
  entry.mpExtHelpDesc = tableRmIndDescExt;
  entry.mcmd          = cmdTableRmIndex;

  RegisterCommand (entry);
}

