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

#include "statement.h"

#include <assert.h>
#include <string.h>

#include "vardecl.h"
#include "brlo_stmts.h"

D_BOOL
init_glbl_stmt (struct Statement * stmt)
{
  memset (stmt, 0, sizeof (*stmt));
  stmt->type = STMT_GLOBAL;

  init_outstream (&(stmt->spec.glb.type_desc), 0);
  init_outstream (&(stmt->spec.glb.const_area), 0);
  init_array (&stmt->spec.glb.proc_decls, sizeof (struct Statement));
  init_array (&stmt->decls, sizeof (struct DeclaredVar));
  return TRUE;
}

void
clear_glbl_stmt (struct Statement *glbl)
{
  D_UINT nprocs = get_array_count (&(glbl->spec.glb.proc_decls));

  assert (glbl->parent == NULL);
  assert (glbl->type == STMT_GLOBAL);

  while (nprocs-- > 0)
    {
      struct Statement *proc =
	get_item (&(glbl->spec.glb.proc_decls), nprocs);
      clear_proc_stmt (proc);
    }
  destroy_outstream (&glbl->spec.glb.type_desc);
  destroy_outstream (&glbl->spec.glb.const_area);
  destroy_array (&(glbl->spec.glb.proc_decls));
  destroy_array (&(glbl->decls));

  glbl->type = STMT_ERR;
  return;
}

D_BOOL
init_proc_stmt (struct Statement * parent, struct Statement * stmt)
{
  static struct DeclaredVar ret_type = { 0, };
  assert (parent != NULL);

  memset (stmt, 0, sizeof (*stmt));
  stmt->type = STMT_PROC;
  stmt->parent = parent;
  stmt->spec.proc.sync_keeper = 0;
  init_array (&stmt->spec.proc.param_list, sizeof (struct DeclaredVar));
  init_array (&stmt->decls, sizeof (struct DeclaredVar));
  init_array (stmt_query_branch_stack (stmt), sizeof (struct BranchData));
  init_array (stmt_query_loop_stack (stmt), sizeof (struct LoopData));

  init_outstream (stmt_query_instrs (stmt), 0);
  /* reserve space for return type */
  if (add_item (&(stmt->spec.proc.param_list), &ret_type) == NULL)
    return FALSE;

  stmt->locals_used++;
  return TRUE;
}

void
clear_proc_stmt (struct Statement *proc)
{
  assert (proc->parent != NULL);
  assert (proc->type == STMT_PROC);
  assert ((proc->spec.proc.name != NULL) && (proc->spec.proc.nlength != 0));

  destroy_array (&(proc->spec.proc.param_list));
  destroy_array (&(proc->decls));
  destroy_array (stmt_query_branch_stack (proc));
  destroy_array (stmt_query_loop_stack (proc));

  destroy_outstream (stmt_query_instrs (proc));

  proc->type = STMT_ERR;
  return;
}

struct DeclaredVar *
stmt_find_declaration (const struct Statement *stmt,
		       const char *label,
		       const D_UINT label_len, const D_BOOL recursive)
{
  struct DeclaredVar *result = NULL;
  D_UINT count = 0;
  D_UINT stored_vals = get_array_count (&stmt->decls);

  while (count < stored_vals)
    {
      result = get_item (&stmt->decls, count);
      assert (result != NULL);
      if (((result->type & T_FIELD_MASK) == 0) &&
	  (label_len == result->l_label) &&
	  (strncmp (label, result->label, label_len) == 0))
	{
	  /*  here it is */
	  return result;
	}
      count++;
    }

  if (stmt->parent != NULL)
    {
      /* let's check if is a parameter */
      stored_vals = get_array_count (&stmt->spec.proc.param_list);
      /* index 0 is reserved to hold the return type */
      count = 1;
      while (count < stored_vals)
	{
	  result = get_item (&stmt->spec.proc.param_list, count);
	  assert (result != NULL);
	  if (((result->type & T_FIELD_MASK) == 0) &&
	      (label_len == result->l_label) &&
	      (strncmp (label, result->label, label_len) == 0))
	    {
	      /*  here it is */
	      return result;
	    }
	  count++;
	}
      /* maybe is global */
      if (recursive)
	{
	  return stmt_find_declaration (stmt->parent, label,
					label_len, recursive);
	}
    }
  return NULL;			/* nothing found */
}

struct DeclaredVar *
stmt_add_declaration (struct Statement *stmt,
		      struct DeclaredVar *var, D_BOOL parameter)
{

  struct OutStream *os;

  if ((var->type & T_FIELD_MASK) == 0)
    var->var_id = stmt->locals_used++;
  else
    var->var_id = ~0;

  if (stmt->type == STMT_GLOBAL)
    {
      var->var_id |= GLOBAL_DECLARED;
      os = &(stmt->spec.glb.type_desc);
    }
  else
    {
      assert (stmt->parent->type == STMT_GLOBAL);
      os = &(stmt->parent->spec.glb.type_desc);
    }

  var->type_spec_pos = type_spec_fill (os, var);

  if (parameter)
    {
      assert (stmt->parent != NULL);
      var = add_item (&stmt->spec.proc.param_list, var);
    }
  else
    {
      var = add_item (&stmt->decls, var);
    }

  return (struct DeclaredVar *) var;
}

const struct DeclaredVar *
stmt_get_param (const struct Statement *const stmt, D_UINT arg_pos)
{
  assert (stmt->type == STMT_PROC);
  return (struct DeclaredVar *)
    get_item (&stmt->spec.proc.param_list, arg_pos);
}

D_UINT
stmt_get_param_count (const struct Statement *const stmt)
{
  assert (stmt->type == STMT_PROC);
  return get_array_count (&stmt->spec.proc.param_list) - 1;
}

D_UINT32
stmt_get_import_id (const struct Statement * const stmt)
{
  assert (stmt->type == STMT_PROC);
  return stmt->spec.proc.proc_id;

}

/*****************************Type specification section ***************/

D_BOOL
is_type_spec_valid (const struct TypeSpec * spec)
{
  D_BOOL result = TRUE;

  if (((spec->type == T_UNKNOWN) || (spec->type > T_UNDETERMINED)) &&
      ((spec->type & T_ARRAY_MASK) == 0) &&
      ((spec->type & T_CONTAINER_MASK) != T_ROW_MASK) &&
      ((spec->type & T_CONTAINER_MASK) != T_TABLE_MASK) &&
      ((spec->type & T_CONTAINER_MASK) != T_RECORD_MASK))
    {
      result = FALSE;
    }
  else if ((spec->data[spec->data_len - 2] != TYPE_SPEC_END_MARK) ||
	   (spec->data[spec->data_len - 1] != 0))
    {
      result = FALSE;
    }
  else if ((spec->type & T_ARRAY_MASK) != 0)
    {
      if ( (spec->data_len != 2) ||
           ((spec->type & ~T_ARRAY_MASK) == T_UNKNOWN) ||
           ((spec->type & ~T_ARRAY_MASK) > T_UNDETERMINED) )
	{
	  result = FALSE;
	}
    }
  else if ((spec->type & T_CONTAINER_MASK) == T_ROW_MASK)
    {
      if ((spec->data_len != 2) && (spec->data_len != 6))
	{
	  result = FALSE;
	}
    }
  else if (((spec->type & T_CONTAINER_MASK) == T_TABLE_MASK) ||
	   ((spec->type & T_CONTAINER_MASK) == T_RECORD_MASK))
    {
      D_UINT index = 0;

      while ((index < (D_UINT) (spec->data_len - 2)) && (result != FALSE))
	{
	  D_UINT16 type;
	  D_UINT id_len = strlen ((char *) &spec->data[index]);

	  /* don't check for zero here, because of strlen */
	  index += id_len + 1;
	  type = ((D_UINT16 *) & (spec->data[index]))[0];
	  /* clear an eventual array mask */
	  type &= ~T_ARRAY_MASK;
	  if ((type == T_UNKNOWN) || (type > T_UNDETERMINED))
	    {
	      result = FALSE;
	      break;
	    }
	  index += 2;
	}
    }

  return result;
}

static D_UINT32
find_type_spec (const D_UINT8 * buffer, D_UINT32 buffer_len,
		const struct TypeSpec *spec)
{
  const struct TypeSpec *it;
  D_UINT position = 0;

  while (position < buffer_len)
    {
      it = (struct TypeSpec *) (buffer + position);
      if (!is_type_spec_valid (it))
	{
	  /* internal error */
	  assert (0);
	  return TYPE_SPEC_ERROR;
	}
      if (type_spec_cmp (it, spec) != FALSE)
	{
	  return position;
	}
      position += it->data_len + 2 * sizeof (D_UINT16);
    }

  return TYPE_SPEC_INVALID_POS;
}

D_BOOL
type_spec_cmp (const struct TypeSpec * pSpec_1,
	       const struct TypeSpec * pSpec_2)
{
  return (pSpec_1->type == pSpec_2->type) &&
    (pSpec_1->data_len == pSpec_2->data_len) &&
    (memcmp (&pSpec_1->data[0], &pSpec_2->data[0], pSpec_1->data_len) == 0);
}

static D_UINT
type_spec_fill_field (struct OutStream *outs, const struct DeclaredVar *list)
{
  D_UINT result = 0;

  while (list && ((list->type & T_FIELD_MASK) != 0))
    {
      if ((data_outstream (outs, (D_UINT8 *) list->label, list->l_label) ==
	   NULL) || (uint8_outstream (outs, 0) == NULL))
	{
	  result = TYPE_SPEC_ERROR;
	  break;
	}
      result += list->l_label + 1;
      if (uint16_outstream (outs, list->type & ~T_FIELD_MASK) == NULL)
	{
	  result = TYPE_SPEC_ERROR;
	  break;
	}
      result += 2;
      list = list->extra;
    }

  return result;
}

static D_UINT
type_spec_fill_container (struct OutStream *outs,
			  const struct DeclaredVar *var)
{
  D_UINT result = 0;
  D_UINT spec_pos = get_size_outstream (outs);

  assert ((var->type & T_CONTAINER_MASK) != 0);
  switch (var->type & T_CONTAINER_MASK)
    {
    case T_RECORD_MASK:
    case T_TABLE_MASK:
      /* output the type and a dummy length to fill
       * after fields are output */
      if ((uint16_outstream (outs, var->type) != NULL) &&
	  (uint16_outstream (outs, 0) != NULL))
	{
	  result = type_spec_fill_field (outs, var->extra);
	}
      else
	{
	  result = TYPE_SPEC_ERROR;
	}

      if ((result != TYPE_SPEC_ERROR) &&
	  (uint8_outstream (outs, TYPE_SPEC_END_MARK) != NULL) &&
	  (uint8_outstream (outs, 0) != NULL))
	{
	  struct TypeSpec *ts = (struct TypeSpec *)
	    (get_buffer_outstream (outs) + spec_pos);
	  result += 2;
	  ts->data_len = result;
	}
      else
	{
	  result = TYPE_SPEC_ERROR;
	}
      break;
    case T_ROW_MASK:
      /* output the type and a dummy length to fill
       * after fields are output */
      if ((uint16_outstream (outs, var->type) != NULL) &&
	  (uint16_outstream (outs, 0) != NULL))
	{
	  if (var->extra)
	    {
	      D_UINT32 tmp = var->extra->var_id & ~EXTERN_DECLARED;
	      if (uint32_outstream (outs, tmp) == NULL)
		result = TYPE_SPEC_ERROR;
	      else
		result = sizeof (D_UINT32);
	    }
	  else
	    {
	      result = 0;
	    }

	}
      else
	{
	  result = TYPE_SPEC_ERROR;
	}
      if ((result != TYPE_SPEC_ERROR) &&
	  (uint8_outstream (outs, TYPE_SPEC_END_MARK) != NULL) &&
	  (uint8_outstream (outs, 0) != NULL))
	{
	  struct TypeSpec *ts = (struct TypeSpec *)
	    (get_buffer_outstream (outs) + spec_pos);
	  ts->data_len = result + 2;
	}
      else
	{
	  result = TYPE_SPEC_ERROR;
	}
      break;
    default:
      assert (0);
      result = 0;
    }
  return result;
}

static D_UINT
type_spec_fill_array (struct OutStream *outs, const struct DeclaredVar *var)
{
  D_UINT result = 0;
  struct TypeSpec ts;
  assert ((var->type & T_ARRAY_MASK) != 0);

  ts.type = var->type;
  ts.data_len = 2;
  ts.data[0] = TYPE_SPEC_END_MARK;
  ts.data[1] = 0;

  if ((uint16_outstream (outs, ts.type) != NULL) &&
      (uint16_outstream (outs, ts.data_len) != NULL) &&
      (data_outstream (outs, ts.data, sizeof ts.data) != NULL))
    {
      result = 2 * sizeof  (D_UINT16) + sizeof (ts.data);
    }
  else
    {
      result = TYPE_SPEC_ERROR;
    }

  return result;

}

static D_UINT
type_spec_fill_basic (struct OutStream *outs, const struct DeclaredVar *var)
{
  D_UINT result = 0;
  struct TypeSpec ts;
  assert (var->type != T_UNKNOWN);
  assert (var->type <= T_UINT64);

  ts.type = var->type;
  ts.data_len = 2;
  ts.data[0] = TYPE_SPEC_END_MARK;
  ts.data[1] = 0;

  if (data_outstream (outs, (D_UINT8 *) & ts, sizeof ts) != NULL)
    {
      result = sizeof ts;
    }
  else
    {
      result = TYPE_SPEC_ERROR;
    }

  return result;

}

D_UINT
type_spec_fill (struct OutStream * outs, const struct DeclaredVar * var)
{

  D_UINT result;
  struct OutStream temp;

  if ((var->type & T_FIELD_MASK) != 0)
    {
      /* filed types are not present in the local/global
       * variable table of type descriptors */
      return 0;
    }
  init_outstream (&temp, OUTSTREAM_INCREMENT_SIZE);

  if ((var->type & (T_CONTAINER_MASK | T_ARRAY_MASK)) == 0)
    {
      result = type_spec_fill_basic (&temp, var);
    }
  else if ((var->type & T_CONTAINER_MASK) != 0)
    {
      result = type_spec_fill_container (&temp, var);
    }
  else if ((var->type & T_ARRAY_MASK) != 0)
    {
      result = type_spec_fill_array (&temp, var);
    }
  else
    {
      assert (0);
      result = TYPE_SPEC_ERROR;
    }

  if ((result == TYPE_SPEC_ERROR) || (result == TYPE_SPEC_INVALID_POS))
    {
      return result;
    }
  else
    {
      result = find_type_spec (get_buffer_outstream (outs),
			       get_size_outstream (outs),
			       (struct TypeSpec *)
			       get_buffer_outstream (&temp));
    }

  if (result == TYPE_SPEC_INVALID_POS)
    {
      result = get_size_outstream (outs);
      if (data_outstream (outs, get_buffer_outstream (&temp),
			  get_size_outstream (&temp)) == NULL)
	{
	  return TYPE_SPEC_INVALID_POS;
	}
    }

  destroy_outstream (&temp);

  return result;
}

D_INT
add_text_const (struct Statement * stmt, const D_UINT8 * buffer, D_UINT size)
{
  struct OutStream *outs = (stmt->type == STMT_GLOBAL) ?
    &stmt->spec.glb.const_area : &stmt->parent->spec.glb.const_area;

  const D_UINT8 *outs_buff = get_buffer_outstream (outs);
  const D_UINT stream_size = get_size_outstream (outs);
  D_INT iterator;

  assert ((stmt->type == STMT_GLOBAL) || (stmt->type == STMT_PROC));

  for (iterator = 0; iterator + size <= stream_size; ++iterator)
    if ((memcmp (outs_buff + iterator, buffer, size) == 0) &&
	((buffer[size - 1] == 0) || (outs_buff[iterator + size] == 0)))
      return iterator;

  if (data_outstream (outs, buffer, size) == NULL)
    return -1;

  /* make sure you add a zero just in case */
  if ((buffer[size - 1] != 0) && (uint8_outstream (outs, 0) == NULL))
    return -1;
  return stream_size;
}
