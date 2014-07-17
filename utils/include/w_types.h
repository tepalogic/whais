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

#ifndef W_TYPES_H_
#define W_TYPES_H_

#include "whisper.h"

enum DBS_FIELD_TYPE
{
  T_UNKNOWN = 0,
  T_BOOL,
  T_CHAR,
  T_DATE,
  T_DATETIME,
  T_HIRESTIME,
  T_INT8,
  T_INT16,
  T_INT32,
  T_INT64,
  T_UINT8,
  T_UINT16,
  T_UINT32,
  T_UINT64,
  T_REAL,
  T_RICHREAL,
  T_TEXT,
  T_UNDETERMINED,

  T_END_OF_TYPES
};

#define DBS_BASIC_TYPE DBS_FIELD_TYPE


struct DBSFieldDescriptor
{
  const char*          name;
  enum DBS_FIELD_TYPE  type;
  bool_t               isArray;
};


#define T_ARRAY_MASK           0x0100    /* Holds an array of values. */
#define T_FIELD_MASK           0x0200    /* It's a 'field of' variable. */
#define T_TABLE_MASK           0x0400    /* It's a table variable. */

#define T_TABLE_FIELD          0x0800    /* Internal flag for table fields. */
#define T_L_VALUE              0x1000    /* Internal flag for l-values. */

#define IS_ARRAY(type)         ((((type) \
                                 & (T_FIELD_MASK | T_ARRAY_MASK)) == \
                                   T_ARRAY_MASK))
#define IS_FIELD(type)         (((type) & T_FIELD_MASK) != 0)
#define IS_TABLE(type)         (((type) & T_TABLE_MASK) != 0)

#define IS_TABLE_FIELD(type)   (((type) & T_TABLE_FIELD) != 0)
#define IS_L_VALUE(type)       (((type) & T_L_VALUE) != 0)

#define MARK_ARRAY(type)       ((type) |= T_ARRAY_MASK)
#define MARK_FIELD(type)       ((type) |= T_FIELD_MASK)
#define MARK_TABLE(type)       ((type) |= T_TABLE_MASK)
#define MARK_TABLE_FIELD(type) ((type) |= T_TABLE_FIELD)
#define MARK_L_VALUE(type)     ((type) |= T_L_VALUE)

#define GET_TYPE(type)         ((type) & ~(T_L_VALUE | T_TABLE_FIELD))
#define GET_FIELD_TYPE(type)   ((type) \
                                 & ~(T_L_VALUE | T_FIELD_MASK | T_TABLE_FIELD))

#define GET_BASIC_TYPE(type)    ((type) & 0xFF)
#define TYPE_SPEC_END_MARK      ';'
#define TYPE_SPEC_INVALID_POS   0xFFFFFFFF
#define TYPE_SPEC_ERROR         0xFFFFFFFD

struct TypeSpec
{
  uint8_t  type[2];
  uint8_t  dataSize[2];
  uint8_t  data[2];     /* VLA - Keep this last. */
};


#ifdef __cplusplus
extern "C" {
#endif


int
wh_define_basic_type (const enum DBS_BASIC_TYPE    type,
                      struct TypeSpec* const       dest);


int
wh_apply_array_modifier (struct TypeSpec* const ioBuffer);


int
wh_apply_field_modifier (struct TypeSpec* const ioBuffer);

int
wh_apply_table_modifier (struct TypeSpec* const ioBuffer);

bool_t
is_type_spec_valid (const struct TypeSpec* spec);


bool_t
compare_type_spec (const struct TypeSpec* const spec1,
                   const struct TypeSpec* const spec2);

#ifdef __cplusplus
}
#endif

#endif /* W_TYPES_H_ */
