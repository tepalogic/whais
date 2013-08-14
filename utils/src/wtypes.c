/******************************************************************************
UTILS - Common routines used trough WHISPER project
Copyright (C) 2009  Iulian Popa

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

#include <string.h>

#include "utils/wtypes.h"
#include "utils/le_converter.h"


int
wh_define_basic_type (const enum DBS_BASIC_TYPE   type,
                      struct TypeSpec* const      dest)
{
  store_le_int16 (type, dest->type);
  store_le_int16 (sizeof (*dest), dest->dataSize);

  dest->data[0] = TYPE_SPEC_END_MARK;
  dest->data[1] = 0;

  return sizeof (*dest);
}


int
wh_apply_array_modifier (struct TypeSpec* const ioBuffer)
{
  uint16_t type = load_le_int16 (ioBuffer->type);

  if ( (is_type_spec_valid (ioBuffer) == FALSE)
      || (GET_BASIC_TYPE (type) == T_TEXT)
      || (GET_BASIC_TYPE (type) <= T_UNKNOWN)
      || (GET_BASIC_TYPE (type) >= T_UNDETERMINED))
    {
      return -1;
    }

  MARK_ARRAY (type);
  store_le_int16 (type, ioBuffer->type);

  return sizeof (*ioBuffer);
}


int
wh_apply_field_modifier (struct TypeSpec* const ioBuffer)
{
  uint16_t type = load_le_int16 (ioBuffer->type);

  if ( (is_type_spec_valid (ioBuffer) == FALSE)
      || (GET_BASIC_TYPE (type) <= T_UNKNOWN)
      || (GET_BASIC_TYPE (type) >= T_UNDETERMINED))
    {
      return -1;
    }

  MARK_FIELD (type);
  store_le_int16 (type, ioBuffer->type);

  return sizeof (*ioBuffer);
}


bool_t
is_type_spec_valid (const struct TypeSpec* spec)
{
  bool_t result = TRUE;

  const uint16_t htype = load_le_int16 (spec->type);
  const uint16_t hsize = load_le_int16 (spec->dataSize);

  if (((htype == T_UNKNOWN) || (htype > T_UNDETERMINED))
      && (IS_FIELD (htype) == FALSE)
      && (IS_ARRAY (htype) == FALSE)
      && (IS_TABLE (htype) == FALSE))
    {
      result = FALSE;
    }
  else if ((spec->data[hsize - 2] != TYPE_SPEC_END_MARK)
           || (spec->data[hsize - 1] != 0))
    {
      result = FALSE;
    }
  else if (IS_FIELD (htype))
    {
      const uint16_t fieldType = GET_FIELD_TYPE (htype);

      if (hsize != 2)
        result = FALSE;

      else if (IS_ARRAY (fieldType))
        {
          if ((GET_BASIC_TYPE (fieldType) == T_UNKNOWN)
              || (GET_BASIC_TYPE (fieldType) > T_UNDETERMINED))
            {
              result = FALSE;
            }
        }
      else
        {
          if ((GET_BASIC_TYPE (fieldType) == T_UNKNOWN)
              || (GET_BASIC_TYPE (fieldType) > T_UNDETERMINED))
            {
              result = FALSE;
            }
        }
    }
  else if (IS_ARRAY (htype))
    {
      if ((hsize != 2)
          || (GET_BASIC_TYPE (htype) == T_UNKNOWN)
          || (GET_BASIC_TYPE (htype) > T_UNDETERMINED) )
        {
          result = FALSE;
        }
    }
  else if (IS_TABLE (htype))
    {
      uint_t index = 0;

      while ((index < (uint_t) (hsize - 2)) && (result != FALSE))
        {
          uint16_t type;
          uint_t   identifierLength = strlen ((char *) &spec->data[index]);

          /* Don't check for zero here, because of strlen () */
          index += identifierLength + 1;
          type   = load_le_int16 (&(spec->data[index]));

          /* Ignore an eventual array mask */
          if ( (GET_BASIC_TYPE (type) == T_UNKNOWN) ||
               (GET_BASIC_TYPE (type) > T_UNDETERMINED))
            {
              result = FALSE;
              break;
            }
          index += 2;
        }
    }

  return result;
}


bool_t
compare_type_spec (const struct TypeSpec* const spec1,
                   const struct TypeSpec* const spec2)
{
  const uint_t t1 = load_le_int16 (spec1->type);
  const uint_t s1 = load_le_int16 (spec1->dataSize);

  const uint_t t2 = load_le_int16 (spec2->type);
  const uint_t s2 = load_le_int16 (spec2->dataSize);

  return (t1 == t2 )
         && (s1 == s2)
         && (memcmp (spec1->data, spec2->data, s1) == 0);
}

