/******************************************************************************
WHISPER - An advanced database system
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

#include <cstring>
#include <sstream>

#include "utils/include/le_converter.h"
#include "utils/include/auto_array.h"
#include "utils/include/utf8.h"
#include "server_protocol.h"

#include "stack_cmds.h"
#include "valtranslator.h"

using namespace std;

static inline void
assert_cmds_values ()
{
  assert (WFT_BOOL       == T_BOOL);
  assert (WFT_CHAR       == T_CHAR);
  assert (WFT_DATE       == T_DATE);
  assert (WFT_DATETIME   == T_DATETIME);
  assert (WFT_HIRESTIME  == T_HIRESTIME);
  assert (WFT_INT8       == T_INT8);
  assert (WFT_INT16      == T_INT16);
  assert (WFT_INT32      == T_INT32);
  assert (WFT_INT64      == T_INT64);
  assert (WFT_UINT8      == T_UINT8);
  assert (WFT_UINT16     == T_UINT16);
  assert (WFT_UINT32     == T_UINT32);
  assert (WFT_UINT64     == T_UINT64);
  assert (WFT_REAL       == T_REAL);
  assert (WFT_RICHREAL   == T_RICHREAL);
  assert (WFT_TEXT       == T_TEXT);
  assert (WFT_ARRAY_MASK == T_ARRAY_MASK);
  assert (WFT_FIELD_MASK == T_FIELD_MASK);
  assert (WFT_TABLE_MASK == T_TABLE_MASK);
}

uint_t
cmd_pop_stack (ClientConnection& rConn, uint_t* const pDataOff)
{
  assert_cmds_values();

  if ((*pDataOff + sizeof (uint32_t)) > rConn.DataSize ())
    {
      throw ConnectionException (
                    "Got frame with invalid content for stack pop operation.",
                    _EXTRA (0)
                                );
    }
  SessionStack& rStack = rConn.Stack ();
  uint32_t      count  = from_le_int32 (rConn.Data () + *pDataOff);

  *pDataOff += sizeof (uint32_t);

  count = MIN (count, rStack.Size ());
  rStack.Pop (count);

  return WCS_OK;
}

uint_t
cmd_push_stack (ClientConnection& rConn, uint_t* const pDataOff)
{
  assert_cmds_values();

  const uint8_t* const data     = rConn.Data ();
  const uint_t         dataSize = rConn.DataSize ();
  SessionStack&        rStack   = rConn.Stack ();
  uint16_t             type;

  if ((*pDataOff + sizeof (uint16_t)) > dataSize)
    goto push_frame_error;

  type       = from_le_int16 (data + *pDataOff);
  *pDataOff += sizeof (uint16_t);

  if (type != WFT_TABLE_MASK)
    {
      if (type & WFT_FIELD_MASK)
        return WCS_INVALID_ARGS;

      const bool isArray = (type & WFT_ARRAY_MASK) != 0;

      type &= ~WFT_ARRAY_MASK;
      if ((type < WFT_BOOL) || (type > WFT_TEXT))
        goto push_frame_error;
      else if (isArray && (type == WFT_TEXT))
        return WCS_OP_NOTSUPP;
      else if (isArray)
        {
          switch (type)
          {
            case WFT_BOOL:
              rStack.Push (DBSArray (_SC (DBSBool*, NULL)));
              break;

            case WFT_CHAR:
              rStack.Push (DBSArray (_SC (DBSChar*, NULL)));
              break;

            case WFT_DATE:
              rStack.Push (DBSArray (_SC (DBSDate*, NULL)));
              break;

            case WFT_DATETIME:
              rStack.Push (DBSArray (_SC (DBSDateTime*, NULL)));
              break;

            case WFT_HIRESTIME:
              rStack.Push (DBSArray (_SC (DBSHiresTime*, NULL)));
              break;

            case WFT_INT8:
              rStack.Push (DBSArray (_SC (DBSInt8*, NULL)));
              break;

            case WFT_INT16:
              rStack.Push (DBSArray (_SC (DBSInt16*, NULL)));
              break;

            case WFT_INT32:
              rStack.Push (DBSArray (_SC (DBSInt32*, NULL)));
              break;

            case WFT_INT64:
              rStack.Push (DBSArray (_SC (DBSInt64*, NULL)));
              break;

            case WFT_UINT8:
              rStack.Push (DBSArray (_SC (DBSUInt8*, NULL)));
              break;

            case WFT_UINT16:
              rStack.Push (DBSArray (_SC (DBSUInt16*, NULL)));
              break;

            case WFT_UINT32:
              rStack.Push (DBSArray (_SC (DBSUInt32*, NULL)));
              break;

            case WFT_UINT64:
              rStack.Push (DBSArray (_SC (DBSUInt64*, NULL)));
              break;

            case WFT_REAL:
              rStack.Push (DBSArray (_SC (DBSReal*, NULL)));
              break;

            case WFT_RICHREAL:
              rStack.Push (DBSArray (_SC (DBSRichReal*, NULL)));
              break;

            case WFT_TEXT:
              assert (false);
            default:
              throw ConnectionException (
                        "Server internal error:\n"
                        "Invalid type encountered, though it passed type checks!",
                        _EXTRA (type)
                                        );
          }
          return WCS_OK;
        }

      switch (type)
      {
        case WFT_BOOL:
          rStack.Push (DBSBool ());
          break;

        case WFT_CHAR:
          rStack.Push (DBSChar ());
          break;

        case WFT_DATE:
          rStack.Push (DBSDate ());
          break;

        case WFT_DATETIME:
          rStack.Push (DBSDateTime ());
          break;

        case WFT_HIRESTIME:
          rStack.Push (DBSHiresTime ());
          break;

        case WFT_INT8:
          rStack.Push (DBSInt8 ());
          break;

        case WFT_INT16:
          rStack.Push (DBSInt16 ());
          break;

        case WFT_INT32:
          rStack.Push (DBSInt32 ());
          break;

        case WFT_INT64:
          rStack.Push (DBSInt64 ());
          break;

        case WFT_UINT8:
          rStack.Push (DBSUInt8 ());
          break;

        case WFT_UINT16:
          rStack.Push (DBSUInt16 ());
          break;

        case WFT_UINT32:
          rStack.Push (DBSUInt32 ());
          break;

        case WFT_UINT64:
          rStack.Push (DBSUInt64 ());
          break;

        case WFT_REAL:
          rStack.Push (DBSReal ());
          break;

        case WFT_RICHREAL:
          rStack.Push (DBSRichReal ());
          break;

        case WFT_TEXT:
          rStack.Push (DBSText ());
          break;

        default:
          throw ConnectionException (
                    "Server internal error:\n"
                    "Invalid type encountered, though it passed type checks!",
                    _EXTRA (type)
                                    );
      }
    }
  else
    {
      static const uint_t            STACK_FIELDS_SIZE = 0x10;
      DBSFieldDescriptor             stackFields[STACK_FIELDS_SIZE];
      auto_array<DBSFieldDescriptor> heapFields;

      if ((*pDataOff + sizeof (uint16_t)) > dataSize)
        goto push_frame_error;

      const uint16_t fieldsCount = from_le_int16 (data + *pDataOff);
      *pDataOff += sizeof (uint16_t);

      if (fieldsCount == 0)
        return WCS_INVALID_ARGS;

      DBSFieldDescriptor* const fields_ = (fieldsCount <= STACK_FIELDS_SIZE) ?
                                          stackFields :
                                          heapFields.Reset (fieldsCount);

      for (uint_t field = 0; field < fieldsCount; ++field)
        {
          fields_[field].m_pFieldName = _RC (const char*, data + *pDataOff);

          const uint_t fieldLen = strlen (fields_[field].m_pFieldName);

          *pDataOff +=  fieldLen+ 1;
          if ((*pDataOff + sizeof (uint16_t) > dataSize)
              || (fieldLen == 0))
            {
              goto push_frame_error;
            }

          const uint16_t fieldType = from_le_int16 (data + *pDataOff);
          *pDataOff += sizeof (uint16_t);

          fields_[field].isArray     = IS_ARRAY (fieldType);
          fields_[field].m_FieldType = _SC (DBS_FIELD_TYPE,
                                            GET_BASIC_TYPE (fieldType));

          if ((fields_[field].m_FieldType < WFT_BOOL)
              || (fields_[field].m_FieldType > WFT_TEXT))
            {
              goto push_frame_error;
            }
          else if ((fields_[field].m_FieldType == WFT_TEXT)
                   && (fields_[field].isArray))
            {
              return WCS_OP_NOTSUPP;
            }
        }
      I_DBSHandler& dbs   = *rConn.Dbs().m_Dbs;
      I_DBSTable&   table = dbs.CreateTempTable (fieldsCount, fields_);

      rStack.Push (dbs, table);
    }

  return WCS_OK;

push_frame_error:
  throw ConnectionException (
                      "Frame with invalid content for stack push operation.",
                      _EXTRA (0)
                            );
  return WCS_GENERAL_ERR; //Not executed anyway!
}

static uint_t
read_value (StackValue&         dest,
            const uint32_t      type,
            const bool          isArray,
            const uint8_t*      data,
            const uint16_t      dataSize,
            uint_t* const       pDataOff)
{
  const uint_t minSize = isArray ? 0 : 1;

  uint_t result = 0;

  switch (type)
  {
  case WFT_BOOL:
    {
      DBSBool value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_CHAR:
    {
      DBSChar value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_DATE:
    {
      DBSDate value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_DATETIME:
    {
      DBSDateTime value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_HIRESTIME:
    {
      DBSHiresTime value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_INT8:
    {
      DBSInt8 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_INT16:
    {
      DBSInt16 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_INT32:
    {
      DBSInt32 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_INT64:
    {
      DBSInt64 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_UINT8:
    {
      DBSUInt8 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_UINT16:
    {
      DBSUInt16 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_UINT32:
    {
      DBSUInt32 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_UINT64:
    {
      DBSUInt64 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_REAL:
    {
      DBSReal value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_RICHREAL:
    {
      DBSRichReal value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  default:
    throw ConnectionException ("Client frame contains invalid update type.",
                               _EXTRA (type));
  }

  if (result > minSize)
    {
      *pDataOff += result;
      return result;
    }

  return 0;
}

uint_t
cmd_update_stack_top (ClientConnection& rConn, uint_t* const pDataOff)
{
  assert_cmds_values ();

  const uint8_t* const data     = rConn.Data ();
  const uint_t         dataSize = rConn.DataSize ();
  SessionStack&        rStack   = rConn.Stack ();
  uint16_t             type     = from_le_int16 (data + *pDataOff);

  if (rStack.Size () == 0)
    return WCS_INVALID_ARGS;

  StackValue& stackTop = rStack[rStack.Size () - 1];

  if ((*pDataOff + sizeof (uint16_t)) > dataSize)
    goto update_frame_error;

  try
  {
      *pDataOff += sizeof (uint16_t);
      if (type & WFT_FIELD_MASK)
        {
          type &= ~WFT_FIELD_MASK;

          const char* const fieldName = _RC (const char*,
                                               data + *pDataOff);
          const uint_t fieldLen = strlen (fieldName) + 1;

          *pDataOff += fieldLen;
          if ((fieldLen <= 1) ||
              ((*pDataOff + sizeof (uint64_t)) > dataSize))
            {
              goto update_frame_error;
            }

          const uint64_t rowIndex = from_le_int64 (data + *pDataOff);
          *pDataOff += sizeof (uint64_t);

          I_DBSTable&    table      = stackTop.GetOperand ().GetTable ();
          const uint32_t fieldIndex = table.GetFieldIndex (fieldName);

          StackValue field    = stackTop.GetOperand ().GetFieldAt (fieldIndex);
          StackValue rowValue = field.GetOperand ().GetValueAt (rowIndex);

          if (type & WFT_ARRAY_MASK)
            {
              type &= ~WFT_ARRAY_MASK;
              assert ((WFT_BOOL <= type) && (type <= WFT_TEXT));

              if (type == WFT_TEXT)
                return WCS_OP_NOTSUPP;

              if (*pDataOff + sizeof (uint16_t) > dataSize)
                goto update_frame_error;

              uint16_t elCount = from_le_int16 (data + *pDataOff);
              *pDataOff += sizeof (uint16_t);

              if (elCount == 0)
                goto update_frame_error;

              uint64_t fromPos = from_le_int64 (data + *pDataOff);
              *pDataOff += sizeof (uint64_t);

              if (*pDataOff > dataSize)
                goto update_frame_error;

              while ((*pDataOff < dataSize) && (elCount > 0))
                {
                  I_Operand& operand         = rowValue.GetOperand ();
                  StackValue arrayValuevalue = operand.GetValueAt (fromPos);

                  const uint_t len = read_value (arrayValuevalue,
                                                 type,
                                                 true,
                                                 data,
                                                 dataSize,
                                                 pDataOff);


                  if (len == 0)
                    goto update_frame_error;

                  ++fromPos, --elCount;
                  assert (*pDataOff <= dataSize);
                }

              if (elCount != 0)
                goto update_frame_error;
            }
          else if (type == WFT_TEXT)
            {
              DBSText fieldText;

              if (rowIndex < table.GetAllocatedRows ())
                rowValue.GetOperand().GetValue (fieldText);

              if ((*pDataOff + sizeof (uint64_t)) >= dataSize)
                goto update_frame_error;

              uint64_t offset = from_le_int64 (data + *pDataOff);
              *pDataOff += sizeof (uint64_t);

              do
                {
                  uint32_t ch    = 0;
                  uint_t   chLen = decode_utf8_char (data + *pDataOff, &ch);

                  if (chLen == 0)
                    goto update_frame_error;
                  else if (ch == 0)
                    return WCS_INVALID_ARGS;

                  fieldText.SetCharAtIndex (DBSChar (ch), offset++);

                  *pDataOff += chLen;
                  if (*pDataOff >= dataSize)
                    goto update_frame_error;
                }
              while (data[*pDataOff] != 0);
              *pDataOff += sizeof (uint8_t);

              assert (*pDataOff <= dataSize);
              rowValue.GetOperand ().SetValue (fieldText);
            }
          else
            {
              const uint_t elLen = read_value (rowValue,
                                               type,
                                               false,
                                               data,
                                               dataSize,
                                               pDataOff);
              if (elLen == 0)
                goto update_frame_error;

              assert (*pDataOff <= dataSize);
            }
        }
      else if (type & WFT_ARRAY_MASK)
        {
          type &= ~WFT_ARRAY_MASK;
          assert ((WFT_BOOL <= type) && (type <= WFT_TEXT));

          if (type == WFT_TEXT)
            return WCS_OP_NOTSUPP;

          if (*pDataOff + sizeof (uint16_t) > dataSize)
            goto update_frame_error;

          uint16_t elCount = from_le_int16 (data + *pDataOff);
          *pDataOff += sizeof (uint16_t);

          if (elCount == 0)
            goto update_frame_error;

          if (*pDataOff + sizeof (uint64_t) > dataSize)
            goto update_frame_error;

          uint64_t fromPos  = from_le_int64 (data + *pDataOff);
          *pDataOff        += sizeof (uint64_t);

          while ((*pDataOff < dataSize) && (elCount > 0))
            {
              I_Operand& operand    = stackTop.GetOperand ();
              StackValue arrayValue = operand.GetValueAt (fromPos);

              const uint_t len = read_value (arrayValue,
                                             type,
                                             true,
                                             data,
                                             dataSize,
                                             pDataOff);

              if (len == 0)
                goto update_frame_error;

              ++fromPos, --elCount;
              assert (*pDataOff <= dataSize);
            }

          if (elCount != 0)
            goto update_frame_error;
        }
      else if (type == WFT_TEXT)
        {
          DBSText fieldText;
          stackTop.GetOperand ().GetValue (fieldText);

          if ((*pDataOff + sizeof (uint64_t)) > dataSize)
            goto update_frame_error;

          uint64_t offset = from_le_int64 (data + *pDataOff);
          *pDataOff += sizeof (uint64_t);

          do
            {
              uint32_t ch    = 0;
              uint_t   chLen = decode_utf8_char (data + *pDataOff, &ch);

              if (chLen == 0)
                goto update_frame_error;
              else if (ch == 0)
                return WCS_INVALID_ARGS;

              fieldText.SetCharAtIndex (DBSChar (ch), offset++);

              *pDataOff += chLen;
              if (*pDataOff >= dataSize)
                goto update_frame_error;
            }
          while (data[*pDataOff] != 0);
          *pDataOff += sizeof (uint8_t);

          stackTop.GetOperand ().SetValue (fieldText);
        }
      else
        {
          assert ((WFT_BOOL <= type) && (type < WFT_TEXT));

          const uint_t elLen = read_value (stackTop,
                                           type,
                                           false,
                                           data,
                                           dataSize,
                                           pDataOff);
          if (elLen == 0)
            goto update_frame_error;

          assert (*pDataOff <= dataSize);
        }
  }
  catch (InterException& e)
  {
      if (e.GetExtra() == InterException::INVALID_OP_REQ)
        return WCS_TYPE_MISMATCH;

      throw;
  }
  catch (DBSException& e)
  {
      const uint_t extra = e.GetExtra ();
      if (extra == DBSException::FIELD_NOT_FOUND)
        return WCS_INVALID_FIELD;
      else if (extra == DBSException::ARRAY_INDEX_TOO_BIG)
        return WCS_INVALID_ARRAY_OFF;
      else if (extra == DBSException::STRING_INDEX_TOO_BIG)
        return WCS_INVALID_TEXT_OFF;
      else if (extra == DBSException::ROW_NOT_ALLOCATED)
        return WCS_INVALID_ROW;

      throw;
  }

  return WCS_OK;

update_frame_error:

    throw ConnectionException (
                    "Frame with invalid content for stack update operation.",
                    _EXTRA (0)
                              );
    return WCS_GENERAL_ERR; //Not executed anyway!
}


static uint_t
write_value (StackValue&      source,
             uint8_t* const   data,
             const uint16_t   maxDataSize)
{
  I_Operand& valueOp = source.GetOperand ();
  uint_t result = 0;

  switch (valueOp.GetType ())
  {
  case WFT_BOOL:
    {

      DBSBool value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WFT_CHAR:
    {
      DBSChar value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WFT_DATE:
    {
      DBSDate value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WFT_DATETIME:
    {
      DBSDateTime value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WFT_HIRESTIME:
    {
      DBSHiresTime value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WFT_INT8:
  case WFT_INT16:
  case WFT_INT32:
  case WFT_INT64:
    {
      DBSInt64 value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WFT_UINT8:
  case WFT_UINT16:
  case WFT_UINT32:
  case WFT_UINT64:
    {
      DBSUInt64 value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WFT_REAL:
    {
      DBSReal value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WFT_RICHREAL:
    {
      DBSRichReal value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  default:
    assert (false);
    result = 0;
  }

  return result;
}


uint_t
cmd_read_basic_stack_top (ClientConnection& rConn,
                          StackValue&       value,
                          uint_t* const     pDataOffset)
{
  uint8_t* const data        = rConn.Data ();
  const uint_t   maxDataSize = rConn.MaxSize ();

  assert (*pDataOffset <= maxDataSize);
  assert (rConn.Stack ().Size () > 0);
  assert ((value.GetOperand ().GetType() >= T_BOOL)
          && (value.GetOperand ().GetType () < T_TEXT));

  const uint16_t length = write_value (value,
                                       data + *pDataOffset,
                                       maxDataSize - *pDataOffset);
  if (length == 0)
    return WCS_LARGE_RESPONSE;
  else
    *pDataOffset += length;

  assert (*pDataOffset <= maxDataSize);

  return WCS_OK;
}

uint_t
cmd_read_array_stack_top (ClientConnection& rConn,
                          StackValue&       value,
                          uint64_t          hintOffset,
                          uint_t* const     pDataOffset)
{
  uint8_t* const data        = rConn.Data ();
  const uint_t   maxDataSize = rConn.MaxSize ();
  uint64_t       maxCount    = 0;

  assert (*pDataOffset < maxDataSize);

  {
    DBSArray temp;
    value.GetOperand ().GetValue (temp);

    if (*pDataOffset + sizeof (uint64_t) >= maxDataSize)
      return WCS_LARGE_RESPONSE;

    maxCount = temp.ElementsCount ();
    store_le_int64 (maxCount, data + *pDataOffset);
    *pDataOffset +=sizeof (uint64_t);

    if (maxCount == 0)
      return WCS_OK;

    if (hintOffset >= maxCount)
      hintOffset = 0;

    if (*pDataOffset + sizeof (uint64_t) >= maxDataSize)
      return WCS_LARGE_RESPONSE;

    store_le_int64 (hintOffset, data + *pDataOffset);
    *pDataOffset += sizeof (uint64_t);
  }

  for (uint64_t index = hintOffset; index < maxCount; ++index)
    {
      StackValue   el        = value.GetOperand ().GetValueAt (index);
      const uint_t writeSize = write_value (el,
                                            data + *pDataOffset,
                                            maxDataSize - *pDataOffset);

      assert (! el.GetOperand ().IsNull());

      if (writeSize == 0)
        {
          if (index == hintOffset)
            return WCS_LARGE_RESPONSE;
          else
            break;
        }
      *pDataOffset += writeSize;
    }

  return WCS_OK;
}

uint_t
cmd_read_text_stack_top (ClientConnection& rConn,
                         StackValue&       value,
                         uint64_t          hintOffset,
                         uint_t* const     pDataOffset)
{
  uint8_t* const data        = rConn.Data ();
  const uint_t   maxDataSize = rConn.MaxSize ();
  uint64_t       maxCount    = 0;

  assert (*pDataOffset < maxDataSize);
  assert (rConn.Stack ().Size () > 0);
  assert (value.GetOperand ().GetType() == T_TEXT);

  {
    DBSText temp;
    value.GetOperand ().GetValue (temp);

    if (*pDataOffset + sizeof (uint64_t) >= maxDataSize)
      return WCS_LARGE_ARGS;

    maxCount = temp.GetCharactersCount ();
    store_le_int64 (maxCount, data + *pDataOffset);
    *pDataOffset += sizeof (uint64_t);

    if (maxCount == 0)
      return WCS_OK;

    if (hintOffset >= maxCount)
      hintOffset = 0;
  }

  if (*pDataOffset + sizeof (uint64_t) >= maxDataSize)
    return WCS_LARGE_ARGS;

  store_le_int64 (hintOffset, data + *pDataOffset);
  *pDataOffset += sizeof (uint64_t);

  for (uint64_t index = hintOffset; index < maxCount; ++index)
    {
      StackValue   el        = value.GetOperand ().GetValueAt (index);
      const uint_t writeSize = write_value (el,
                                            data + *pDataOffset,
                                            maxDataSize - *pDataOffset);
      assert (! el.GetOperand ().IsNull());

      if (writeSize == 0)
        {
          if (index == hintOffset)
            return WCS_LARGE_RESPONSE;
          else
            break;
        }

      assert (writeSize > 1);

      *pDataOffset += writeSize - 1; //Don't include the null terminator yet

      assert (data[*pDataOffset] == 0);
    }

  assert (*pDataOffset < maxDataSize);
  assert (data[*pDataOffset] == 0);
  *pDataOffset += 1; //Make sure we count the null terminator too!

  return WCS_OK;
}

static uint_t
cmd_read_table_field_internal (ClientConnection& rConn,
                               StackValue&       tableValue,
                               uint_t            hintField,
                               uint64_t          hintRow,
                               uint64_t          hintArrayOff,
                               uint64_t          hintTextOff,
                               uint_t* const     pDataOffset)
{
  uint_t          cs          = WCS_OK;
  uint8_t* const  data        = rConn.Data ();
  const uint_t    maxDataSize = rConn.DataSize ();
  I_DBSTable&     table       = tableValue.GetOperand ().GetTable ();

  DBSFieldDescriptor fd      = table.GetFieldDescriptor (hintField);
  const uint_t       nameLen = strlen (fd.m_pFieldName) + 1;

  if (*pDataOffset + nameLen + sizeof (uint16_t) >= maxDataSize)
    return WCS_LARGE_RESPONSE;

  memcpy (data + *pDataOffset, fd.m_pFieldName, nameLen);
  *pDataOffset += nameLen;

  StackValue fieldVal = tableValue.GetOperand ().GetFieldAt (hintField);
  StackValue rowValue = fieldVal.GetOperand ().GetValueAt (hintRow);

  if (fd.isArray)
    {
      store_le_int16 (fd.m_FieldType | WFT_ARRAY_MASK, data + *pDataOffset);
      *pDataOffset += sizeof (uint16_t);

      if (fd.m_FieldType == WFT_TEXT)
        {
          assert (false);
          return WCS_GENERAL_ERR;
        }
      else if (hintTextOff != WIGNORE_OFF)
        return WCS_TYPE_MISMATCH;

      cs = cmd_read_array_stack_top (rConn,
                                     rowValue,
                                     hintArrayOff,
                                     pDataOffset);
    }
  else if (fd.m_FieldType == WFT_TEXT)
    {
      store_le_int16 (fd.m_FieldType, data + *pDataOffset);
      *pDataOffset += sizeof (uint16_t);

      if (hintArrayOff != WIGNORE_OFF)
        return WCS_TYPE_MISMATCH;

      cs = cmd_read_text_stack_top (rConn,
                                    rowValue,
                                    hintTextOff,
                                    pDataOffset);
    }
  else
    {
      store_le_int16 (fd.m_FieldType, data + *pDataOffset);
      *pDataOffset += sizeof (uint16_t);

      if ((hintArrayOff != WIGNORE_OFF) && (hintTextOff != WIGNORE_OFF))
        return WCS_TYPE_MISMATCH;

      cs = cmd_read_basic_stack_top (rConn, rowValue, pDataOffset);
    }

  return cs;
}

uint_t
cmd_read_field_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          uint64_t          hintRow,
                          uint64_t          hintArrayOff,
                          uint64_t          hintTextOff,
                          uint_t* const     pDataOffset)
{
  assert (*pDataOffset < rConn.MaxSize ());
  assert (rConn.Stack ().Size () > 0);
  assert (IS_FIELD (topValue.GetOperand ().GetType()));

  uint8_t* const    data  = rConn.Data ();
  I_DBSTable&       table = topValue.GetOperand ().GetTable ();
  const FIELD_INDEX field = topValue.GetOperand ().GetField ();

  DBSFieldDescriptor fieldDesc = table.GetFieldDescriptor (field);

  const uint_t minSize = sizeof (uint64_t) * sizeof (uint64_t);
  if (*pDataOffset + minSize > rConn.MaxSize ())
    return WCS_LARGE_RESPONSE;

  uint16_t type = fieldDesc.m_FieldType | WFT_FIELD_MASK;
  if (fieldDesc.isArray)
    type |= WFT_ARRAY_MASK;


  const uint64_t rowsCount = table.GetAllocatedRows ();
  store_le_int64 (rowsCount, data + *pDataOffset);
  *pDataOffset += sizeof (uint64_t);

  if (rowsCount == 0)
    return WCS_OK;

  if (hintRow >= rowsCount)
    hintRow = 0;

  uint_t status = WCS_GENERAL_ERR;
  for (uint64_t row = hintRow; row < rowsCount; ++row)
    {
      const uint16_t  prevOffset = *pDataOffset;

      StackValue  el = topValue.GetOperand ().GetFieldAt (row);

      if (fieldDesc.isArray)
        {
          if (hintTextOff != WIGNORE_OFF)
            return WCS_INVALID_ARGS;

          status = cmd_read_array_stack_top (rConn,
                                             el,
                                             hintArrayOff,
                                             pDataOffset);
        }
      else if (fieldDesc.m_FieldType == T_TEXT)
        {
          if (hintArrayOff != WIGNORE_OFF)
            return WCS_INVALID_ARGS;

          status = cmd_read_text_stack_top (rConn,
                                            el,
                                            hintTextOff,
                                            pDataOffset);
        }
      else
        {
          if ((hintArrayOff != WIGNORE_OFF) || (hintTextOff != WIGNORE_OFF))
              return WCS_INVALID_ARGS;

          status = cmd_read_basic_stack_top (rConn, el, pDataOffset);
        }

      if (status != WCS_OK)
        {
          if (row == hintRow)
            return status;
          else
            {
              //At least one iteration was good! Return what was fitted.
              *pDataOffset = prevOffset;
              return WCS_OK;
            }
        }
    }

  return status;
}

uint_t
cmd_read_table_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          const uint_t      hintField,
                          uint64_t          hintRow,
                          uint64_t          hintArrayOff,
                          uint64_t          hintTextOff,
                          uint_t* const     pDataOffset)
{
  assert (*pDataOffset < rConn.MaxSize ());
  assert (rConn.Stack ().Size () > 0);
  assert (IS_TABLE (topValue.GetOperand ().GetType()));

  uint8_t* const  data        = rConn.Data ();
  const uint_t    maxDataSize = rConn.MaxSize ();
  I_DBSTable&     table       = topValue.GetOperand ().GetTable ();

  const FIELD_INDEX fieldsCount = table.GetFieldsCount();
  const ROW_INDEX   rowsCount   = table.GetAllocatedRows ();
  if (hintField >= fieldsCount)
    return WCS_INVALID_ARGS;
  else
  {
      const uint_t minSize = sizeof (uint64_t) +
                             sizeof (uint64_t) +
                             sizeof (uint16_t);
      if (*pDataOffset + minSize >= maxDataSize)
        return WCS_LARGE_RESPONSE;
  }

  store_le_int64 (rowsCount, data + *pDataOffset);
  *pDataOffset += sizeof (uint64_t);

  if (rowsCount == 0)
    return WCS_OK;

  if (hintRow >= rowsCount)
    hintRow = 0;

  uint64_t currentRow = hintRow;

  store_le_int64 (currentRow, data + *pDataOffset);
  *pDataOffset += sizeof (uint64_t);

  if (fieldsCount > 0xFFFF)
    return WCS_OP_NOTSUPP;

  store_le_int16 (fieldsCount, data + *pDataOffset);
  *pDataOffset += sizeof (uint16_t);

  while (currentRow < rowsCount)
    {
      for (uint_t fieldId = 0; fieldId < fieldsCount; ++fieldId)
        {
          const uint16_t prevOffset = *pDataOffset;

          //Get as much fields values as we can as it fits,
          //but the hinted one should be first.
          const FIELD_INDEX fi = (fieldId + hintField) % fieldsCount;

          const uint_t cs = cmd_read_table_field_internal (rConn,
                                                           topValue,
                                                           fi,
                                                           currentRow,
                                                           hintArrayOff,
                                                           hintTextOff,
                                                           pDataOffset);
          if (cs != WCS_OK)
            {
              if ((fieldId == 0) && (currentRow == hintRow))
                return cs;
              else
                {
                  //At least one iteration was good! Return what was fitted.
                  *pDataOffset = prevOffset;
                  return WCS_OK;
                }
            }
        }
      ++currentRow;
    }

  return WCS_OK;
}
