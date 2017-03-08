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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>


#include "utils/wutf.h"
#include "utils/range.h"
#include "dbs/dbs_valtranslator.h"
#include "compiler/whaisc.h"

#include "wcmd_valparser.h"

using namespace std;
using namespace whais;



static const int  MAX_VALUE_OUTPUT_SIZE = 128;


static const char NULL_VALUE[]     = "null";
static const char NON_NULL_VALUE[] = "set";
static const char MIN_VALUE[]      = "min";
static const char MAX_VALUE[]      = "max";
static const char NULL_LABEL[]     = "(null)";



template<class T>
bool parse_defined_values(const char*       src,
                           size_t*           outSrcSize,
                           T* const          outValue,
                           T* const          outValue2)
{
  if (strncmp(src, NULL_VALUE, sizeof NULL_VALUE - 1) == 0)
    {
      *outSrcSize = sizeof NULL_VALUE - 1;
      *outValue   = T();

      return true;
    }
  else if (strncmp(src, NON_NULL_VALUE, sizeof NON_NULL_VALUE - 1) == 0)
    {
      *outSrcSize = sizeof NON_NULL_VALUE - 1;
      *outValue   = T::Min();

      if (outValue2 == nullptr)
        return false;

      *outValue2  = T::Max();

      return true;
    }
  else if (strncmp(src, MIN_VALUE, sizeof MIN_VALUE - 1) == 0)
    {
      *outSrcSize = sizeof MIN_VALUE - 1;
      *outValue   = T::Min();

      return true;
    }
  else if (strncmp(src, MAX_VALUE, sizeof MAX_VALUE - 1) == 0)
    {
      *outSrcSize = sizeof MAX_VALUE - 1;
      *outValue   = T::Max();

      return true;
    }

  return false;
}


template<typename T> bool
ParseFieldValue(ostream* const  os,
                 const char*     src,
                 const bool      apostrophe,
                 size_t* const   outSrcSize,
                 T* const        outValue,
                 T* const        outValue2 = nullptr)
{
  uint_t offset = 0;

  if (parse_defined_values(src, outSrcSize, outValue, outValue2))
    return true;

  if (outValue2 != nullptr)
    *outValue2 = T();

  if (apostrophe && (src[offset++] != '\''))
    return false;

  try
    {
       int size = Utf8Translator::Read(_RC(const uint8_t*, src) + offset,
                                        strlen(src) - offset,
                                        outValue);
       if (size <= 0)
         return false;

       offset += size;
    }
  catch(DBSException &)
    {
      return false;
    }

  if (apostrophe && (src[offset++] != '\''))
    return false;

  *outSrcSize = offset;
  return true;
}

template<> bool
ParseFieldValue<DChar> (ostream* const  os,
                        const char*     src,
                        const bool      apostrophe,
                        size_t* const   outSrcSize,
                        DChar* const    outValue,
                        DChar* const    outValue2)

{
  (void)apostrophe;
  uint_t offset = 0;

  if (parse_defined_values(src, outSrcSize, outValue, outValue2))
    return true;

  if (outValue2 != nullptr)
    *outValue2 = DChar();

  const bool specialChar = src[offset] == '\"';
  const char delimCh     = specialChar ? '\"' : '\'';

  if (src[offset++] != delimCh)
    return false;

  try
    {
       int size = Utf8Translator::Read(_RC(const uint8_t*, src) + offset,
                                        strlen(src) - offset,
                                        specialChar,
                                        outValue);
       if (size <= 0)
         return false;

       offset += size;
    }
  catch(DBSException &)
    {
      return false;
    }

  if (src[offset++] != delimCh)
    return false;

  *outSrcSize = offset;
  return true;
}

template<typename T> bool
ParseFieldSpecifier(ostream* const              os,
                     const char*                 src,
                     size_t* const               outSrcSize,
                     const bool                  apostrophe,
                     const string&               fieldName,
                     FieldValuesSelection&       outFieldValues)
{
  Range<T>& range = *_RC(Range<T>*, outFieldValues.mRange);

  size_t offset = 0;
  bool   result = false;

  while ((src[offset] != 0) && ! isspace(src[offset]))
    {
      T first, second;

      *outSrcSize = offset;

      if (! ParseFieldValue<T> (os,
                                src + *outSrcSize,
                                apostrophe,
                                &offset,
                                &first,
                                &second))
        {
          goto ParseFieldSpecifier_msg_err;
        }

      offset += *outSrcSize;

      if ((src[offset] == ',')
          || (src[offset] == 0)
          || (isspace(src[offset])))
        {
          if ((first == T::Min()) && (second == T::Max()))
            range.Join(Interval<T> (first, second));

          else if (first.IsNull())
            {
              assert(second == T());

              outFieldValues.mSearchNull = true;
            }
          else
            {
              assert(second == T());

              range.Join(Interval<T> (first, first));
            }

          if (src[offset] == ',')
            ++offset;
        }
      else if (src[offset] == '@')
        {
          if ((first == T::Min()) && (second == T::Max()))
            goto ParseFieldSpecifier_msg_err;

          *outSrcSize = offset + 1;

          if (! ParseFieldValue<T> (os,
                                    src + *outSrcSize,
                                    apostrophe,
                                    &offset,
                                    &second))
            {
              goto ParseFieldSpecifier_msg_err;
            }

          offset += *outSrcSize;

          if ((src[offset] != ',')
              && (src[offset] != 0)
              && ! isspace(src[offset]))
            {
              goto ParseFieldSpecifier_msg_err;
            }

          if (first.IsNull() || second.IsNull())
            goto ParseFieldSpecifier_msg_err;

          range.Join(Interval<T> (first, second));

          if (src[offset] == ',')
            ++offset;
        }
      else
        goto ParseFieldSpecifier_msg_err;

      result = true;
    }

  *outSrcSize = offset;

  return result;

ParseFieldSpecifier_msg_err:

  if (os != nullptr)
    *os << "Invalid selector values for field '" << fieldName <<"'.\n";

  return false;
}


static void*
allocate_value_range(const uint_t fieldType)
{
  void* result = nullptr;

  switch(fieldType)
    {
    case T_BOOL:
      result = new Range<DBool> ();
      break;

    case T_CHAR:
      result = new Range<DChar> ();
      break;

    case T_DATE:
      result = new Range<DDate> ();
      break;

    case T_DATETIME:
      result = new Range<DDateTime> ();
      break;

    case T_HIRESTIME:
      result = new Range<DHiresTime> ();
      break;

    case T_INT8:
      result = new Range<DInt8> ();
      break;

    case T_INT16:
      result = new Range<DInt16> ();
      break;

    case T_INT32:
      result = new Range<DInt32> ();
      break;

    case T_INT64:
      result = new Range<DInt64> ();
      break;

    case T_UINT8:
      result = new Range<DUInt8> ();
      break;

    case T_UINT16:
      result = new Range<DUInt16> ();
      break;

    case T_UINT32:
      result = new Range<DUInt32> ();
      break;

    case T_UINT64:
      result = new Range<DUInt64> ();
      break;

    case T_REAL:
      result = new Range<DReal> ();
      break;

    case T_RICHREAL:
      result = new Range<DRichReal> ();
      break;

     default:
      assert(false);
    }

  return result;
}

static void
free_value_range(const uint_t fieldType, void* const range)
{
  switch(fieldType)
    {
    case T_BOOL:
      delete _RC(Range<DBool>*, range);
      break;

    case T_CHAR:
      delete _RC(Range<DChar>*, range);
      break;

    case T_DATE:
      delete _RC(Range<DDate>*, range);
      break;

    case T_DATETIME:
      delete _RC(Range<DDateTime>*, range);
      break;

    case T_HIRESTIME:
      delete _RC(Range<DHiresTime>*, range);
      break;

    case T_INT8:
      delete _RC(Range<DInt8>*, range);
      break;

    case T_INT16:
      delete _RC(Range<DInt16>*, range);
      break;

    case T_INT32:
      delete _RC(Range<DInt32>*, range);
      break;

    case T_INT64:
      delete _RC(Range<DInt64>*, range);
      break;

    case T_UINT8:
      delete _RC(Range<DUInt8>*, range);
      break;

    case T_UINT16:
      delete _RC(Range<DUInt16>*, range);
      break;

    case T_UINT32:
      delete _RC(Range<DUInt32>*, range);
      break;

    case T_UINT64:
      delete _RC(Range<DUInt64>*, range);
      break;

    case T_REAL:
      delete _RC(Range<DReal>*, range);
      break;

    case T_RICHREAL:
      delete _RC(Range<DRichReal>*, range);
      break;

     default:
      assert(false);
    }
}


static bool
parse_rows_selection(ostream* const     os,
                      ITable&            table,
                      const char* const  str,
                      size_t* const      outSelectSize,
                      Range<ROW_INDEX>&  outRowsRange)
{
  const size_t strSize = strlen(str);
  size_t&      offset  = *outSelectSize;

  offset = 0;
  while ((offset < strSize) && isspace(str[offset]))
    ++offset;

  outRowsRange.Clear();

  Range<ROW_INDEX> allRows;

  if (table.AllocatedRows() > 0)
    allRows.Join(Interval<ROW_INDEX> (0, table.AllocatedRows() - 1));

  if (offset >= strSize)
    {
      outRowsRange = allRows;
      return true;
    }

  if (str[offset] == '*')
    {
      // All available rows.
      ++offset;

      //Check if the entry is valid.
      if ((str[offset] != 0) && ! isspace(str[offset]))
        return false;

      outRowsRange = allRows;

      //Make sure we parse all the white chars after it.
      while ((str[offset] != 0) && isspace(str[offset]))
        ++offset;

      return true;
    }

  ROW_INDEX from, to;
  while ((str[offset] != 0)
         && ! isspace(str[offset]))
    {
      bool fromEntered = false;

      from = 0, to = table.AllocatedRows();
      while (isdigit(str[offset]))
        {
          const ROW_INDEX temp = from;

          from *= 10;
          from += str[offset] - '0';

          if (from < temp)
            goto invalid_row_spec;

          ++offset;
          fromEntered = true;
        }

      if ((str[offset] == 0) || isspace(str[offset]))
        {

          if (! fromEntered)
            goto invalid_row_spec;

          outRowsRange.Join(Interval<ROW_INDEX> (from, from));

          while ((str[offset] != 0) && isspace(str[offset]))
            ++offset;

          break;
        }
      else if (str[offset] == ',')
        {
          if (! fromEntered || (++offset >= strSize))
            goto invalid_row_spec;

          outRowsRange.Join(Interval<ROW_INDEX> (from, from));

          continue;
        }
      else if (str[offset++] != '-')
        goto invalid_row_spec;

      if (isdigit(str[offset]))
        to = 0;

      while (isdigit(str[offset]))
        {
          const ROW_INDEX temp = to;

          to *= 10;
          to += str[offset] - '0';

          if (to < temp)
            goto invalid_row_spec;

          ++offset;
        }

       outRowsRange.Join(Interval<ROW_INDEX> (from, to));

       if (str[offset] == ',')
         {
           if (++offset >= strSize)
             goto invalid_row_spec;
         }
    }

  outRowsRange.Match(allRows);

  return true;

invalid_row_spec:
  if (os != nullptr)
    *os << "The rows are not properly specified.\n";

  return false;
}


static bool
parse_field_values_selection(ostream* const              os,
                              ITable&                     table,
                              const char*                 str,
                              size_t*                     outSelectSize,
                              FieldValuesSelection* const outRowsRange)
{
  size_t& offset  = *outSelectSize;

  outRowsRange->~FieldValuesSelection();
  _placement_new<FieldValuesSelection> (outRowsRange);

  /* Get the field information first. */
  offset = 0;
  string field;
  while ((str[offset] != 0) && (str[offset] != '='))
    field.push_back(str[offset++]);

  outRowsRange->mFieldId = table.RetrieveField(field.c_str());
  DBSFieldDescriptor fd  = table.DescribeField(outRowsRange->mFieldId);

  if (fd.isArray)
    {
      if (os != nullptr)
        *os << "Array type fields are not supported in value selections.\n";

      return false;
    }
  else if (fd.type == T_TEXT)
    {
      if (os != nullptr)
        *os << "Text type fields are not supported in value selections.\n";

      return false;
    }

  outRowsRange->mFieldType = fd.type;
  outRowsRange->mRange     = allocate_value_range(fd.type);

  assert(outRowsRange->mRange != nullptr);

  if (str[offset] == '=')
    ++offset;

  if ((str[offset] == 0) || isspace(str[offset]))
    {
      if (os != nullptr)
        *os << "Field values are missing or are not properly specified.\n";

      return false;
    }

  size_t temp;
  switch(outRowsRange->mFieldType)
    {
    case T_BOOL:
      if (! ParseFieldSpecifier<DBool> (os,
                                        str + offset,
                                        &temp,
                                        false,
                                        field,
                                        *outRowsRange))
        {
          return false;
        }
      break;

    case T_CHAR:
      if (! ParseFieldSpecifier<DChar> (os,
                                        str + offset,
                                        &temp,
                                        true,
                                        field,
                                        *outRowsRange))
        {
          return false;
        }
      break;

    case T_DATE:
      if (! ParseFieldSpecifier<DDate> (os,
                                        str + offset,
                                        &temp,
                                        true,
                                        field,
                                        *outRowsRange))
        {
          return false;
        }
      break;

    case T_DATETIME:
      if (! ParseFieldSpecifier<DDateTime> (os,
                                            str + offset,
                                            &temp,
                                            true,
                                            field,
                                            *outRowsRange))
        {
          return false;
        }
      break;

    case T_HIRESTIME:
      if (! ParseFieldSpecifier<DHiresTime> (os,
                                             str + offset,
                                             &temp,
                                             true,
                                             field,
                                             *outRowsRange))
        {
          return false;
        }
      break;

    case T_INT8:
      if (! ParseFieldSpecifier<DInt8> (os,
                                        str + offset,
                                        &temp,
                                        false,
                                        field,
                                        *outRowsRange))
        {
          return false;
        }
      break;

    case T_INT16:
      if (! ParseFieldSpecifier<DInt16> (os,
                                         str + offset,
                                         &temp,
                                         false,
                                         field,
                                         *outRowsRange))
        {
          return false;
        }
      break;

    case T_INT32:
      if (! ParseFieldSpecifier<DInt32> (os,
                                         str + offset,
                                         &temp,
                                         false,
                                         field,
                                         *outRowsRange))
        {
          return false;
        }
      break;

    case T_INT64:
      if (! ParseFieldSpecifier<DInt64> (os,
                                         str + offset,
                                         &temp,
                                         false,
                                         field,
                                         *outRowsRange))
        {
          return false;
        }
      break;

    case T_UINT8:
      if (! ParseFieldSpecifier<DUInt8> (os,
                                         str + offset,
                                         &temp,
                                         false,
                                         field,
                                         *outRowsRange))
        {
          return false;
        }
      break;

    case T_UINT16:
      if (! ParseFieldSpecifier<DUInt16> (os,
                                          str + offset,
                                          &temp,
                                          false,
                                          field,
                                          *outRowsRange))
        {
          return false;
        }
      break;

    case T_UINT32:
      if (! ParseFieldSpecifier<DUInt32> (os,
                                          str + offset,
                                          &temp,
                                          false,
                                          field,
                                          *outRowsRange))
        {
          return false;
        }
      break;

    case T_UINT64:
      if (! ParseFieldSpecifier<DUInt64> (os,
                                          str + offset,
                                          &temp,
                                          false,
                                          field,
                                          *outRowsRange))
        {
          return false;
        }
      break;

    case T_REAL:
      if (! ParseFieldSpecifier<DReal> (os,
                                        str + offset,
                                        &temp,
                                        false,
                                        field,
                                        *outRowsRange))
        {
          return false;
        }
      break;

    case T_RICHREAL:
      if (! ParseFieldSpecifier<DRichReal> (os,
                                            str + offset,
                                            &temp,
                                            false,
                                            field,
                                            *outRowsRange))
        {
          return false;
        }
      break;

    default:
      assert(false);
    }
  offset += temp;

  assert(offset <= strlen(str));

  return true;
}


bool
ParseRowsSelectionClause(ostream* const    os,
                          whais::ITable&  table,
                          const char*       str,
                          RowsSelection&    outRowsSelection)
{
  size_t offset = 0;

  outRowsSelection.~RowsSelection();
  _placement_new<RowsSelection> (&outRowsSelection);

  if (! parse_rows_selection(os,
                              table,
                              str,
                              &offset,
                              outRowsSelection.mRows))
    {
      return false;
    }

  while (str[offset] != 0)
    {
      if (isspace(str[offset]))
        {
          ++offset;
          continue;
        }

      FieldValuesSelection sel;
      size_t               temp;
      if (! parse_field_values_selection(os,
                                          table,
                                          str + offset,
                                          &temp,
                                          &sel))
        {
          return false;
        }

      outRowsSelection.mSearchedValue.push_back(sel);
      sel.mRange = nullptr;  //Manually release the ownership!

      offset += temp;
    }

  assert(offset == strlen(str));

  return true;
}



FieldValuesSelection::FieldValuesSelection()
  : mFieldId(0),
    mFieldType(T_UNKNOWN),
    mSearchNull(false),
    mRange(nullptr)
{
}


FieldValuesSelection::~FieldValuesSelection()
{
  if (mFieldType != T_UNKNOWN)
    free_value_range(mFieldType, mRange);
}



FieldValuesUpdate::~FieldValuesUpdate()
{
  assert((T_UNKNOWN < GET_BASIC_TYPE(mFieldType))
          || (mFieldType == T_UNKNOWN));
  assert(GET_BASIC_TYPE(mFieldType) < T_UNDETERMINED);

  if (IS_ARRAY(mFieldType))
    _RC(DArray*, mValue)->~DArray();

  else if (GET_BASIC_TYPE(mFieldType) == T_TEXT)
    _RC(DText*, mValue)->~DText();
}



bool
ParseFieldUpdateValues(ostream* const              os,
                        ITable&                     table,
                        const char*                 str,
                        size_t* const               outSize,
                        vector<FieldValuesUpdate>&  outUpdates)
{
  size_t& offset = *outSize;


  outUpdates.clear();
  offset = 0;

  while ((str[offset] != 0) && isspace(str[offset]))
    ++offset;

  if (str[offset] == 0 )
    {
      if (os != nullptr)
        *os << "Field values have not been specified.\n";

      return false;
    }

  while ((str[offset] != 0) && ! isspace(str[offset]))
    {
      string field;

      while ((str[offset] != 0) && (str[offset] != '='))
          field.push_back(str[offset++]);

      if (str[offset] == 0)
        {
          if (os != nullptr)
            *os << "Field update value not properly specified.\n";

          return false;
        }
      else
        ++offset;

      const FIELD_INDEX        fieldId = table.RetrieveField(field.c_str());
      const DBSFieldDescriptor fd      = table.DescribeField(fieldId);

      if (fd.isArray)
        {
          uint_t type = fd.type;
          DArray arrayVal;

          MARK_ARRAY(type);
          if (strncmp(str + offset,
                       NULL_VALUE,
                       strlen(NULL_VALUE)) == 0)
            {
              offset += strlen(NULL_VALUE);
              outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                       type,
                                                       arrayVal));
              if (str[offset] == ',')
                  ++offset;

              continue;
            }
          else if (str[offset++] != '[')
            return false;

          while ((str[offset] != 0) && (str[offset] != ']'))
            {
              if (isspace(str[offset]))
                {
                  ++offset;
                  continue;
                }

              const size_t temp = offset;
              switch(GET_BASIC_TYPE(fd.type))
                {
                case T_BOOL:
                    {
                      DBool value;
                      if (! ParseFieldValue<DBool> (os,
                                                    str + offset,
                                                    false,
                                                    &offset,
                                                    &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_CHAR:
                    {
                      DChar value;
                      if (! ParseFieldValue<DChar> (os,
                                                    str + offset,
                                                    true,
                                                    &offset,
                                                    &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_DATE:
                    {
                      DDate value;
                      if (! ParseFieldValue<DDate> (os,
                                                    str + offset,
                                                    true,
                                                    &offset,
                                                    &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_DATETIME:
                    {
                      DDateTime value;
                      if (! ParseFieldValue<DDateTime> (os,
                                                        str + offset,
                                                        true,
                                                        &offset,
                                                        &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_HIRESTIME:
                    {
                      DHiresTime value;
                      if (! ParseFieldValue<DHiresTime> (os,
                                                         str + offset,
                                                         true,
                                                         &offset,
                                                         &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_INT8:
                    {
                      DInt8 value;
                      if (! ParseFieldValue<DInt8> (os,
                                                    str + offset,
                                                    false,
                                                    &offset,
                                                    &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_INT16:
                    {
                      DInt16 value;
                      if (! ParseFieldValue<DInt16> (os,
                                                     str + offset,
                                                     false,
                                                     &offset,
                                                     &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_INT32:
                    {
                      DInt32 value;
                      if (! ParseFieldValue<DInt32> (os,
                                                     str + offset,
                                                     false,
                                                     &offset,
                                                     &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_INT64:
                    {
                      DInt64 value;
                      if (! ParseFieldValue<DInt64> (os,
                                                     str + offset,
                                                     false,
                                                     &offset,
                                                     &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_UINT8:
                    {
                      DUInt8 value;
                      if (! ParseFieldValue<DUInt8> (os,
                                                     str + offset,
                                                     false,
                                                     &offset,
                                                     &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_UINT16:
                    {
                      DUInt16 value;
                      if (! ParseFieldValue<DUInt16> (os,
                                                      str + offset,
                                                      false,
                                                      &offset,
                                                      &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_UINT32:
                    {
                      DUInt32 value;
                      if (! ParseFieldValue<DUInt32> (os,
                                                      str + offset,
                                                      false,
                                                      &offset,
                                                      &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_UINT64:
                    {
                      DUInt64 value;
                      if (! ParseFieldValue<DUInt64> (os,
                                                      str + offset,
                                                      false,
                                                      &offset,
                                                      &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_REAL:
                    {
                      DReal value;
                      if (! ParseFieldValue<DReal> (os,
                                                    str + offset,
                                                    false,
                                                    &offset,
                                                    &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                case T_RICHREAL:
                    {
                      DRichReal value;
                      if (! ParseFieldValue<DRichReal> (os,
                                                        str + offset,
                                                        false,
                                                        &offset,
                                                        &value))
                        {
                          return false;
                        }

                      arrayVal.Add(value);
                    }
                  break;

                default:
                  assert(false);
                }
              offset += temp;
            }

          if (str[offset++] != ']')
            return false;

          outUpdates.push_back(FieldValuesUpdate(fieldId, type, arrayVal));
        }
      else
        {
          const size_t temp = offset;
          switch(fd.type)
            {
            case T_BOOL:
                {
                  DBool value;
                  if (! ParseFieldValue<DBool> (os,
                                                str + offset,
                                                false,
                                                &offset,
                                                &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_CHAR:
                {
                  DChar value;
                  if (! ParseFieldValue<DChar> (os,
                                                str + offset,
                                                false,
                                                &offset,
                                                &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_DATE:
                {
                  DDate value;
                  if (! ParseFieldValue<DDate> (os,
                                                str + offset,
                                                true,
                                                &offset,
                                                &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_DATETIME:
                {
                  DDateTime value;
                  if (! ParseFieldValue<DDateTime> (os,
                                                    str + offset,
                                                    true,
                                                    &offset,
                                                    &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_HIRESTIME:
                {
                  DHiresTime value;
                  if (! ParseFieldValue<DHiresTime> (os,
                                                     str + offset,
                                                     true,
                                                     &offset,
                                                     &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_INT8:
                {
                  DInt8 value;
                  if (! ParseFieldValue<DInt8> (os,
                                                str + offset,
                                                false,
                                                &offset,
                                                &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_INT16:
                {
                  DInt16 value;
                  if (! ParseFieldValue<DInt16> (os,
                                                 str + offset,
                                                 false,
                                                 &offset,
                                                 &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_INT32:
                {
                  DInt32 value;
                  if (! ParseFieldValue<DInt32> (os,
                                                 str + offset,
                                                 false,
                                                 &offset,
                                                 &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_INT64:
                {
                  DInt64 value;
                  if (! ParseFieldValue<DInt64> (os,
                                                 str + offset,
                                                 false,
                                                 &offset,
                                                 &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_UINT8:
                {
                  DUInt8 value;
                  if (! ParseFieldValue<DUInt8> (os,
                                                 str + offset,
                                                 false,
                                                 &offset,
                                                 &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_UINT16:
                {
                  DUInt16 value;
                  if (! ParseFieldValue<DUInt16> (os,
                                                  str + offset,
                                                  false,
                                                  &offset,
                                                  &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_UINT32:
                {
                  DUInt32 value;
                  if (! ParseFieldValue<DUInt32> (os,
                                                  str + offset,
                                                  false,
                                                  &offset,
                                                  &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_UINT64:
                {
                  DUInt64 value;
                  if (! ParseFieldValue<DUInt64> (os,
                                                  str + offset,
                                                  false,
                                                  &offset,
                                                  &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_REAL:
                {
                  DReal value;
                  if (! ParseFieldValue<DReal> (os,
                                                str + offset,
                                                false,
                                                &offset,
                                                &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_RICHREAL:
                {
                  DRichReal value;
                  if (! ParseFieldValue<DRichReal> (os,
                                                    str + offset,
                                                    false,
                                                    &offset,
                                                    &value))
                    {
                      return false;
                    }

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            case T_TEXT:
                {
                  DText value;

                  if (strncmp(str + offset,
                               NULL_VALUE,
                               strlen(NULL_VALUE)) == 0)
                    {
                      offset += strlen(NULL_VALUE);
                      offset -= temp;
                      outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                               fd.type,
                                                               value));
                      break ;
                    }

                  const bool specialChars = (str[offset] == '\"');
                  const char delimCh      = specialChars ? '\"' : '\'';

                  if (str[offset++] != delimCh)
                    return false;

                  const size_t strSize = strlen(str);
                  while ((str[offset] != 0) && (str[offset] != delimCh))
                    {
                      DChar  ch;

                      const int chLen = Utf8Translator::Read(
                                          _RC(const uint8_t*, str) + offset,
                                          strSize - offset,
                                          specialChars,
                                          &ch
                                                              );
                      if (chLen <= 0)
                        return false;

                      assert(ch.IsNull() == false);

                      value.Append(ch);
                      offset += chLen;
                    }
                  assert(offset <= strSize);

                  if (str[offset++] != delimCh)
                    return false;

                  assert(temp < offset);
                  offset -= temp; //Needed here for consistency. It will be
                                  //cleared outside the 'switch' block.

                  outUpdates.push_back(FieldValuesUpdate(fieldId,
                                                           fd.type,
                                                           value));
                }
              break;

            default:
              assert(false);
            }
          offset += temp;
        }

      if (str[offset] == ',')
          ++offset;
    }

  return true;
}

bool
UpdateTableRow(ostream const*                   os,
                ITable&                          table,
                const ROW_INDEX                  row,
                const vector<FieldValuesUpdate>& fieldVals)
{
  size_t field = fieldVals.size();

  while (field-- > 0)
    {
      const FieldValuesUpdate& fv = fieldVals[field];

      if (IS_ARRAY(fv.mFieldType))
        {
          const DArray& value = *_RC(const DArray*, fv.mValue);
          table.Set(row, fv.mFieldId, value);
        }
      else
        {
          switch(fv.mFieldType)
            {
            case T_BOOL:
                {
                  const DBool& value = *_RC(const DBool*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_CHAR:
                {
                  const DChar& value = *_RC(const DChar*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;


            case T_DATE:
                {
                  const DDate& value = *_RC(const DDate*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_DATETIME:
                {
                  const DDateTime& value = *_RC(const DDateTime*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_HIRESTIME:
                {
                  const DHiresTime& value = *_RC(const DHiresTime*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_INT8:
                {
                  const DInt8& value = *_RC(const DInt8*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_INT16:
                {
                  const DInt16& value = *_RC(const DInt16*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_INT32:
                {
                  const DInt32& value = *_RC(const DInt32*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_INT64:
                {
                  const DInt64& value = *_RC(const DInt64*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_UINT8:
                {
                  const DUInt8& value = *_RC(const DUInt8*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_UINT16:
                {
                  const DUInt16& value = *_RC(const DUInt16*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_UINT32:
                {
                  const DUInt32& value = *_RC(const DUInt32*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_UINT64:
                {
                  const DUInt64& value = *_RC(const DUInt64*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_REAL:
                {
                  const DReal& value = *_RC(const DReal*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_RICHREAL:
                {
                  const DRichReal& value = *_RC(const DRichReal*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            case T_TEXT:
                {
                  const DText& value = *_RC(const DText*, fv.mValue);
                  table.Set(row, fv.mFieldId, value);
                }
              break;

            default:
              assert(false);
            }
        }
    }

  return true;
}


static void
add_rows_to_interval(DArray&           rows,
                      Range<ROW_INDEX>& inoutRange)
{
  const uint64_t count = rows.Count();

  DROW_INDEX  prev, last, first;

  for (uint64_t i = 0; i < count; ++i)
    {
      rows.Get(i, last);

      if (prev.IsNull())
        first = prev = last;

      else if (last.Prev() == prev)
        prev = last;

      else
        {
          assert(first.IsNull() == false);

          inoutRange.Join(Interval<ROW_INDEX> (first.mValue, prev.mValue));

          first = prev = last;
        }
    }

  if (! first.IsNull())
    {
      assert(last.IsNull() == false);

      inoutRange.Join(Interval<ROW_INDEX> (first.mValue, last.mValue));
    }
}


template<typename T>
void match_field_value(ITable&                     table,
                        const FieldValuesSelection  values,
                        Range<ROW_INDEX>&           inoutRow)
{
  const Range<T>& valRange = * _RC(Range<T>*, values.mRange);

  const size_t rowIntervals = inoutRow.mIntervals.size();
  const size_t valIntervals = valRange.mIntervals.size();

  Range<ROW_INDEX> fieldRows;

  for (size_t r = 0; r < rowIntervals; ++r)
    {
      const Interval<ROW_INDEX>& rows = inoutRow.mIntervals[r];

      for (size_t v = 0; v < valIntervals; ++v)
        {
          const Interval<T>& vals = valRange.mIntervals[v];

          DArray rowsFound = table.MatchRows(vals.mFrom,
                                              vals.mTo,
                                              rows.mFrom,
                                              rows.mTo,
                                              values.mFieldId);
          add_rows_to_interval(rowsFound, fieldRows);
        }

      if (values.mSearchNull)
        {
          DArray rowsFound = table.MatchRows(T(),
                                              T(),
                                              rows.mFrom,
                                              rows.mTo,
                                              values.mFieldId);
          add_rows_to_interval(rowsFound, fieldRows);
        }
    }

  fieldRows.Complement();
  inoutRow.Match(fieldRows);
}


void
MatchSelectedRows(ITable&             table,
                   RowsSelection&      select)
{
  const size_t fieldsCount = select.mSearchedValue.size();

  for (size_t i = 0; i < fieldsCount; ++i)
    {
      Range<ROW_INDEX> fieldRows = select.mRows;
      switch(select.mSearchedValue[i].mFieldType)
        {
        case T_BOOL:
          match_field_value<DBool> (table,
                                    select.mSearchedValue[i],
                                    fieldRows);
          break;

        case T_CHAR:
          match_field_value<DChar> (table,
                                    select.mSearchedValue[i],
                                    fieldRows);
          break;

        case T_DATE:
          match_field_value<DDate> (table,
                                    select.mSearchedValue[i],
                                    fieldRows);
          break;

        case T_DATETIME:
          match_field_value<DDateTime> (table,
                                        select.mSearchedValue[i],
                                        fieldRows);
          break;

        case T_HIRESTIME:
          match_field_value<DHiresTime> (table,
                                        select.mSearchedValue[i],
                                        fieldRows);
          break;

        case T_INT8:
          match_field_value<DInt8> (table,
                                    select.mSearchedValue[i],
                                    fieldRows);
          break;

        case T_INT16:
          match_field_value<DInt16> (table,
                                     select.mSearchedValue[i],
                                     fieldRows);
          break;

        case T_INT32:
          match_field_value<DInt32> (table,
                                     select.mSearchedValue[i],
                                     fieldRows);
          break;

        case T_INT64:
          match_field_value<DInt64> (table,
                                     select.mSearchedValue[i],
                                     fieldRows);
          break;

        case T_UINT8:
          match_field_value<DUInt8> (table,
                                     select.mSearchedValue[i],
                                     fieldRows);
          break;

        case T_UINT16:
          match_field_value<DUInt16> (table,
                                      select.mSearchedValue[i],
                                      fieldRows);
          break;

        case T_UINT32:
          match_field_value<DUInt32> (table,
                                      select.mSearchedValue[i],
                                      fieldRows);
          break;

        case T_UINT64:
          match_field_value<DUInt64> (table,
                                      select.mSearchedValue[i],
                                      fieldRows);
          break;

        case T_REAL:
          match_field_value<DReal> (table,
                                    select.mSearchedValue[i],
                                    fieldRows);
          break;

        case T_RICHREAL:
          match_field_value<DRichReal> (table,
                                        select.mSearchedValue[i],
                                        fieldRows);
          break;

        default:
          assert(false);
        }

      fieldRows.Complement();
      select.mRows.Match(fieldRows);
    }
}


template<typename T> void
print_basic_value(ostream&      os,
                   const T&      value,
                   const bool    apostrophe)
{
  char temp[MAX_VALUE_OUTPUT_SIZE];

  if (value.IsNull())
    {
      os << NULL_LABEL;
      return ;
    }

  Utf8Translator::Write(_RC(uint8_t*, temp), sizeof(temp), value);

  if (apostrophe)
    os << '\'';

  os << temp;

  if (apostrophe)
    os << '\'';
}

template<> void
print_basic_value<DChar> (ostream&        os,
                          const DChar&    value,
                          const bool      apostrophe)
{
  char temp[MAX_VALUE_OUTPUT_SIZE];

  if (value.IsNull())
    {
      os << NULL_LABEL;
      return ;
    }

  Utf8Translator::Write(_RC(uint8_t*, temp), sizeof(temp), true, value);

  if (apostrophe)
    os << '\'';

  os << temp;

  if (apostrophe)
    os << '\'';
}



void
PrintFieldValue(ostream&             os,
                 ITable&              table,
                 const ROW_INDEX      row,
                 const FIELD_INDEX    field)
{
  DBSFieldDescriptor fd = table.DescribeField(field);

  if (fd.isArray)
    {
      DArray array;
      table.Get(row, field, array);

      const uint64_t count = array.Count();
      if (count == 0)
        {
          os << ' ' << NULL_LABEL;
          return ;
        }

      for (uint64_t i = 0; i < count; i++)
        {
          os << ' ';
          switch(fd.type)
            {
            case T_BOOL:
                {
                  DBool value;
                  array.Get(i, value);
                  print_basic_value<DBool> (os, value, true);
                }
              break;

            case T_CHAR:
                {
                  DChar value;
                  array.Get(i, value);
                  print_basic_value<DChar> (os, value, true);
                }
              break;

            case T_DATE:
                {
                  DDate value;
                  array.Get(i, value);
                  print_basic_value<DDate> (os, value, true);
                }
              break;

            case T_DATETIME:
                {
                  DDateTime value;
                  array.Get(i, value);
                  print_basic_value<DDateTime> (os, value, true);
                }
              break;

            case T_HIRESTIME:
                {
                  DHiresTime value;
                  array.Get(i, value);
                  print_basic_value<DHiresTime> (os, value, true);
                }
              break;

            case T_INT8:
                {
                  DInt8 value;
                  array.Get(i, value);
                  print_basic_value<DInt8> (os, value, true);
                }
              break;

            case T_INT16:
                {
                  DInt16 value;
                  array.Get(i, value);
                  print_basic_value<DInt16> (os, value, true);
                }
              break;

            case T_INT32:
                {
                  DInt32 value;
                  array.Get(i, value);
                  print_basic_value<DInt32> (os, value, true);
                }
              break;

            case T_INT64:
                {
                  DInt64 value;
                  array.Get(i, value);
                  print_basic_value<DInt64> (os, value, true);
                }
              break;

            case T_UINT8:
                {
                  DUInt8 value;
                  array.Get(i, value);
                  print_basic_value<DUInt8> (os, value, true);
                }
              break;

            case T_UINT16:
                {
                  DUInt16 value;
                  array.Get(i, value);
                  print_basic_value<DUInt16> (os, value, true);
                }
              break;

            case T_UINT32:
                {
                  DUInt32 value;
                  array.Get(i, value);
                  print_basic_value<DUInt32> (os, value, true);
                }
              break;

            case T_UINT64:
                {
                  DUInt64 value;
                  array.Get(i, value);
                  print_basic_value<DUInt64> (os, value, true);
                }
              break;

            case T_REAL:
                {
                  DReal value;
                  array.Get(i, value);
                  print_basic_value<DReal> (os, value, true);
                }
              break;

            case T_RICHREAL:
                {
                  DRichReal value;
                  array.Get(i, value);
                  print_basic_value<DRichReal> (os, value, true);
                }
              break;
            default:
              assert(false);
            }
        }
    }
  else
    {
      os << ' ';
      switch(fd.type)
        {
        case T_BOOL:
            {
              DBool value;
              table.Get(row, field, value);
              print_basic_value<DBool> (os, value, true);
            }
          break;

        case T_CHAR:
            {
              DChar value;
              table.Get(row, field, value);
              print_basic_value<DChar> (os, value, true);
            }
          break;

        case T_DATE:
            {
              DDate value;
              table.Get(row, field, value);
              print_basic_value<DDate> (os, value, true);
            }
          break;

        case T_DATETIME:
            {
              DDateTime value;
              table.Get(row, field, value);
              print_basic_value<DDateTime> (os, value, true);
            }
          break;

        case T_HIRESTIME:
            {
              DHiresTime value;
              table.Get(row, field, value);
              print_basic_value<DHiresTime> (os, value, true);
            }
          break;

        case T_INT8:
            {
              DInt8 value;
              table.Get(row, field, value);
              print_basic_value<DInt8> (os, value, true);
            }
          break;

        case T_INT16:
            {
              DInt16 value;
              table.Get(row, field, value);
              print_basic_value<DInt16> (os, value, true);
            }
          break;

        case T_INT32:
            {
              DInt32 value;
              table.Get(row, field, value);
              print_basic_value<DInt32> (os, value, true);
            }
          break;

        case T_INT64:
            {
              DInt64 value;
              table.Get(row, field, value);
              print_basic_value<DInt64> (os, value, true);
            }
          break;

        case T_UINT8:
            {
              DUInt8 value;
              table.Get(row, field, value);
              print_basic_value<DUInt8> (os, value, true);
            }
          break;

        case T_UINT16:
            {
              DUInt16 value;
              table.Get(row, field, value);
              print_basic_value<DUInt16> (os, value, true);
            }
          break;

        case T_UINT32:
            {
              DUInt32 value;
              table.Get(row, field, value);
              print_basic_value<DUInt32> (os, value, true);
            }
          break;

        case T_UINT64:
            {
              DUInt64 value;
              table.Get(row, field, value);
              print_basic_value<DUInt64> (os, value, true);
            }
          break;

        case T_REAL:
            {
              DReal value;
              table.Get(row, field, value);
              print_basic_value<DReal> (os, value, true);
            }
          break;

        case T_RICHREAL:
            {
              DRichReal value;
              table.Get(row, field, value);
              print_basic_value<DRichReal> (os, value, true);
            }
          break;

        case T_TEXT:
            {
              DText value;

              table.Get(row, field, value);
              const uint64_t textLength = value.Count();
              if (value.IsNull())
                {
                  os << NULL_LABEL;
                  return ;
                }

              assert(textLength > 0);

              os << '\'';
              for (uint64_t i = 0; i < textLength; ++i)
                print_basic_value<DChar> (os, value.CharAt(i), false);
              os << '\'';
            }
          break;

        default:
          assert(false);
        }
    }
}

