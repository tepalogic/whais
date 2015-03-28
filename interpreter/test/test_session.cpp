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

using namespace whais;

static const char admin[] = "administrator";

static const uint8_t firstCode[] =
    "LET gb0 AS DATE;\n"
    "LET gb1 AS UINT32;\n"
    "LET tab1 AS TABLE OF (t_field AS int8, vasile as TEXT);\n"
    "\n"
    "PROCEDURE proced_1 (p1v1 AS TEXT,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME\n"
    "DO\n"
    "\n"
    "LET p1 AS TEXT;\n"
    "LET p2 AS ARRAY OF UINT8;\n"
    "LET p3 AS UINT64;\n"
    "\n"
    "p3 = p2[p1v2[0]] / gb1;\n"
    "\n"
    "tab1[p3, vasile] = p1v1;\n"
    "\n"
    "RETURN gb0;\n"
    "\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE proc_2 () RETURN HIRESTIME\n"
    "DO\n"
    "LET dummy AS ARRAY OF HIRESTIME;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p1 as INT8;\n"
    "LET p2 as UINT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "dummy2[p3] = p1;\n"
    "p1 = p3;\n"
    "p2 = p1;\n"
    "\n"
    "RETURN proced_1 (\"Iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const uint8_t secondCode[] =
    "EXTERN LET gb1 AS UINT32;\n"
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
    "LET dummy  AS ARRAY OF UINT16;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "p3 = 1;\n"
    "dummy2[p3] = gb1;\n"
    "\n"
    "RETURN proced_1 (\"iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const uint8_t notDefGlbCode[] =
    "EXTERN LET someGlobal AS UINT32;"
    "PROCEDURE random_proc_name() RETURN INT32\n"
    "DO\n"
    "RETURN someGlobal;\n"
    "\n"
    "ENDPROC\n";

static const uint8_t notDefProcCode[] =
    "EXTERN PROCEDURE some_proc_name (p1v1 AS TEXT,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n"
    "\n"
    "PROCEDURE ref_proc()\n"
    "RETURN HIRESTIME\n"
    "DO\n"
    "RETURN some_proc_name (NULL, NULL, NULL, NULL);\n"
    "\n"
    "ENDPROC\n";



static const uint8_t doubleDefGlbCode[] =
    "EXTERN LET gb1 AS UINT32;\n"
    "LET gb0 AS UINT32;"
    "PROCEDURE random_proc_name_2 () RETURN INT32\n"
    "DO\n"
    "RETURN gb1;\n"
    "\n"
    "ENDPROC\n";

static const uint8_t doubleDefProcCode[] =
    "EXTERN PROCEDURE proced_1 (p1v1 AS TEXT,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n"
    "\n"
    "PROCEDURE proc_2 () RETURN HIRESTIME\n"
    "DO\n"
    "LET dummy AS ARRAY OF INT8;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "RETURN proced_1 (\"Iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const uint8_t diffDefGlbCode[] =
    "EXTERN LET gb1 AS UINT64;\n"
    "PROCEDURE random_proc_name_4 () RETURN INT64\n"
    "DO\n"
    "RETURN gb1;\n"
    "\n"
    "ENDPROC\n";

static const uint8_t diffDefProcCode[] =
    "EXTERN PROCEDURE proced_1 (p1v1 AS DATE,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n"
    "\n"
    "PROCEDURE ref_proc_2 ()\n"
    "RETURN HIRESTIME\n"
    "DO\n"
    "RETURN proced_1 (NULL, NULL, NULL, NULL);\n"
    "\n"
    "ENDPROC\n";




static const char *MSG_PREFIX[] = {
                                      "", "error ", "warning ", "error "
                                    };

static uint_t
get_line_from_buffer (const char * buffer, uint_t buff_pos)
{
  uint_t count = 0;
  int result = 1;

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
my_postman (WH_MESSENGER_CTXT data,
            uint_t            buff_pos,
            uint_t            msg_id,
            uint_t            msgType,
            const char*     pMsgFormat,
            va_list           args)
{
  const char *buffer = (const char *) data;
  int buff_line = get_line_from_buffer (buffer, buff_pos);

  fprintf (stderr, MSG_PREFIX[msgType]);
  fprintf (stderr, "%d : line %d: ", msg_id, buff_line);
  vfprintf (stderr, pMsgFormat, args);
  fprintf (stderr, "\n");
}

bool
load_units (ISession& testSession)
{
  bool result = true;

  try
  {
      CompiledBufferUnit firstUnit (firstCode, sizeof firstCode, my_postman, firstCode);
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
      CompiledBufferUnit secondUnit (secondCode, sizeof secondCode, my_postman, secondCode);
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
check_global_def_err (ISession& testSession)
{
  bool result = false;

  try
  {
    CompiledBufferUnit unit (notDefGlbCode,
                              sizeof notDefGlbCode,
                              my_postman,
                              notDefGlbCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with external global not defined!" << std::endl;
  }
  catch (InterException& e)
  {
      if (e.Code() == InterException::EXTERNAL_FIRST)
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
check_global_doubledef_err (ISession& testSession)
{
  bool result = false;

  try
  {
    CompiledBufferUnit unit (doubleDefGlbCode,
                              sizeof doubleDefGlbCode,
                              my_postman,
                              doubleDefGlbCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with global defined twice !" << std::endl;
  }
  catch (InterException& e)
  {
      if (e.Code() == InterException::DUPLICATE_DEFINITION)
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
check_global_diffdef_err (ISession& testSession)
{
  bool result = false;

  try
  {
    CompiledBufferUnit unit (diffDefGlbCode,
                              sizeof diffDefGlbCode,
                              my_postman,
                              diffDefGlbCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with two different type globals defined!" << std::endl;
  }
  catch (InterException& e)
  {
      if (e.Code() == InterException::EXTERNAL_MISMATCH)
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
check_proc_def_err (ISession& testSession)
{
  bool result = false;

  try
  {
    CompiledBufferUnit unit (notDefProcCode,
                              sizeof notDefProcCode,
                              my_postman,
                              notDefProcCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with external procedure not defined!" << std::endl;
  }
  catch (InterException& e)
  {
      if (e.Code() == InterException::EXTERNAL_FIRST)
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
check_proc_doubledef_err (ISession& testSession)
{
  bool result = false;

  try
  {
    CompiledBufferUnit unit (doubleDefProcCode,
                              sizeof doubleDefProcCode,
                              my_postman,
                              doubleDefProcCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with procedure defined twice !" << std::endl;
  }
  catch (InterException& e)
  {
      if (e.Code() == InterException::DUPLICATE_DEFINITION)
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
check_proc_diffdef_err (ISession& testSession)
{
  bool result = false;

  try
  {
    CompiledBufferUnit unit (diffDefProcCode,
                              sizeof diffDefProcCode,
                              my_postman,
                              diffDefProcCode);
    testSession.LoadCompiledUnit (unit);
    std::cout << "ERROR: Unit loaded with two procedures with different signatures !" << std::endl;
  }
  catch (InterException& e)
  {
      if (e.Code() == InterException::EXTERNAL_MISMATCH)
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
main()
{
  bool success = true;
  {
    DBSInit (DBSSettings());
  }

  DBSCreateDatabase (admin);
  InitInterpreter ();

  {
    ISession& commonSession = GetInstance (NULL);
    success = true;

    success = success && load_units (commonSession);
    success = success && check_global_def_err (commonSession);
    success = success && check_global_doubledef_err (commonSession);
    success = success && check_global_diffdef_err (commonSession);
    success = success && check_proc_def_err (commonSession);
    success = success && check_proc_doubledef_err (commonSession);
    success = success && check_proc_diffdef_err (commonSession);

    ReleaseInstance (commonSession);
  }

  CleanInterpreter ();
  DBSRemoveDatabase (admin);
  DBSShoutdown();

  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
