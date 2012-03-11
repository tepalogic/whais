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

#define T_ARRAY_MASK    0x0100	/* a mask to tell that variable is an array */

#define T_RECORD_MASK   0x0200	/*the variable is a set */
#define T_ROW_MASK      0x0400	/*the variable is a row of a table */
#define T_TABLE_MASK    0x0800	/*the variable is a table */
#define T_CONTAINER_MASK  (T_ROW_MASK | T_TABLE_MASK | T_RECORD_MASK)

#define T_FIELD_MASK   0x1000	/* this variable was not declared directly
				   and it is a field of a record,
				   a raw or table */
#define T_L_VALUE      0x2000	/* Mask applied to suggest if this is a
				   genuine l-value */

typedef const void* WHC_HANDLER;
typedef const void* WHC_PROC_HANDLER;
typedef const void* WHC_MESSENGER_ARG;
typedef void (*WHC_MESSENGER) (WHC_MESSENGER_ARG data,
			       unsigned int buff_pos,
			       unsigned int msg_id,
			       unsigned int msg_type,
			       const char *msg_format, va_list args);

#define WHC_IGNORE_BUFFER_POS      (D_UINT)(-1)

typedef struct
{
  const char *name;		/* global variable name */
  unsigned int name_len;	/* length of the name */
  const unsigned char *type;	/* describe the type of the variable */
  unsigned char defined;	/* 0 for externally declared global variables */
} WHC_GLBVAR_DESC;

typedef struct
{
  const char *name;
  unsigned int name_len;
  unsigned int param_count;
  unsigned int locals_count;
  unsigned int syncs_count;
  unsigned int instrs_size;
  const unsigned char *instrs;
  unsigned char defined;
} WHC_PROC_DESC;

#ifdef __cplusplus
extern "C"
{
#endif

WHC_HANDLER
whc_hnd_create (const char *buffer,
                unsigned buffer_len,
                WHC_MESSENGER msg_func, WHC_MESSENGER_ARG data);

void
whc_hnd_destroy (WHC_HANDLER hnd);

unsigned int
whc_get_globals_count (WHC_HANDLER hnd);

unsigned int
whc_get_global (WHC_HANDLER hnd, unsigned int glb_id, WHC_GLBVAR_DESC * output);

unsigned int
whc_get_procs_count (WHC_HANDLER hnd);

unsigned int
whc_get_proc (WHC_HANDLER hnd, unsigned int proc_id, WHC_PROC_DESC * output);

WHC_PROC_HANDLER
whc_get_proc_hnd (WHC_HANDLER hnd, unsigned int proc_id);

void
whc_release_proc_hnd (WHC_HANDLER hnd, WHC_PROC_HANDLER h_proc);

const unsigned char*
whc_get_proc_rettype (WHC_HANDLER hnd, WHC_PROC_HANDLER h_proc);

const unsigned char *
whc_get_local_type (WHC_HANDLER hnd, WHC_PROC_HANDLER h_proc,
                    unsigned int local);

unsigned int
whc_get_typedec_pool (WHC_HANDLER hnd, const unsigned char **types);

unsigned int
whc_get_const_area (WHC_HANDLER hnd, const unsigned char **consts);

void
whc_get_libver (unsigned int *major, unsigned int *minor);

void
whc_get_lang_ver (unsigned int *major, unsigned int *minor);

#ifdef __cplusplus
}				/* extern "C" */
#endif

#endif				/* WHISPERC_H */
