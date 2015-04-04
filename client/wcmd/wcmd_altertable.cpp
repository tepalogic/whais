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

#include <memory>
#include <cstring>

#include "wcmd_altertable.h"

#include "interpreter/interpreter.h"
#include "interpreter/operands.h"


using namespace std;
using namespace whais;

typedef void (*FIELD_COPIER) (ITable&           srcTable,
                              ITable&           dstTable,
                              const FIELD_INDEX srcField,
                              const FIELD_INDEX dstField,
                              const ROW_INDEX   srcRow,
                              const ROW_INDEX   dstRow);

struct FIELD_ASSOCIATION
{
  FIELD_INDEX   src;
  FIELD_INDEX   dst;
  FIELD_COPIER  copier;
};

template<typename T> void
copy_field_no_conversion (ITable&           srcTable,
                          ITable&           dstTable,
                          const FIELD_INDEX srcField,
                          const FIELD_INDEX dstField,
                          const ROW_INDEX   srcRow,
                          const ROW_INDEX   dstRow)
{
  T value;

  srcTable.Get (srcRow, srcField, value);

  if (value.IsNull ())
    return ;

  dstTable.Set (dstRow, dstField, value);
}


template<typename T1, typename T2> void
copy_basic_field (ITable&           srcTable,
                  ITable&           dstTable,
                  const FIELD_INDEX srcField,
                  const FIELD_INDEX dstField,
                  const ROW_INDEX   srcRow,
                  const ROW_INDEX   dstRow)
{
  T1 orgValue;

  srcTable.Get (srcRow, srcField, orgValue);

  if (orgValue.IsNull ())
    return ;

  StackValue converter = StackValue::Create (orgValue);

  T2 newValue;

  try
  {
      converter.Operand().GetValue (newValue);
  }
  catch (whais::InterException& e)
  {
      if (e.Code () == whais::InterException::INVALID_OP_REQ)
        e.Message ("Cannot convert field value.");

      throw ;
  }

  dstTable.Set (dstRow, dstField, newValue);
}


template<typename T> void
copy_array_field (ITable&           srcTable,
                  ITable&           dstTable,
                  const FIELD_INDEX srcField,
                  const FIELD_INDEX dstField,
                  const ROW_INDEX   srcRow,
                  const ROW_INDEX   dstRow)
{
  DArray orgValue, newValue;

  srcTable.Get (srcRow, srcField, orgValue);

  StackValue converter = StackValue::Create (orgValue);

  const uint64_t count = orgValue.Count ();

  if (count == 0)
    return ;

  for (uint64_t i = 0; i < count; ++i)
    {
      T element;

      try
      {
          converter.Operand().GetValueAt (i).Operand().GetValue (element);
      }
      catch (whais::InterException& e)
      {
          if (e.Code () == whais::InterException::INVALID_OP_REQ)
            e.Message ("Cannot convert an array field value.");

          throw;
      }

      newValue.Add (element);
    }

  dstTable.Set (dstRow, dstField, newValue);
}

template<typename T> FIELD_COPIER
get_copier_for_second_field (DBS_FIELD_TYPE type)
{
  switch (type)
  {
    case T_BOOL:
      return copy_basic_field<T, DBool>;

    case T_CHAR:
      return copy_basic_field<T, DChar>;

    case T_DATE:
      return copy_basic_field<T, DDate>;

    case T_DATETIME:
      return copy_basic_field<T, DDateTime>;

    case T_HIRESTIME:
      return copy_basic_field<T, DHiresTime>;

    case T_INT8:
      return copy_basic_field<T, DInt8>;

    case T_INT16:
      return copy_basic_field<T, DInt16>;

    case T_INT32:
      return copy_basic_field<T, DInt32>;

    case T_INT64:
      return copy_basic_field<T, DInt64>;

    case T_UINT8:
      return copy_basic_field<T, DUInt8>;

    case T_UINT16:
      return copy_basic_field<T, DUInt16>;

    case T_UINT32:
      return copy_basic_field<T, DUInt32>;

    case T_UINT64:
      return copy_basic_field<T, DUInt64>;

    case T_REAL:
      return copy_basic_field<T, DReal>;

    case T_RICHREAL:
      return copy_basic_field<T, DRichReal>;

    case T_TEXT:
      return copy_basic_field<T, DText>;

    default:
      throw DBSException (
          _EXTRA (DBSException::GENERAL_CONTROL_ERROR)
                         );
  }
}



static FIELD_COPIER
get_field_copier (const DBS_FIELD_TYPE      srcType,
                  const bool                isSrcArray,
                  const DBS_FIELD_TYPE      dstType,
                  const bool                isDstArray)
{
  if (isSrcArray != isDstArray)
    {
      throw InterException (_EXTRA (InterException::INVALID_OP_REQ),
                            "Cannot convert an array to a a basic field value "
                              "(nor the other way around).");
    }
  else if (isSrcArray)
    {
      if (srcType == dstType)
        {
          return copy_field_no_conversion<DArray>;
        }
      else
        {
          switch (dstType)
          {
            case T_BOOL:
              return copy_array_field<DBool>;

            case T_CHAR:
              return copy_array_field<DChar>;

            case T_DATE:
              return copy_array_field<DDate>;

            case T_DATETIME:
              return copy_array_field<DDateTime>;

            case T_HIRESTIME:
              return copy_array_field<DHiresTime>;

            case T_INT8:
              return copy_array_field<DInt8>;

            case T_INT16:
              return copy_array_field<DInt16>;

            case T_INT32:
              return copy_array_field<DInt32>;

            case T_INT64:
              return copy_array_field<DInt64>;

            case T_UINT8:
              return copy_array_field<DUInt8>;

            case T_UINT16:
              return copy_array_field<DUInt16>;
              break;

            case T_UINT32:
              return copy_array_field<DUInt32>;

            case T_UINT64:
              return copy_array_field<DUInt64>;

            case T_REAL:
              return copy_array_field<DReal>;

            case T_RICHREAL:
              return copy_array_field<DRichReal>;

            default:
              throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
          }
        }
    }
  else if (srcType == dstType)
    {
      switch (dstType)
      {
        case T_BOOL:
          return copy_field_no_conversion<DBool>;

        case T_CHAR:
          return copy_field_no_conversion<DChar>;

        case T_DATE:
          return copy_field_no_conversion<DDate>;

        case T_DATETIME:
          return copy_field_no_conversion<DDateTime>;

        case T_HIRESTIME:
          return copy_field_no_conversion<DHiresTime>;

        case T_INT8:
          return copy_field_no_conversion<DInt8>;

        case T_INT16:
          return copy_field_no_conversion<DInt16>;

        case T_INT32:
          return copy_field_no_conversion<DInt32>;

        case T_INT64:
          return copy_field_no_conversion<DInt64>;

        case T_UINT8:
          return copy_field_no_conversion<DUInt8>;

        case T_UINT16:
          return copy_field_no_conversion<DUInt16>;

        case T_UINT32:
          return copy_field_no_conversion<DUInt32>;

        case T_UINT64:
          return copy_field_no_conversion<DUInt64>;

        case T_REAL:
          return copy_field_no_conversion<DReal>;

        case T_RICHREAL:
          return copy_field_no_conversion<DRichReal>;

        case T_TEXT:
          return copy_field_no_conversion<DText>;

        default:
          throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
      }
    }
  else
    {
      switch (srcType)
      {
        case T_BOOL:
          return get_copier_for_second_field<DBool> (dstType);

        case T_CHAR:
          return get_copier_for_second_field<DChar> (dstType);

        case T_DATE:
          return get_copier_for_second_field<DDate> (dstType);

        case T_DATETIME:
          return get_copier_for_second_field<DDateTime> (dstType);

        case T_HIRESTIME:
          return get_copier_for_second_field<DHiresTime> (dstType);

        case T_INT8:
          return get_copier_for_second_field<DInt8> (dstType);

        case T_INT16:
          return get_copier_for_second_field<DInt16> (dstType);

        case T_INT32:
          return get_copier_for_second_field<DInt32> (dstType);

        case T_INT64:
          return get_copier_for_second_field<DInt64> (dstType);

        case T_UINT8:
          return get_copier_for_second_field<DUInt8> (dstType);

        case T_UINT16:
          return get_copier_for_second_field<DUInt16> (dstType);

        case T_UINT32:
          return get_copier_for_second_field<DUInt32> (dstType);

        case T_UINT64:
          return get_copier_for_second_field<DUInt64> (dstType);

        case T_REAL:
          return get_copier_for_second_field<DReal> (dstType);

        case T_RICHREAL:
          return get_copier_for_second_field<DRichReal> (dstType);

        case T_TEXT:
          return get_copier_for_second_field<DText> (dstType);

        default:
          throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
      }
    }
}


TableAlterRules::TableAlterRules (IDBSHandler&               dbs,
                                  const string&              table,
                                  const std::vector<string>& fields)
  : mDbs (dbs),
    mTableName (table),
    mTable (&dbs.RetrievePersistentTable (table.c_str ()))
{
  for (size_t f = 0; f < fields.size (); ++f)
    {
      mSrcFields.push_back (mTable->DescribeField (
                                mTable->RetrieveField (fields[f].c_str ())
                                                  ));
    }

  if (fields.size () == 0)
    {
      const FIELD_INDEX fieldsCount = mTable->FieldsCount ();

      for (FIELD_INDEX f = 0; f < fieldsCount; ++f)
        mSrcFields.push_back (mTable->DescribeField (f));
    }

  for (size_t f = 0; f < mSrcFields.size (); ++f)
    {
      mDstFields.push_back (mSrcFields[f]);
      mDstFields[f].name = NULL;

      mDstFields[f].name = new char[strlen (mSrcFields[f].name) + 1];
      strcpy (_CC (char*&, mDstFields[f].name), mSrcFields[f].name);
    }
}


TableAlterRules::TableAlterRules (IDBSHandler&  dbs,
                                  ITable&       table)
  : mDbs (dbs),
    mTableName (),
    mTable (&table)
{
  const FIELD_INDEX fieldsCount = mTable->FieldsCount ();

  for (FIELD_INDEX f = 0; f < fieldsCount; ++f)
    mSrcFields.push_back (mTable->DescribeField (f));

  for (size_t f = 0; f < mSrcFields.size (); ++f)
    {
      mDstFields.push_back (mSrcFields[f]);
      mDstFields[f].name = NULL;

      mDstFields[f].name = new char[strlen (mSrcFields[f].name) + 1];
      strcpy (_CC (char*&, mDstFields[f].name), mSrcFields[f].name);
    }
}



TableAlterRules::~TableAlterRules()
{
  for (size_t f = 0; f < mDstFields.size (); ++f)
    delete [] mDstFields[f].name;

  if (mTable != NULL)
    mDbs.ReleaseTable (*mTable);
}


void
TableAlterRules::DropField (const string& field)
{
  assert (mSrcFields.size () <= mDstFields.size ());

  for (size_t f = 0; f < mDstFields.size (); ++f)
    {
      if (strcmp (mDstFields[f].name, field.c_str ()) != 0)
        continue;

      delete [] mDstFields[f].name;
      mDstFields[f].name = NULL;

      mDstFields.erase (mDstFields.begin () + f);

      if (f < mSrcFields.size ())
        mSrcFields.erase (mSrcFields.begin () + f);

      return ;
    }

  throw DBSException (_EXTRA (DBSException::FIELD_NOT_FOUND),
                      "Cannot find field '%s'.",
                      field.c_str ());
}


void
TableAlterRules::RetypeField (const std::string&   name,
                              const DBS_FIELD_TYPE type,
                              const bool           isArray)
{
  assert (mSrcFields.size () <= mDstFields.size ());

  for (size_t f = 0; f < mDstFields.size (); ++f)
    {
      if (strcmp (mDstFields[f].name, name.c_str ()) != 0)
        continue;

      mDstFields[f].type    = type;
      mDstFields[f].isArray = isArray;

      return ;
    }

  throw DBSException (_EXTRA (DBSException::FIELD_NOT_FOUND),
                      "Cannot find field '%s'.",
                      name.c_str ());
}


void
TableAlterRules::RenameField (const std::string& oldName,
                              const std::string& newName)
{
  assert (mSrcFields.size () <= mDstFields.size ());

  if (oldName == newName)
    return ;

  for (size_t f = 0; f < mDstFields.size (); ++f)
    {
      if (strcmp (mDstFields[f].name, newName.c_str ()) != 0)
        continue;

      throw DBSException (_EXTRA (DBSException::FIELD_NAME_INVALID),
                          "The table cannot have more than one field with the "
                            "same name '%s'.",
                          newName.c_str ());
    }

  for (size_t f = 0; f < mDstFields.size (); ++f)
    {
      if (strcmp (mDstFields[f].name, oldName.c_str ()) != 0)
        continue;

      delete [] mDstFields[f].name;
      mDstFields[f].name = NULL;

      mDstFields[f].name = new char[newName.length () + 1];
      strcpy (_CC (char*&, mDstFields[f].name), newName.c_str ());

      return ;
    }

  throw DBSException (_EXTRA (DBSException::FIELD_NOT_FOUND),
                      "Cannot find field '%s'.",
                      oldName.c_str ());
}

void
TableAlterRules::AddField (const std::string&   name,
                           const DBS_FIELD_TYPE type,
                           const bool           isArray)
{
  assert (mSrcFields.size () <= mDstFields.size ());

  for (size_t f = 0; f < mDstFields.size (); ++f)
    {
      if (strcmp (mDstFields[f].name, name.c_str ()) != 0)
        continue;

      throw DBSException (_EXTRA (DBSException::FIELD_NAME_INVALID),
                          "The table cannot have more than one field with the "
                            "same name '%s'.",
                          name.c_str ());
    }


  DBSFieldDescriptor fd = {0, };

  fd.name = new char[name.length() + 1];
  strcpy (_CC (char*&, fd.name), name.c_str ());

  fd.type    = type;
  fd.isArray = isArray;

  mDstFields.push_back (fd);
}


void
TableAlterRules::CommitToTable (const string&           newTableName,
                                const Range<ROW_INDEX>& selectedRows)

{
  ITable* table = NULL;
  vector<DBSFieldDescriptor> dummy = mDstFields;

  assert (mSrcFields.size () <= mDstFields.size ());


  mDbs.AddTable (newTableName.c_str (), dummy.size (), &dummy[0]);

  try
  {
      table = &mDbs.RetrievePersistentTable (newTableName.c_str ());
      CommitToTable (*table, selectedRows);
  }
  catch (...)
  {
      if (table != NULL)
        mDbs.ReleaseTable (*table);

      mDbs.DeleteTable (newTableName.c_str ());
      throw ;
  }

  mDbs.ReleaseTable (*table);
}


void
TableAlterRules::CommitToTable (ITable&                 table,
                                const Range<ROW_INDEX>& selectedRows)

{
  assert (mSrcFields.size () <= mDstFields.size ());

  vector<FIELD_ASSOCIATION> rules;

  for (size_t f = 0; f < mSrcFields.size (); ++f)
    {
      FIELD_ASSOCIATION assoc;

      assoc.src = mTable->RetrieveField (mSrcFields[f].name);
      assoc.dst = table.RetrieveField (mDstFields[f].name);

      assert (mTable->DescribeField (assoc.src).type == mSrcFields[f].type);
      assert (mTable->DescribeField (assoc.src).isArray ==
              mSrcFields[f].isArray);
      assert (table.DescribeField (assoc.dst).type == mDstFields[f].type);
      assert (table.DescribeField (assoc.dst).isArray == mDstFields[f].isArray);

      assoc.copier = get_field_copier (mSrcFields[f].type,
                                       mSrcFields[f].isArray,
                                       mDstFields[f].type,
                                       mDstFields[f].isArray);

      rules.push_back (assoc);
    }

  if (rules.size () == 0)
    return ;

  for (size_t ri = 0; ri < selectedRows.mIntervals.size (); ++ri)
    {
      const Interval<ROW_INDEX>& intv = selectedRows.mIntervals[ri];

      for (ROW_INDEX row = intv.mFrom; row <= intv.mTo; ++row)
        {
          const ROW_INDEX dstRow = table.AddRow ();
          for (size_t r = 0; r < rules.size (); ++r)
            {
              rules[r].copier (*mTable,
                               table,
                               rules[r].src,
                               rules[r].dst,
                               row,
                               dstRow);
            }
        }
    }
}

void
TableAlterRules::Commit ()
{
  static const char temporalTableName[] = "_temporal_persitent_table_";
  assert (mTableName.length () > 0);

  vector<DBSFieldDescriptor> dummy = mDstFields;
  ITable* table = NULL;

  Range<ROW_INDEX> allRows;

  if (mTable->AllocatedRows () > 0)
    allRows.Join (Interval<ROW_INDEX> (0, mTable->AllocatedRows ()));

  else
    return ;

  try
  {
      mDbs.AddTable (temporalTableName, dummy.size (), &dummy[0]);
      table = &mDbs.RetrievePersistentTable (temporalTableName);
  }
  catch (const DBSException& e)
  {
      if (e.Code () != DBSException::FIELD_TYPE_INVALID)
        mDbs.DeleteTable ("_temporal_persitent_table_");

      throw ;
  }
  catch (...)
  {
      assert (table == NULL);

      mDbs.DeleteTable ("_temporal_persitent_table_");
      throw ;
  }

  try
  {
      CommitToTable (*table, allRows);

      mDbs.ReleaseTable (*mTable);
      mTable = NULL;
      mDbs.DeleteTable (mTableName.c_str ());

      TableAlterRules copyBack (mDbs, *table);
      table = NULL;
      copyBack.CommitToTable (mTableName, allRows);
  }
  catch (...)
  {
      if (table != NULL)
        mDbs.ReleaseTable(*table);

      mDbs.DeleteTable (temporalTableName);
      throw ;
  }

  if (table != NULL)
    mDbs.ReleaseTable(*table);

  mDbs.DeleteTable (temporalTableName);
}
