/*
 * test_session.cpp
 *
 *  Created on: Mar 9, 2012
 *      Author: ipopa
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#include "compiler/compiledunit.h"
#include "dbs/dbs_mgr.h"
#include "interpreter.h"
#include "custom/include/test/test_fmw.h"

static const D_UINT MAX_PROC_PARAM = 10;
static const D_UINT MAX_FIELDS     = 10;

struct FieldDesc
{
  const D_CHAR*  field_name;
  const D_UINT16 field_type;
  bool           field_visited;
};

struct ParameterDesc
{
  const D_UINT16      type;
  bool                desc_visited;
  D_UINT16            field_count;
  FieldDesc           fields[MAX_FIELDS];
};

struct ProcDesc
{
  const D_CHAR*  name;
  const D_UINT   param_count;
  bool           desc_visited;
  ParameterDesc  params[MAX_PROC_PARAM];
};

static const D_CHAR admin[]    = "administrator";
static const D_CHAR test_db1[] = "t_testdb_1";

static const D_UINT ADMIN_PROCS_COUNT = 3;
static const D_UINT USERS_PROCS_COUNT = 4;

static ProcDesc admin_procs[ADMIN_PROCS_COUNT] =
    {
        {"c1", 1, false, {
              {T_DATE, false, },
                         }
        },
        {"c2", 2, false, {
              {T_ARRAY_MASK | T_UINT32, false, },
              {T_FIELD_MASK | T_UNDETERMINED, false, },
                          }
        },
        {"c3", 4, false, {
              {T_DATETIME, false, },
              {T_INT32, false, },
              {T_TABLE_MASK, false, },
              {T_ARRAY_MASK | T_UNDETERMINED, false, },
                         }
        }
    };

static const D_UINT8 commonCode[] =
    "PROCEDURE c1 () RETURN DATE\n"
    "DO\n"
    "RETURN NULL;\n"
    "ENDPROC\n"
    "PROCEDURE c2 (field1 AS FIELD) RETURN ARRAY OF UNSIGNED INT32\n"
    "DO\n"
    "RETURN NULL;\n"
    "ENDPROC\n"
    "PROCEDURE c3 (p1 AS INT32, p2 AS TABLE, p3 AS ARRAY) RETURN DATETIME\n"
    "DO\n"
    "RETURN NULL;\n"
    "ENDPROC\n";

static ProcDesc user_procs[USERS_PROCS_COUNT] =
    {
        {"u1", 1, false, {
              {T_INT8, false, },
                          }
        },
        {"u2", 4, false, {
              {T_DATETIME, false, },
              {T_INT32, false, },
              {T_TABLE_MASK, false, 3, {
                        {"f1", T_HIRESTIME, false},
                        {"a2", T_BOOL, false},
                        {"b3", T_ARRAY_MASK | T_DATE, false},
                                        }
              },
              {T_ARRAY_MASK | T_INT64, false, },
                        }
        },
        {"u3", 2, false, {
              {T_ARRAY_MASK | T_DATETIME, false},
              {T_FIELD_MASK | T_TEXT, false},
                         }
        },
        {"u4", 1, false, {
              {T_INT8, false},
                         }
        }
    };

static const D_UINT8 userCode[] =
    "PROCEDURE u1 () RETURN INT8\n"
    "DO\n"
    "RETURN NULL;\n"
    "ENDPROC\n"
    "PROCEDURE u2 (p1 AS INT32, p2 AS TABLE OF (f1 AS HIRESTIME, a2 AS BOOL, b3 AS ARRAY OF DATE), p3 AS ARRAY OF INT64) RETURN DATETIME\n"
    "DO\n"
    "RETURN NULL;\n"
    "ENDPROC\n"
    "PROCEDURE u3 (field1 AS FIELD OF TEXT) RETURN ARRAY OF DATETIME\n"
    "DO\n"
    "RETURN NULL;\n"
    "ENDPROC\n"
    "PROCEDURE u4 () RETURN INT8\n"
    "DO\n"
    "RETURN NULL;\n"
    "ENDPROC\n";

static const D_CHAR *MSG_PREFIX[] = {
                                      "", "error ", "warning ", "error "
                                    };

static D_UINT
get_line_from_buffer (const D_CHAR * buffer, D_UINT buff_pos)
{
  D_UINT count = 0;
  D_INT result = 1;

  if (buff_pos == WHC_IGNORE_BUFFER_POS)
    return -1;

  while (count < buff_pos)
    {
      if (buffer[count] == '\n')
        ++result;
      else if (buffer[count] == 0)
        {
          assert (0);
        }
      ++count;
    }
  return result;
}

void
my_postman (WHC_MESSENGER_ARG data,
            D_UINT            buff_pos,
            D_UINT            msg_id,
            D_UINT            msgType,
            const D_CHAR*     pMsgFormat,
            va_list           args)
{
  const D_CHAR *buffer = (const D_CHAR *) data;
  D_INT buff_line = get_line_from_buffer (buffer, buff_pos);

  fprintf (stderr, MSG_PREFIX[msgType]);
  fprintf (stderr, "%d : line %d: ", msg_id, buff_line);
  vfprintf (stderr, pMsgFormat, args);
  fprintf (stderr, "\n");
}

bool
load_unit (I_Session&           session,
           const D_UINT8* const unitCode,
           const D_UINT         unitCodeSize)
{
  bool result = true;

  try
  {
    WBufferCompiledUnit unit (unitCode,
                              unitCodeSize,
                              my_postman,
                              unitCode);
    session.LoadCompiledUnit (unit);
    std::cout << "Unit loaded successfully!" << std::endl;
  }
  catch (...)
  {
      std::cout << "Error! Unable to load unit!" << std::endl;
      result = false;
  }

  return result;
}

FieldDesc*
find_field_desc (const D_CHAR* name,
                 FieldDesc*    fields,
                 const D_UINT  fields_count)
{
  for (D_UINT index = 0; index < fields_count; ++index)
    {
      if (strcmp (fields[index].field_name, name) == 0)
        {
          if (fields[index].field_visited)
            return NULL;
          else
            return &fields[index];
        }
    }
  return NULL;
}

ProcDesc*
find_proc_desc (const D_CHAR* name,
                ProcDesc*     procs,
                const D_UINT  procs_count)
{
  for (D_UINT index = 0; index < procs_count; ++index)
    {
      if (strcmp (procs[index].name, name) == 0)
        {
          if (procs[index].desc_visited)
            return NULL;
          else
            return &procs[index];
        }
    }
  return NULL;
}

bool
test_fields_are_ok (I_Session&          session,
                    const D_CHAR*       procName,
                    const D_UINT        paramId,
                    FieldDesc*          fields,
                    const D_UINT        fields_count)
{
  for (D_UINT index = 0;
       index < session.ProcedurePameterFieldsCount (procName, paramId);
       ++index)
    {
      FieldDesc* field = find_field_desc (
                        session.ProcedurePameterFieldName (procName,
                                                           paramId,
                                                           index),
                        fields,
                        fields_count
                                          );
      const D_UINT type = session.ProcedurePameterFieldType (procName,
                                                             paramId,
                                                             index);
      if (field == NULL)
        return FALSE;
      else if (type != field->field_type)
        return FALSE;

      field->field_visited = true;
    }

  for (D_UINT index = 0; index < fields_count; ++index)
    {
      if (! fields[index].field_visited)
        return false;
    }

  try
  {
      session.ProcedurePameterFieldName (procName, paramId, fields_count);
      return false;
  }
  catch (DBSException&)
  {
  }
  catch (InterException&)
  {
  }

  try
  {
      session.ProcedurePameterFieldType (procName, paramId, fields_count);
      return false;
  }
  catch (DBSException&)
  {
  }
  catch (InterException&)
  {
  }

  return true;
}

bool
test_func_paramter (I_Session&     session,
                    const D_CHAR*  procName,
                    const D_UINT   paramId,
                    ParameterDesc* param)
{
  const D_UINT type     = session.ProcedurePameterRawType (procName, paramId);
  const D_UINT fields_c = session.ProcedurePameterFieldsCount (procName,
                                                               paramId);

  if (param->desc_visited)
    return false;
  param->desc_visited = true;

  if ((type != param->type) || (fields_c != param->field_count))
    return false;
  else if (type == T_TABLE_MASK)
    {
      if (! test_fields_are_ok (session,
                                procName,
                                paramId,
                                param->fields,
                                param->field_count))
        {
          return false;
        }
    }
  else if (fields_c != 0)
    return false;

  return true;
}

bool
test_procedures (I_Session&     session,
                 ProcDesc       procedures[],
                 const D_UINT   procsCount)
{
  if (procsCount != session.ProceduresCount ())
    return false;

  for (D_UINT index = 0;
       index < session.ProceduresCount ();
       ++index)
    {
      ProcDesc* desc = find_proc_desc (session.ProcedureName (index),
                                       procedures,
                                       procsCount);
      if ((desc == NULL) || (desc->desc_visited))
        return false;
      else if ((session.ProcedureParametersCount (desc->name) != desc->param_count)
               || (session.ProcedureParametersCount (index) != desc->param_count)
               || (desc->param_count <= 0))
        {
          return false;
        }
      desc->desc_visited = true;

      for (D_UINT param = 0;
           param < session.ProcedureParametersCount (desc->name);
           ++param)
        {
          if (! test_func_paramter (session,
                                    desc->name,
                                    param,
                                    &desc->params [param]))
            {
              return false;
            }
        }

      for (D_UINT param = 0; param < desc->param_count; ++param)
        {
          if (! desc->params[param].desc_visited)
            return false;
        }
    }

  for (D_UINT index = 0; index < procsCount; ++index)
    {
      if (! procedures[index].desc_visited)
        return false;
    }

  try
  {
      session.ProcedureName (procsCount);
      return false;
  }
  catch (InterException&)
  {
  }

  try
  {
      session.ProcedureParametersCount (procsCount);
      return false;
  }
  catch (InterException&)
  {
  }

  try
  {
      session.ProcedureParametersCount ("some_weird_name");
      return false;
  }
  catch (InterException&)
  {
  }

  return true;
}

int
main ()
{
  bool success = true;
  {
    DBSInit (DBSSettings ());
  }

  DBSCreateDatabase (admin);
  DBSCreateDatabase (test_db1);
  InitInterpreter ();

  {
    I_Session& adminSession = GetInstance (NULL);
    I_Session& userSession  = GetInstance (test_db1);

    success = true;
    success = success && load_unit (adminSession,
                                    commonCode,
                                    sizeof commonCode);
    success = success && load_unit (userSession,
                                    userCode,
                                    sizeof userCode);

    std::cout << "Testing admin procedures ... ";
    success = success && test_procedures (adminSession,
                                          admin_procs,
                                          ADMIN_PROCS_COUNT);
    std::cout << (success ? "OK" : "FAIL") << std::endl;

    std::cout << "Testing user procedures ... ";
    success = success && test_procedures (userSession,
                                          user_procs,
                                          USERS_PROCS_COUNT);
    std::cout << (success ? "OK" : "FAIL") << std::endl;

    ReleaseInstance (adminSession);
    ReleaseInstance (userSession);
  }

  CleanInterpreter ();
  DBSRemoveDatabase (admin);
  DBSRemoveDatabase (test_db1);
  DBSShoutdown ();

  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

#ifdef ENABLE_MEMORY_TRACE
D_UINT32 WMemoryTracker::sm_InitCount = 0;
const D_CHAR* WMemoryTracker::sm_Module = "T";
#endif
