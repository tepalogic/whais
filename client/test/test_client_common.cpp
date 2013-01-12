/*
 * test_client_common.cpp
 *
 *  Created on: Jan 9, 2013
 *      Author: ipopa
 */

#include <iostream>
#include <cstring>

#include "test_client_common.h"

using namespace std;

extern const D_CHAR* DefaultDatabaseName ();
extern const D_UINT  DefaultUserId ();
extern const D_CHAR* DefaultUserPassword ();

static const char DEFAULT_HOST_SEREVR[]   = "localhost";
static const char DEFAULT_PORT_SERVER[]   = "1761";

static const D_INT ROOT_ID                = 0;

static const char ARG_ROOT[]              = "--root";
static const char ARG_HOST_NAME[]         = "-h";
static const char ARG_PORT[]              = "-p";
static const char ARG_DATABASE[]          = "-d";
static const char ARG_PASSWORD[]          = "-k";

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
                      W_CONNECTOR_HND* pHnd)
{
  const char*   hostname      = DEFAULT_HOST_SEREVR;
  const char*   port          = DEFAULT_PORT_SERVER;
  const char*   database      = DefaultDatabaseName ();
  D_UINT        userid        = DefaultUserId ();
  const char*   password      = DefaultUserPassword ();

  D_UINT status = WCS_OK;

  for (D_INT argi = 1; argi < argc; ++argi)
    {
      if (strcmp (argv[argi], ARG_ROOT) == 0)
        userid = ROOT_ID;
      else if (strcmp (argv[argi], ARG_HOST_NAME) == 0)
        hostname = argv[++argi];
      else if (strcmp (argv[argi], ARG_PORT) == 0)
        port = argv[++argi];
      else if (strcmp (argv[argi], ARG_DATABASE) == 0)
        database = argv[++argi];
      else if (strcmp (argv[argi], ARG_PASSWORD) == 0)
        password = argv[++argi];
      else
        {
          cout << "Dont't know what to do with argument '";
          cout << argv[argi] << "'\n";
          print_usage (argv[0]);

          return false;
        }
    }

  if (hostname == NULL)
    {
      cout << "No host name supplied!\n";
      print_usage (argv[0]);

      return false;
    }

  cout << "Hostname: " << hostname << endl;
  cout << "Port: " << port << endl;
  cout << "Database: " << database << endl;
  cout << "User: " << userid << endl;
  cout << "Password: " << password << endl;
  cout << "Connecting ... ";

  status = WConnect (hostname, port, database, password, userid, pHnd);
  if (status != WCS_OK)
    {
      cout << "FAIL (0x" << hex << status << dec << ")\n";
      return false;
    }

  cout << "OK\n";
  return true;
}


#ifdef ENABLE_MEMORY_TRACE
D_UINT32 WMemoryTracker::sm_InitCount = 0;
const D_CHAR* WMemoryTracker::sm_Module = "T";
#endif
