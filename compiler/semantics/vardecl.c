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

#include "whisper.h"

#include "wlog.h"
#include "statement.h"
#include "vardecl.h"

YYSTYPE
add_id_to_list (YYSTYPE list, YYSTYPE id)
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
  struct SemValue *result = alloc_sem_value (pState);

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

static D_BOOL
process_table_decls (struct ParserState* pState,
                     struct DeclaredVar* pVar,
                     void*               pExtra)
{
  D_BOOL result = TRUE;

  assert (IS_TABLE (pVar->type));

  pVar->extra = (struct DeclaredVar*) pExtra;

  return result;
}

static D_BOOL
process_field_decls (struct ParserState* pState,
                     struct DeclaredVar* pVar)
{
  D_BOOL result = TRUE;

  assert (IS_FIELD (pVar->type));

  if (IS_ARRAY (GET_FIELD_TYPE (pVar->type)))
    {
      assert ((GET_BASIC_TYPE (pVar->type) > T_UNKNOWN) &&
              (GET_BASIC_TYPE (pVar->type) <= T_UNDETERMINED));

      if (GET_BASIC_TYPE (pVar->type) == T_UNDETERMINED)
        {
          D_CHAR tname[128];

          copy_text_truncate (tname, pVar->label, sizeof tname, pVar->labelLength);
          w_log_msg (pState, pState->bufferPos, MSG_FIELD_INV_ARRAY, tname);

          result = FALSE;
        }
    }

  return result;
}

struct DeclaredVar *
install_declaration (struct ParserState* pState,
                     YYSTYPE             pVar,
                     YYSTYPE             pType,
                     D_BOOL              parameter,
                     D_BOOL              unique)
{
  struct DeclaredVar*     result = NULL;
  struct DeclaredVar*     pDecl  = NULL;
  struct SemId* const     pId    = &(pVar->val.u_id);
  struct Statement* const stmt   = pState->pCurrentStmt;

  assert (pVar->val_type == VAL_ID);
  assert (pType->val_type == VAL_TYPE_SPEC);

  if (unique)
    {
      assert (IS_TABLE_FIELD (pType->val.u_tspec.type) == FALSE);
      pDecl = stmt_find_declaration (stmt, pId->text, pId->length, FALSE, FALSE);
    }
  else
    {
      assert (IS_TABLE_FIELD (pType->val.u_tspec.type));
    }

  if (pDecl != NULL)
    {
      /* Already declared! */
      D_CHAR text[128];
      copy_text_truncate (text, pDecl->label, sizeof text, pDecl->labelLength);
      w_log_msg (pState, pState->bufferPos, MSG_VAR_DEFINED, text);
    }
  else
    {
      struct DeclaredVar var;

      var.label       = pId->text;
      var.labelLength = pId->length;
      var.type        = pType->val.u_tspec.type;
      var.extra       = NULL;
      var.offset      = 0;

      if (IS_TABLE (var.type) &&
          (process_table_decls (pState, &var, pType->val.u_tspec.extra) == FALSE))
        {
          result = NULL;        /* something went wrong along the way */
        }
      else if (IS_FIELD (var.type) &&
          (process_field_decls(pState, &var) == FALSE))
        {
          result = NULL; /* something went wrong along the way */
        }
      else if ((result = stmt_add_declaration (stmt, &var, parameter)) == NULL)
        {
          /* no more memory */
          w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
          pState->abortError = TRUE;
        }
      else if (IS_TABLE (var.type))
        {
          /* Set a sentinel for the extra field. */
          struct DeclaredVar *it = result->extra;

          if (it == NULL)
            result->extra = result;
          else
            {
              while (it->extra && IS_TABLE_FIELD (it->extra->type))
                it = it->extra;

              it->extra = result;
            }

        }
    }

  if (result && pState->externDeclaration)
    {
      assert (pState->pCurrentStmt == &pState->globalStmt);
      assert (result->varId & GLOBAL_DECL);

      MARK_AS_EXTERNAL (result->varId);
    }
  else if (result &&
           (IS_TABLE_FIELD (result->type) == FALSE) &&
           (IS_GLOBAL (result->varId)))
    {
      assert (pState->pCurrentStmt == &pState->globalStmt);

      /* Defined globals are referenced by default. */
      result->varId |= pState->globalStmt.localsUsed++;
      MARK_AS_REFERENCED (result->varId);
    }

  return result;
}

YYSTYPE
install_list_declrs (struct ParserState* pState,
                     YYSTYPE             pVarsList,
                     YYSTYPE             pType)
{
  YYSTYPE           result = NULL;
  struct SemIdList* pIt    = &pVarsList->val.u_idlist;

  assert (pVarsList->val_type == VAL_ID_LIST);
  assert (pType->val_type == VAL_TYPE_SPEC);

  /* we do not need this anymore */
  free_sem_value (pVarsList);

  while (pIt != NULL)
    {
      struct SemValue id;

      id.val_type = VAL_ID;
      id.val.u_id = pIt->id;

      if ((result = (YYSTYPE)install_declaration (pState, &id, pType, FALSE, TRUE)) == NULL)
        break;                /* some error has been encountered */

      /* next in list */
      if (pIt->next != NULL)
        {
          assert (pIt->next->val_type == VAL_ID_LIST);

          free_sem_value (pIt->next);
          pIt = &(pIt->next->val.u_idlist);
        }
      else
        pIt = NULL;
    }

  /* mark this as free for reuse */
  free_sem_value (pType);

  return result;
}

YYSTYPE
install_field_declaration (struct ParserState*       pState,
                           YYSTYPE                   pVar,
                           YYSTYPE                   pType,
                           struct DeclaredVar* const pExtra)
{
  struct DeclaredVar* result = NULL;
  struct DeclaredVar* pPrev  = NULL;
  struct DeclaredVar* pIt    = pExtra;
  struct SemId*       pSemId = &pVar->val.u_id;

  assert (pType->val_type == VAL_TYPE_SPEC);
  assert (IS_TABLE_FIELD (pType->val.u_tspec.type));
  assert (pVar->val_type == VAL_ID);

  /* check for fields with the same name */
  while (pIt != NULL)
    {
      assert (IS_TABLE_FIELD (pIt->type) != 0);
      if ((pIt->labelLength == pSemId->length) &&
          (strncmp (pIt->label, pSemId->text, pIt->labelLength) == 0))
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
  result = install_declaration (pState, pVar, pType, FALSE, FALSE);

  result->extra = NULL;
  pIt           = pExtra;
  while (pIt != NULL)
    {
      /* Insert this alphabetically to make sure we avoid equivalent fields declarations. */
      const D_INT compare = strncmp (pIt->label,
                                     result->label,
                                     MIN (pIt->labelLength, result->labelLength));
      if ((compare > 0) ||
          ((compare == 0) && (pIt->labelLength >= result->labelLength)))
        {
          assert ((compare > 0) || (pIt->labelLength > result->labelLength));

          if (pPrev == NULL)
            {
              result->extra = pIt;
              break;
            }
          else
            {
              assert (pPrev->extra == pIt);

              result->extra = pPrev->extra;
              result        = pExtra;

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

  free_sem_value (pVar);
  free_sem_value (pType);

  return (YYSTYPE) result;
}
