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

#include "whisperc/compiledunit.h"
#include "dbs_mgr.h"
#include "interpreter.h"
#include "test/test_fmw.h"

static const D_CHAR admin[] = "administrator";

static const D_UINT8 firstCode[] =
    "LET gb0 AS DATE;\n"
    "LET gb1 AS UNSIGNED INT32;\n"
    "LET tab1 AS TABLE WITH (t_field AS int8, vasile as TEXT);\n"
    "\n"
    "PROCEDURE proced_1 (p1v1 AS TEXT,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME\n"
    "DO\n"
    "\n"
    "LET p1 AS TEXT;\n"
    "LET p2 AS ARRAY OF UNSIGNED INT8;\n"
    "LET p3 AS UNSIGNED INT64;\n"
    "\n"
    "p3 = p2[p1v2[0]] / gb1;\n"
    "\n"
    "tab1[p3].vasile = p1v1;\n"
    "\n"
    "RETURN gb0;\n"
    "\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE proc_2 () RETURN HIRESTIME\n"
    "DO\n"
    "LET dummy AS ARRAY;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p1 as INT8;\n"
    "LET p2 as UNSIGNED INT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "dummy2[p3] = p1;\n"
    "p1 = p3;\n"
    "p2 = p1;\n"
    "\n"
    "RETURN proced_1 (\"Iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const D_UINT8 secondCode[] =
    "EXTERN LET gb1 AS UNSIGNED INT32;\n"
    "\n"
    "EXTERN PROCEDURE proced_1 (p1v1 AS TEXT,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n"
    "\n"
    "\n"
    "PROCEDURE proc_3 () RETURN HIRESTIME\n"
    "DO\n"
    "LET dummy  AS ARRAY;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "p3 = 1;\n"
    "dummy2[p3] = gb1;\n"
    "\n"
    "RETURN proced_1 (\"iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const D_UINT8 notDefGlbCode[] =
    "EXTERN LET someGlobal AS UNSIGNED INT32;";

static const D_UINT8 notDefProcCode[] =
    "EXTERN PROCEDURE some_proc_name (p1v1 AS TEXT,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n";

static const D_UINT8 doubleDefGlbCode[] =
    "EXTERN LET gb1 AS UNSIGNED INT32;\n"
    "LET gb0 AS UNSIGNED INT32;";

static const D_UINT8 doubleDefProcCode[] =
    "EXTERN PROCEDURE proced_1 (p1v1 AS TEXT,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n"
    "\n"
    "PROCEDURE proc_2 () RETURN HIRESTIME\n"
    "DO\n"
    "LET dummy AS ARRAY;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "RETURN proced_1 (\"Iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const D_UINT8 diffDefGlbCode[] =
    "EXTERN LET gb1 AS UNSIGNED INT64;\n";

static const D_UINT8 diffDefProcCode[] =
    "EXTERN PROCEDURE proced_1 (p1v1 AS DATE,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n";



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
load_units (I_Session& testSession)
{
  bool result = true;

  try
  {
      WBufferCompiledUnit firstUnit (firstCode, sizeof firstCode, my_postman, firstCode);
      testSession.LoadCompiledUnit (firstUnit);
      std::cout << "Loading first unit  OK" << std::endl;

  }
  catch (...)
  {
      std::cout << "Could not load unit 1";
      result = false;
  }

  if (result == false)
    return false;

  try
  {
      WBufferCompiledUnit secondUnit (secondCode, sizeof secondCode, my_postman, secondCode);
      testSession.LoadCompiledUnit (secondUnit);
      std::cout << "Loading second unit  OK" << std::endl;
  }
  catch (...)
  {
      std::cout << "Could not load unit 2";
      result = false;
  }

  return result;
}

bool
check_global_def_err (I_Session& testSession)
{
  bool result = false;

  try
  {
    WBufferCompiledUnit unit (notDefGlbCode,
                              sizeof notDefGlbCode,
                              my_postman,
                              notDefGlbCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with external global not defined!" << std::endl;
  }
  catch (InterpreterException& e)
  {
      if (e.GetExtra () == InterpreterException::EXTERNAL_FIRST)
        {
          std::cout << "Found global with out defintion OK" << std::endl;
          result = true;
        }
      else
        std::cout << "Error! Wrong exception throwed!" << std::endl;
  }
  catch (...)
  {
      std::cout << "Error! Unknown exception throwed!" << std::endl;
  }

  return result;
}

bool
check_global_doubledef_err (I_Session& testSession)
{
  bool result = false;

  try
  {
    WBufferCompiledUnit unit (doubleDefGlbCode,
                              sizeof doubleDefGlbCode,
                              my_postman,
                              doubleDefGlbCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with global defined twice !" << std::endl;
  }
  catch (InterpreterException& e)
  {
      if (e.GetExtra () == InterpreterException::DUPLICATE_DEFINITION)
        {
          std::cout << "Found global with duplicate defintion OK" << std::endl;
          result = true;
        }
      else
        std::cout << "Error! Wrong exception throwed!" << std::endl;
  }
  catch (...)
  {
      std::cout << "Error! Unknown exception throwed!" << std::endl;
  }

  return result;
}

bool
check_global_diffdef_err (I_Session& testSession)
{
  bool result = false;

  try
  {
    WBufferCompiledUnit unit (diffDefGlbCode,
                              sizeof diffDefGlbCode,
                              my_postman,
                              diffDefGlbCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with two different type globals defined!" << std::endl;
  }
  catch (InterpreterException& e)
  {
      if (e.GetExtra () == InterpreterException::EXTERNAL_MISMATCH)
        {
          std::cout << "Found global with duplicate defintion OK" << std::endl;
          result = true;
        }
      else
        std::cout << "Error! Wrong exception throwed!" << std::endl;
  }
  catch (...)
  {
      std::cout << "Error! Unknown exception throwed!" << std::endl;
  }

  return result;
}


bool
check_proc_def_err (I_Session& testSession)
{
  bool result = false;

  try
  {
    WBufferCompiledUnit unit (notDefProcCode,
                              sizeof notDefProcCode,
                              my_postman,
                              notDefProcCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with external procedure not defined!" << std::endl;
  }
  catch (InterpreterException& e)
  {
      if (e.GetExtra () == InterpreterException::EXTERNAL_FIRST)
        {
          std::cout << "Found procedure with out defintion OK" << std::endl;
          result = true;
        }
      else
        std::cout << "Error! Wrong exception throwed!" << std::endl;
  }
  catch (...)
  {
      std::cout << "Error! Unknown exception throwed!" << std::endl;
  }

  return result;
}

bool
check_proc_doubledef_err (I_Session& testSession)
{
  bool result = false;

  try
  {
    WBufferCompiledUnit unit (doubleDefProcCode,
                              sizeof doubleDefProcCode,
                              my_postman,
                              doubleDefProcCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with procedure defined twice !" << std::endl;
  }
  catch (InterpreterException& e)
  {
      if (e.GetExtra () == InterpreterException::DUPLICATE_DEFINITION)
        {
          std::cout << "Found procedure with duplicate defintion OK" << std::endl;
          result = true;
        }
      else
        std::cout << "Error! Wrong exception throwed!" << std::endl;
  }
  catch (...)
  {
      std::cout << "Error! Unknown exception throwed!" << std::endl;
  }

  return result;
}

bool
check_proc_diffdef_err (I_Session& testSession)
{
  bool result = false;

  try
  {
    WBufferCompiledUnit unit (diffDefProcCode,
                              sizeof diffDefProcCode,
                              my_postman,
                              diffDefProcCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with two procedures with different signatures !" << std::endl;
  }
  catch (InterpreterException& e)
  {
      if (e.GetExtra () == InterpreterException::EXTERNAL_MISMATCH)
        {
          std::cout << "Found procedure with duplicate defintion OK" << std::endl;
          result = true;
        }
      else
        std::cout << "Error! Wrong exception throwed!" << std::endl;
  }
  catch (...)
  {
      std::cout << "Error! Unknown exception throwed!" << std::endl;
  }

  return result;
}


int
main ()
{
  // VC++ allocates memory when the C++ runtime is initialized
  // We need not to test against it!
  D_UINT prealloc_mem = test_get_mem_used ();
  bool success = true;

  {
    std::string dir = ".";
    dir += whc_get_directory_delimiter ();

    DBSInit (dir.c_str (), dir.c_str ());
  }

  DBSCreateDatabase (admin, NULL);
  InitInterpreter ();

  {
    I_Session& commonSession = GetInstance (NULL);
    success = true;

    success = success && load_units (commonSession);
    success = success && check_global_def_err (commonSession);
    success = success && check_global_doubledef_err(commonSession);
    success = success && check_global_diffdef_err (commonSession);
    success = success && check_proc_def_err (commonSession);
    success = success && check_proc_doubledef_err (commonSession);
    success = success && check_proc_diffdef_err (commonSession);

    ReleaseInterpreterInstance (commonSession);
  }

  CleanInterpreter ();
  DBSRemoveDatabase (admin);
  DBSShoutdown ();
  D_UINT mem_usage = test_get_mem_used () - prealloc_mem;

  if (mem_usage)
    {
      success = false;
      test_print_unfree_mem();
    }

  std::cout << "Memory peak (no prealloc): " <<
            test_get_mem_peak () - prealloc_mem << " bytes." << std::endl;
  std::cout << "Preallocated mem: " << prealloc_mem << " bytes." << std::endl;
  std::cout << "Current memory usage: " << mem_usage << " bytes." << std::endl;
  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return -1;
    }
  else
    std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}
