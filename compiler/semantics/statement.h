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

#ifndef STATEMENT_H_
#define STATEMENT_H_

#include <string.h>
#include <assert.h>

#include "../../utils/include/array.h"
#include "../../utils/include/list.h"
#include "../../utils/include/outstream.h"

enum STATEMENT_TYPE
{
  STMT_ERR = 0,			/* some weird error */
  STMT_GLOBAL,
  STMT_PROC
};

struct StatementGlobalSymbol
{
  D_CHAR *symbol;
  D_UINT index;
};

struct _GlobalStatmentSpec
{
  struct OutStream type_desc;	/* describes the variable types */
  struct OutStream const_area;
  struct UArray proc_decls;	/* for GLOBAL statement contains the list
				   of procedures */
};

struct _ProcStatementSpec
{
  const D_CHAR *name;		/* name of the procedure, not necessarily
				   null terminated */
  D_UINT nlength;		/* length of the name */
  struct UArray param_list;	/* Used only for procedures
				   0 - special case for return type
				   1 - first parameter, 2 second parameter */
  struct OutStream instrs;	/* the execution path for procedure
				   statements */
#if 0
  struct OutStream const_area;	/* area where all constant values for this
				   statement are placed */
#endif
  struct UArray branch_stack;	/* keep track of conditional branches */
  struct UArray loop_stack;	/* keep the track of looping statements */
  D_UINT16 proc_id;		/* ID of the procedure in the import table */
  D_UINT16 sync_keeper;
};

struct Statement
{
  struct Statement *parent;	/* NULL for global statement */
  D_UINT locals_used;		/* used to assign IDs to declared variables */
  enum STATEMENT_TYPE type;	/* type of the statement */
  struct UArray decls;		/* variables declared in this statement */
  union
  {
    struct _ProcStatementSpec proc;
    struct _GlobalStatmentSpec glb;
  } spec;
};

D_BOOL init_glbl_stmt (struct Statement *stmt);

void clear_glbl_stmt (struct Statement *glbl);

D_BOOL init_proc_stmt (struct Statement *parent, struct Statement *stmt);

void clear_proc_stmt (struct Statement *proc);

struct DeclaredVar *stmt_find_declaration (const struct Statement *stmt,
					   const char *label,
					   const D_UINT label_len,
					   const D_BOOL recursive);

struct DeclaredVar *stmt_add_declaration (struct Statement *stmt,
					  struct DeclaredVar *var,
					  D_BOOL parameter);

const struct DeclaredVar *stmt_get_param (const struct Statement *const stmt,
					  D_UINT arg_n);
D_UINT stmt_get_param_count (const struct Statement *const stmt);

D_UINT16 stmt_get_import_id (const struct Statement *const proc);

/* some inline functions to access statement members */
static INLINE struct OutStream *
stmt_query_instrs (struct Statement *const stmt)
{
  assert (stmt->type == STMT_PROC);
  return &stmt->spec.proc.instrs;
}

static INLINE struct UArray *
stmt_query_branch_stack (struct Statement *const stmt)
{
  assert (stmt->type == STMT_PROC);
  return &stmt->spec.proc.branch_stack;
}

static INLINE struct UArray *
stmt_query_loop_stack (struct Statement *const stmt)
{
  assert (stmt->type == STMT_PROC);
  return &stmt->spec.proc.loop_stack;
}

/*****************************Type specification section ***************/
#define TYPE_SPEC_END_MARK ';'
#define TYPE_SPEC_INVALID_POS   0xFFFFFFFF
#define TYPE_SPEC_ERROR         0xFFFFFFFD

struct TypeSpec
{
  D_UINT16 type;
  D_UINT16 data_len;
  D_UINT8 data[2];		/* keep this last */
};

D_BOOL is_type_spec_valid (const struct TypeSpec *spec);

D_BOOL
type_spec_cmp (const struct TypeSpec *pSpec_1,
	       const struct TypeSpec *pSpec_2);

D_UINT type_spec_fill (struct OutStream *outs, const struct DeclaredVar *var);

D_INT
add_text_const (struct Statement *stmt, const D_UINT8 * buffer, D_UINT size);

#endif /*STATEMENT_H_ */
