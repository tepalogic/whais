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

#include "dbs/include/dbs_types.h"
#include "compiler/include/whisperc/whisperc.h"

#include "pm_typemanager.h"

using namespace std;
using namespace prima;

struct TypeSpec
{
  static const D_UINT8  TYPE_SPEC_END_MARK = ';';

  D_UINT16 type;
  D_UINT16 data_len;
  D_UINT8  data[2];    /* keep this last */

  D_UINT16 GetSize () const { return data_len + sizeof (data) + sizeof (data_len); }

  bool operator== (const TypeSpec& second) const
    {
      if (GetSize () == second.GetSize ())
        return memcmp (data, second.data, GetSize ());

      return false;
    }
};

TypeManager::TypeManager () :
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

  bool result = FindTypeDescription (pTypeDescription);

  if (result != INVALID_OFFSET)
    return result;

  result = m_TypesDescriptions.size ();
  const TypeSpec* const pSpec = _RC (const TypeSpec*, &m_TypesDescriptions[result]);

  m_TypesDescriptions.resize (result + pSpec->GetSize ());

  for (D_UINT8 index = result; index < m_TypesDescriptions.size (); ++index)
    m_TypesDescriptions [index] = pTypeDescription[index];

  return result;
}

const D_UINT8*
TypeManager::GetTypeDescription (const D_UINT32 offset) const
{
  assert (offset < m_TypesDescriptions.size ());

  const D_UINT8 *const pTypeDescription = &m_TypesDescriptions[offset];

  assert (IsTypeDescriptionValid (pTypeDescription));

  return pTypeDescription;
}

bool
TypeManager::IsTypeDescriptionValid (const D_UINT8* pTypeDescription)
{
  const TypeSpec& spec = *_RC (const TypeSpec*, pTypeDescription);
  bool            result = true;

  if (((spec.type == T_UNKNOWN) || (spec.type > T_UNDETERMINED)) &&
      ((spec.type & T_ARRAY_MASK) == 0) &&
      ((spec.type & T_CONTAINER_MASK) != T_ROW_MASK) &&
      ((spec.type & T_CONTAINER_MASK) != T_TABLE_MASK) &&
      ((spec.type & T_CONTAINER_MASK) != T_RECORD_MASK))
    {
      result = false;
    }
  else if ((spec.data[spec.data_len - 2] != spec.TYPE_SPEC_END_MARK) ||
           (spec.data[spec.data_len - 1] != 0))
    {
      result = false;
    }
  else if ((spec.type & T_ARRAY_MASK) != 0)
    {
      if ( (spec.data_len != 2) ||
           ((spec.type & ~T_ARRAY_MASK) == T_UNKNOWN) ||
           ((spec.type & ~T_ARRAY_MASK) > T_UNDETERMINED) )
        {
          result = false;
        }
    }
  else if ((spec.type & T_CONTAINER_MASK) == T_ROW_MASK)
    {
      if ((spec.data_len != 2) && (spec.data_len != 6))
        {
          result = false;
        }
    }
  else if (((spec.type & T_CONTAINER_MASK) == T_TABLE_MASK) ||
           ((spec.type & T_CONTAINER_MASK) == T_RECORD_MASK))
    {
      D_UINT index = 0;

      while ((index < (D_UINT) (spec.data_len - 2)) && (result != FALSE))
        {
          D_UINT   id_len = strlen ((char *) &spec.data[index]);
          D_UINT16 type;

          /* don't check for zero here, because of strlen */
          index += id_len + 1;
          type  = ((D_UINT16 *) & (spec.data[index]))[0];
          /* clear an eventual array mask */
          type &= ~T_ARRAY_MASK;
          if ((type == T_UNKNOWN) || (type > T_UNDETERMINED))
            {
              result = false;
              break;
            }
          index += 2;
        }
    }

  return result;

}



