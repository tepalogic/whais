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
/* #include "../../utils/include/list.h" */
#include "../../utils/include/outstream.h"

enum STATEMENT_TYPE
{
  STMT_ERR = 0, /* Inva;od type */
  STMT_GLOBAL,
  STMT_PROC
};

struct StatementGlobalSymbol
{
  D_CHAR* symbol;
  D_UINT  index;
};

struct _GlobalStatmentSpec
{
  struct OutStream typesDescs;  /* describes the variable types */
  struct OutStream constsArea;
  struct UArray    procsDecls;  /* for GLOBAL statement contains the list
                                   of procedures */
  D_UINT32         procsCount;
};

struct _ProcStatementSpec
{
  const D_CHAR*    name;         /* name of the procedure */
  D_UINT           nameLength;   /* length of the name */
  struct UArray    paramsList;   /* Used only for procedures
                                    0 - special case for return type
                                    1 - first parameter, 2 second parameter */
  struct OutStream code;         /* the execution path for procedure
                                    statements */
  struct UArray    branchStack;  /* keep track of conditional branches */
  struct UArray    loopStack;    /* keep the track of looping statements */
  D_UINT32         procId;       /* ID of the procedure in the import table */
  D_UINT16         syncTracker;
};

struct Statement
{
  struct Statement*   pParentStmt;  /* NULL for global statement */
  D_UINT              localsUsed;   /* used to assign IDs to declared variables */
  enum STATEMENT_TYPE type;         /* type of the statement */
  struct UArray       decls;        /* variables declared in this statement */
  union
  {
    struct _ProcStatementSpec  proc;
    struct _GlobalStatmentSpec glb;
  } spec;
};

D_BOOL
init_glbl_stmt (struct Statement* pStmt);

void
clear_glbl_stmt (struct Statement* pGlbStmt);

D_BOOL
init_proc_stmt (struct Statement* pParentStmt, struct Statement* pOutStmt);

void
clear_proc_stmt (struct Statement* pStmt);

struct DeclaredVar*
stmt_find_declaration (struct Statement* pStmt,
                       const char*       pName,
                       const D_UINT      nameLength,
                       const D_BOOL      recursive,
                       const D_BOOL      refferenced);

struct DeclaredVar*
stmt_add_declaration (struct Statement*   pStmt,
                      struct DeclaredVar* pVar,
                      D_BOOL              parameter);

const struct DeclaredVar*
stmt_get_param (const struct Statement* const pStmt, D_UINT param);

D_UINT
stmt_get_param_count (const struct Statement* const pStmt);

D_UINT32
stmt_get_import_id (const struct Statement* const pProc);

/* some inline functions to access statement members */
static INLINE struct OutStream*
stmt_query_instrs (struct Statement* const pStmt)
{
  assert (pStmt->type == STMT_PROC);
  return &pStmt->spec.proc.code;
}

static INLINE struct UArray*
stmt_query_branch_stack (struct Statement* const pStmt)
{
  assert (pStmt->type == STMT_PROC);
  return &pStmt->spec.proc.branchStack;
}

static INLINE struct UArray*
stmt_query_loop_stack (struct Statement* const pStmt)
{
  assert (pStmt->type == STMT_PROC);
  return &pStmt->spec.proc.loopStack;
}

/*****************************Type specification section ***************/
#define TYPE_SPEC_END_MARK      ';'
#define TYPE_SPEC_INVALID_POS   0xFFFFFFFF
#define TYPE_SPEC_ERROR         0xFFFFFFFD

struct TypeSpec
{
  D_UINT16 type;
  D_UINT16 dataSize;
  D_UINT8  data[2];     /* keep this last */
};

D_BOOL
is_type_spec_valid (const struct TypeSpec* pSpec);

D_BOOL
type_spec_cmp (const struct TypeSpec* const pSpec_1,
               const struct TypeSpec* const pSpec_2);

D_UINT
type_spec_fill (struct OutStream* const         pOutStream,
                const struct DeclaredVar* const pVar);

D_INT
add_text_const (struct Statement* const pStmt,
                const D_UINT8* const    pText,
                const D_UINT            textSize);

#endif /*STATEMENT_H_ */
