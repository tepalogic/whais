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

#include "utils/le_converter.h"
#include "utils/auto_array.h"
#include "utils/utf8.h"
#include "server_protocol.h"

#include "stack_cmds.h"
#include "valtranslator.h"

using namespace std;

static inline void
assert_cmds_values ()
{
  assert (WHC_TYPE_BOOL       == T_BOOL);
  assert (WHC_TYPE_CHAR       == T_CHAR);
  assert (WHC_TYPE_DATE       == T_DATE);
  assert (WHC_TYPE_DATETIME   == T_DATETIME);
  assert (WHC_TYPE_HIRESTIME  == T_HIRESTIME);
  assert (WHC_TYPE_INT8       == T_INT8);
  assert (WHC_TYPE_INT16      == T_INT16);
  assert (WHC_TYPE_INT32      == T_INT32);
  assert (WHC_TYPE_INT64      == T_INT64);
  assert (WHC_TYPE_UINT8      == T_UINT8);
  assert (WHC_TYPE_UINT16     == T_UINT16);
  assert (WHC_TYPE_UINT32     == T_UINT32);
  assert (WHC_TYPE_UINT64     == T_UINT64);
  assert (WHC_TYPE_REAL       == T_REAL);
  assert (WHC_TYPE_RICHREAL   == T_RICHREAL);
  assert (WHC_TYPE_TEXT       == T_TEXT);
  assert (WHC_TYPE_ARRAY_MASK == T_ARRAY_MASK);
  assert (WHC_TYPE_FIELD_MASK == T_FIELD_MASK);
  assert (WHC_TYPE_TABLE_MASK == T_TABLE_MASK);
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
  uint32_t      count  = load_le_int32 (rConn.Data () + *pDataOff);

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

  type       = load_le_int16 (data + *pDataOff);
  *pDataOff += sizeof (uint16_t);

  if (type != WHC_TYPE_TABLE_MASK)
    {
      if (type & WHC_TYPE_FIELD_MASK)
        return WCS_INVALID_ARGS;

      const bool isArray = (type & WHC_TYPE_ARRAY_MASK) != 0;

      type &= ~WHC_TYPE_ARRAY_MASK;
      if ((type < WHC_TYPE_BOOL) || (type > WHC_TYPE_TEXT))
        goto push_frame_error;
      else if (isArray && (type == WHC_TYPE_TEXT))
        return WCS_OP_NOTSUPP;
      else if (isArray)
        {
          switch (type)
          {
            case WHC_TYPE_BOOL:
              rStack.Push (DArray (_SC (DBool*, NULL)));
              break;

            case WHC_TYPE_CHAR:
              rStack.Push (DArray (_SC (DChar*, NULL)));
              break;

            case WHC_TYPE_DATE:
              rStack.Push (DArray (_SC (DDate*, NULL)));
              break;

            case WHC_TYPE_DATETIME:
              rStack.Push (DArray (_SC (DDateTime*, NULL)));
              break;

            case WHC_TYPE_HIRESTIME:
              rStack.Push (DArray (_SC (DHiresTime*, NULL)));
              break;

            case WHC_TYPE_INT8:
              rStack.Push (DArray (_SC (DInt8*, NULL)));
              break;

            case WHC_TYPE_INT16:
              rStack.Push (DArray (_SC (DInt16*, NULL)));
              break;

            case WHC_TYPE_INT32:
              rStack.Push (DArray (_SC (DInt32*, NULL)));
              break;

            case WHC_TYPE_INT64:
              rStack.Push (DArray (_SC (DInt64*, NULL)));
              break;

            case WHC_TYPE_UINT8:
              rStack.Push (DArray (_SC (DUInt8*, NULL)));
              break;

            case WHC_TYPE_UINT16:
              rStack.Push (DArray (_SC (DUInt16*, NULL)));
              break;

            case WHC_TYPE_UINT32:
              rStack.Push (DArray (_SC (DUInt32*, NULL)));
              break;

            case WHC_TYPE_UINT64:
              rStack.Push (DArray (_SC (DUInt64*, NULL)));
              break;

            case WHC_TYPE_REAL:
              rStack.Push (DArray (_SC (DReal*, NULL)));
              break;

            case WHC_TYPE_RICHREAL:
              rStack.Push (DArray (_SC (DRichReal*, NULL)));
              break;

            case WHC_TYPE_TEXT:
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
        case WHC_TYPE_BOOL:
          rStack.Push (DBool ());
          break;

        case WHC_TYPE_CHAR:
          rStack.Push (DChar ());
          break;

        case WHC_TYPE_DATE:
          rStack.Push (DDate ());
          break;

        case WHC_TYPE_DATETIME:
          rStack.Push (DDateTime ());
          break;

        case WHC_TYPE_HIRESTIME:
          rStack.Push (DHiresTime ());
          break;

        case WHC_TYPE_INT8:
          rStack.Push (DInt8 ());
          break;

        case WHC_TYPE_INT16:
          rStack.Push (DInt16 ());
          break;

        case WHC_TYPE_INT32:
          rStack.Push (DInt32 ());
          break;

        case WHC_TYPE_INT64:
          rStack.Push (DInt64 ());
          break;

        case WHC_TYPE_UINT8:
          rStack.Push (DUInt8 ());
          break;

        case WHC_TYPE_UINT16:
          rStack.Push (DUInt16 ());
          break;

        case WHC_TYPE_UINT32:
          rStack.Push (DUInt32 ());
          break;

        case WHC_TYPE_UINT64:
          rStack.Push (DUInt64 ());
          break;

        case WHC_TYPE_REAL:
          rStack.Push (DReal ());
          break;

        case WHC_TYPE_RICHREAL:
          rStack.Push (DRichReal ());
          break;

        case WHC_TYPE_TEXT:
          rStack.Push (DText ());
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

      const uint16_t fieldsCount = load_le_int16 (data + *pDataOff);
      *pDataOff += sizeof (uint16_t);

      if (fieldsCount == 0)
        return WCS_INVALID_ARGS;

      DBSFieldDescriptor* const fields_ = (fieldsCount <= STACK_FIELDS_SIZE) ?
                                          stackFields :
                                          heapFields.Reset (fieldsCount);

      for (uint_t field = 0; field < fieldsCount; ++field)
        {
          fields_[field].name = _RC (const char*, data + *pDataOff);

          const uint_t fieldLen = strlen (fields_[field].name);

          *pDataOff +=  fieldLen+ 1;
          if ((*pDataOff + sizeof (uint16_t) > dataSize)
              || (fieldLen == 0))
            {
              goto push_frame_error;
            }

          const uint16_t fieldType = load_le_int16 (data + *pDataOff);
          *pDataOff += sizeof (uint16_t);

          fields_[field].isArray     = IS_ARRAY (fieldType);
          fields_[field].type = _SC (DBS_FIELD_TYPE,
                                            GET_BASIC_TYPE (fieldType));

          if ((fields_[field].type < WHC_TYPE_BOOL)
              || (fields_[field].type > WHC_TYPE_TEXT))
            {
              goto push_frame_error;
            }
          else if ((fields_[field].type == WHC_TYPE_TEXT)
                   && (fields_[field].isArray))
            {
              return WCS_OP_NOTSUPP;
            }
        }
      I_DBSHandler& dbs   = *rConn.Dbs().mDbs;
      ITable&   table = dbs.CreateTempTable (fieldsCount, fields_);

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
  case WHC_TYPE_BOOL:
    {
      DBool value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_CHAR:
    {
      DChar value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_DATE:
    {
      DDate value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_DATETIME:
    {
      DDateTime value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_HIRESTIME:
    {
      DHiresTime value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_INT8:
    {
      DInt8 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_INT16:
    {
      DInt16 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_INT32:
    {
      DInt32 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_INT64:
    {
      DInt64 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_UINT8:
    {
      DUInt8 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_UINT16:
    {
      DUInt16 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_UINT32:
    {
      DUInt32 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_UINT64:
    {
      DUInt64 value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_REAL:
    {
      DReal value;
      result = Utf8Translator::Read (data + *pDataOff,
                                     dataSize - *pDataOff,
                                     &value);
      if (result > minSize)
        dest.GetOperand ().SetValue (value);
    }
    break;

  case WHC_TYPE_RICHREAL:
    {
      DRichReal value;
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
  uint16_t             type     = load_le_int16 (data + *pDataOff);

  if (rStack.Size () == 0)
    return WCS_INVALID_ARGS;

  StackValue& stackTop = rStack[rStack.Size () - 1];

  if ((*pDataOff + sizeof (uint16_t)) > dataSize)
    goto update_frame_error;

  try
  {
      *pDataOff += sizeof (uint16_t);
      if (type & WHC_TYPE_FIELD_MASK)
        {
          type &= ~WHC_TYPE_FIELD_MASK;

          const char* const fieldName = _RC (const char*,
                                               data + *pDataOff);
          const uint_t fieldLen = strlen (fieldName) + 1;

          *pDataOff += fieldLen;
          if ((fieldLen <= 1) ||
              ((*pDataOff + sizeof (uint64_t)) > dataSize))
            {
              goto update_frame_error;
            }

          const uint64_t rowIndex = load_le_int64 (data + *pDataOff);
          *pDataOff += sizeof (uint64_t);

          ITable&    table      = stackTop.GetOperand ().GetTable ();
          const uint32_t fieldIndex = table.RetrieveField (fieldName);

          StackValue field    = stackTop.GetOperand ().GetFieldAt (fieldIndex);
          StackValue rowValue = field.GetOperand ().GetValueAt (rowIndex);

          if (type & WHC_TYPE_ARRAY_MASK)
            {
              type &= ~WHC_TYPE_ARRAY_MASK;
              assert ((WHC_TYPE_BOOL <= type) && (type <= WHC_TYPE_TEXT));

              if (type == WHC_TYPE_TEXT)
                return WCS_OP_NOTSUPP;

              if (*pDataOff + sizeof (uint16_t) > dataSize)
                goto update_frame_error;

              uint16_t elCount = load_le_int16 (data + *pDataOff);
              *pDataOff += sizeof (uint16_t);

              if (elCount == 0)
                goto update_frame_error;

              uint64_t fromPos = load_le_int64 (data + *pDataOff);
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
          else if (type == WHC_TYPE_TEXT)
            {
              DText fieldText;

              if (rowIndex < table.AllocatedRows ())
                rowValue.GetOperand().GetValue (fieldText);

              if ((*pDataOff + sizeof (uint64_t)) >= dataSize)
                goto update_frame_error;

              uint64_t offset = load_le_int64 (data + *pDataOff);
              *pDataOff += sizeof (uint64_t);

              do
                {
                  uint32_t ch    = 0;
                  uint_t   chLen = wh_load_utf8_cp (data + *pDataOff, &ch);

                  if (chLen == 0)
                    goto update_frame_error;
                  else if (ch == 0)
                    return WCS_INVALID_ARGS;

                  fieldText.CharAt (offset++, DChar (ch));

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
      else if (type & WHC_TYPE_ARRAY_MASK)
        {
          type &= ~WHC_TYPE_ARRAY_MASK;
          assert ((WHC_TYPE_BOOL <= type) && (type <= WHC_TYPE_TEXT));

          if (type == WHC_TYPE_TEXT)
            return WCS_OP_NOTSUPP;

          if (*pDataOff + sizeof (uint16_t) > dataSize)
            goto update_frame_error;

          uint16_t elCount = load_le_int16 (data + *pDataOff);
          *pDataOff += sizeof (uint16_t);

          if (elCount == 0)
            goto update_frame_error;

          if (*pDataOff + sizeof (uint64_t) > dataSize)
            goto update_frame_error;

          uint64_t fromPos  = load_le_int64 (data + *pDataOff);
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
      else if (type == WHC_TYPE_TEXT)
        {
          DText fieldText;
          stackTop.GetOperand ().GetValue (fieldText);

          if ((*pDataOff + sizeof (uint64_t)) > dataSize)
            goto update_frame_error;

          uint64_t offset = load_le_int64 (data + *pDataOff);
          *pDataOff += sizeof (uint64_t);

          do
            {
              uint32_t ch    = 0;
              uint_t   chLen = wh_load_utf8_cp (data + *pDataOff, &ch);

              if (chLen == 0)
                goto update_frame_error;
              else if (ch == 0)
                return WCS_INVALID_ARGS;

              fieldText.CharAt (offset++, DChar (ch));

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
          assert ((WHC_TYPE_BOOL <= type) && (type < WHC_TYPE_TEXT));

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
      if (e.Extra() == InterException::INVALID_OP_REQ)
        return WCS_TYPE_MISMATCH;

      throw;
  }
  catch (DBSException& e)
  {
      const uint_t extra = e.Extra ();

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
  case WHC_TYPE_BOOL:
    {

      DBool value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_CHAR:
    {
      DChar value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_DATE:
    {
      DDate value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_DATETIME:
    {
      DDateTime value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_HIRESTIME:
    {
      DHiresTime value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_INT8:
  case WHC_TYPE_INT16:
  case WHC_TYPE_INT32:
  case WHC_TYPE_INT64:
    {
      DInt64 value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_UINT8:
  case WHC_TYPE_UINT16:
  case WHC_TYPE_UINT32:
  case WHC_TYPE_UINT64:
    {
      DUInt64 value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_REAL:
    {
      DReal value;
      source.GetOperand ().GetValue (value);

      result = Utf8Translator::Write (data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_RICHREAL:
    {
      DRichReal value;
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
    DArray temp;
    value.GetOperand ().GetValue (temp);

    if (*pDataOffset + sizeof (uint64_t) >= maxDataSize)
      return WCS_LARGE_RESPONSE;

    maxCount = temp.Count ();
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
    DText temp;
    value.GetOperand ().GetValue (temp);

    if (*pDataOffset + sizeof (uint64_t) >= maxDataSize)
      return WCS_LARGE_ARGS;

    maxCount = temp.Count ();
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
  ITable&     table       = tableValue.GetOperand ().GetTable ();

  DBSFieldDescriptor fd      = table.DescribeField (hintField);
  const uint_t       nameLen = strlen (fd.name) + 1;

  if (*pDataOffset + nameLen + sizeof (uint16_t) >= maxDataSize)
    return WCS_LARGE_RESPONSE;

  memcpy (data + *pDataOffset, fd.name, nameLen);
  *pDataOffset += nameLen;

  StackValue fieldVal = tableValue.GetOperand ().GetFieldAt (hintField);
  StackValue rowValue = fieldVal.GetOperand ().GetValueAt (hintRow);

  if (fd.isArray)
    {
      store_le_int16 (fd.type | WHC_TYPE_ARRAY_MASK, data + *pDataOffset);
      *pDataOffset += sizeof (uint16_t);

      if (fd.type == WHC_TYPE_TEXT)
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
  else if (fd.type == WHC_TYPE_TEXT)
    {
      store_le_int16 (fd.type, data + *pDataOffset);
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
      store_le_int16 (fd.type, data + *pDataOffset);
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
  ITable&       table = topValue.GetOperand ().GetTable ();
  const FIELD_INDEX field = topValue.GetOperand ().GetField ();

  DBSFieldDescriptor fieldDesc = table.DescribeField (field);

  const uint_t minSize = sizeof (uint64_t) * sizeof (uint64_t);
  if (*pDataOffset + minSize > rConn.MaxSize ())
    return WCS_LARGE_RESPONSE;

  uint16_t type = fieldDesc.type | WHC_TYPE_FIELD_MASK;
  if (fieldDesc.isArray)
    type |= WHC_TYPE_ARRAY_MASK;


  const uint64_t rowsCount = table.AllocatedRows ();
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
      else if (fieldDesc.type == T_TEXT)
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
  ITable&     table       = topValue.GetOperand ().GetTable ();

  const FIELD_INDEX fieldsCount = table.FieldsCount();
  const ROW_INDEX   rowsCount   = table.AllocatedRows ();
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
