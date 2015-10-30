/******************************************************************************
WHAISC - A compiler for whais programs
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

#include "whais.h"

#include "wlog.h"
#include "statement.h"
#include "vardecl.h"

YYSTYPE
add_id_to_list (YYSTYPE list, YYSTYPE id)
{
  struct SemId temp;

  assert (id->val_type == VAL_ID);
  assert ((list == NULL) || (list->val_type == VAL_ID_LIST));

  /* transform the id into in an list of ids node */
  memcpy (&temp, &(id->val.u_id), sizeof (temp));
  memcpy (&(id->val.u_idlist.id), &temp, sizeof (temp));

  id->val_type          = VAL_ID_LIST;
  id->val.u_idlist.next = list;

  return id;
}


YYSTYPE
create_type_spec (struct ParserState* parser, const uint16_t type)
{
  struct SemValue* result = alloc_sem_value (parser);

  if (result != NULL)
    {
      result->val_type          = VAL_TYPE_SPEC;
      result->val.u_tspec.type  = type;
      result->val.u_tspec.extra = NULL;
    }
  else
    log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

  return result;
}


static bool_t
process_table_decls (struct ParserState* parser,
                     struct DeclaredVar* var,
                     void*               extra)
{
  bool_t result = TRUE;

  assert (IS_TABLE( var->type));

  var->extra = (struct DeclaredVar*) extra;

  return result;
}

bool_t
compare_extern_table_types (const struct DeclaredVar* const firstTable,
                            const struct DeclaredVar* const secondFields)
{
  const struct DeclaredVar* fit = firstTable->extra;
  const struct DeclaredVar* sit = secondFields;

  while (fit != firstTable)
    {
      if (sit == NULL)
        return FALSE;

      assert (IS_TABLE_FIELD (fit->type));
      assert (IS_TABLE_FIELD (sit->type));

      fit = fit->extra, sit = sit->extra;
    }

  if (sit != NULL)
    return FALSE;

  fit = firstTable->extra;
  while (fit != firstTable)
    {
      sit = secondFields;
      while (sit != NULL)
        {
          if ((fit->labelLength == sit->labelLength)
              && (strncmp (fit->label, sit->label, fit->labelLength) == 0))
            {
              break;
            }
          sit = sit->extra;
        }

      if ((sit == NULL) || (fit->type != sit->type))
        return FALSE;

      fit = fit->extra;
    }

  return TRUE;
}


struct DeclaredVar*
add_declaration (struct ParserState* const parser,
                 YYSTYPE                   var,
                 YYSTYPE                   type,
                 const bool_t              parameter,
                 const bool_t              unique)
{
  struct DeclaredVar*     result = NULL;
  struct DeclaredVar*     decl   = NULL;
  struct SemId* const     id     = &(var->val.u_id);
  struct Statement* const stmt   = parser->pCurrentStmt;

  assert (var->val_type == VAL_ID);
  assert ((type == NULL) || (type->val_type == VAL_TYPE_SPEC));

  if (unique)
    {
      assert ((type == NULL) ||
              (IS_TABLE_FIELD( type->val.u_tspec.type) == FALSE));

      if ((type == NULL)
          && (parser->pCurrentStmt->type == STMT_GLOBAL))
        {
          char text[128];

          wh_copy_first (text, id->name, sizeof text, id->length);
          log_message (parser, parser->bufferPos, MSG_VAR_GLB_RETURN, text);

          parser->abortError = TRUE;
          return NULL;
        }

      decl = stmt_find_declaration (stmt, id->name, id->length, FALSE, FALSE);
      if ((decl != NULL)
          && (parser->pCurrentStmt->type == STMT_GLOBAL))
        {
          assert ( ! parameter);

          if ((decl->type != type->val.u_tspec.type)
              || (IS_TABLE (decl->type)
                  && ! compare_extern_table_types (
                                        decl,
                                        type->val.u_tspec.extra)))
            {
              char text[128];

              wh_copy_first (text, decl->label, sizeof text, decl->labelLength);
              log_message (parser, parser->bufferPos, MSG_VAR_DECL_NA, text);
              log_message (parser, decl->declarationPos, MSG_DECL_PREV);

              return decl;
            }
          else if ( ! IS_EXTERNAL (decl->varId) && parser->externDecl)
            {
              char text[128];

              wh_copy_first (text, decl->label, sizeof text, decl->labelLength);
              log_message (parser, parser->bufferPos, MSG_VAR_EXT_LATE, text);
              log_message (parser, decl->definitionPos, MSG_DECL_PREV);

              return decl;
            }
          else if (IS_EXTERNAL (decl->varId) && ! parser->externDecl)
            {
              decl->definitionPos = (id->name - parser->buffer);
              decl->declarationPos = decl->definitionPos;

              if (! IS_REFERRED (decl->varId))
                decl->varId = parser->globalStmt.localsUsed++;

              else
                decl->varId = RETRIVE_ID (decl->varId);

              MARK_AS_GLOBAL (decl->varId);
              MARK_AS_REFERENCED (decl->varId);

              return decl;
            }
          else if (IS_EXTERNAL (decl->varId))
            return decl;
        }
    }
  else
    {
      assert (IS_TABLE_FIELD( type->val.u_tspec.type));
    }

  if (decl != NULL)
    {
      char text[128];

      wh_copy_first (text, decl->label, sizeof text, decl->labelLength);
      log_message (parser, parser->bufferPos, MSG_VAR_DEFINED, text);
      log_message (parser, decl->declarationPos, MSG_DECL_PREV);

      parser->abortError = TRUE;
    }
  else
    {
      struct DeclaredVar var = {0, };

      var.label          = id->name;
      var.labelLength    = id->length;
      var.declarationPos = (id->name - parser->buffer);
      var.type           = (type != NULL) ? type->val.u_tspec.type : 0;

      if (var.type == 0)
        {
          if ((result = stmt_add_declaration (stmt, &var, FALSE)) == NULL)
            {
              log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
              parser->abortError = TRUE;
            }
        }
      else if (IS_TABLE( var.type)
          && ! process_table_decls (parser, &var, type->val.u_tspec.extra))
        {
          result = NULL;   /* Something went wrong along the way */
        }
      else if ((stmt->type != STMT_GLOBAL)
               && ! IS_TABLE_FIELD (var.type)
               && (wh_ostream_size (&stmt->spec.proc.code) > 0))
        {
          char text[128];

          wh_copy_first (text, var.label, sizeof text, var.labelLength);
          log_message (parser, parser->bufferPos, MSG_VAR_LATE, text);

          parser->abortError = TRUE;
        }
      else if ((result = stmt_add_declaration (stmt, &var, parameter)) == NULL)
        {
          log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
          parser->abortError = TRUE;
        }
      else if (IS_TABLE( var.type))
        {
          struct DeclaredVar* it = result->extra;

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

  if (result && parser->externDecl)
    {
      assert (parser->pCurrentStmt == &parser->globalStmt);
      assert (result->varId & GLOBAL_DECL);

      MARK_AS_EXTERNAL (result->varId);
    }
  else if (result
           && (IS_TABLE_FIELD( result->type) == FALSE)
           && IS_GLOBAL (result->varId))
    {
      assert (parser->pCurrentStmt == &parser->globalStmt);

      /* Defined globals are referenced by default. */
      result->varId |= parser->globalStmt.localsUsed++;
      result->definitionPos = result->declarationPos;
      MARK_AS_REFERENCED (result->varId);
    }

  return result;
}


YYSTYPE
add_list_declaration (struct ParserState* parser,
                     YYSTYPE              varsList,
                     YYSTYPE              type)
{
  YYSTYPE           result = NULL;
  struct SemIdList* it     = &varsList->val.u_idlist;

  assert (varsList->val_type == VAL_ID_LIST);
  assert ((type == NULL) || (type->val_type == VAL_TYPE_SPEC));

  free_sem_value (varsList);

  while (it != NULL)
    {
      struct SemValue id;

      id.val_type = VAL_ID;
      id.val.u_id = it->id;

      result = (YYSTYPE)add_declaration( parser, &id, type, FALSE, TRUE);
      if (result == NULL)
        break;    /* Some error has been encountered */

      if (it->next != NULL)
        {
          assert (it->next->val_type == VAL_ID_LIST);

          free_sem_value (it->next);
          it = &(it->next->val.u_idlist);
        }
      else
        it = NULL;
    }

  /* mark this as free for reuse */
  if (type != NULL)
    free_sem_value (type);

  return result;
}


YYSTYPE
add_field_declaration (struct ParserState*       parser,
                       YYSTYPE                   var,
                       YYSTYPE                   type,
                       struct DeclaredVar* const extra)
{
  struct DeclaredVar* result = NULL;
  struct DeclaredVar* prev   = NULL;
  struct DeclaredVar* it     = extra;
  struct SemId*       id     = &var->val.u_id;

  assert (type->val_type == VAL_TYPE_SPEC);
  assert (IS_TABLE_FIELD( type->val.u_tspec.type));
  assert (var->val_type == VAL_ID);

  /* Check for fields with the same name. */
  while (it != NULL)
    {
      assert (IS_TABLE_FIELD( it->type) != 0);

      if ((it->labelLength == id->length) &&
          (strncmp( it->label, id->name, it->labelLength) == 0))
        {
          char tname[128];

          wh_copy_first (tname, id->name, sizeof tname, id->length);

          log_message (parser, parser->bufferPos, MSG_SAME_FIELD, tname);

          parser->abortError = TRUE;
          return NULL;
        }
      it = it->extra;
    }

  if (GET_BASIC_TYPE( type->val.u_tspec.type) == T_UNDETERMINED)
    {
      char tname[128];

      wh_copy_first (tname, id->name, sizeof tname, id->length);

      log_message (parser, parser->bufferPos, MSG_FIELD_TYPE_INVALID, tname);

      return NULL;
    }

  result = add_declaration (parser, var, type, FALSE, FALSE);

  result->extra = NULL, it = extra;
  while (it != NULL)
    {
      /* Insert this alphabetically to avoid equivalent fields declarations. */
      const int compare = strncmp (it->label,
                                   result->label,
                                   MIN (it->labelLength,
                                        result->labelLength));
      if ((compare > 0)
          || ((compare == 0) && (it->labelLength >= result->labelLength)))
        {
          assert ((compare > 0) || (it->labelLength > result->labelLength));

          if (prev == NULL)
            {
              result->extra = it;
              break;
            }
          else
            {
              assert (prev->extra == it);

              result->extra = prev->extra;
              prev->extra  = result;
              result        = extra;

              break;
            }
        }

      prev = it;
      it   = it->extra;
      if (it == NULL)
        {
          prev->extra = result;
          result      = extra;
        }
    }

  free_sem_value (var);
  free_sem_value (type);

  return (YYSTYPE) result;
}

