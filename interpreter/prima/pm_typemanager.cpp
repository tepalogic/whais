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
#include "utils/include/le_converter.h"

#include "pm_typemanager.h"
#include "pm_interpreter.h"
#include "pm_general_table.h"

using namespace std;
using namespace prima;

static const D_UINT8  TYPE_SPEC_END_MARK = ';';

class TypeSpec
{
public:
  TypeSpec (const D_UINT8* const pTI)
    : m_Type (from_le_int16 (pTI)),
      m_Size (from_le_int16 (pTI + sizeof (D_UINT16))),
      m_Data (pTI + 2 * sizeof (D_UINT16))
  {
  }

  D_UINT16 RawSize () const
  {
    return m_Size + 2 * sizeof (D_UINT16);
  }

  D_UINT16 DataSize () const
  {
    return m_Size;
  }

  bool operator== (const TypeSpec& second) const
    {
      if ((m_Type == second.m_Type ) && (m_Size == second.m_Size))
        return (memcmp (m_Data, second.m_Data, m_Size) == 0);

      return false;
    }

  D_UINT16       Type () const { return m_Type; }
  const D_UINT8* Data () const { return m_Data; }

private:
  const D_UINT16       m_Type;
  const D_UINT16       m_Size;
  const D_UINT8* const m_Data;
};

TypeManager::TypeManager (NameSpace& space)
  : m_NameSpace (space),
    m_TypesDescriptions ()
{
}

TypeManager::~TypeManager ()
{
}

D_UINT32
TypeManager::FindType (const D_UINT8* const pTI)
{
  assert (IsTypeValid (pTI));

  const TypeSpec spec (pTI);

  D_UINT32 result = 0;

  while (result < m_TypesDescriptions.size ())
    {
      assert (IsTypeValid (&m_TypesDescriptions[result]));

      const TypeSpec specIt (&m_TypesDescriptions[result]);

      if (spec == specIt)
        return result;

      result += specIt.RawSize();

      assert (result <= m_TypesDescriptions.size ());
    }

  return INVALID_OFFSET;
}

D_UINT32
TypeManager::AddType (const D_UINT8* const pTI)
{
  assert (IsTypeValid (pTI));

  D_UINT32 result = FindType (pTI);

  if (result != INVALID_OFFSET)
    return result;

  result = m_TypesDescriptions.size ();

  const TypeSpec spec (pTI);

  m_TypesDescriptions.insert (m_TypesDescriptions.end (),
                              pTI,
                              pTI + spec.RawSize ());
  return result;
}

const D_UINT8*
TypeManager::GetType (const D_UINT32 offset) const
{
  assert ((offset == 0) || (offset < m_TypesDescriptions.size ()));

  const D_UINT8 *const pTypeDescription = &m_TypesDescriptions[offset];

  assert (IsTypeValid (pTypeDescription));

  return pTypeDescription;
}

static I_DBSTable&
create_non_persistent_table (I_DBSHandler&  dbsHndler,
                             D_UINT8* const pInOutTI)
{
  assert (TypeManager::IsTypeValid (pInOutTI));

  TypeSpec spec (pInOutTI);

  assert (IS_TABLE (spec.Type ()));

  vector<DBSFieldDescriptor> vFields;
  D_INT                      typeOff = 0;

  while (typeOff < spec.DataSize () - 2)
    {
      DBSFieldDescriptor field;
      D_UINT16           type;

      field.m_pFieldName = _RC (const D_CHAR*, spec.Data () + typeOff);

      typeOff += strlen (field.m_pFieldName) + 1;
      assert (typeOff < spec.DataSize () - 2);

      type    =  from_le_int16 (spec.Data () + typeOff);
      typeOff += 2;

      field.isArray     = IS_ARRAY (type);
      field.m_FieldType = _SC (DBS_FIELD_TYPE, GET_BASIC_TYPE (type));

      vFields.push_back (field);
    }

  if (vFields.size () == 0)
    return GeneralTable::Instance ();

  I_DBSTable& table = dbsHndler.CreateTempTable (vFields.size (),
                                                 &vFields[0]);
  assert (table.GetFieldsCount () == vFields.size ());

  typeOff = 0;
  for (D_UINT fieldIndex = 0; fieldIndex < vFields.size (); ++fieldIndex)
    {
      DBSFieldDescriptor field   = table.GetFieldDescriptor (fieldIndex);
      D_UINT16           type    = field.m_FieldType;
      const D_UINT       nameLen = strlen (field.m_pFieldName) + 1;

      if (field.isArray)
        MARK_ARRAY (type);

      memcpy (_CC (D_UINT8*, spec.Data()) + typeOff,
              field.m_pFieldName,
              nameLen);
      typeOff += nameLen;
      store_le_int16 (type, _CC (D_UINT8*, spec.Data()) + typeOff);
      typeOff += sizeof (D_UINT16);
    }

  return table;
}

GlobalValue
TypeManager::CreateGlobalValue (D_UINT8*    pInOutTI,
                                I_DBSTable* pPersistentTable)
{
  assert (TypeManager::IsTypeValid (pInOutTI));

  const TypeSpec spec (pInOutTI);

  if ((spec.Type () > T_UNKNOWN) && (spec.Type () < T_UNDETERMINED))
    {
      assert (pPersistentTable == NULL);
      switch (spec.Type ())
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
  else if (IS_ARRAY (spec.Type ()))
    {
      assert (pPersistentTable == NULL);
      switch (GET_BASIC_TYPE (spec.Type ()))
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
        assert (false);
        return GlobalValue (ArrayOperand ( DBSArray ()));
      default:
        assert (false);
      }
    }
  else if (IS_FIELD (spec.Type ()))
    {
      assert (pPersistentTable == NULL);
      assert (GET_FIELD_TYPE (spec.Type ()) > T_UNKNOWN);
      assert (GET_FIELD_TYPE (spec.Type ()) < T_UNDETERMINED);
      return GlobalValue (FieldOperand (GET_FIELD_TYPE (spec.Type ())));
    }
  else if (IS_TABLE (spec.Type ()))
    {
      if (pPersistentTable == NULL)
        {
          I_DBSTable& table = create_non_persistent_table (
                                             m_NameSpace.GetDBSHandler (),
                                             pInOutTI
                                                          );
          return GlobalValue (TableOperand (m_NameSpace.GetDBSHandler(),
                                            table));
        }
      else
        return GlobalValue (TableOperand (m_NameSpace.GetDBSHandler(),
                                          *pPersistentTable));
    }
  assert (false);
  return GlobalValue (NullOperand ());
}

StackValue
TypeManager::CreateLocalValue (D_UINT8* pInOutTI)
{
  assert (TypeManager::IsTypeValid (pInOutTI));

  const TypeSpec spec (pInOutTI);

  if ((spec.Type () > T_UNKNOWN) && (spec.Type () < T_UNDETERMINED))
    {
      switch (spec.Type ())
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
  else if (IS_ARRAY (spec.Type ()))
    {
      switch (GET_BASIC_TYPE (spec.Type ()))
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
        return StackValue (ArrayOperand (DBSArray ()));
      default:
        assert (false);
      }
    }
  else if (IS_FIELD (spec.Type ()))
    {
      assert (GET_FIELD_TYPE (spec.Type ()) > T_UNKNOWN);
      return StackValue (FieldOperand (GET_FIELD_TYPE (spec.Type ())));
    }
  else if (IS_TABLE (spec.Type ()))
    {
      I_DBSTable& table = create_non_persistent_table (
                                             m_NameSpace.GetDBSHandler (),
                                             pInOutTI);

      return StackValue (TableOperand (m_NameSpace.GetDBSHandler(), table));
    }

  assert (false);
  return StackValue (NullOperand ());
}

bool
TypeManager::IsTypeValid (const D_UINT8* pTI)
{
  bool result = true;
  const TypeSpec spec (pTI);

  if (((spec.Type () == T_UNKNOWN) || (spec.Type () > T_UNDETERMINED))
      && (IS_ARRAY (spec.Type ()) == false)
      && (IS_TABLE (spec.Type ()) == false)
      && (IS_FIELD (spec.Type ()) == false))
    {
      result = false;
    }
  else if ((spec.Data()[spec.DataSize () - 2] != TYPE_SPEC_END_MARK)
           || (spec.Data()[spec.DataSize () - 1] != 0))
    {
      result = false;
    }
  else if (IS_FIELD (spec.Type ()))
    {
      const D_UINT16 fieldType = GET_FIELD_TYPE (spec.Type ());
      if (spec.DataSize () != 2)
        result = false;
      else if (IS_ARRAY (fieldType))
        {
          if ((GET_BASIC_TYPE (fieldType) == T_UNKNOWN)
              || (GET_BASIC_TYPE (fieldType) >= T_UNDETERMINED)
              || (GET_BASIC_TYPE (fieldType) == T_TEXT)) // Not supported yet!
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
  else if (IS_ARRAY (spec.Type ()))
    {
      if ( (spec.DataSize () != 2)
          || (GET_BASIC_TYPE (spec.Type ()) == T_UNKNOWN)
          || (GET_BASIC_TYPE (spec.Type ()) > T_UNDETERMINED)
          || (GET_BASIC_TYPE (spec.Type ()) == T_TEXT)) // Not supported yet!
        {
          result = false;
        }
    }
  else if (IS_TABLE (spec.Type ()))
    {
      D_UINT index = 0;

      while ((index < (D_UINT) (spec.DataSize () - 2)) && (result != FALSE))
        {
          D_UINT id_len = strlen (_RC (const D_CHAR*, spec.Data ()) + index);

          /* don't check for zero here, because of strlen */
          index += id_len + 1;

          D_UINT16 type = from_le_int16 (spec.Data () + index);
          /* clear an eventual array mask */
          type = GET_BASIC_TYPE (type);
          if ((type == T_UNKNOWN) || (type >= T_UNDETERMINED))
            {
              result = false;
              break;
            }
          index += sizeof (D_UINT16);
        }
    }

  return result;
}

D_UINT
TypeManager::GetTypeLength (const D_UINT8* pTI)
{
  assert (IsTypeValid (pTI));

  const TypeSpec ts (pTI);

  return ts.RawSize ();
}

vector<D_UINT8>
prima::compute_table_typeinfo (I_DBSTable& table)
{
  vector<D_UINT8> data;

  const FIELD_INDEX fieldsCount = table.GetFieldsCount ();
  for (FIELD_INDEX fieldId = 0; fieldId < fieldsCount; ++fieldId)
    {
      DBSFieldDescriptor field = table.GetFieldDescriptor (fieldId);

      const D_UINT nameLen = strlen (field.m_pFieldName) + 1;

      data.insert (data.end (),
                   field.m_pFieldName,
                   field.m_pFieldName + nameLen);

      D_UINT16 type = field.m_FieldType;
      if (field.isArray)
        MARK_ARRAY (type);

      data.insert (data.end (),
                   _RC (D_UINT8*, &type),
                   _RC (D_UINT8*, &type) + 2);
    }

  data.push_back (TYPE_SPEC_END_MARK);
  data.push_back (0);

  vector<D_UINT8> result;

  D_UINT16 temp = 0;
  MARK_TABLE (temp);
  result.insert (result.end (),
                 _RC (D_UINT8*, &temp),
                 _RC (D_UINT8*, &temp) + sizeof (temp));

  temp = data.size ();
  result.insert (result.end (),
                 _RC (D_UINT8*, &temp),
                 _RC (D_UINT8*, &temp) + sizeof (temp));

  result.insert (result.end (), data.begin (), data.end ());

  return result;
}
