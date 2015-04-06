/*
 * test_client_common.cpp
 *
 *  Created on: Jan 9, 2013
 *      Author: ipopa
 */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cassert>

#include "utils/wtypes.h"
#include "test_client_common.h"


using namespace std;

extern const char* DefaultDatabaseName ();
extern const uint_t  DefaultUserId ();
extern const char* DefaultUserPassword ();

static const char DEFAULT_HOST_SEREVR[]   = "localhost";
static const char DEFAULT_PORT_SERVER[]   = "1761";

static const int ROOT_ID                  = 0;

static const char ARG_ROOT[]              = "--root";
static const char ARG_HOST_NAME[]         = "-h";
static const char ARG_PORT[]              = "-p";
static const char ARG_DATABASE[]          = "-d";
static const char ARG_PASSWORD[]          = "-k";
static const char ARG_FRAMESIZE[]         = "--fs";

static void
print_usage (const char* cmd)
{
  cout << cmd << ' ';
  cout << ARG_HOST_NAME << " hostname ";
  cout << ARG_PORT << " port ";
  cout << ARG_DATABASE << " databasename ";
  cout << ARG_ROOT << endl;
  cout << ARG_PASSWORD << " password ";
}

bool
tc_settup_connection (int              argc,
                      const char**     argv,
                      WH_CONNECTION*   pHnd)
{
  const char*   host          = DEFAULT_HOST_SEREVR;
  const char*   port          = DEFAULT_PORT_SERVER;
  const char*   database      = DefaultDatabaseName ();
  uint_t        userid        = DefaultUserId ();
  const char*   password      = DefaultUserPassword ();
  uint_t        frameSize     = DEFAULT_FRAME_SIZE;

  uint_t status = WCS_OK;

  for (int argi = 1; argi < argc; ++argi)
    {
      if (strcmp (argv[argi], ARG_ROOT) == 0)
        userid = ROOT_ID;
      else if (strcmp (argv[argi], ARG_HOST_NAME) == 0)
        host = argv[++argi];
      else if (strcmp (argv[argi], ARG_PORT) == 0)
        port = argv[++argi];
      else if (strcmp (argv[argi], ARG_DATABASE) == 0)
        database = argv[++argi];
      else if (strcmp (argv[argi], ARG_PASSWORD) == 0)
        password = argv[++argi];
      else if (strcmp (argv[argi], ARG_FRAMESIZE) == 0)
        frameSize = atoi (argv[++argi]);
      else
        {
          cout << "Dont't know what to do with argument '";
          cout << argv[argi] << "'\n";
          print_usage (argv[0]);

          return false;
        }
    }

  if (host == NULL)
    {
      cout << "No host name supplied!\n";
      print_usage (argv[0]);

      return false;
    }

  cout << "Host: " << host << endl;
  cout << "Port: " << port << endl;
  cout << "Database: " << database << endl;
  cout << "User: " << userid << endl;
  cout << "Password: " << password << endl;
  cout << "Frame size: " << frameSize << endl;
  cout << "Connecting ... ";

  status = WConnect (host, port, database, password, userid, frameSize, pHnd);
  if (status != WCS_OK)
    {
      cout << "FAIL (0x" << hex << status << dec << ")\n";
      return false;
    }

  cout << "OK\n";
  return true;
}


static string
decode_basic_type (const uint16_t type)
{
  switch (GET_BASIC_TYPE (type))
  {
  case T_BOOL:
    return "BOOL";

  case T_CHAR:
    return "CHAR";

  case T_DATE:
    return "DATE";

  case T_DATETIME:
    return "DATETIME";

  case T_HIRESTIME:
    return "HIRESTIME";

  case T_UINT8:
    return "UINT8";

  case T_UINT16:
    return "UINT16";

  case T_UINT32:
    return "UINT32";

  case T_UINT64:
    return "UINT64";

  case T_INT8:
    return "INT8";

  case T_INT16:
    return "INT16";

  case T_INT32:
    return "INT32";

  case T_INT64:
    return "INT64";

  case T_REAL:
    return "REAL";

  case T_RICHREAL:
    return "RICHREAL";

  case T_TEXT:
    return "TEXT";

  case T_UNDETERMINED:
    return "UNDEFINED";

  default:
    assert (false);
  }

  return NULL;
}


static string
decode_array_typeinfo (unsigned int type)
{
  string result;
  bool   arrayDesc = false;

  if (IS_ARRAY (type))
    {
      result    += "ARRAY";
      arrayDesc  = true;
    }

  if (arrayDesc)
    {
      if (GET_BASIC_TYPE (type) == WHC_TYPE_NOTSET)
        return result;

      result += " OF ";
      result += decode_basic_type (GET_BASIC_TYPE (type));

      return result;
    }

  return decode_basic_type (GET_BASIC_TYPE (type));
}

static string
decode_field_typeinfo (unsigned int type)
{
  string result;

  if (GET_FIELD_TYPE (type) == T_UNDETERMINED)
    return "FIELD";

  return "FIELD OF " + decode_array_typeinfo (GET_FIELD_TYPE (type));
}


string
decode_typeinfo (unsigned int type)
{
  string result;

  if (IS_TABLE (type))
    return "TABLE";

  else if (IS_FIELD (type))
    return decode_field_typeinfo (type);

  else if (IS_ARRAY (type))
    return decode_array_typeinfo (type);

  return decode_basic_type (type);
}


#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
