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

#include "whais.h"
#include "whais_time.h"

#include "utils/range.h"
#include "utils/tokenizer.h"
#include "dbs/dbs_mgr.h"

#include "wcmd_tabcomds.h"
#include "wcmd_cmdsmgr.h"
#include "wcmd_optglbs.h"
#include "wcmd_valparser.h"


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
  "  table_remove table_name [second_table_name ...] ";

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
  "  table_add table_name field_specifier [second_field_specifier] ...\n"
  "Example:\n"
  "  table_add SomeTableName fieldName DATE otherFieldname ARRAY INT8";

static const char tableAddIndDesc[]    = "Index the specified field tables.";
static const char tableAddIndDescExt[] =
  "Index the values of the specified table fields for faster searching.\n"
  "Currently it does not support to index array nor text field types.\n"
  "Usage:\n"
  "  table_index table_name field_name [second_field_name ...]";

static const char tableRmIndDesc[]    = "Remove the index associated with some"
                                        " table fields.";
static const char tableRmIndDescExt[] =
  "Remove the associated index of the specified table fields.\n"
  "Usage:\n"
  "  table_rmindex table_name field_name [second_field_name ...]";

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
  "           Will remove all rows that have field1 is null, field2 is set\n"
  "           to minimum and field3 is set to maximum.\n"
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
  IDBSHandler&               dbs     = *_RC (IDBSHandler*, context);
  size_t                     linePos = 0;
  string                     token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL       level   = GetVerbosityLevel();
  vector<DBSFieldDescriptor> fields;
  vector<string>             fieldsNames;
  string                     tableName;
  bool                       result   = true;


  assert (token == "table_add");

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

      if (linePos >= cmdLine.length())
        goto invalid_args;

      //The filed type
      token = CmdLineNextToken (cmdLine, linePos);

      if (token == "ARRAY")
        {
          if (linePos >= cmdLine.length())
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
        {
          cerr << "Token '" << token << "' is not a valid type.\n";
          return false;
        }

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
cmdTableRemove (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  IDBSHandler&         dbs     = *_RC (IDBSHandler*, context);
  size_t               linePos = 0;
  string               token   = CmdLineNextToken (cmdLine, linePos);
  const  VERBOSE_LEVEL level   = GetVerbosityLevel();
  bool                 result  = false;

  assert (token == "table_remove");

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

  assert (token == "table_index");

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

  assert (token == "table_rmindex");

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

  if (GetVerbosityLevel() >= VL_INFO)
    return;

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
            goto invalid_args;
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
            goto invalid_args;
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
        RowsSelection select;

        if ( ! ParseRowsSelectionClause (&cerr,
                                         *table,
                                         cmdLine.c_str () + linePos,
                                         select))
          {
            goto invalid_args;
          }

        MatchSelectedRows (*table, select);

        const size_t rowIntervals = select.mRows.mIntervals.size();
        for (size_t rowI = 0; rowI < rowIntervals; ++rowI)
          {
            const Interval<ROW_INDEX>& r = select.mRows.mIntervals[rowI];

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
  entry.mName         = "table_add";
  entry.mDesc         = tableAddDesc;
  entry.mExtendedDesc = tableAddDescExt;
  entry.mCmd          = cmdTableAdd;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "table_remove";
  entry.mDesc         = tableRmDesc;
  entry.mExtendedDesc = tableRmDescExt;
  entry.mCmd          = cmdTableRemove;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "table_index";
  entry.mDesc         = tableAddIndDesc;
  entry.mExtendedDesc = tableAddIndDescExt;
  entry.mCmd          = cmdTableAddIndex;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "table_rmindex";
  entry.mDesc         = tableRmIndDesc;
  entry.mExtendedDesc = tableRmIndDescExt;
  entry.mCmd          = cmdTableRmIndex;

  RegisterCommand (entry);

  entry.mShowStatus   = true;
  entry.mName         = "rows";
  entry.mDesc         = rowsDesc;
  entry.mExtendedDesc = rowsDescExt;
  entry.mCmd          = cmdRowsMgm;

  RegisterCommand (entry);
}

