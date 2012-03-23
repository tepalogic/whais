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

#include <assert.h>
#include <string.h>

#include "wlog.h"
#include "statement.h"
#include "vardecl.h"

YYSTYPE
add_idlist (YYSTYPE list, YYSTYPE id)
{
  struct SemId temp;

  assert (id->val_type == VAL_ID);
  assert ((list == NULL) || (list->val_type == VAL_ID_LIST));

  /* transform the id into in an id list's node */
  memcpy (&temp, &(id->val.u_id), sizeof (temp));
  memcpy (&(id->val.u_idlist.id), &temp, sizeof (temp));

  id->val_type          = VAL_ID_LIST;
  id->val.u_idlist.next = list;

  return id;
}

YYSTYPE
create_type_spec (struct ParserState* pState, D_UINT16 type)
{
  struct SemValue *result = get_sem_value (pState);

  if (result != NULL)
    {
      result->val_type          = VAL_TYPE_SPEC;
      result->val.u_tspec.type  = type;
      result->val.u_tspec.extra = NULL;
    }
  else
    w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);

  return result;
}

D_BOOL
process_table_decls (struct ParserState* pState,
                     struct DeclaredVar* pVar,
                     void*               pExtra)
{
  D_BOOL result = TRUE;

  assert (pVar->type == T_TABLE_MASK);
  pVar->extra = (struct DeclaredVar*) pExtra;

  return result;
}

struct DeclaredVar *
install_declaration (struct ParserState* pState,
		     YYSTYPE             sem_var,
		     YYSTYPE             sem_type,
		     D_BOOL              parameter,
		     D_BOOL              unique)
{
  struct DeclaredVar*     result = NULL;
  struct DeclaredVar*     pDecl  = NULL;
  struct SemId* const     pId    = &(sem_var->val.u_id);
  struct Statement* const stmt   = pState->pCurrentStmt;

  assert (sem_var->val_type == VAL_ID);
  assert (sem_type->val_type == VAL_TYPE_SPEC);

  if (unique)
    {
      assert ((sem_type->val.u_tspec.type & T_FIELD_MASK) == 0);
      pDecl = stmt_find_declaration (stmt, pId->text, pId->length, FALSE, FALSE);
    }
  else
    {
      assert ((sem_type->val.u_tspec.type & T_FIELD_MASK) != 0);
    }

  if (pDecl != NULL)
    {
      /* Already declared! */
      D_CHAR text[128];
      copy_text_truncate (text, pDecl->label, sizeof text, pDecl->l_label);
      w_log_msg (pState, pState->bufferPos, MSG_VAR_DEFINED, text);
    }
  else
    {
      struct DeclaredVar var;
      var.label   = pId->text;
      var.l_label = pId->length;
      var.type    = sem_type->val.u_tspec.type;
      var.extra   = NULL;
      var.offset  = 0;

      if ((var.type & T_TABLE_MASK) &&
	  (process_table_decls (pState, &var, sem_type->val.u_tspec.extra) == FALSE))
	{
	  result = NULL;	/* something went wrong along the way */
	}
      else if ((result = stmt_add_declaration (stmt, &var, parameter)) == NULL)
	{
	  /* no more memory */
	  w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
	  pState->abortError = TRUE;
	}
      else if (var.type & T_TABLE_MASK )
	{
	  /* Set a sentinel for the extra field. */
	  struct DeclaredVar *it = result->extra;

	  if (it == NULL)
            result->extra = result;
	  else
	    {
	      while (it->extra && ((it->extra->type & T_FIELD_MASK) != 0))
                it = it->extra;

	      it->extra = result;
	    }

	}
    }

  if (result && pState->externDeclaration)
    {
      assert (pState->pCurrentStmt == &pState->globalStmt);
      assert (result->var_id & GLOBAL_DECL);

      result->var_id |= EXTERN_DECL;
    }
  else if (result &&
           ((result->var_id & T_FIELD_MASK) == 0) &&
           ((result->var_id & GLOBAL_DECL) != 0) )
    {
      assert (pState->pCurrentStmt == &pState->globalStmt);

      /* Defined globals are referenced by default. */
      result->var_id &= ~NOTREF_DECL;
      result->var_id |= pState->globalStmt.localsUsed++;
    }

  return result;
}

YYSTYPE
install_list_declrs (struct ParserState* pState,
                     YYSTYPE             sem_vars,
		     YYSTYPE             sem_type)
{
  struct SemIdList *it;
  YYSTYPE result = NULL;

  assert (sem_vars->val_type == VAL_ID_LIST);
  assert (sem_type->val_type == VAL_TYPE_SPEC);

  it = &sem_vars->val.u_idlist;

  /* we do not need this anymore */
  sem_vars->val_type = VAL_REUSE;

  while (it != NULL)
    {
      struct SemValue id;

      id.val_type = VAL_ID;
      id.val.u_id = it->id;

      if ((result = (YYSTYPE) install_declaration (pState, &id, sem_type, FALSE, TRUE)) == NULL)
        break;		/* some error has been encountered */

      /* next in list */
      if (it->next != NULL)
	{
	  assert (it->next->val_type == VAL_ID_LIST);

	  /* mark this as free for reuse */
	  it->next->val_type = VAL_REUSE;

	  it = &(it->next->val.u_idlist);
	}
      else
        it = NULL;
    }

  /* mark this as free for reuse */
  sem_type->val_type = VAL_REUSE;
  return result;
}

YYSTYPE
install_field_declaration (struct ParserState*       pState,
			   YYSTYPE                   sem_var,
			   YYSTYPE                   sem_type,
			   struct DeclaredVar* const pExtra)
{
  struct DeclaredVar* result = NULL;
  struct DeclaredVar* pPrev  = NULL;
  struct DeclaredVar* pIt = pExtra;
  struct SemId*       pSemId = &sem_var->val.u_id;

  assert (sem_type->val_type == VAL_TYPE_SPEC);
  assert ((sem_type->val.u_tspec.type & T_FIELD_MASK) != 0);
  assert (sem_var->val_type == VAL_ID);

  /* check for fields with the same name */
  while (pIt != NULL)
    {
      assert ((pIt->type & T_FIELD_MASK) != 0);
      if ((pIt->l_label == pSemId->length) &&
	  (strncmp (pIt->label, pSemId->text, pIt->l_label) == 0))
	{
	  D_CHAR tname[128];

	  copy_text_truncate (tname, pSemId->text, sizeof tname, pSemId->length);
	  w_log_msg (pState, pState->bufferPos, MSG_SAME_FIELD, tname);

	  pState->abortError = TRUE;
	  return NULL;
	}
      /* next one */
      pIt = pIt->extra;
    }
  result = install_declaration (pState, sem_var, sem_type, FALSE, FALSE);

  result->extra = NULL;
  pIt           = pExtra;
  while (pIt != NULL)
    {
      /* Insert this alphabetically to make sure we avoid equivalent fields declarations. */
      const D_INT compare = strncmp (pIt->label, result->label, MIN (pIt->l_label, result->l_label));
      if ((compare > 0) || ((compare == 0) && (pIt->l_label >= result->l_label)))
        {
          assert ((compare > 0) || (pIt->l_label > result->l_label));

          if (pPrev == NULL)
            {
              result->extra = pIt;
              break;
            }
          else
            {
              assert (pPrev->extra == pIt);
              result->extra = pPrev->extra;
              result = pExtra;
              break;
            }
        }
      pPrev = pIt;
      pIt   = pIt->extra;
      if (pIt == NULL)
        {
          pPrev->extra = result;
          result       = pExtra;
        }
    }

  /* Mark for reuse! */
  sem_var->val_type = VAL_REUSE;
  sem_type->val_type = VAL_REUSE;

  return (YYSTYPE) result;
}
