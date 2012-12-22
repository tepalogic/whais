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


static const D_UINT NULL_ELEMENT_LEN = sizeof ("");

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
  assert (WFT_UINT8      == T_INT8);
  assert (WFT_UINT16     == T_INT16);
  assert (WFT_UINT32     == T_INT32);
  assert (WFT_UINT64     == T_INT64);
  assert (WFT_REAL       == T_REAL);
  assert (WFT_RICHREAL   == T_RICHREAL);
  assert (WFT_TEXT       == T_TEXT);
  assert (WFT_ARRAY_MASK == T_ARRAY_MASK);
  assert (WFT_FIELD_MASK == T_FIELD_MASK);
  assert (WFT_TABLE_MASK == T_TABLE_MASK);
}

D_UINT
cmd_pop_stack (ClientConnection& rConn, D_UINT16* const pDataOff)
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
cmd_push_stack (ClientConnection& rConn, D_UINT16* const pDataOff)
{
  assert_cmds_values();

  const D_UINT8* const data   = rConn.Data ();
  SessionStack&        rStack = rConn.Stack ();
  D_UINT16             type;

  if ((*pDataOff + sizeof (D_UINT16)) > rConn.DataSize ())
    goto push_frame_error;

  type       = from_le_int16 (data + *pDataOff);
  *pDataOff += sizeof (D_UINT16);

  if (type != WFT_TABLE_MASK)
    {
      const bool isArray = (type & WFT_ARRAY_MASK) != 0;

      type &= ~WFT_ARRAY_MASK;
      if ((type < WFT_BOOL) || (type > WFT_TEXT))
        goto push_frame_error;
      else if (isArray && (type == WFT_TEXT))
        return WCS_OP_NOTSUPP;
      else if (isArray)
        {
          rStack.Push (DBSArray ());
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

      if ((*pDataOff + sizeof (D_UINT16)) > rConn.DataSize ())
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
          if ((*pDataOff + sizeof (D_UINT16) > rConn.DataSize())
              || (fieldLen == 0))
            {
              goto push_frame_error;
            }

          const D_UINT16 fieldType = from_le_int16 (data + *pDataOff);
          *pDataOff += sizeof (D_UINT16);

          fields_[field].isArray     = IS_ARRAY (fieldType);
          fields_[field].m_FieldType = _SC (DBS_FIELD_TYPE,
                                            GET_FIELD_TYPE (fieldType));

          if ((fields_[field].m_FieldType < WFT_BOOL)
              || (fields_[field].m_FieldType > WFT_TEXT))
            {
              goto push_frame_error;
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
            const W_VALUE_TYPE    type,
            const D_UINT8*      data,
            const D_UINT16      dataSize,
            D_UINT16* const     pDataOff)
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
cmd_update_stack_top (ClientConnection& rConn, D_UINT16* const pDataOff)
{
  assert_cmds_values ();

  const D_UINT8* const data   = rConn.Data ();
  SessionStack&        rStack = rConn.Stack ();
  const D_UINT16       flags  = from_le_int16 (data + *pDataOff);

  if (rStack.Size () == 0)
    return WCS_INVALID_ARGS;

  StackValue topValue      = rStack[rStack.Size () - 1];
  bool       clearTopValue = false;

  if ((*pDataOff + sizeof (D_UINT16)) > rConn.DataSize ())
    goto update_frame_error;

  try
  {
      *pDataOff += sizeof (D_UINT16);
      if (flags & WFT_FIELD_MASK)
        {
          const D_CHAR* const fieldName = _RC (const D_CHAR*,
                                               data + *pDataOff);

          *pDataOff += strlen (fieldName) + 1;
          if ((*pDataOff + sizeof (D_UINT64)) > rConn.DataSize ())
            goto update_frame_error;

          const D_UINT64 rowIndex = from_le_int64 (data + *pDataOff);
          *pDataOff += sizeof (D_UINT8);

          I_DBSTable&    table      = topValue.GetOperand ().GetTable ();
          const D_UINT32 fieldIndex = table.GetFieldIndex (fieldName);
          /* topValue will be a field now! */
          StackValue temp = topValue;
          topValue  = temp.GetOperand ().GetFieldAt (fieldIndex);
          clearTopValue = true;

          if (flags & WFT_ARRAY_MASK)
            {
              if ((flags & WFT_ARRAY_MASK) == WFT_TEXT)
                {
                  assert (clearTopValue);
                  topValue.Clear ();
                  return WCS_OP_NOTSUPP;
                }

              /* topValue will be an array field now! */
              temp     = topValue;
              topValue = temp.GetOperand ().GetValueAt (rowIndex);
              temp.Clear ();

              if (*pDataOff + sizeof (D_UINT16) > rConn.DataSize ())
                goto update_frame_error;

              D_UINT16 elCount = from_le_int16 (data + *pDataOff);
              *pDataOff += sizeof (D_UINT16);

              if (elCount == 0)
                goto update_frame_error;

              D_UINT64 fromPos = from_le_int64 (data + *pDataOff);
              *pDataOff += sizeof (D_UINT64);

              if (*pDataOff > rConn.DataSize ())
                goto update_frame_error;

              temp = topValue;
              const D_UINT16 valTypes = flags & 0xFF;
              while (elCount-- > 0)
                {
                  topValue  = temp.GetOperand ().GetValueAt (fromPos);
                  clearTopValue = true;

                  const D_UINT len = read_value (topValue,
                                                 valTypes,
                                                 data,
                                                 rConn.DataSize (),
                                                 pDataOff);
                  if (len <= NULL_ELEMENT_LEN)
                    goto update_frame_error;

                  assert (*pDataOff <= rConn.DataSize ());

                  ++fromPos;
                  topValue.Clear ();
                  clearTopValue = false;
                }
              temp.Clear ();
            }
          else if ((flags & 0xFF) == WFT_TEXT)
            {
              DBSText fieldText;
              topValue.GetOperand ().GetValue (fieldText);

              if ((*pDataOff + NULL_ELEMENT_LEN + sizeof (D_UINT64)) >
                  rConn.DataSize ())
                {
                  goto update_frame_error;
                }

              D_UINT64 fromPos = from_le_int64 (data + *pDataOff);
              *pDataOff += sizeof (D_UINT64);
              do
                {
                  D_UINT32 ch    = 0;
                  D_UINT   chLen = decode_utf8_char (data + *pDataOff, &ch);

                  if ((chLen == 0)
                      || (ch == 0)
                      || (*pDataOff + chLen > rConn.DataSize ()))
                    {
                      goto update_frame_error;
                    }

                  *pDataOff += chLen;
                  fieldText.SetCharAtIndex (DBSChar (ch), fromPos++);
                }
              while (data[*pDataOff] != 0);

              topValue.GetOperand ().SetValue (fieldText);
            }
          else
            {
              const D_UINT elLen = read_value (topValue,
                                               flags & 0xFF,
                                               data,
                                               rConn.DataSize (),
                                               pDataOff);
              if (elLen <= NULL_ELEMENT_LEN)
                goto update_frame_error;

              assert (*pDataOff <= rConn.DataSize ());
            }
        }
      else if (flags & WFT_ARRAY_MASK)
        {
          if ((flags & 0xFF) == WFT_TEXT)
            {
              assert (clearTopValue);
              topValue.Clear ();
              return WCS_OP_NOTSUPP;
            }


          if (*pDataOff + sizeof (D_UINT16) > rConn.DataSize ())
            goto update_frame_error;

          D_UINT16 elCount = from_le_int16 (data + *pDataOff);
          *pDataOff += sizeof (D_UINT16);

          if (elCount == 0)
            goto update_frame_error;

          if (*pDataOff + sizeof (D_UINT64) > rConn.DataSize ())
            goto update_frame_error;

          D_UINT64 fromPos =  from_le_int64 (data + *pDataOff);
          *pDataOff        += sizeof (D_UINT64);

          StackValue temp = topValue;
          while (elCount-- > 0)
            {
              topValue  = temp.GetOperand ().GetValueAt (fromPos);
              clearTopValue = true;

              const D_UINT len = read_value (topValue,
                                             flags & 0xFF,
                                             data,
                                             rConn.DataSize (),
                                             pDataOff);
              if (len <= NULL_ELEMENT_LEN)
                goto update_frame_error;

              assert (*pDataOff <= rConn.DataSize ());

              ++fromPos;
              topValue.Clear ();
              clearTopValue = false;
            }
        }
      else if ((flags & 0xFF) == WFT_TEXT)
        {
          DBSText fieldText;
          topValue.GetOperand ().GetValue (fieldText);

          if ((*pDataOff + sizeof (D_UINT8) + sizeof (D_UINT64)) >
              rConn.DataSize ())
            {
              goto update_frame_error;
            }

          D_UINT64 fromPos = from_le_int64 (data + *pDataOff);
          *pDataOff += sizeof (D_UINT64);

          do
            {
              D_UINT32 ch    = 0;
              D_UINT   chLen = decode_utf8_char (data + *pDataOff, &ch);

              if ((chLen == 0)
                  || (ch == 0)
                  || (*pDataOff + chLen > rConn.DataSize ()))
                {
                  goto update_frame_error;
                }

              *pDataOff += chLen;
              fieldText.SetCharAtIndex (DBSChar (ch), fromPos++);
            }
          while (data[*pDataOff] != 0);
          topValue.GetOperand ().SetValue (fieldText);
        }
      else
        {
          const D_UINT elLen = read_value (topValue,
                                           flags & 0xFF,
                                           data,
                                           rConn.DataSize (),
                                           pDataOff);
          if (elLen <= NULL_ELEMENT_LEN)
            goto update_frame_error;

          assert (*pDataOff <= rConn.DataSize ());
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


static D_UINT
cmd_read_basic_internal (ClientConnection& rConn,
                         StackValue&       value,
                         const bool        writeType,
                         D_UINT16* const   pDataOffset)
{
  D_UINT8* const data = rConn.Data ();

  assert (*pDataOffset < rConn.MaxSize ());
  assert (rConn.Stack ().Size () > 0);
  assert ((value.GetOperand ().GetType() >= T_BOOL)
          && (value.GetOperand ().GetType () < T_TEXT));

  if (writeType)
    {
      assert (*pDataOffset + sizeof (D_UINT16) < rConn.MaxSize ());

      store_le_int16 (value.GetOperand ().GetType (), data + *pDataOffset);
      *pDataOffset += sizeof (D_UINT16);
    }

  const D_UINT16 length = write_value (value,
                                       data + *pDataOffset,
                                       rConn.MaxSize () - *pDataOffset);
  if (length == 0)
    return WCS_LARGE_ARGS;
  else
    *pDataOffset += length;

  return WCS_OK;
}

D_UINT
cmd_read_basic_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          D_UINT16* const   pDataOffset)
{
  return cmd_read_basic_internal (rConn, topValue, true, pDataOffset);
}

static D_UINT
cmd_read_array_internal (ClientConnection& rConn,
                         StackValue&       value,
                         const D_UINT64    hintPosition,
                         const bool        writeType,
                         D_UINT16* const   pDataOffset)
{
  D_UINT8* const data     = rConn.Data ();
  D_UINT64       maxCount = 0;

  assert (*pDataOffset < rConn.MaxSize ());
  assert (rConn.Stack ().Size () > 0);
  assert (IS_ARRAY (value.GetOperand ().GetType()));

  {
    DBSArray temp;
    value.GetOperand ().GetValue (temp);

    assert ((temp.ElementsType () >= T_BOOL)
            && (temp.ElementsType () < T_TEXT));

    if (writeType)
      {
        assert (*pDataOffset + sizeof (D_UINT16) < rConn.MaxSize ());

        store_le_int16 (temp.ElementsType () | WFT_ARRAY_MASK,
                        data + *pDataOffset);
        *pDataOffset += sizeof (D_UINT16);
      }

    if (*pDataOffset + sizeof (D_UINT64) >= rConn.MaxSize ())
      return WCS_LARGE_ARGS;

    maxCount = temp.ElementsCount ();
    store_le_int64 (maxCount, data + *pDataOffset);
    *pDataOffset +=sizeof (D_UINT64);

    if ((hintPosition >= maxCount) && (maxCount > 0))
      return WCS_INVALID_ARGS;
    else if (maxCount == 0)
      return WCS_OK;
  }

  if (*pDataOffset + sizeof (D_UINT64) >= rConn.MaxSize ())
    return WCS_LARGE_ARGS;

  store_le_int64 (hintPosition, data + *pDataOffset);
  *pDataOffset += sizeof (D_UINT64);

  for (D_UINT64 index = hintPosition; index < maxCount; ++index)
    {
      StackValue   el        = value.GetOperand ().GetValueAt (index);
      const D_UINT writeSize = write_value (el,
                                            data + *pDataOffset,
                                            rConn.MaxSize () - *pDataOffset);

      assert (! el.GetOperand ().IsNull());

      if (writeSize == 0)
        {
          if (index == hintPosition)
            return WCS_LARGE_ARGS;
          else
            break;
        }

      assert (writeSize > 1);

      *pDataOffset += writeSize - 1; //Don't include the null terminator
    }

  *pDataOffset += 1; //Add the null terminator

  return WCS_OK;
}

D_UINT
cmd_read_array_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          D_UINT64          hintPosition,
                          D_UINT16* const   pDataOffset)
{
  return cmd_read_array_internal (rConn,
                                  topValue,
                                  hintPosition,
                                  true,
                                  pDataOffset);
}

static D_UINT
cmd_read_text_internal (ClientConnection& rConn,
                        StackValue&       value,
                        const D_UINT64    hintPosition,
                        const bool        writeType,
                        D_UINT16* const   pDataOffset)
{
  D_UINT8* const data     = rConn.Data ();
  D_UINT64       maxCount = 0;

  assert (*pDataOffset < rConn.MaxSize ());
  assert (rConn.Stack ().Size () > 0);
  assert (value.GetOperand ().GetType() == T_TEXT);

  {
    DBSText temp;
    value.GetOperand ().GetValue (temp);

    if (writeType)
      {
        assert (*pDataOffset + sizeof (D_UINT16) < rConn.MaxSize ());

        store_le_int16 (WFT_TEXT, data + *pDataOffset);
        *pDataOffset = sizeof (D_UINT16);
      }

    if (*pDataOffset + sizeof (D_UINT64) >= rConn.MaxSize ())
      return WCS_LARGE_ARGS;

    maxCount = temp.GetCharactersCount ();
    store_le_int64 (maxCount, data + *pDataOffset);
    *pDataOffset +=sizeof (D_UINT64);

    if ((hintPosition >= maxCount) && (maxCount > 0))
      return WCS_INVALID_ARGS;
    else if (maxCount == 0)
      return WCS_OK;
  }

  if (*pDataOffset + sizeof (D_UINT64) >= rConn.MaxSize ())
    return WCS_LARGE_ARGS;

  store_le_int64 (hintPosition, data + *pDataOffset);
  *pDataOffset += sizeof (D_UINT64);

  for (D_UINT64 index = hintPosition; index < maxCount; ++index)
    {
      StackValue   el        = value.GetOperand ().GetValueAt (index);
      const D_UINT writeSize = write_value (el,
                                            data + *pDataOffset,
                                            rConn.MaxSize () - *pDataOffset);
      assert (! el.GetOperand ().IsNull());

      if (writeSize == 0)
        {
          if (index == hintPosition)
            return WCS_LARGE_ARGS;
          else
            break;
        }

      assert (writeSize > 1);

      *pDataOffset += writeSize - 1; //Don't include the null terminator
    }

  *pDataOffset += 1; //Make sure we count the null terminator too!

  return WCS_OK;
}

D_UINT
cmd_read_text_stack_top (ClientConnection& rConn,
                         StackValue&       topValue,
                         D_UINT64          hintPosition,
                         D_UINT16* const   pDataOffset)
{
  return cmd_read_text_internal (rConn,
                                 topValue,
                                 hintPosition,
                                 true,
                                 pDataOffset);
}

static D_UINT
cmd_read_field_internal (ClientConnection& rConn,
                         StackValue&       topValue,
                         D_UINT64          hintRow,
                         D_UINT64          hintPosition,
                         const bool        tableEntry,
                         D_UINT16* const   pDataOffset)
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
  else if (tableEntry)
    {
      //For a table entry print only one row. The hinted one.
      maxCount = hintRow + 1;
    }

  DBSFieldDescriptor fieldDesc = table.GetFieldDescriptor (field);

  const D_UINT minSize = sizeof (D_UINT16) +
                         strlen (fieldDesc.m_pFieldName) + 1 +
                         tableEntry ?
                               sizeof (D_UINT64) :
                               2 * sizeof (D_UINT64);
  if (*pDataOffset + minSize > rConn.MaxSize ())
    return WCS_LARGE_ARGS;

  D_UINT16 type = fieldDesc.m_FieldType;
  MARK_FIELD (type);
  if (fieldDesc.isArray)
    MARK_ARRAY (type);

  store_le_int16 (type, data + *pDataOffset);
  *pDataOffset += sizeof (D_UINT16);

  strcpy (_RC (D_CHAR*, data + *pDataOffset), fieldDesc.m_pFieldName);
  *pDataOffset += strlen (fieldDesc.m_pFieldName) + 1;

  if (! tableEntry)
    {
      //For a non table entry total rows count
      //and the starting row must have to be specified.

      const D_UINT64 rowsCount = table.GetAllocatedRows ();

      store_le_int64 (rowsCount, data + *pDataOffset);
      *pDataOffset += sizeof (D_UINT64);

      if (rowsCount > 0)
        {
          store_le_int64 (hintRow, data + *pDataOffset);
          *pDataOffset += sizeof (D_UINT64);
        }
      else
        return WCS_OK;
    }
  else
    {
      assert (table.GetAllocatedRows () > 0);
    }

  D_UINT status = WCS_GENERAL_ERR;
  for (D_UINT64 row = hintRow; row < maxCount; ++row)
    {
      const D_UINT16  prevOffset = *pDataOffset;

      StackValue  el = topValue.GetOperand ().GetFieldAt (row);

      if (fieldDesc.isArray)
        {
          assert (fieldDesc.m_FieldType != T_TEXT);
          status = cmd_read_array_internal (rConn,
                                            el,
                                            hintPosition,
                                            false,
                                            pDataOffset);
        }
      else if (fieldDesc.m_FieldType == T_TEXT)
        {
          status = cmd_read_text_internal (rConn,
                                           el,
                                           hintPosition,
                                           false,
                                           pDataOffset);
        }
      else
        status = cmd_read_basic_internal (rConn, el, false, pDataOffset);

      if (status != WCS_OK)
        {
          if (row == hintRow)
            return status;
          else
            {
              //At least one iteration was good! Return what was fitted.
              *pDataOffset = prevOffset;
              break;
            }
        }
    }

  return status;
}

D_UINT
cmd_read_field_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          D_UINT64          hintRow,
                          D_UINT64          hintPosition,
                          D_UINT16* const   pDataOffset)
{
  return cmd_read_field_internal (rConn,
                                  topValue,
                                  hintRow,
                                  hintPosition,
                                  false,
                                  pDataOffset);

}

D_UINT
cmd_read_table_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          D_UINT64          hintField,
                          D_UINT64          hintRow,
                          D_UINT64          hintPosition,
                          D_UINT16* const   pDataOffset)
{
  assert (*pDataOffset < rConn.MaxSize ());
  assert (rConn.Stack ().Size () > 0);
  assert (IS_TABLE (topValue.GetOperand ().GetType()));

  D_UINT8* const    data  = rConn.Data ();
  I_DBSTable&       table = topValue.GetOperand ().GetTable ();

  const FIELD_INDEX fieldsCount = table.GetFieldsCount();
  const ROW_INDEX   rowsCount  = table.GetAllocatedRows ();
  if ((hintField >= fieldsCount) || (hintRow >= rowsCount))
    return WCS_INVALID_ARGS;
  else
  {
      const D_UINT minSize = sizeof (D_UINT16) +
                             sizeof (D_UINT64) +
                             sizeof (D_UINT32);
      if (*pDataOffset + minSize >= rConn. MaxSize())
        return WCS_LARGE_ARGS;
  }


  store_le_int16 (WFT_TABLE_MASK, data + *pDataOffset);
  *pDataOffset += sizeof (D_UINT16);

  store_le_int64 (rowsCount, data + *pDataOffset);
  *pDataOffset += sizeof (D_UINT64);

  if ((rowsCount > 0) && (hintRow >= rowsCount))
    return WCS_INVALID_ARGS;
  else if (rowsCount > 0)
    {
      store_le_int32 (fieldsCount, data + *pDataOffset);
      *pDataOffset += sizeof (D_UINT32);

      for (D_UINT fieldId = 0; fieldId < fieldsCount; ++fieldId)
        {
          const D_UINT16 prevOffset = *pDataOffset;

          //Get as much fields values as we can as it fits,
          //but the hinted one should be first.
          const FIELD_INDEX field = (fieldId + hintPosition) % fieldsCount;
          StackValue        el    = topValue.GetOperand ().GetFieldAt (field);

          const D_UINT cs = cmd_read_field_internal (rConn,
                                                     el,
                                                     hintRow,
                                                     hintPosition,
                                                     true,
                                                     pDataOffset);
          if (cs != WCS_OK)
            {
              if (fieldId == 0)
                return cs;
              else
                {
                  //At least one iteration was good! Return what was fitted.
                  *pDataOffset = prevOffset;
                  break;
                }
            }

          hintPosition = 0;
        }

    }

  return WCS_OK;
}
