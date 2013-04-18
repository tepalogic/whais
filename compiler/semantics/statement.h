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

#include "utils/warray.h"
#include "utils/woutstream.h"

enum STATEMENT_TYPE
{
  STMT_ERR = 0, /* Inva;od type */
  STMT_GLOBAL,
  STMT_PROC
};

struct StatementGlobalSymbol
{
  char* symbol;
  uint_t  index;
};

struct _GlobalStatmentSpec
{
  struct WOutputStream typesDescs;  /* describes the variable types */
  struct WOutputStream constsArea;
  struct WArray       procsDecls;  /* for GLOBAL statement contains the list
                                   of procedures */
  uint32_t            procsCount;
};

struct _ProcStatementSpec
{
  const char*      name;         /* name of the procedure */
  uint_t             nameLength;   /* length of the name */
  struct WArray      paramsList;   /* Used only for procedures
                                    0 - special case for return type
                                    1 - first parameter, 2 second parameter */
  struct WOutputStream code;         /* the execution path for procedure
                                    statements */
  struct WArray       branchStack;  /* keep track of conditional branches */
  struct WArray       loopStack;    /* keep the track of looping statements */
  uint32_t            procId;       /* ID of the procedure in the import table */
  uint16_t            syncTracker;
};

struct Statement
{
  struct Statement*   pParentStmt;  /* NULL for global statement */
  uint_t              localsUsed;   /* used to assign IDs to declared variables */
  enum STATEMENT_TYPE type;         /* type of the statement */
  struct WArray       decls;        /* variables declared in this statement */
  union
  {
    struct _ProcStatementSpec  proc;
    struct _GlobalStatmentSpec glb;
  } spec;
};

bool_t
init_glbl_stmt (struct Statement* pStmt);

void
clear_glbl_stmt (struct Statement* pGlbStmt);

bool_t
init_proc_stmt (struct Statement* pParentStmt, struct Statement* pOutStmt);

void
clear_proc_stmt (struct Statement* pStmt);

struct DeclaredVar*
stmt_find_declaration (struct Statement* pStmt,
                       const char*       pName,
                       const uint_t      nameLength,
                       const bool_t      recursive,
                       const bool_t      refferenced);

struct DeclaredVar*
stmt_add_declaration (struct Statement* const   pStmt,
                      struct DeclaredVar*       pVar,
                      const bool_t              parameter);

const struct DeclaredVar*
stmt_get_param (const struct Statement* const pStmt, uint_t param);

uint_t
stmt_get_param_count (const struct Statement* const pStmt);

uint32_t
stmt_get_import_id (const struct Statement* const pProc);

/* some inline functions to access statement members */
static INLINE struct WOutputStream*
stmt_query_instrs (struct Statement* const pStmt)
{
  assert (pStmt->type == STMT_PROC);
  return &pStmt->spec.proc.code;
}

static INLINE struct WArray*
stmt_query_branch_stack (struct Statement* const pStmt)
{
  assert (pStmt->type == STMT_PROC);
  return &pStmt->spec.proc.branchStack;
}

static INLINE struct WArray*
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
  uint16_t type;
  uint16_t dataSize;
  uint8_t  data[2];     /* keep this last */
};

bool_t
is_type_spec_valid (const struct TypeSpec* pSpec);

bool_t
type_spec_cmp (const struct TypeSpec* const pSpec_1,
               const struct TypeSpec* const pSpec_2);

uint_t
type_spec_fill (struct WOutputStream* const      pOutStream,
                const struct DeclaredVar* const pVar);

int
add_text_const (struct Statement* const pStmt,
                const uint8_t* const    pText,
                const uint_t            textSize);

#endif /*STATEMENT_H_ */
