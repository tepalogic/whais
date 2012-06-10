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

static const D_CHAR admin[]    = "administrator";
static const D_CHAR test_db1[] = "t_testdb_1";
static const D_CHAR test_db2[] = "t_testdb_2";

static const D_UINT8 commonCode[] =
    "LET gb0 AS DATE;\n"
    "LET gb1 AS UNSIGNED INT32;\n"
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
    "LET p2 AS ARRAY OF UNSIGNED INT8;\n"
    "LET p3 AS UNSIGNED INT64;\n"
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
    "RETURN c_proc_1 (\"Iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const D_UINT8 db1Code_Fail_1 [] =
    "LET gb1 AS UNSIGNED INT32;\n"
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

static const D_UINT8 db1Code_Fail_2 [] =
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

static const D_UINT8 db1Code_Fail_3 [] =
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
    "RETURN c_proc_1 (\"Iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const D_UINT8 db1Code_Fail_4 [] =
    "\n"
    "EXTERN LET tab1 AS TABLE OF (t_field AS int8, vasile as DATE);\n"
    "\n"
    "PROCEDURE ref_proc ()\n"
    "RETURN DATE\n"
    "DO\n"
    "\n"
    "LET ind AS UNSIGNED INT64;\n"
    "\n"
    "RETURN tab1[ind, vasile];\n"
    "\n"
    "ENDPROC\n";

static const D_UINT8 db1Code_Fail_5 [] =
    "EXTERN LET gb1 AS UNSIGNED INT32;\n"
    "EXTERN LET tab1 AS TABLE OF (t_field AS int8, vasile as TEXT);\n"
    "PROCEDURE c_proc_1 (p1v1 AS TEXT,\n"
    "                    p1v2 AS ARRAY OF INT8, \n"
    "                    p1v3 AS ARRAY,\n"
    "                    p1v4 AS DATETIME)\n"
    "RETURN HIRESTIME\n"
    "DO\n"
    "\n"
    "LET p1 AS TEXT;\n"
    "LET p2 AS ARRAY OF UNSIGNED INT8;\n"
    "LET p3 AS UNSIGNED INT64;\n"
    "LET p4 AS HIRESTIME;\n"
    "\n"
    "p3 = p2[p1v2[0]] / gb1;\n"
    "\n"
    "tab1[p3, vasile] = p1v1;\n"
    "\n"
    "RETURN p4;\n"
    "\n"
    "ENDPROC\n";

static const D_UINT8 db1Code_Fail_6 [] =
    "PROCEDURE c_proc_1 ()\n"
    "RETURN BOOL\n"
    "DO\n"
    "\n"
    "RETURN FALSE;\n"
    "\n"
    "ENDPROC\n";

static const D_UINT8 db1_Code_1 [] =
    "LET private_gb1 AS UNSIGNED INT32;\n"
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
    "LET dummy  AS ARRAY;\n"
    "LET dummy2 AS ARRAY OF INT8;\n"
    "LET p3 as INT64;\n"
    "\n"
    "p3 = 1;\n"
    "dummy2[p3] = private_gb1;\n"
    "\n"
    "RETURN c_proc_1 (\"iulian\", dummy2, dummy, '1981/11/11');\n"
    "\n"
    "ENDPROC\n";

static const D_UINT8 db1_Code_2 [] =
    "EXTERN LET private_gb1 AS UNSIGNED INT32;\n"
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

static const D_UINT8 db2_Code_1 [] =
    "LET private_gb1 AS HIRESTIME;\n"
    "\n"
    "PROCEDURE private_proc () RETURN HIRESTIME\n"
    "DO\n"
    "\n"
    "RETURN private_gb1;\n"
    "\n"
    "ENDPROC\n";

static const D_UINT8 db2_Code_2 [] =
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
test_fault (I_Session& session,
            const D_UINT8* const unitCode,
            const D_UINT         unitCodeSize,
            const D_UINT         expectedCode)
{
  bool result = false;

  try
  {
    WBufferCompiledUnit unit (unitCode,
                              unitCodeSize,
                              my_postman,
                              unitCode);
    session.LoadCompiledUnit (unit);
    std::cout << "ERROR: Invalid unit loaded successfully!" << std::endl;
  }
  catch (InterException& e)
  {
      if (e.GetExtra () == expectedCode)
        {
          std::cout << "Got the expected except code OK" << std::endl;
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
load_common_session (I_Session& testSession)
{
  bool result = true;

  try
  {
      WBufferCompiledUnit commonUnit (commonCode,
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
  DBSCreateDatabase (test_db1, NULL);
  DBSCreateDatabase (test_db2, NULL);
  InitInterpreter ();

  {
    I_Session& commonSession = GetInstance (NULL);
    I_Session& db1Session    = GetInstance (test_db1);
    I_Session& db2Session    = GetInstance (test_db2);

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
