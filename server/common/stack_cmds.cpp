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
  assert (FT_BOOL       == T_BOOL);
  assert (FT_CHAR       == T_CHAR);
  assert (FT_DATE       == T_DATE);
  assert (FT_DATETIME   == T_DATETIME);
  assert (FT_HIRESTIME  == T_HIRESTIME);
  assert (FT_INT8       == T_INT8);
  assert (FT_INT16      == T_INT16);
  assert (FT_INT32      == T_INT32);
  assert (FT_INT64      == T_INT64);
  assert (FT_UINT8      == T_INT8);
  assert (FT_UINT16     == T_INT16);
  assert (FT_UINT32     == T_INT32);
  assert (FT_UINT64     == T_INT64);
  assert (FT_REAL       == T_REAL);
  assert (FT_RICHREAL   == T_RICHREAL);
  assert (FT_TEXT       == T_TEXT);
  assert (FT_ARRAY_MASK == T_ARRAY_MASK);
  assert (FT_FIELD_MASK == T_FIELD_MASK);
  assert (FT_TABLE_MASK == T_TABLE_MASK);
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

  return CS_OK;
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

  if (type != FT_TABLE_MASK)
    {
      const bool isArray = (type & FT_ARRAY_MASK) != 0;

      type &= ~FT_ARRAY_MASK;
      if ((type < FT_BOOL) || (type > FT_TEXT))
        goto push_frame_error;
      else if (isArray && (type == FT_TEXT))
        return CS_OP_NOTSUPP;
      else if (isArray)
        {
          rStack.Push (DBSArray ());
          return CS_OK;
        }

      switch (type)
      {
        case FT_BOOL:
          rStack.Push (DBSBool ());
          break;

        case FT_CHAR:
          rStack.Push (DBSChar ());
          break;

        case FT_DATE:
          rStack.Push (DBSDate ());
          break;

        case FT_DATETIME:
          rStack.Push (DBSDateTime ());
          break;

        case FT_HIRESTIME:
          rStack.Push (DBSHiresTime ());
          break;

        case FT_INT8:
          rStack.Push (DBSInt8 ());
          break;

        case FT_INT16:
          rStack.Push (DBSInt16 ());
          break;

        case FT_INT32:
          rStack.Push (DBSInt32 ());
          break;

        case FT_INT64:
          rStack.Push (DBSInt64 ());
          break;

        case FT_UINT8:
          rStack.Push (DBSUInt8 ());
          break;

        case FT_UINT16:
          rStack.Push (DBSUInt16 ());
          break;

        case FT_UINT32:
          rStack.Push (DBSUInt32 ());
          break;

        case FT_UINT64:
          rStack.Push (DBSUInt64 ());
          break;

        case FT_REAL:
          rStack.Push (DBSReal ());
          break;

        case FT_RICHREAL:
          rStack.Push (DBSRichReal ());
          break;

        case FT_TEXT:
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
        return CS_INVALID_ARGS;

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

          if ((fields_[field].m_FieldType < FT_BOOL)
              || (fields_[field].m_FieldType > FT_TEXT))
            {
              goto push_frame_error;
            }
        }
      I_DBSHandler& dbs   = *rConn.Dbs().m_Dbs;
      I_DBSTable&   table = dbs.CreateTempTable (fieldsCount, fields_);

      rStack.Push (dbs, table);
    }

  return CS_OK;

push_frame_error:
  throw ConnectionException (
                      "Frame with invalid content for stack push operation.",
                      _EXTRA (0)
                            );
  return CS_GENERAL_ERR; //Not executed anyway!
}

static D_UINT
read_value (StackValue&         dest,
            const VALUE_TYPE    type,
            const D_UINT8*      data,
            const D_UINT16      dataSize,
            D_UINT16* const     pDataOff)
{
  D_UINT result = 0;
  switch (type)
  {
  case FT_BOOL:
    {
      DBSBool value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_CHAR:
    {
      DBSChar value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_DATE:
    {
      DBSDate value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_DATETIME:
    {
      DBSDateTime value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_HIRESTIME:
    {
      DBSHiresTime value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_INT8:
    {
      DBSInt8 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_INT16:
    {
      DBSInt16 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_INT32:
    {
      DBSInt32 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_INT64:
    {
      DBSInt64 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_UINT8:
    {
      DBSUInt8 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_UINT16:
    {
      DBSUInt16 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_UINT32:
    {
      DBSUInt32 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_UINT64:
    {
      DBSUInt64 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_REAL:
    {
      DBSReal value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      dest.GetOperand ().SetValue (value);
    }
    break;

  case FT_RICHREAL:
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
    return CS_INVALID_ARGS;

  StackValue topValue      = rStack[rStack.Size () - 1];
  bool       clearTopValue = false;

  if ((*pDataOff + sizeof (D_UINT16)) > rConn.DataSize ())
    goto update_frame_error;

  try
  {
      *pDataOff += sizeof (D_UINT16);
      if (flags & FT_FIELD_MASK)
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

          if (flags & FT_ARRAY_MASK)
            {
              if ((flags & FT_ARRAY_MASK) == FT_TEXT)
                {
                  assert (clearTopValue);
                  topValue.Clear ();
                  return CS_OP_NOTSUPP;
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
          else if ((flags & 0xFF) == FT_TEXT)
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
      else if (flags & FT_ARRAY_MASK)
        {
          if ((flags & 0xFF) == FT_TEXT)
            {
              assert (clearTopValue);
              topValue.Clear ();
              return CS_OP_NOTSUPP;
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
      else if ((flags & 0xFF) == FT_TEXT)
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
        return CS_INVALID_ARGS;

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
          return CS_INVALID_ARGS;
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

  return CS_OK;

update_frame_error:

  if (clearTopValue)
    topValue.Clear ();

    throw ConnectionException (
                    "Frame with invalid content for stack update operation.",
                    _EXTRA (0)
                              );
    return CS_GENERAL_ERR; //Not executed anyway!
}
