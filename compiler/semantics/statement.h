/******************************************************************************
WHAISC - A compiler for whais programs
Copyright(C) 2009  Iulian Popa

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
  STMT_ERR = 0,     /* Invalid type */
  STMT_GLOBAL,
  STMT_PROC
};

struct StatementGlobalSymbol
{
  char    *symbol;
  uint_t   index;
};

struct _GlobalStatmentSpec
{
  struct WOutputStream   typesDescs;  /* Holds the variable types. */
  struct WOutputStream   constsArea;
  struct WArray          procsDecls;  /* for GLOBAL statements is the list of procedures */
  uint32_t               procsCount;
};

struct _ProcStatementSpec
{
  const char            *name;         /* Procedure name. */
  uint_t                 nameLength;   /* Length of the procedure name. */
  struct WArray          paramsList;   /* Used only for procedures
                                          0 - special case for return type
                                          1 - first parameter,
                                          2 - second parameter. etc */
  struct WOutputStream   code;           /* Procedure's execution instructions.*/
  struct WArray          branchStack;    /* Keeps track of conditional branches.*/
  struct WArray          loopStack;      /* Keeps track of looping statements */
  struct WArray          iteratorsStack; /* Keeps track of loop iterators */
  struct WArray          iteratorsUsage; /* Keeps track of loop iterators */
  uint32_t               procId;         /* Procedure's ID in the import table. */
  uint16_t               syncTracker;    /* Keeps track of sync statements. */
  uint_t                 declarationPos;
  uint_t                 definitionPos;
  bool_t                 checkParams;
  bool_t                 returnDetected;
  bool_t                 deadCodeWarned;

};

struct Statement
{
  struct Statement     *parent;     /* nullptr for global statement */
  uint_t                localsUsed; /* Count of local values of this statement. */
  enum STATEMENT_TYPE   type;
  struct WArray         decls;      /* Holds the local variables declared in this statement */
  union
  {
    struct _ProcStatementSpec    proc;
    struct _GlobalStatmentSpec   glb;
  } spec;
};


bool_t
init_glbl_stmt(struct Statement* const stmt);

void
clear_glbl_stmt(struct Statement* const stmt);

bool_t
init_proc_stmt(struct Statement* const   parent,
               struct Statement* const   outStmt);

void
clear_proc_stmt(struct Statement* const stmt);

struct DeclaredVar*
stmt_find_declaration(struct Statement* const  stmt,
                      const char* const        name,
                      const uint_t             nameLength,
                      const bool_t             recursive,
                      const bool_t             reffered);

struct DeclaredVar*
stmt_add_declaration(struct Statement* const   stmt,
                     struct DeclaredVar       *var,
                     const bool_t              procPram);

const struct DeclaredVar*
stmt_get_param(const struct Statement* const   stmt,
               const uint_t                    param);

uint_t
stmt_get_param_count(const struct Statement* const stmt);

uint32_t
stmt_get_import_id(const struct Statement* const proc);

/* some inline functions to access statement members */
static INLINE struct WOutputStream*
stmt_query_instrs(struct Statement* const stmt)
{
  assert(stmt->type == STMT_PROC);
  return &stmt->spec.proc.code;
}

static INLINE struct WArray*
stmt_query_branch_stack(struct Statement* const stmt)
{
  assert(stmt->type == STMT_PROC);
  return &stmt->spec.proc.branchStack;
}

static INLINE struct WArray*
stmt_query_loop_stack(struct Statement* const stmt)
{
  assert(stmt->type == STMT_PROC);
  return &stmt->spec.proc.loopStack;
}

static INLINE struct WArray*
stmt_query_loop_iterators_stack(struct Statement* const stmt)
{
  assert(stmt->type == STMT_PROC);
  return &stmt->spec.proc.iteratorsStack;
}

static INLINE struct WArray*
stmt_query_usage_iterators_stack(struct Statement* const stmt)
{
  assert(stmt->type == STMT_PROC);
  return &stmt->spec.proc.iteratorsUsage;
}


uint_t
fill_type_spec(struct WOutputStream* const       typeStream,
               const struct DeclaredVar* const   var);

int
add_constant_text(struct Statement* const   stmt,
                  const uint8_t* const      text,
                  const uint_t              textSize);

#endif /*STATEMENT_H_ */
