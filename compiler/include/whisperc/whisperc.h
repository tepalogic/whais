/******************************************************************************
WHISPERC - A compiler for whisper programs
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

#ifndef WHISPERC_H
#define WHISPERC_H

#include <stdarg.h>

#include <dbs/include/dbs_types.h>

typedef enum DBS_FIELD_TYPE VARTYPES;

#define T_ARRAY_MASK          0x0100    /* The variable is an array */
#define T_FIELD_MASK          0x0200    /* A 'field of' variable */
#define T_TABLE_MASK          0x0400    /* The variable is a table */

#define T_TABLE_FIELD_MASK    0x0800    /* The variable is a table */
#define T_L_VALUE             0x1000    /* Is a genuine l-value */

#define IS_ARRAY(type)          ((((type) & (T_FIELD_MASK | T_ARRAY_MASK)) == T_ARRAY_MASK))
#define IS_FIELD(type)          (((type) & T_FIELD_MASK) != 0)
#define IS_TABLE(type)          (((type) & T_TABLE_MASK) != 0)

#define IS_TABLE_FIELD(type)    (((type) & T_TABLE_FIELD_MASK) != 0)
#define IS_L_VALUE(type)        (((type) & T_L_VALUE) != 0)

#define MARK_ARRAY(type)        ((type) |= T_ARRAY_MASK)
#define MARK_FIELD(type)        ((type) |= T_FIELD_MASK)
#define MARK_TABLE(type)        ((type) |= T_TABLE_MASK)
#define MARK_TABLE_FIELD(type)  ((type) |= T_TABLE_FIELD_MASK)
#define MARK_L_VALUE(type)      ((type) |= T_L_VALUE)

#define GET_TYPE(type)          ((type) & ~(T_L_VALUE | T_TABLE_FIELD_MASK))
#define GET_FIELD_TYPE(type)    ((type) & ~(T_L_VALUE | T_FIELD_MASK | T_TABLE_FIELD_MASK))
#define GET_BASIC_TYPE(type)    ((type) & 0xFF)


typedef const void* WHC_HANDLER;
typedef const void* WHC_PROC_HANDLER;
typedef const void* WHC_MESSENGER_ARG;
typedef void        (*WHC_MESSENGER) (WHC_MESSENGER_ARG data,
                                      unsigned int      buffPos,
                                      unsigned int      msgId,
                                      unsigned int      msgType,
                                      const char*       pMessage,
                                      va_list           args);

#define WHC_IGNORE_BUFFER_POS      (D_UINT)(-1)

typedef struct
{
  const char*          m_Name;       /* global variable name */
  unsigned int         m_NameLength; /* length of the name */
  const unsigned char* m_Type;       /* describe the type of the variable */
  unsigned char        m_Defined;    /* 0 for externally declared global variables */
} WHC_GLBVAR_DESC;

typedef struct
{
  const char*          m_Name;
  unsigned int         m_NameLength;
  unsigned int         m_ParamsCount;
  unsigned int         m_LocalsCount;
  unsigned int         m_SyncsCount;
  unsigned int         m_CodeSize;
  const unsigned char* m_Code;
  unsigned char        m_Defined;
} WHC_PROC_DESC;

#ifdef __cplusplus
extern "C"
{
#endif

WHC_HANDLER
whc_hnd_create (const char*       pBuffer,
                unsigned          bufferSize,
                WHC_MESSENGER     messenger,
                WHC_MESSENGER_ARG messengerContext);

void
whc_hnd_destroy (WHC_HANDLER hnd);

unsigned int
whc_get_globals_count (WHC_HANDLER hnd);

unsigned int
whc_get_global (WHC_HANDLER      hnd,
                unsigned int     globalId,
                WHC_GLBVAR_DESC* pOutDescript);

unsigned int
whc_get_procs_count (WHC_HANDLER hnd);

unsigned int
whc_get_proc (WHC_HANDLER  hnd,
            unsigned int   procId,
            WHC_PROC_DESC* pOutDesc);

WHC_PROC_HANDLER
whc_get_proc_hnd (WHC_HANDLER  hnd,
                  unsigned int procId);

void
whc_release_proc_hnd (WHC_HANDLER      hnd,
                      WHC_PROC_HANDLER hProc);

const unsigned char*
whc_get_proc_rettype (WHC_HANDLER      hnd,
                      WHC_PROC_HANDLER hProc);

const unsigned char*
whc_get_local_type (WHC_HANDLER      hnd,
                    WHC_PROC_HANDLER hProc,
                    unsigned int     localId);

unsigned int
whc_get_typedec_pool (WHC_HANDLER           hnd,
                      const unsigned char** pOutPTypes);

unsigned int
whc_get_const_area (WHC_HANDLER           hnd,
                    const unsigned char** pOutPConsts);

void
whc_get_libver (unsigned int* pOutMajor,
                unsigned int* pOutMinor);

void
whc_get_lang_ver (unsigned int* pOutMajor,
                  unsigned int* pOutMinor);

#ifdef __cplusplus
}       /* extern "C" */
#endif

#endif  /* WHISPERC_H */
