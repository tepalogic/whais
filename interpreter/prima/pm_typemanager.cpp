/******************************************************************************
 PASTRA - A light database one file system and more.
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
 *****************************************************************************/

#include <assert.h>
#include <memory.h>
#include <vector>

#include "dbs/include/dbs_types.h"
#include "compiler/include/whisperc/whisperc.h"

#include "pm_typemanager.h"
#include "pm_interpreter.h"

using namespace std;
using namespace prima;

struct TypeSpec
{
  static const D_UINT8  TYPE_SPEC_END_MARK = ';';

  D_UINT16 type;
  D_UINT16 dataSize;
  D_UINT8  data[2];    /* keep this last */

  D_UINT16 GetSize () const { return dataSize + sizeof (type) + sizeof (dataSize); }

  bool operator== (const TypeSpec& second) const
    {
      if ((type == second.type ) && (GetSize () == second.GetSize ()))
        return (memcmp (data, second.data, GetSize ()) == 0);

      return false;
    }
};

TypeManager::TypeManager (Session& session) :
    m_Session (session),
    m_TypesDescriptions ()
{
}

TypeManager::~TypeManager ()
{
}

D_UINT32
TypeManager::FindTypeDescription (const D_UINT8* const pTypeDescription)
{
  assert (IsTypeDescriptionValid (pTypeDescription));

  const TypeSpec* const pSpec = _RC (const TypeSpec*, pTypeDescription);

  D_UINT32 result = 0;

  while (result < m_TypesDescriptions.size ())
    {
      assert (IsTypeDescriptionValid (&m_TypesDescriptions[result]));

      const TypeSpec* const pSpecIt = _RC (const TypeSpec*, &m_TypesDescriptions[result]);

      if (*pSpecIt == *pSpec)
        return result;

      result += pSpecIt->GetSize ();

      assert (result <= m_TypesDescriptions.size ());
    }

  return INVALID_OFFSET;
}

D_UINT32
TypeManager::AddTypeDescription (const D_UINT8* const pTypeDescription)
{
  assert (IsTypeDescriptionValid (pTypeDescription));

  D_UINT32 result = FindTypeDescription (pTypeDescription);

  if (result != INVALID_OFFSET)
    return result;

  result = m_TypesDescriptions.size ();

  const TypeSpec* const pSpec = _RC (const TypeSpec*, pTypeDescription);

  m_TypesDescriptions.insert (m_TypesDescriptions.end (),
                              pTypeDescription,
                              pTypeDescription + pSpec->GetSize ());
  return result;
}

const D_UINT8*
TypeManager::GetTypeDescription (const D_UINT32 offset) const
{
  assert ((offset == 0) || (offset < m_TypesDescriptions.size ()));

  const D_UINT8 *const pTypeDescription = &m_TypesDescriptions[offset];

  assert (IsTypeDescriptionValid (pTypeDescription));

  return pTypeDescription;
}

static I_DBSTable&
create_non_persistent_table (I_DBSHandler& dbsHndler, D_UINT8* pInOutTypeDescription)
{
  assert (TypeManager::IsTypeDescriptionValid (pInOutTypeDescription));

  TypeSpec& spec  = *_RC(TypeSpec*, pInOutTypeDescription);

  assert (IS_TABLE (spec.type));

  vector<DBSFieldDescriptor> vFields;
  D_INT                      typeIt = 0;

  while (typeIt < spec.dataSize - 2)
    {
      DBSFieldDescriptor field;
      D_UINT16           type;

      field.m_pFieldName = _RC (D_CHAR*, &spec.data[typeIt]);

      typeIt += strlen (field.m_pFieldName) + 1;
      assert (typeIt < spec.dataSize - 2);

      type   =  spec.data[typeIt + 1]; type <<= 8; type += spec.data[typeIt];
      typeIt += 2;

      field.isArray     = IS_ARRAY (type);
      field.m_FieldType = _SC (DBS_FIELD_TYPE, GET_BASIC_TYPE (type));

      vFields.push_back (field);
    }

  I_DBSTable& table = dbsHndler.CreateTempTable (vFields.size (),
                                                 &vFields[0]);
  assert (table.GetFieldsCount () == vFields.size ());

  typeIt = 0;
  for (D_UINT fieldIndex = 0; fieldIndex < vFields.size (); ++fieldIndex)
    {
      DBSFieldDescriptor field   = table.GetFieldDescriptor (fieldIndex);
      D_UINT16           type    = field.m_FieldType;
      const D_UINT       nameLen = strlen (field.m_pFieldName) + 1;

      if (field.isArray)
        MARK_ARRAY (type);

      memcpy (&spec.data[typeIt], field.m_pFieldName, nameLen);
      typeIt += nameLen;
      spec.data[typeIt++] = type & 0xFF;
      spec.data[typeIt++] = (type >> 8) & 0xFF;
    }

  return table;
}

GlobalValue
TypeManager::CreateGlobalValue (D_UINT8* pInOutTypeDescription)
{
  assert (TypeManager::IsTypeDescriptionValid (pInOutTypeDescription));

  TypeSpec& spec = *_RC(TypeSpec*, pInOutTypeDescription);

  if ((spec.type > T_UNKNOWN) && (spec.type < T_UNDETERMINED))
    {
      switch (spec.type)
      {
      case T_BOOL:
        return GlobalValue (BoolOperand (DBSBool ()));
      case T_CHAR:
        return GlobalValue (CharOperand (DBSChar ()));
      case T_DATE:
        return GlobalValue (DateOperand (DBSDate ()));
      case T_DATETIME:
        return GlobalValue (DateTimeOperand (DBSDateTime ()));
      case T_HIRESTIME:
        return GlobalValue (HiresTimeOperand (DBSHiresTime ()));
      case T_INT8:
        return GlobalValue (Int8Operand (DBSInt8 ()));
      case T_INT16:
        return GlobalValue (Int16Operand (DBSInt16 ()));
      case T_INT32:
        return GlobalValue (Int32Operand (DBSInt32 ()));
      case T_INT64:
        return GlobalValue (Int64Operand (DBSInt64 ()));
      case T_REAL:
        return GlobalValue (RealOperand (DBSReal ()));
      case T_RICHREAL:
        return GlobalValue (RichRealOperand (DBSRichReal ()));
      case T_TEXT:
        return GlobalValue (TextOperand (DBSText ()));
      case T_UINT8:
        return GlobalValue (UInt8Operand (DBSUInt8 ()));
      case T_UINT16:
        return GlobalValue (UInt16Operand (DBSUInt16 ()));
      case T_UINT32:
        return GlobalValue (UInt32Operand (DBSUInt32 ()));
      case T_UINT64:
        return GlobalValue (UInt64Operand (DBSUInt64 ()));
      default:
        assert (false);
      }
    }
  else if (IS_ARRAY (spec.type))
    {
      switch (GET_BASIC_TYPE (spec.type))
      {
      case T_BOOL:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSBool*) NULL)));
      case T_CHAR:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSChar*) NULL)));
      case T_DATE:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSDate*) NULL)));
      case T_DATETIME:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSDateTime*) NULL)));
      case T_HIRESTIME:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSHiresTime*) NULL)));
      case T_INT8:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSInt8*) NULL)));
      case T_INT16:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSInt16*) NULL)));
      case T_INT32:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSInt32*) NULL)));
      case T_INT64:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSInt64*) NULL)));
      case T_REAL:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSReal*) NULL)));
      case T_RICHREAL:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSRichReal*) NULL)));
      case T_UINT8:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSUInt8*) NULL)));
      case T_UINT16:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSUInt16*) NULL)));
      case T_UINT32:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSUInt32*) NULL)));
      case T_UINT64:
        return GlobalValue (ArrayOperand ( DBSArray ((DBSUInt64*) NULL)));
      case T_UNDETERMINED:
        //Just a default
        return GlobalValue (ArrayOperand ( DBSArray ((DBSUInt8*) NULL)));
      default:
        assert (false);
      }
    }
  else if (IS_TABLE (spec.type))
    {
      I_DBSTable& table = create_non_persistent_table (m_Session.GetDBSHandler (),
                                                       pInOutTypeDescription);

      return GlobalValue (TableOperand (m_Session.GetDBSHandler(), table));
    }
  assert (false);
  return GlobalValue (I_Operand ());
}

StackValue
TypeManager::CreateLocalValue (D_UINT8* pInOutTypeDescription)
{
  assert (TypeManager::IsTypeDescriptionValid (pInOutTypeDescription));

  const TypeSpec& spec = *_RC(const TypeSpec*, pInOutTypeDescription);

  if ((spec.type > T_UNKNOWN) && (spec.type < T_UNDETERMINED))
    {
      switch (spec.type)
      {
      case T_BOOL:
        return StackValue (BoolOperand (DBSBool ()));
      case T_CHAR:
        return StackValue (CharOperand (DBSChar ()));
      case T_DATE:
        return StackValue (DateOperand (DBSDate ()));
      case T_DATETIME:
        return StackValue (DateTimeOperand (DBSDateTime ()));
      case T_HIRESTIME:
        return StackValue (HiresTimeOperand (DBSHiresTime ()));
      case T_INT8:
        return StackValue (Int8Operand (DBSInt8 ()));
      case T_INT16:
        return StackValue (Int16Operand (DBSInt16 ()));
      case T_INT32:
        return StackValue (Int32Operand (DBSInt32 ()));
      case T_INT64:
        return StackValue (Int64Operand (DBSInt64 ()));
      case T_REAL:
        return StackValue (RealOperand (DBSReal ()));
      case T_RICHREAL:
        return StackValue (RichRealOperand (DBSRichReal ()));
      case T_TEXT:
        return StackValue (TextOperand (DBSText ()));
      case T_UINT8:
        return StackValue (UInt8Operand (DBSUInt8 ()));
      case T_UINT16:
        return StackValue (UInt16Operand (DBSUInt16 ()));
      case T_UINT32:
        return StackValue (UInt32Operand (DBSUInt32 ()));
      case T_UINT64:
        return StackValue (UInt64Operand (DBSUInt64 ()));
      default:
        assert (false);
      }
    }
  else if (IS_ARRAY (spec.type))
    {
      switch (GET_BASIC_TYPE (spec.type))
      {
      case T_BOOL:
        return StackValue (ArrayOperand ( DBSArray ((DBSBool*) NULL)));
      case T_CHAR:
        return StackValue (ArrayOperand ( DBSArray ((DBSChar*) NULL)));
      case T_DATE:
        return StackValue (ArrayOperand ( DBSArray ((DBSDate*) NULL)));
      case T_DATETIME:
        return StackValue (ArrayOperand ( DBSArray ((DBSDateTime*) NULL)));
      case T_HIRESTIME:
        return StackValue (ArrayOperand ( DBSArray ((DBSHiresTime*) NULL)));
      case T_INT8:
        return StackValue (ArrayOperand ( DBSArray ((DBSInt8*) NULL)));
      case T_INT16:
        return StackValue (ArrayOperand ( DBSArray ((DBSInt16*) NULL)));
      case T_INT32:
        return StackValue (ArrayOperand ( DBSArray ((DBSInt32*) NULL)));
      case T_INT64:
        return StackValue (ArrayOperand ( DBSArray ((DBSInt64*) NULL)));
      case T_REAL:
        return StackValue (ArrayOperand ( DBSArray ((DBSReal*) NULL)));
      case T_RICHREAL:
        return StackValue (ArrayOperand ( DBSArray ((DBSRichReal*) NULL)));
      case T_UINT8:
        return StackValue (ArrayOperand ( DBSArray ((DBSUInt8*) NULL)));
      case T_UINT16:
        return StackValue (ArrayOperand ( DBSArray ((DBSUInt16*) NULL)));
      case T_UINT32:
        return StackValue (ArrayOperand ( DBSArray ((DBSUInt32*) NULL)));
      case T_UINT64:
        return StackValue (ArrayOperand ( DBSArray ((DBSUInt64*) NULL)));
      case T_UNDETERMINED:
        //Just a default
        return StackValue (ArrayOperand ( DBSArray ((DBSUInt8*) NULL)));
      default:
        assert (false);
      }
    }
  else if (IS_TABLE (spec.type))
    {
      I_DBSTable& table = create_non_persistent_table (m_Session.GetDBSHandler (),
                                                       pInOutTypeDescription);

      return StackValue (TableOperand (m_Session.GetDBSHandler(), table));
    }

  assert (false);
  return StackValue (I_Operand ());
}

bool
TypeManager::IsTypeDescriptionValid (const D_UINT8* pTypeDescription)
{
  const TypeSpec& spec   = *_RC (const TypeSpec*, pTypeDescription);
  bool            result = true;

  if (((spec.type == T_UNKNOWN) || (spec.type > T_UNDETERMINED)) &&
      (IS_ARRAY (spec.type) == false) &&
      (IS_TABLE (spec.type) == false))
    {
      result = false;
    }
  else if ((spec.data[spec.dataSize - 2] != spec.TYPE_SPEC_END_MARK) ||
           (spec.data[spec.dataSize - 1] != 0))
    {
      result = false;
    }
  else if (IS_FIELD (spec.type))
    {
      const D_UINT16 fieldType = GET_FIELD_TYPE (spec.type);
      if (spec.dataSize != 2)
        result = false;
      else if (IS_ARRAY (fieldType))
        {
          if (GET_BASIC_TYPE (fieldType) == T_UNKNOWN ||
              GET_BASIC_TYPE (fieldType) >= T_UNDETERMINED)
            {
              result = false;
            }
        }
      else
        {
          if (GET_BASIC_TYPE (fieldType) == T_UNKNOWN ||
              GET_BASIC_TYPE (fieldType) > T_UNDETERMINED)
            {
              result = false;
            }
        }
    }
  else if (IS_ARRAY (spec.type))
    {
      if ( (spec.dataSize != 2) ||
           (GET_BASIC_TYPE (spec.type) == T_UNKNOWN) ||
           (GET_BASIC_TYPE (spec.type) > T_UNDETERMINED) ||
           (GET_BASIC_TYPE (spec.type) == T_TEXT) )
        {
          result = false;
        }
    }
  else if (IS_TABLE (spec.type))
    {
      D_UINT index = 0;

      while ((index < (D_UINT) (spec.dataSize - 2)) && (result != FALSE))
        {
          D_UINT   id_len = strlen ((char *) &spec.data[index]);
          D_UINT16 type;

          /* don't check for zero here, because of strlen */
          index += id_len + 1;
          type = spec.data [index + 1];
          type <<= 8;
          type += spec.data [index];
          /* clear an eventual array mask */
          type = GET_BASIC_TYPE (type);
          if ((type == T_UNKNOWN) || (type >= T_UNDETERMINED))
            {
              result = false;
              break;
            }
          index += 2;
        }
    }

  return result;
}

D_UINT
TypeManager::GetTypeLength (const D_UINT8* pTypeDescription)
{
  assert (IsTypeDescriptionValid (pTypeDescription));

  const TypeSpec& ts = *_RC (const TypeSpec*, pTypeDescription);

  return ts.dataSize + sizeof (ts) - sizeof (ts.data);
}

