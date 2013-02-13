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

D_UINT
cmd_pop_stack (ClientConnection& rConn, D_UINT* const pDataOff)
{
  assert_cmds_values();

  if ((*pDataOff + sizeof (D_UINT32)) > rConn.DataSize ())
    {
      throw ConnectionException (
                    "Got frame with invalid content for stack pop operation.",
                    _EXTRA (0)
                                );
    }
  SessionStack& rStack = rConn.Stack ();
  D_UINT32      count  = from_le_int32 (rConn.Data () + *pDataOff);

  *pDataOff += sizeof (D_UINT32);

  count = MIN (count, rStack.Size ());
  rStack.Pop (count);

  return WCS_OK;
}

D_UINT
cmd_push_stack (ClientConnection& rConn, D_UINT* const pDataOff)
{
  assert_cmds_values();

  const D_UINT8* const data     = rConn.Data ();
  const D_UINT         dataSize = rConn.DataSize ();
  SessionStack&        rStack   = rConn.Stack ();
  D_UINT16             type;

  if ((*pDataOff + sizeof (D_UINT16)) > dataSize)
    goto push_frame_error;

  type       = from_le_int16 (data + *pDataOff);
  *pDataOff += sizeof (D_UINT16);

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
      static const D_UINT            STACK_FIELDS_SIZE = 0x10;
      DBSFieldDescriptor             stackFields[STACK_FIELDS_SIZE];
      auto_array<DBSFieldDescriptor> heapFields;

      if ((*pDataOff + sizeof (D_UINT16)) > dataSize)
        goto push_frame_error;

      const D_UINT16 fieldsCount = from_le_int16 (data + *pDataOff);
      *pDataOff += sizeof (D_UINT16);

      if (fieldsCount == 0)
        return WCS_INVALID_ARGS;

      DBSFieldDescriptor* const fields_ = (fieldsCount <= STACK_FIELDS_SIZE) ?
                                          stackFields :
                                          heapFields.Reset (fieldsCount);

      for (D_UINT field = 0; field < fieldsCount; ++field)
        {
          fields_[field].m_pFieldName = _RC (const D_CHAR*, data + *pDataOff);

          const D_UINT fieldLen = strlen (fields_[field].m_pFieldName);

          *pDataOff +=  fieldLen+ 1;
          if ((*pDataOff + sizeof (D_UINT16) > dataSize)
              || (fieldLen == 0))
            {
              goto push_frame_error;
            }

          const D_UINT16 fieldType = from_le_int16 (data + *pDataOff);
          *pDataOff += sizeof (D_UINT16);

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

static D_UINT
read_value (StackValue&         dest,
            const D_UINT32      type,
            const D_UINT8*      data,
            const D_UINT16      dataSize,
            D_UINT* const       pDataOff)
{
  D_UINT result = 0;
  switch (type)
  {
  case WFT_BOOL:
    {
      DBSBool value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_CHAR:
    {
      DBSChar value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_DATE:
    {
      DBSDate value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_DATETIME:
    {
      DBSDateTime value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_HIRESTIME:
    {
      DBSHiresTime value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_INT8:
    {
      DBSInt8 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_INT16:
    {
      DBSInt16 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_INT32:
    {
      DBSInt32 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_INT64:
    {
      DBSInt64 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_UINT8:
    {
      DBSUInt8 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_UINT16:
    {
      DBSUInt16 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_UINT32:
    {
      DBSUInt32 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_UINT64:
    {
      DBSUInt64 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_REAL:
    {
      DBSReal value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case WFT_RICHREAL:
    {
      DBSRichReal value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  default:
    throw ConnectionException ("Client frame contains invalid update type.",
                               _EXTRA (type));
  }

  *pDataOff += result;
  return result;
}

D_UINT
cmd_update_stack_top (ClientConnection& rConn, D_UINT* const pDataOff)
{
  assert_cmds_values ();

  const D_UINT8* const data     = rConn.Data ();
  const D_UINT8        dataSize = rConn.DataSize ();
  SessionStack&        rStack   = rConn.Stack ();
  D_UINT16             type     = from_le_int16 (data + *pDataOff);

  if (rStack.Size () == 0)
    return WCS_INVALID_ARGS;

  StackValue& topValue      = rStack[rStack.Size () - 1];
  bool        clearTopValue = false;

  if ((*pDataOff + sizeof (D_UINT16)) > dataSize)
    goto update_frame_error;

  try
  {
      *pDataOff += sizeof (D_UINT16);
      if (type & WFT_FIELD_MASK)
        {
          type &= ~WFT_FIELD_MASK;

          const D_CHAR* const fieldName = _RC (const D_CHAR*,
                                               data + *pDataOff);
          const D_UINT fieldLen = strlen (fieldName) + 1;

          *pDataOff += fieldLen;
          if ((fieldLen <= 1) ||
              ((*pDataOff + sizeof (D_UINT64)) > dataSize))
            {
              goto update_frame_error;
            }

          const D_UINT64 rowIndex = from_le_int64 (data + *pDataOff);
          *pDataOff += sizeof (D_UINT64);

          I_DBSTable&    table      = topValue.GetOperand ().GetTable ();
          const D_UINT32 fieldIndex = table.GetFieldIndex (fieldName);

          /* topValue will be a field now! */
          StackValue temp = topValue;
          topValue  = temp.GetOperand ().GetFieldAt (fieldIndex);
          clearTopValue = true;

          if (type & WFT_ARRAY_MASK)
            {
              type &= ~WFT_ARRAY_MASK;
              assert ((WFT_BOOL <= type) && (type <= WFT_TEXT));

              if (type == WFT_TEXT)
                {
                  assert (clearTopValue);
                  topValue.Clear ();
                  return WCS_OP_NOTSUPP;
                }

              /* topValue will be an array field now! */
              temp     = topValue;
              topValue = temp.GetOperand ().GetValueAt (rowIndex);
              temp.Clear ();

              if (*pDataOff + sizeof (D_UINT16) > dataSize)
                goto update_frame_error;

              D_UINT16 elCount = from_le_int16 (data + *pDataOff);
              *pDataOff += sizeof (D_UINT16);

              if (elCount == 0)
                goto update_frame_error;

              D_UINT64 fromPos = from_le_int64 (data + *pDataOff);
              *pDataOff += sizeof (D_UINT64);

              if (*pDataOff > dataSize)
                goto update_frame_error;

              temp = topValue;
              while ((*pDataOff < dataSize)
                      && (elCount-- > 0))
                {
                  topValue  = temp.GetOperand ().GetValueAt (fromPos);
                  clearTopValue = true;

                  const D_UINT len = read_value (topValue,
                                                 type,
                                                 data,
                                                 dataSize,
                                                 pDataOff);
                  if (len == 0)
                    {
                      temp.Clear ();
                      goto update_frame_error;
                    }

                  assert (*pDataOff <= dataSize);

                  ++fromPos;
                  topValue.Clear ();
                  clearTopValue = false;
                }
              temp.Clear ();

              if (elCount != 0)
                goto update_frame_error;
            }
          else if (type == WFT_TEXT)
            {
              DBSText fieldText;
              topValue.GetOperand ().GetValue (fieldText);

              if ((*pDataOff + sizeof (D_UINT64)) >= dataSize)
                goto update_frame_error;

              D_UINT64 textOff = from_le_int64 (data + *pDataOff);
              *pDataOff += sizeof (D_UINT64);

              if (*pDataOff >= dataSize)
                goto update_frame_error;

              while (data[*pDataOff] != 0)
                {
                  D_UINT32 ch    = 0;
                  D_UINT   chLen = decode_utf8_char (data + *pDataOff, &ch);

                  if ((chLen == 0)
                      || (ch == 0)
                      || (*pDataOff + chLen > dataSize))
                    {
                      goto update_frame_error;
                    }

                  *pDataOff += chLen;
                  fieldText.SetCharAtIndex (DBSChar (ch), textOff++);
                }

              topValue.GetOperand ().SetValue (fieldText);
            }
          else
            {
              const D_UINT elLen = read_value (topValue,
                                               type,
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
            {
              assert (clearTopValue);

              topValue.Clear ();
              return WCS_OP_NOTSUPP;
            }

          if (*pDataOff + sizeof (D_UINT16) > dataSize)
            goto update_frame_error;

          D_UINT16 elCount = from_le_int16 (data + *pDataOff);
          *pDataOff += sizeof (D_UINT16);

          if (elCount == 0)
            goto update_frame_error;

          if (*pDataOff + sizeof (D_UINT64) > dataSize)
            goto update_frame_error;

          D_UINT64 fromPos =  from_le_int64 (data + *pDataOff);
          *pDataOff        += sizeof (D_UINT64);

          StackValue temp = topValue;
          while ((*pDataOff < dataSize)
                 && (elCount-- > 0))
            {
              topValue  = temp.GetOperand ().GetValueAt (fromPos);
              clearTopValue = true;

              const D_UINT len = read_value (topValue,
                                             type,
                                             data,
                                             dataSize,
                                             pDataOff);
              if (len <= 0)
                goto update_frame_error;

              assert (*pDataOff <= dataSize);

              ++fromPos;
              topValue.Clear ();
              clearTopValue = false;
            }

          if (elCount != 0)
            goto update_frame_error;
        }
      else if (type == WFT_TEXT)
        {
          DBSText fieldText;
          topValue.GetOperand ().GetValue (fieldText);

          if ((*pDataOff + sizeof (D_UINT64)) > dataSize)
            goto update_frame_error;

          D_UINT64 offset = from_le_int64 (data + *pDataOff);
          *pDataOff += sizeof (D_UINT64);

          while ((data[*pDataOff] != 0)
                 && (*pDataOff < dataSize))
            {
              D_UINT32 ch    = 0;
              D_UINT   chLen = decode_utf8_char (data + *pDataOff, &ch);

              if (chLen == 0)
                goto update_frame_error;

              *pDataOff += chLen;
              fieldText.SetCharAtIndex (DBSChar (ch), offset++);
            }
          if (*pDataOff >= dataSize)
            goto update_frame_error;

          fieldText.SetCharAtIndex (DBSChar (), offset);

          topValue.GetOperand ().SetValue (fieldText);
        }
      else
        {
          assert ((WFT_BOOL <= type) && (type < WFT_TEXT));
          const D_UINT elLen = read_value (topValue,
                                           type,
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
      if (clearTopValue)
        topValue.Clear ();

      if (e.GetExtra() == InterException::INVALID_OP_REQ)
        return WCS_INVALID_ARGS;

      throw;
  }
  catch (DBSException& e)
  {
      if (clearTopValue)
        topValue.Clear ();

      if ((e.GetExtra() == DBSException::FIELD_NOT_FOUND)
          || (e.GetExtra () == DBSException::ARRAY_INDEX_TOO_BIG)
          || (e.GetExtra () == DBSException::STRING_INDEX_TOO_BIG))
        {
          return WCS_INVALID_ARGS;
        }

      throw;
  }
  catch (...)
  {
      if (clearTopValue)
        topValue.Clear ();

      throw;
  }

  if (clearTopValue)
    topValue.Clear ();

  return WCS_OK;

update_frame_error:

  if (clearTopValue)
    topValue.Clear ();

    throw ConnectionException (
                    "Frame with invalid content for stack update operation.",
                    _EXTRA (0)
                              );
    return WCS_GENERAL_ERR; //Not executed anyway!
}


static D_UINT
write_value (StackValue&      source,
             D_UINT8* const   data,
             const D_UINT16   maxDataSize)
{
  I_Operand& valueOp = source.GetOperand ();
  D_UINT result = 0;

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


D_UINT
cmd_read_basic_stack_top (ClientConnection& rConn,
                          StackValue&       value,
                          D_UINT* const     pDataOffset)
{
  D_UINT8* const data        = rConn.Data ();
  const D_UINT   maxDataSize = rConn.MaxSize ();

  assert (*pDataOffset <= maxDataSize);
  assert (rConn.Stack ().Size () > 0);
  assert ((value.GetOperand ().GetType() >= T_BOOL)
          && (value.GetOperand ().GetType () < T_TEXT));

  const D_UINT16 length = write_value (value,
                                       data + *pDataOffset,
                                       maxDataSize - *pDataOffset);
  if (length == 0)
    return WCS_LARGE_RESPONSE;
  else
    *pDataOffset += length;

  assert (*pDataOffset <= maxDataSize);

  return WCS_OK;
}

D_UINT
cmd_read_array_stack_top (ClientConnection& rConn,
                          StackValue&       value,
                          const D_UINT64    hintOffset,
                          D_UINT* const     pDataOffset)
{
  D_UINT8* const data        = rConn.Data ();
  const D_UINT   maxDataSize = rConn.MaxSize ();
  D_UINT64       maxCount    = 0;

  assert (*pDataOffset < maxDataSize);

  {
    DBSArray temp;
    value.GetOperand ().GetValue (temp);

    if (*pDataOffset + sizeof (D_UINT64) >= maxDataSize)
      return WCS_LARGE_RESPONSE;

    maxCount = temp.ElementsCount ();
    store_le_int64 (maxCount, data + *pDataOffset);
    *pDataOffset +=sizeof (D_UINT64);

    if ((hintOffset >= maxCount) && (maxCount > 0))
      return WCS_INVALID_ARGS;
    else if (maxCount == 0)
      return WCS_OK;
  }

  *pDataOffset += sizeof (D_UINT64);
  if (*pDataOffset + sizeof (D_UINT64) >= maxDataSize)
    return WCS_LARGE_RESPONSE;

  store_le_int64 (hintOffset, data + *pDataOffset);
  *pDataOffset += sizeof (D_UINT64);

  for (D_UINT64 index = hintOffset; index < maxCount; ++index)
    {
      StackValue   el        = value.GetOperand ().GetValueAt (index);
      const D_UINT writeSize = write_value (el,
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

      assert (writeSize > 2);

      *pDataOffset += writeSize;
    }

  return WCS_OK;
}

D_UINT
cmd_read_text_stack_top (ClientConnection& rConn,
                         StackValue&       value,
                         const D_UINT64    hintOffset,
                         D_UINT* const     pDataOffset)
{
  D_UINT8* const data        = rConn.Data ();
  const D_UINT   maxDataSize = rConn.MaxSize ();
  D_UINT64       maxCount    = 0;

  assert (*pDataOffset < maxDataSize);
  assert (rConn.Stack ().Size () > 0);
  assert (value.GetOperand ().GetType() == T_TEXT);

  {
    DBSText temp;
    value.GetOperand ().GetValue (temp);

    if (*pDataOffset + sizeof (D_UINT64) >= maxDataSize)
      return WCS_LARGE_ARGS;

    maxCount = temp.GetCharactersCount ();
    store_le_int64 (maxCount, data + *pDataOffset);

    if ((hintOffset >= maxCount) && (maxCount > 0))
      return WCS_INVALID_ARGS;
    else if (maxCount == 0)
      return WCS_OK;
  }

  *pDataOffset +=sizeof (D_UINT64);
  if (*pDataOffset + sizeof (D_UINT64) >= maxDataSize)
    return WCS_LARGE_ARGS;

  store_le_int64 (hintOffset, data + *pDataOffset);
  *pDataOffset += sizeof (D_UINT64);

  for (D_UINT64 index = hintOffset; index < maxCount; ++index)
    {
      StackValue   el        = value.GetOperand ().GetValueAt (index);
      const D_UINT writeSize = write_value (el,
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

      *pDataOffset += writeSize - 1; //Don't include the null terminator
      assert (data[*pDataOffset] == 0);
    }

  assert (*pDataOffset < maxDataSize);
  assert (data[*pDataOffset] == 0);
  *pDataOffset += 1; //Make sure we count the null terminator too!

  return WCS_OK;
}

static D_UINT
cmd_read_table_field_internal (ClientConnection& rConn,
                               StackValue&       topValue,
                               D_UINT            hintField,
                               D_UINT64          hintRow,
                               D_UINT64          hintArrayOff,
                               D_UINT64          hintTextOff,
                               D_UINT* const     pDataOffset)
{
  D_UINT          cs          = WCS_OK;
  D_UINT8* const  data        = rConn.Data ();
  const D_UINT    maxDataSize = rConn.DataSize ();
  I_DBSTable&     table       = topValue.GetOperand ().GetTable ();
  const D_UINT64  rowsCount   = table.GetAllocatedRows ();

  if (hintRow >= rowsCount)
    return WCS_INVALID_ARGS;

  DBSFieldDescriptor fd      = table.GetFieldDescriptor (hintField);
  const D_UINT       nameLen = strlen (fd.m_pFieldName) + 1;

  if (*pDataOffset + nameLen + sizeof (D_UINT16) >= maxDataSize)
    return WCS_LARGE_RESPONSE;

  memcpy (data + *pDataOffset, fd.m_pFieldName, nameLen);
  store_le_int16 (WFT_ARRAY_MASK | fd.m_FieldType, data + *pDataOffset);

  if (fd.isArray)
    {
      if (fd.m_FieldType == WFT_TEXT)
        {
          assert (false);
          return WCS_GENERAL_ERR;
        }
      else if (hintTextOff != WIGNORE_OFF)
        return WCS_OP_NOTSUPP;

      cs = cmd_read_array_stack_top (rConn,
                                     topValue,
                                     hintArrayOff,
                                     pDataOffset);
    }
  else if (fd.m_FieldType == WFT_TEXT)
    {
      if (hintArrayOff != WIGNORE_OFF)
        return WCS_INVALID_ARGS;

      cs = cmd_read_text_stack_top (rConn,
                                    topValue,
                                    hintTextOff,
                                    pDataOffset);
    }
  else
    {
      if ((hintArrayOff != WIGNORE_OFF) && (hintTextOff != WIGNORE_OFF))
        return WCS_INVALID_ARGS;

      cs = cmd_read_basic_stack_top (rConn, topValue, pDataOffset);
    }

  return cs;
}

D_UINT
cmd_read_field_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          D_UINT64          hintRow,
                          D_UINT64          hintArrayOff,
                          D_UINT64          hintTextOff,
                          D_UINT* const     pDataOffset)
{
  assert (*pDataOffset < rConn.MaxSize ());
  assert (rConn.Stack ().Size () > 0);
  assert (IS_FIELD (topValue.GetOperand ().GetType()));

  D_UINT8* const    data  = rConn.Data ();
  I_DBSTable&       table = topValue.GetOperand ().GetTable ();
  const FIELD_INDEX field = topValue.GetOperand ().GetField ();

  D_UINT64 maxCount = table.GetAllocatedRows ();
  if (hintRow >= maxCount)
    return WCS_INVALID_ARGS;

  DBSFieldDescriptor fieldDesc = table.GetFieldDescriptor (field);

  const D_UINT minSize = sizeof (D_UINT64) * sizeof (D_UINT64);
  if (*pDataOffset + minSize > rConn.MaxSize ())
    return WCS_LARGE_RESPONSE;

  D_UINT16 type = fieldDesc.m_FieldType | WFT_FIELD_MASK;
  if (fieldDesc.isArray)
    type |= WFT_ARRAY_MASK;


  const D_UINT64 rowsCount = table.GetAllocatedRows ();
  store_le_int64 (rowsCount, data + *pDataOffset);
  *pDataOffset += sizeof (D_UINT64);


  if ((hintRow > rowsCount) && (rowsCount > 0))
    return WCS_INVALID_ARGS;
  else if (rowsCount == 0)
    return WCS_OK;

  D_UINT status = WCS_GENERAL_ERR;
  for (D_UINT64 row = hintRow; row < maxCount; ++row)
    {
      const D_UINT16  prevOffset = *pDataOffset;

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
              status = WCS_OK;
              break;
            }
        }
    }

  return status;
}

D_UINT
cmd_read_table_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          const D_UINT      hintField,
                          const D_UINT64    hintRow,
                          const D_UINT64    hintArrayOff,
                          const D_UINT64    hintTextOff,
                          D_UINT* const     pDataOffset)
{
  assert (*pDataOffset < rConn.MaxSize ());
  assert (rConn.Stack ().Size () > 0);
  assert (IS_TABLE (topValue.GetOperand ().GetType()));

  D_UINT8* const  data        = rConn.Data ();
  const D_UINT    maxDataSize = rConn.MaxSize ();
  I_DBSTable&     table       = topValue.GetOperand ().GetTable ();

  const FIELD_INDEX fieldsCount = table.GetFieldsCount();
  const ROW_INDEX   rowsCount  = table.GetAllocatedRows ();
  if ((hintField >= fieldsCount) || (hintRow >= rowsCount))
    return WCS_INVALID_ARGS;
  else
  {
      const D_UINT minSize = sizeof (D_UINT64) +
                             sizeof (D_UINT64) +
                             sizeof (D_UINT16);
      if (*pDataOffset + minSize >= maxDataSize)
        return WCS_LARGE_RESPONSE;
  }

  store_le_int64 (rowsCount, data + *pDataOffset);
  *pDataOffset += sizeof (D_UINT64);

  if ((rowsCount > 0) && (hintRow >= rowsCount))
    return WCS_INVALID_ARGS;
  else if (rowsCount > 0)
    {
      store_le_int64 (hintRow, data + *pDataOffset);
      *pDataOffset += sizeof (D_UINT64);

      if (fieldsCount > 0xFFFF)
        return WCS_OP_NOTSUPP;

      store_le_int16 (fieldsCount, data + *pDataOffset);
      *pDataOffset += sizeof (D_UINT32);

      D_UINT64 currentRow = hintRow;
      while (currentRow < rowsCount)
        {
          for (D_UINT fieldId = 0; fieldId < fieldsCount; ++fieldId)
            {
              const D_UINT16 prevOffset = *pDataOffset;

              //Get as much fields values as we can as it fits,
              //but the hinted one should be first.
              const FIELD_INDEX fi = (fieldId + hintField) % fieldsCount;
              StackValue        el = topValue.GetOperand ().GetFieldAt (fi);

              const D_UINT cs = cmd_read_table_field_internal (rConn,
                                                               el,
                                                               fi,
                                                               currentRow,
                                                               hintArrayOff,
                                                               hintTextOff,
                                                               pDataOffset);
              if (cs != WCS_OK)
                {
                  if ((fieldId == 0) && (currentRow != hintRow))
                    return cs;
                  else
                    {
                      //At least one iteration was good! Return what was fitted.
                      *pDataOffset = prevOffset;
                      break;
                    }
                }
            }
        }
    }

  return WCS_OK;
}
