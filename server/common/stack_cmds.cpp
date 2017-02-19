/******************************************************************************
WHAIS - An advanced database system
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

#include <cstring>
#include <sstream>

#include "dbs/dbs_valtranslator.h"
#include "utils/endianness.h"
#include "utils/auto_array.h"
#include "utils/wutf.h"
#include "server_protocol.h"

#include "stack_cmds.h"

using namespace std;

static inline void
assert_cmds_values()
{
  assert(WHC_TYPE_BOOL       == T_BOOL);
  assert(WHC_TYPE_CHAR       == T_CHAR);
  assert(WHC_TYPE_DATE       == T_DATE);
  assert(WHC_TYPE_DATETIME   == T_DATETIME);
  assert(WHC_TYPE_HIRESTIME  == T_HIRESTIME);
  assert(WHC_TYPE_INT8       == T_INT8);
  assert(WHC_TYPE_INT16      == T_INT16);
  assert(WHC_TYPE_INT32      == T_INT32);
  assert(WHC_TYPE_INT64      == T_INT64);
  assert(WHC_TYPE_UINT8      == T_UINT8);
  assert(WHC_TYPE_UINT16     == T_UINT16);
  assert(WHC_TYPE_UINT32     == T_UINT32);
  assert(WHC_TYPE_UINT64     == T_UINT64);
  assert(WHC_TYPE_REAL       == T_REAL);
  assert(WHC_TYPE_RICHREAL   == T_RICHREAL);
  assert(WHC_TYPE_TEXT       == T_TEXT);
  assert(WHC_TYPE_ARRAY_MASK == T_ARRAY_MASK);
  assert(WHC_TYPE_FIELD_MASK == T_FIELD_MASK);
  assert(WHC_TYPE_TABLE_MASK == T_TABLE_MASK);
}




uint_t
cmd_pop_stack(ClientConnection& conn, uint_t* const inoutDataOff)
{
  assert_cmds_values();

  if ((*inoutDataOff + sizeof(uint32_t)) > conn.DataSize())
    {
      throw ConnectionException(
                    _EXTRA(0),
                    "Got frame with invalid content for stack pop operation."
                                );
    }

  SessionStack& stack = conn.Stack();
  uint32_t      count = load_le_int32(conn.Data() + *inoutDataOff);

  *inoutDataOff += sizeof(uint32_t);

  count = MIN(count, stack.Size());
  stack.Pop(count);

  return WCS_OK;
}


uint_t
cmd_push_stack(ClientConnection& conn, uint_t* const inoutDataOff)
{
  assert_cmds_values();

  const uint8_t* const data     = conn.Data();
  const uint_t         dataSize = conn.DataSize();
  SessionStack&        stack    = conn.Stack();
  uint16_t             type;

  if ((*inoutDataOff + sizeof(uint16_t)) > dataSize)
    goto push_frame_error;

  type           = load_le_int16(data + *inoutDataOff);
  *inoutDataOff += sizeof(uint16_t);

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
          switch(type)
          {
            case WHC_TYPE_BOOL:
              stack.Push(DArray(_SC(DBool*, NULL)));
              break;

            case WHC_TYPE_CHAR:
              stack.Push(DArray(_SC(DChar*, NULL)));
              break;

            case WHC_TYPE_DATE:
              stack.Push(DArray(_SC(DDate*, NULL)));
              break;

            case WHC_TYPE_DATETIME:
              stack.Push(DArray(_SC(DDateTime*, NULL)));
              break;

            case WHC_TYPE_HIRESTIME:
              stack.Push(DArray(_SC(DHiresTime*, NULL)));
              break;

            case WHC_TYPE_INT8:
              stack.Push(DArray(_SC(DInt8*, NULL)));
              break;

            case WHC_TYPE_INT16:
              stack.Push(DArray(_SC(DInt16*, NULL)));
              break;

            case WHC_TYPE_INT32:
              stack.Push(DArray(_SC(DInt32*, NULL)));
              break;

            case WHC_TYPE_INT64:
              stack.Push(DArray(_SC(DInt64*, NULL)));
              break;

            case WHC_TYPE_UINT8:
              stack.Push(DArray(_SC(DUInt8*, NULL)));
              break;

            case WHC_TYPE_UINT16:
              stack.Push(DArray(_SC(DUInt16*, NULL)));
              break;

            case WHC_TYPE_UINT32:
              stack.Push(DArray(_SC(DUInt32*, NULL)));
              break;

            case WHC_TYPE_UINT64:
              stack.Push(DArray(_SC(DUInt64*, NULL)));
              break;

            case WHC_TYPE_REAL:
              stack.Push(DArray(_SC(DReal*, NULL)));
              break;

            case WHC_TYPE_RICHREAL:
              stack.Push(DArray(_SC(DRichReal*, NULL)));
              break;

            case WHC_TYPE_TEXT:
              assert(false);
              break;

            default:
              throw ConnectionException(
                    _EXTRA(type),
                    "Server internal error:\n"
                    "Invalid type encountered, though it passed type checks!"
                                        );
          }
          return WCS_OK;
        }

      switch(type)
      {
        case WHC_TYPE_BOOL:
          stack.Push(DBool());
          break;

        case WHC_TYPE_CHAR:
          stack.Push(DChar());
          break;

        case WHC_TYPE_DATE:
          stack.Push(DDate());
          break;

        case WHC_TYPE_DATETIME:
          stack.Push(DDateTime());
          break;

        case WHC_TYPE_HIRESTIME:
          stack.Push(DHiresTime());
          break;

        case WHC_TYPE_INT8:
          stack.Push(DInt8());
          break;

        case WHC_TYPE_INT16:
          stack.Push(DInt16());
          break;

        case WHC_TYPE_INT32:
          stack.Push(DInt32());
          break;

        case WHC_TYPE_INT64:
          stack.Push(DInt64());
          break;

        case WHC_TYPE_UINT8:
          stack.Push(DUInt8());
          break;

        case WHC_TYPE_UINT16:
          stack.Push(DUInt16());
          break;

        case WHC_TYPE_UINT32:
          stack.Push(DUInt32());
          break;

        case WHC_TYPE_UINT64:
          stack.Push(DUInt64());
          break;

        case WHC_TYPE_REAL:
          stack.Push(DReal());
          break;

        case WHC_TYPE_RICHREAL:
          stack.Push(DRichReal());
          break;

        case WHC_TYPE_TEXT:
          stack.Push(DText());
          break;

        default:
          throw ConnectionException(
                    _EXTRA(type),
                    "Server internal error:\n"
                    "Invalid type encountered, though it passed type checks!"
                                    );
      }
    }
  else
    {
      static const uint_t            STACK_FIELDS_SIZE = 0x10;
      DBSFieldDescriptor             stackFields[STACK_FIELDS_SIZE];
      auto_array<DBSFieldDescriptor> heapFields;

      if ((*inoutDataOff + sizeof(uint16_t)) > dataSize)
        goto push_frame_error;

      const uint16_t fieldsCount = load_le_int16(data + *inoutDataOff);
      *inoutDataOff += sizeof(uint16_t);

      if (fieldsCount == 0)
        return WCS_INVALID_ARGS;

      DBSFieldDescriptor* const fields_ = (fieldsCount <= STACK_FIELDS_SIZE) ?
                                            stackFields :
                                            heapFields.Reset(fieldsCount);

      for (uint_t field = 0; field < fieldsCount; ++field)
        {
          fields_[field].name = _RC(const char*, data + *inoutDataOff);

          const uint_t fieldLen = strlen(fields_[field].name);

          *inoutDataOff +=  fieldLen+ 1;
          if ((*inoutDataOff + sizeof(uint16_t) > dataSize)
              || (fieldLen == 0))
            {
              goto push_frame_error;
            }

          const uint16_t fieldType = load_le_int16(data + *inoutDataOff);
          *inoutDataOff += sizeof(uint16_t);

          fields_[field].isArray = IS_ARRAY(fieldType);
          fields_[field].type    = _SC(DBS_FIELD_TYPE,
                                        GET_BASIC_TYPE(fieldType));

          if ((_SC(uint_t, fields_[field].type) < WHC_TYPE_BOOL)
              || (_SC(uint_t, fields_[field].type) > WHC_TYPE_TEXT))
            {
              goto push_frame_error;
            }
          else if ((fields_[field].type == WHC_TYPE_TEXT)
                   && (fields_[field].isArray))
            {
              return WCS_OP_NOTSUPP;
            }
        }

      IDBSHandler& dbs   = *conn.Dbs().mDbs;
      ITable&      table = dbs.CreateTempTable(fieldsCount, fields_);

      stack.Push(dbs, table);
    }

  return WCS_OK;

push_frame_error:
  throw ConnectionException(
                    _EXTRA(0),
                    "Frame with invalid content for stack push operation."
                            );
  return WCS_GENERAL_ERR; //Not executed anyway!
}


static uint_t
read_value(StackValue&         dest,
            const uint32_t      type,
            const uint8_t*      data,
            const uint16_t      dataSize,
            uint_t* const       inoutDataOff)
{
  int result = -1;

  switch(type)
  {
  case WHC_TYPE_BOOL:
    {
      DBool value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);

    }
    break;

  case WHC_TYPE_CHAR:
    {
      DChar value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     false,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_DATE:
    {
      DDate value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_DATETIME:
    {
      DDateTime value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_HIRESTIME:
    {
      DHiresTime value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_INT8:
    {
      DInt8 value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_INT16:
    {
      DInt16 value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_INT32:
    {
      DInt32 value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_INT64:
    {
      DInt64 value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_UINT8:
    {
      DUInt8 value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_UINT16:
    {
      DUInt16 value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_UINT32:
    {
      DUInt32 value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_UINT64:
    {
      DUInt64 value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_REAL:
    {
      DReal value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  case WHC_TYPE_RICHREAL:
    {
      DRichReal value;
      result = Utf8Translator::Read(data + *inoutDataOff,
                                     dataSize - *inoutDataOff,
                                     &value);

      if ((result >= 0) && (data[*inoutDataOff + result++] == 0))
        dest.Operand().SetValue(value);
    }
    break;

  default:
    throw ConnectionException(_EXTRA(type),
                               "Client frame contains invalid update type.");

  }

  if (result >= 0)
    {
      *inoutDataOff += result;

      return result;
    }

  return 0;
}


uint_t
cmd_update_stack_top(ClientConnection& conn, uint_t* const inoutDataOff)
{
  assert_cmds_values();

  const uint8_t* const data     = conn.Data();
  const uint_t         dataSize = conn.DataSize();
  SessionStack&        stack    = conn.Stack();
  uint16_t             type     = load_le_int16(data + *inoutDataOff);

  if (stack.Size() == 0)
    return WCS_INVALID_ARGS;

  StackValue& stackTop = stack[stack.Size() - 1];

  if ((*inoutDataOff + sizeof(uint16_t)) > dataSize)
    goto update_frame_error;

  try
  {
      *inoutDataOff += sizeof(uint16_t);
      if (type & WHC_TYPE_FIELD_MASK)
        {
          type &= ~WHC_TYPE_FIELD_MASK;

          const char* const fieldName = _RC(const char*, data + *inoutDataOff);
          const uint_t      fieldLen  = strlen(fieldName) + 1;

          *inoutDataOff += fieldLen;
          if ((fieldLen <= 1)
              || ((*inoutDataOff + sizeof(uint64_t)) > dataSize))
            {
              goto update_frame_error;
            }

          const uint64_t rowIndex = load_le_int64(data + *inoutDataOff);
          *inoutDataOff += sizeof(uint64_t);

          ITable&        table      = stackTop.Operand().GetTable();
          const uint32_t fieldIndex = table.RetrieveField(fieldName);

          StackValue field    = stackTop.Operand().GetFieldAt(fieldIndex);
          StackValue rowValue = field.Operand().GetValueAt(rowIndex);

          if (type & WHC_TYPE_ARRAY_MASK)
            {
              type &= ~WHC_TYPE_ARRAY_MASK;
              assert((WHC_TYPE_BOOL <= type) && (type <= WHC_TYPE_TEXT));

              if (type == WHC_TYPE_TEXT)
                return WCS_OP_NOTSUPP;

              if (*inoutDataOff + sizeof(uint16_t) > dataSize)
                goto update_frame_error;

              uint16_t elCount = load_le_int16(data + *inoutDataOff);
              *inoutDataOff += sizeof(uint16_t);

              if (elCount == 0)
                goto update_frame_error;

              uint64_t fromPos = load_le_int64(data + *inoutDataOff);
              *inoutDataOff += sizeof(uint64_t);

              if (*inoutDataOff > dataSize)
                goto update_frame_error;

              while ((*inoutDataOff < dataSize) && (elCount > 0))
                {
                  IOperand& operand          = rowValue.Operand();
                  StackValue arrayValuevalue = operand.GetValueAt(fromPos);

                  const uint_t len = read_value(arrayValuevalue,
                                                 type,
                                                 data,
                                                 dataSize,
                                                 inoutDataOff);
                  if (len == 0)
                    goto update_frame_error;

                  ++fromPos, --elCount;

                  assert(*inoutDataOff <= dataSize);
                }

              if (elCount != 0)
                goto update_frame_error;
            }
          else if (type == WHC_TYPE_TEXT)
            {
              DText fieldText;

              if (rowIndex < table.AllocatedRows())
                rowValue.Operand().GetValue(fieldText);

              if ((*inoutDataOff + sizeof(uint64_t)) >= dataSize)
                goto update_frame_error;

              uint64_t offset = load_le_int64(data + *inoutDataOff);
              *inoutDataOff += sizeof(uint64_t);

              do
                {
                  uint32_t ch    = 0;
                  uint_t   chLen = wh_load_utf8_cp(data + *inoutDataOff, &ch);

                  if (chLen == 0)
                    goto update_frame_error;

                  else if (ch == 0)
                    return WCS_INVALID_ARGS;

                  fieldText.CharAt(offset++, DChar(ch));

                  *inoutDataOff += chLen;

                  if (*inoutDataOff >= dataSize)
                    goto update_frame_error;
                }
              while (data[*inoutDataOff] != 0);

              *inoutDataOff += sizeof(uint8_t);

              assert(*inoutDataOff <= dataSize);

              rowValue.Operand().SetValue(fieldText);
            }
          else
            {
              const uint_t elLen = read_value(rowValue,
                                               type,
                                               data,
                                               dataSize,
                                               inoutDataOff);
              if (elLen == 0)
                goto update_frame_error;

              assert(*inoutDataOff <= dataSize);
            }
        }
      else if (type & WHC_TYPE_ARRAY_MASK)
        {
          type &= ~WHC_TYPE_ARRAY_MASK;

          assert((WHC_TYPE_BOOL <= type) && (type <= WHC_TYPE_TEXT));

          if (type == WHC_TYPE_TEXT)
            return WCS_OP_NOTSUPP;

          if (*inoutDataOff + sizeof(uint16_t) > dataSize)
            goto update_frame_error;

          uint16_t elCount = load_le_int16(data + *inoutDataOff);
          *inoutDataOff += sizeof(uint16_t);

          if (elCount == 0)
            goto update_frame_error;

          if (*inoutDataOff + sizeof(uint64_t) > dataSize)
            goto update_frame_error;

          uint64_t fromPos = load_le_int64(data + *inoutDataOff);
          *inoutDataOff += sizeof(uint64_t);

          while ((*inoutDataOff < dataSize) && (elCount > 0))
            {
              IOperand&  operand    = stackTop.Operand();
              StackValue arrayValue = operand.GetValueAt(fromPos);

              const uint_t len = read_value(arrayValue,
                                             type,
                                             data,
                                             dataSize,
                                             inoutDataOff);

              if (len == 0)
                goto update_frame_error;

              ++fromPos, --elCount;

              assert(*inoutDataOff <= dataSize);
            }

          if (elCount != 0)
            goto update_frame_error;
        }
      else if (type == WHC_TYPE_TEXT)
        {
          DText fieldText;

          stackTop.Operand().GetValue(fieldText);

          if ((*inoutDataOff + sizeof(uint64_t)) > dataSize)
            goto update_frame_error;

          uint64_t offset = load_le_int64(data + *inoutDataOff);
          *inoutDataOff += sizeof(uint64_t);

          do
            {
              uint32_t ch    = 0;
              uint_t   chLen = wh_load_utf8_cp(data + *inoutDataOff, &ch);

              if (chLen == 0)
                goto update_frame_error;

              else if (ch == 0)
                return WCS_INVALID_ARGS;

              fieldText.CharAt(offset++, DChar(ch));

              *inoutDataOff += chLen;
              if (*inoutDataOff >= dataSize)
                goto update_frame_error;
            }
          while (data[*inoutDataOff] != 0);

          *inoutDataOff += sizeof(uint8_t);

          stackTop.Operand().SetValue(fieldText);
        }
      else
        {
          assert((WHC_TYPE_BOOL <= type) && (type < WHC_TYPE_TEXT));

          const uint_t elLen = read_value(stackTop,
                                           type,
                                           data,
                                           dataSize,
                                           inoutDataOff);
          if (elLen == 0)
            goto update_frame_error;

          assert(*inoutDataOff <= dataSize);
        }
  }
  catch(InterException& e)
  {
      if (e.Code() == InterException::INVALID_OP_REQ)
        return WCS_TYPE_MISMATCH;

      throw;
  }
  catch(DBSException& e)
  {
      const uint_t extra = e.Code();

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
  throw ConnectionException(
                    _EXTRA(0),
                    "Frame with invalid content for stack update operation."
                             );
  return WCS_GENERAL_ERR; //Not executed anyway!
}


static uint_t
write_value(StackValue&      source,
             uint8_t* const   data,
             const uint16_t   maxDataSize)
{
  IOperand& valueOp = source.Operand();
  uint_t result = 0;

  switch(valueOp.GetType())
  {
  case WHC_TYPE_BOOL:
    {
      DBool value;

      source.Operand().GetValue(value);

      result = Utf8Translator::Write(data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_CHAR:
    {
      DChar value;

      source.Operand().GetValue(value);

      result = Utf8Translator::Write(data, maxDataSize, false, value);
    }
    break;

  case WHC_TYPE_DATE:
    {
      DDate value;

      source.Operand().GetValue(value);

      result = Utf8Translator::Write(data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_DATETIME:
    {
      DDateTime value;

      source.Operand().GetValue(value);

      result = Utf8Translator::Write(data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_HIRESTIME:
    {
      DHiresTime value;

      source.Operand().GetValue(value);

      result = Utf8Translator::Write(data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_INT8:
  case WHC_TYPE_INT16:
  case WHC_TYPE_INT32:
  case WHC_TYPE_INT64:
    {
      DInt64 value;

      source.Operand().GetValue(value);

      result = Utf8Translator::Write(data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_UINT8:
  case WHC_TYPE_UINT16:
  case WHC_TYPE_UINT32:
  case WHC_TYPE_UINT64:
    {
      DUInt64 value;

      source.Operand().GetValue(value);

      result = Utf8Translator::Write(data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_REAL:
    {
      DReal value;

      source.Operand().GetValue(value);

      result = Utf8Translator::Write(data, maxDataSize, value);
    }
    break;

  case WHC_TYPE_RICHREAL:
    {
      DRichReal value;

      source.Operand().GetValue(value);

      result = Utf8Translator::Write(data, maxDataSize, value);
    }
    break;

  default:
    assert(false);
    result = 0;
  }

  return result;
}


uint_t
cmd_read_basic_stack_top(ClientConnection& conn,
                          StackValue&       value,
                          uint_t* const     inoutDataOffset)
{
  uint8_t* const data        = conn.Data();
  const uint_t   maxDataSize = conn.MaxSize();

  assert(conn.Stack().Size() > 0);
  assert((value.Operand().GetType() >= T_BOOL)
          && (value.Operand().GetType() < T_TEXT));

  const uint16_t length = write_value(value,
                                       data + *inoutDataOffset,
                                       maxDataSize - *inoutDataOffset);
  if (length == 0)
    return WCS_LARGE_RESPONSE;

  else
    *inoutDataOffset += length;

  assert(*inoutDataOffset <= maxDataSize);

  return WCS_OK;
}


uint_t
cmd_read_array_stack_top(ClientConnection& conn,
                          StackValue&       value,
                          uint64_t          hintOffset,
                          uint_t* const     inoutDataOffset)
{
  uint8_t* const data        = conn.Data();
  const uint_t   maxDataSize = conn.MaxSize();
  uint64_t       maxCount    = 0;

  {
    DArray temp;
    value.Operand().GetValue(temp);

    if (*inoutDataOffset + sizeof(uint64_t) >= maxDataSize)
      return WCS_LARGE_RESPONSE;

    maxCount = temp.Count();
    store_le_int64(maxCount, data + *inoutDataOffset);
    *inoutDataOffset +=sizeof(uint64_t);

    if (maxCount == 0)
      return WCS_OK;

    if (hintOffset >= maxCount)
      hintOffset = 0;

    if (*inoutDataOffset + sizeof(uint64_t) >= maxDataSize)
      return WCS_LARGE_RESPONSE;

    store_le_int64(hintOffset, data + *inoutDataOffset);
    *inoutDataOffset += sizeof(uint64_t);
  }

  for (uint64_t index = hintOffset; index < maxCount; ++index)
    {
      StackValue   el        = value.Operand().GetValueAt(index);
      const uint_t writeSize = write_value(el,
                                            data + *inoutDataOffset,
                                            maxDataSize - *inoutDataOffset);

      assert(! el.Operand().IsNull());

      if (writeSize == 0)
        {
          if (index == hintOffset)
            return WCS_LARGE_RESPONSE;

          else
            break;
        }
      *inoutDataOffset += writeSize;
    }

  assert(*inoutDataOffset <= maxDataSize);

  return WCS_OK;
}

uint_t
cmd_read_text_stack_top(ClientConnection& conn,
                         StackValue&       value,
                         uint64_t          hintOffset,
                         uint_t* const     inoutDataOffset)
{
  uint8_t* const data        = conn.Data();
  const uint_t   maxDataSize = conn.MaxSize();
  uint64_t       maxCount    = 0;

  assert(conn.Stack().Size() > 0);
  assert(value.Operand().GetType() == T_TEXT);

  {
    DText temp;
    value.Operand().GetValue(temp);

    if (*inoutDataOffset + sizeof(uint64_t) >= maxDataSize)
      return WCS_LARGE_ARGS;

    maxCount = temp.Count();
    store_le_int64(maxCount, data + *inoutDataOffset);
    *inoutDataOffset += sizeof(uint64_t);

    if (maxCount == 0)
      return WCS_OK;

    if (hintOffset >= maxCount)
      hintOffset = 0;
  }

  if (*inoutDataOffset + sizeof(uint64_t) >= maxDataSize)
    return WCS_LARGE_ARGS;

  store_le_int64(hintOffset, data + *inoutDataOffset);
  *inoutDataOffset += sizeof(uint64_t);

  for (uint64_t index = hintOffset; index < maxCount; ++index)
    {
      StackValue   el        = value.Operand().GetValueAt(index);
      const uint_t writeSize = write_value(el,
                                            data + *inoutDataOffset,
                                            maxDataSize - *inoutDataOffset);
      assert(! el.Operand().IsNull());

      if (writeSize == 0)
        {
          if (index == hintOffset)
            return WCS_LARGE_RESPONSE;

          else
            break;
        }

      assert(writeSize > 1);

      *inoutDataOffset += writeSize - 1; //Don't include the null terminator yet

      assert(data[*inoutDataOffset] == 0);
    }

  assert(*inoutDataOffset <= maxDataSize);
  assert(data[*inoutDataOffset] == 0);

  *inoutDataOffset += 1; //Make sure we count the null terminator too!

  return WCS_OK;
}


static uint_t
cmd_read_table_field_internal(ClientConnection& conn,
                               StackValue&       tableValue,
                               uint_t            hintField,
                               uint64_t          hintRow,
                               uint64_t          hintArrayOff,
                               uint64_t          hintTextOff,
                               uint_t* const     inoutDataOffset)
{
  uint_t          cs          = WCS_OK;
  uint8_t* const  data        = conn.Data();
  const uint_t    maxDataSize = conn.DataSize();
  ITable&     table       = tableValue.Operand().GetTable();

  DBSFieldDescriptor fd      = table.DescribeField(hintField);
  const uint_t       nameLen = strlen(fd.name) + 1;

  if (*inoutDataOffset + nameLen + sizeof(uint16_t) >= maxDataSize)
    return WCS_LARGE_RESPONSE;

  memcpy(data + *inoutDataOffset, fd.name, nameLen);
  *inoutDataOffset += nameLen;

  StackValue fieldVal = tableValue.Operand().GetFieldAt(hintField);
  StackValue rowValue = fieldVal.Operand().GetValueAt(hintRow);

  if (fd.isArray)
    {
      store_le_int16(fd.type | WHC_TYPE_ARRAY_MASK, data + *inoutDataOffset);
      *inoutDataOffset += sizeof(uint16_t);

      if (fd.type == WHC_TYPE_TEXT)
        {
          assert(false);
          return WCS_GENERAL_ERR;
        }
      else if (hintTextOff != WIGNORE_OFF)
        return WCS_TYPE_MISMATCH;

      cs = cmd_read_array_stack_top(conn,
                                     rowValue,
                                     hintArrayOff,
                                     inoutDataOffset);
    }
  else if (fd.type == WHC_TYPE_TEXT)
    {
      store_le_int16(fd.type, data + *inoutDataOffset);
      *inoutDataOffset += sizeof(uint16_t);

      if (hintArrayOff != WIGNORE_OFF)
        return WCS_TYPE_MISMATCH;

      cs = cmd_read_text_stack_top(conn,
                                    rowValue,
                                    hintTextOff,
                                    inoutDataOffset);
    }
  else
    {
      store_le_int16(fd.type, data + *inoutDataOffset);
      *inoutDataOffset += sizeof(uint16_t);

      if ((hintArrayOff != WIGNORE_OFF) && (hintTextOff != WIGNORE_OFF))
        return WCS_TYPE_MISMATCH;

      cs = cmd_read_basic_stack_top(conn, rowValue, inoutDataOffset);
    }

  return cs;
}


uint_t
cmd_read_field_stack_top(ClientConnection& conn,
                          StackValue&       topValue,
                          uint64_t          hintRow,
                          uint64_t          hintArrayOff,
                          uint64_t          hintTextOff,
                          uint_t* const     inoutDataOffset)
{
  assert(*inoutDataOffset < conn.MaxSize());
  assert(conn.Stack().Size() > 0);
  assert(IS_FIELD(topValue.Operand().GetType()));

  uint8_t* const    data  = conn.Data();
  ITable&           table = topValue.Operand().GetTable();
  const FIELD_INDEX field = topValue.Operand().GetField();

  DBSFieldDescriptor fieldDesc = table.DescribeField(field);

  const uint_t minSize = sizeof(uint64_t) * sizeof(uint64_t);
  if (*inoutDataOffset + minSize > conn.MaxSize())
    return WCS_LARGE_RESPONSE;

  uint16_t type = fieldDesc.type | WHC_TYPE_FIELD_MASK;
  if (fieldDesc.isArray)
    type |= WHC_TYPE_ARRAY_MASK;


  const uint64_t rowsCount = table.AllocatedRows();
  store_le_int64(rowsCount, data + *inoutDataOffset);
  *inoutDataOffset += sizeof(uint64_t);

  if (rowsCount == 0)
    return WCS_OK;

  if (hintRow >= rowsCount)
    hintRow = 0;

  store_le_int64(hintRow, data + *inoutDataOffset);
  *inoutDataOffset += sizeof(uint64_t);

  uint_t status = WCS_GENERAL_ERR;
  for (uint64_t row = hintRow; row < rowsCount; ++row)
    {
      const uint16_t  prevOffset = *inoutDataOffset;

      StackValue  el = topValue.Operand().GetValueAt(row);

      if (fieldDesc.isArray)
        {
          if (hintTextOff != WIGNORE_OFF)
            return WCS_INVALID_ARGS;

          status = cmd_read_array_stack_top(conn,
                                             el,
                                             hintArrayOff,
                                             inoutDataOffset);
        }
      else if (fieldDesc.type == T_TEXT)
        {
          if (hintArrayOff != WIGNORE_OFF)
            return WCS_INVALID_ARGS;

          status = cmd_read_text_stack_top(conn,
                                            el,
                                            hintTextOff,
                                            inoutDataOffset);
        }
      else
        {
          if ((hintArrayOff != WIGNORE_OFF) || (hintTextOff != WIGNORE_OFF))
              return WCS_INVALID_ARGS;

          status = cmd_read_basic_stack_top(conn, el, inoutDataOffset);
        }

      if (status != WCS_OK)
        {
          if (row == hintRow)
            return status;

          else
            {
              //At least one iteration was good! Return what was fitted.
              *inoutDataOffset = prevOffset;
              return WCS_OK;
            }
        }
    }

  return status;
}


uint_t
cmd_read_table_stack_top(ClientConnection& conn,
                          StackValue&       topValue,
                          const uint_t      hintField,
                          uint64_t          hintRow,
                          uint64_t          hintArrayOff,
                          uint64_t          hintTextOff,
                          uint_t* const     inoutDataOffset)
{
  assert(*inoutDataOffset < conn.MaxSize());
  assert(conn.Stack().Size() > 0);
  assert(IS_TABLE(topValue.Operand().GetType()));

  uint8_t* const  data        = conn.Data();
  const uint_t    maxDataSize = conn.MaxSize();
  ITable&         table       = topValue.Operand().GetTable();

  const FIELD_INDEX fieldsCount = table.FieldsCount();
  const ROW_INDEX   rowsCount   = table.AllocatedRows();
  if (hintField >= fieldsCount)
    return WCS_INVALID_ARGS;

  else
  {
      const uint_t minSize = sizeof(uint64_t) +
                             sizeof(uint64_t) +
                             sizeof(uint16_t);

      if (*inoutDataOffset + minSize >= maxDataSize)
        return WCS_LARGE_RESPONSE;
  }

  store_le_int64(rowsCount, data + *inoutDataOffset);
  *inoutDataOffset += sizeof(uint64_t);

  if (rowsCount == 0)
    return WCS_OK;

  if (hintRow >= rowsCount)
    hintRow = 0;

  uint64_t currentRow = hintRow;

  store_le_int64(currentRow, data + *inoutDataOffset);
  *inoutDataOffset += sizeof(uint64_t);

  if (fieldsCount > 0xFFFF)
    return WCS_OP_NOTSUPP;

  store_le_int16(fieldsCount, data + *inoutDataOffset);
  *inoutDataOffset += sizeof(uint16_t);

  while (currentRow < rowsCount)
    {
      for (uint_t fieldId = 0; fieldId < fieldsCount; ++fieldId)
        {
          const uint16_t prevOffset = *inoutDataOffset;

          //Get as much fields values as we can as it fits,
          //but the hinted one should be first.
          const FIELD_INDEX fi = (fieldId + hintField) % fieldsCount;

          const uint_t cs = cmd_read_table_field_internal(conn,
                                                           topValue,
                                                           fi,
                                                           currentRow,
                                                           hintArrayOff,
                                                           hintTextOff,
                                                           inoutDataOffset);
          if (cs != WCS_OK)
            {
              if ((fieldId == 0) && (currentRow == hintRow))
                return cs;

              else
                {
                  //At least one iteration was good! Return what was fitted.
                  *inoutDataOffset = prevOffset;
                  return WCS_OK;
                }
            }
        }
      ++currentRow;
    }

  return WCS_OK;
}


uint_t
cmd_update_stack_table_add_rows(ClientConnection& conn,
                                 uint_t* const inoutDataOff)
{
  assert_cmds_values();

  const uint8_t* const data     = conn.Data();
  const uint_t         dataSize = conn.DataSize();
  SessionStack&        stack    = conn.Stack();

  if (stack.Size() == 0)
    return WCS_INVALID_ARGS;

  StackValue& stackTop = stack[stack.Size() - 1];

  if ((*inoutDataOff + sizeof(uint32_t)) > dataSize)
    goto update_frame_error;

  try
  {
      const int32_t rowsCount = load_le_int32(data + *inoutDataOff);
      *inoutDataOff += sizeof(uint32_t);

      if (rowsCount < 0)
        return WCS_INVALID_ARGS;

      ITable& table = stackTop.Operand().GetTable();
      for (int r = 0; r < rowsCount; ++r)
        table.AddRow();
  }
  catch(InterException& e)
  {
      if (e.Code() == InterException::INVALID_OP_REQ)
        return WCS_TYPE_MISMATCH;

      throw;
  }

  return WCS_OK;

update_frame_error:
  throw ConnectionException(
                      _EXTRA(0),
                      "Frame with invalid content for stack update operation."
                            );
  return WCS_GENERAL_ERR; //Not executed anyway!
}

