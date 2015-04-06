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

static const char admin[]    = "administrator";
static const char test_db1[] = "t_testdb_1";
static const char test_db2[] = "t_testdb_2";

static const uint8_t commonCode[] =
    "LET gb0 AS DATE;\n"
    "LET gb1 AS UINT32;\n"
    "LET tab1 AS TABLE OF (t_field AS int8, vasile as TEXT);\n"
    "\n"
    "PROCEDURE c_proc_1 (p1v1 AS TEXT,\n"
    "                    p1v2 AS ARRAY OF INT8, \n"
    "                    p1v3 AS ARRAY,\n"
    "                    p1v4 AS DATETIME)\n"
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
    "PROCEDURE c_proc_2 () RETURN HIRESTIME\n"
    "DO\n"
    "LET dummy AS ARRAY OF INT64;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p1 as INT8;\n"
    "LET p2 as UINT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "dummy2[p3] = p1;\n"
    "p1 = p3;\n"
    "p2 = p1;\n"
    "\n"
    "RETURN c_proc_1 (\"Iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const uint8_t db1Code_Fail_1 [] =
    "LET gb1 AS UINT32;\n"
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
    "LET dummy  AS ARRAY OF BOOL;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "p3 = 1;\n"
    "dummy2[p3] = gb1;\n"
    "\n"
    "RETURN proced_1 (\"iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const uint8_t db1Code_Fail_2 [] =
    "EXTERN LET gb1 AS  INT32;\n"
    "\n"
    "EXTERN PROCEDURE proced_1 (p1v1 AS TEXT,\n"
    "                           p1v2 AS ARRAY OF INT8, \n"
    "                           p1v3 AS ARRAY,\n"
    "                           p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n"
    "\n"
    "\n"
    "PROCEDURE proc_3 () RETURN HIRESTIME\n"
    "DO\n"
    "LET dummy  AS ARRAY OF DATE;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "p3 = 1;\n"
    "dummy2[p3] = gb1;\n"
    "\n"
    "RETURN proced_1 (\"iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const uint8_t db1Code_Fail_3 [] =
    "\n"
    "\n"
    "EXTERN PROCEDURE c_proc_1 (p1v1 AS TEXT,\n"
    "                           p1v2 AS ARRAY OF INT8, \n"
    "                           p1v3 AS ARRAY,\n"
    "                           p1v4 AS DATETIME)\n"
    "RETURN BOOL;\n"
    "\n"
    "PROCEDURE proc_2 () RETURN BOOL\n"
    "DO\n"
    "LET dummy AS ARRAY OF DATETIME;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p1 as INT8;\n"
    "LET p2 as UINT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "dummy2[p3] = p1;\n"
    "p1 = p3;\n"
    "p2 = p1;\n"
    "\n"
    "RETURN c_proc_1 (\"Iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const uint8_t db1Code_Fail_4 [] =
    "\n"
    "EXTERN LET tab1 AS TABLE OF (t_field AS int8, vasile as DATE);\n"
    "\n"
    "PROCEDURE ref_proc ()\n"
    "RETURN DATE\n"
    "DO\n"
    "\n"
    "LET ind AS UINT64;\n"
    "\n"
    "RETURN tab1[ind, vasile];\n"
    "\n"
    "ENDPROC\n";

static const uint8_t db1Code_Fail_5 [] =
    "EXTERN LET gb1 AS UINT32;\n"
    "EXTERN LET tab1 AS TABLE OF (t_field AS int8, vasile as TEXT);\n"
    "PROCEDURE c_proc_1 (p1v1 AS TEXT,\n"
    "                    p1v2 AS ARRAY OF INT8, \n"
    "                    p1v3 AS ARRAY,\n"
    "                    p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME\n"
    "DO\n"
    "\n"
    "LET p1 AS TEXT;\n"
    "LET p2 AS ARRAY OF UINT8;\n"
    "LET p3 AS UINT64;\n"
    "LET p4 AS HIRESTIME;\n"
    "\n"
    "p3 = p2[p1v2[0]] / gb1;\n"
    "\n"
    "tab1[p3, vasile] = p1v1;\n"
    "\n"
    "RETURN p4;\n"
    "\n"
    "ENDPROC\n";

static const uint8_t db1Code_Fail_6 [] =
    "PROCEDURE c_proc_1 ()\n"
    "RETURN BOOL\n"
    "DO\n"
    "\n"
    "RETURN FALSE;\n"
    "\n"
    "ENDPROC\n";

static const uint8_t db1_Code_1 [] =
    "LET private_gb1 AS UINT32;\n"
    "\n"
    "EXTERN PROCEDURE c_proc_1 (p1v1 AS TEXT,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n"
    "\n"
    "\n"
    "PROCEDURE private_proc () RETURN HIRESTIME\n"
    "DO\n"
    "LET dummy  AS ARRAY OF INT16;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "p3 = 1;\n"
    "dummy2[p3] = private_gb1;\n"
    "\n"
    "RETURN c_proc_1 (\"iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const uint8_t db1_Code_2 [] =
    "EXTERN LET private_gb1 AS UINT32;\n"
    "\n"
    "EXTERN PROCEDURE c_proc_1 (p1v1 AS TEXT,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n"
    "\n"
    "\n"
    "PROCEDURE private_proc_2 () RETURN HIRESTIME\n"
    "DO\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "p3 = 1;\n"
    "dummy2[p3] = private_gb1;\n"
    "\n"
    "RETURN c_proc_1 (\"iulian\", dummy2, dummy2, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const uint8_t db2_Code_1 [] =
    "LET private_gb1 AS HIRESTIME;\n"
    "\n"
    "PROCEDURE private_proc () RETURN HIRESTIME\n"
    "DO\n"
    "\n"
    "RETURN private_gb1;\n"
    "\n"
    "ENDPROC\n";

static const uint8_t db2_Code_2 [] =
    "EXTERN LET private_gb1 AS HIRESTIME;\n"
    "\n"
    "EXTERN PROCEDURE c_proc_1 (p1v1 AS TEXT,\n"
    "                   p1v2 AS ARRAY OF INT8, \n"
    "                   p1v3 AS ARRAY,\n"
    "                   p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME;\n"
    "\n"
    "\n"
    "PROCEDURE private_proc_2 () RETURN HIRESTIME\n"
    "DO\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "private_gb1 = c_proc_1 (\"iulian\", dummy2, dummy2, '1981/11/11');\n"
    "\n"
    "RETURN private_gb1;\n"
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
test_fault (ISession& session,
            const uint8_t* const unitCode,
            const uint_t         unitCodeSize,
            const uint_t         expectedCode)
{
  bool result = false;

  try
  {
    CompiledBufferUnit unit (unitCode,
                              unitCodeSize,
                              my_postman,
                              unitCode);
    session.LoadCompiledUnit (unit);
    std::cout << "ERROR: Invalid unit loaded successfully!" << std::endl;
  }
  catch (InterException& e)
  {
      if (e.Code () == expectedCode)
        {
          std::cout << "Got the expected exception code OK" << std::endl;
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
load_common_session (ISession& testSession)
{
  bool result = true;

  try
  {
      CompiledBufferUnit commonUnit (commonCode,
                                      sizeof commonCode,
                                      my_postman,
                                      commonCode);
      testSession.LoadCompiledUnit (commonUnit);
      std::cout << "Loading common unit  OK" << std::endl;
  }
  catch (...)
  {
      std::cout << "Could not load common unit";
      result = false;
  }

  return result;
}

bool
load_unit (ISession&           session,
           const uint8_t* const unitCode,
           const uint_t         unitCodeSize)
{
  bool result = true;

  try
  {
    CompiledBufferUnit unit (unitCode,
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

int
main ()
{
  bool success = true;
  {
    DBSInit (DBSSettings ());
  }

  DBSCreateDatabase (admin);
  DBSCreateDatabase (test_db1);
  DBSCreateDatabase (test_db2);
  InitInterpreter ();

  {
    ISession& commonSession = GetInstance (NULL);
    ISession& db1Session    = GetInstance (test_db1);
    ISession& db2Session    = GetInstance (test_db2);

    success = true;
    success = success && load_common_session (commonSession);
    success = success && test_fault (db1Session,
                                     db1Code_Fail_1,
                                     sizeof db1Code_Fail_1,
                                     InterException::DUPLICATE_DEFINITION );
    success = success && test_fault (db1Session,
                                     db1Code_Fail_2,
                                     sizeof db1Code_Fail_2,
                                     InterException::EXTERNAL_MISMATCH );
    success = success && test_fault (db1Session,
                                     db1Code_Fail_3,
                                     sizeof db1Code_Fail_3,
                                     InterException::EXTERNAL_MISMATCH );
    success = success && test_fault (db1Session,
                                     db1Code_Fail_4,
                                     sizeof db1Code_Fail_4,
                                     InterException::EXTERNAL_MISMATCH );
    success = success && test_fault (db1Session,
                                     db1Code_Fail_5,
                                     sizeof db1Code_Fail_5,
                                     InterException::DUPLICATE_DEFINITION );
    success = success && test_fault (db1Session,
                                     db1Code_Fail_6,
                                     sizeof db1Code_Fail_6,
                                     InterException::DUPLICATE_DEFINITION );
    success = success && load_unit (db1Session,
                                    db1_Code_1,
                                    sizeof db1_Code_1);
    success = success && load_unit (db1Session,
                                    db1_Code_2,
                                    sizeof db1_Code_2);
    success = success && load_unit (db2Session,
                                    db2_Code_1,
                                    sizeof db2_Code_1);
    success = success && load_unit (db2Session,
                                    db2_Code_2,
                                    sizeof db2_Code_2);
    ReleaseInstance (commonSession);
    ReleaseInstance (db1Session);
    ReleaseInstance (db2Session);
  }

  CleanInterpreter ();
  DBSRemoveDatabase (admin);
  DBSRemoveDatabase (test_db1);
  DBSRemoveDatabase (test_db2);
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
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
