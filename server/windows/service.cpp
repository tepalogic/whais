
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "whais.h"
#include "winreg.h"

#include "dbs/dbs_mgr.h"
#include "utils/logger.h"
#include "utils/license.h"

#include "../common/configuration.h"
#include "../common/loader.h"
#include "../common/server.h"

using namespace std;
using namespace whais;

static const char sProgramName[] = "Whais";
static const char sProgramDesc[] = "A database server Windows service.";

static bool sDbsInited         = false;
static bool sInterpreterInited = false;

static SERVICE_STATUS          sSvcStatus;
static SERVICE_STATUS_HANDLE   sSvcStatusHandle;

static vector<DBSDescriptors> databases;

char sServiceName[]    = "whais_srv";
char sServiceDesc[]    = "Whais Server";
char sServiceDescExt[] = "Whais Server (version 1.0  x86)";

const char sSubKeyName[]  = "SOFTWARE\\Whais";
const char sConfigValue[] = "ConfigFile";


static VOID
svc_report_info (const char* text, const bool isError)
{
  static DWORD eventIndex;
  const char* lpszStrings[2];

  HANDLE hEventSource = RegisterEventSource (NULL, sServiceName);
  if (NULL != hEventSource)
    {
      lpszStrings[0] = sServiceName;
      lpszStrings[1] = text;

      ReportEvent (hEventSource,
                   EVENTLOG_ERROR_TYPE,
                   0,
                   (isError ? 0xC0000000 : 0) | eventIndex++,
                   NULL,
                   2,
                   0,
                   lpszStrings,
                   NULL);

      DeregisterEventSource (hEventSource);
    }
}


static VOID
svc_report_api_fail (const char* func)
{
  const char* lpszStrings[2];
  char        buffer[80];

  HANDLE hEventSource = RegisterEventSource (NULL, sServiceName);
  if (NULL != hEventSource)
    {
      snprintf (buffer,
                sizeof buffer,
                "%s failed with %d",
                func,
                GetLastError());

      lpszStrings[0] = sServiceName;
      lpszStrings[1] = buffer;

      ReportEvent (hEventSource,
                   EVENTLOG_ERROR_TYPE,
                   0,
                   0xC0000000,
                   NULL,
                   2,
                   0,
                   lpszStrings,
                   NULL);

      DeregisterEventSource (hEventSource);
    }
}


static VOID
svc_report_status (DWORD dwCurrentState,
                   DWORD dwWin32ExitCode,
                   DWORD dwWaitHint)
{
  static DWORD dwCheckPoint = 1;

  sSvcStatus.dwCurrentState     = dwCurrentState;
  sSvcStatus.dwWin32ExitCode    = dwWin32ExitCode;
  sSvcStatus.dwWaitHint         = dwWaitHint;
  sSvcStatus.dwControlsAccepted = 0;

  if (dwCurrentState == SERVICE_RUNNING)
    {
      sSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP
                                      | SERVICE_ACCEPT_SHUTDOWN;
    }

  if ((dwCurrentState == SERVICE_RUNNING)
      || (dwCurrentState == SERVICE_STOPPED))
    {
      sSvcStatus.dwCheckPoint = 0;
    }
  else
    sSvcStatus.dwCheckPoint = dwCheckPoint++;

  // Report the status of the service to the SCM.
  SetServiceStatus(sSvcStatusHandle, &sSvcStatus);
}


static void
clean_frameworks (FileLogger& log)
{
  if (sInterpreterInited)
    {
      assert (sDbsInited);

      vector<DBSDescriptors>::reverse_iterator dbsIterator;

      for (dbsIterator = databases.rbegin ();
           dbsIterator != databases.rend ();
           ++dbsIterator)
        {
          if (dbsIterator->mSession != NULL)
            {
              ReleaseInstance (*(dbsIterator->mSession));
              dbsIterator->mSession = NULL;
            }

          ostringstream logEntry;
          logEntry << "Closing session '";
          logEntry << dbsIterator->mDbsName << "'.\n";
          log.Log (LT_INFO, logEntry.str ());
        }
    }

  if (sInterpreterInited)
    CleanInterpreter ();

  if (sInterpreterInited)
    {
      assert (sDbsInited);

      vector<DBSDescriptors>::reverse_iterator dbsIterator;

      for (dbsIterator = databases.rbegin ();
           dbsIterator != databases.rend ();
           ++dbsIterator)
        {
          if (dbsIterator->mDbs != NULL)
            DBSReleaseDatabase (*(dbsIterator->mDbs));

          if (dbsIterator->mLogger != NULL)
            {
              dbsIterator->mLogger->Log (LT_INFO, "Database context ended!");
              delete dbsIterator->mLogger;
            }

          ostringstream logEntry;
          logEntry << "Cleaned resources of database '";
          logEntry << dbsIterator->mDbsName << "'.\n";
          log.Log (LT_INFO, logEntry.str ());
        }
    }

  if (sDbsInited)
    DBSShoutdown ();
}



static bool
boot_server (const char* configFile, ostream& errOut)
{
  unique_ptr<ifstream>   config (NULL);
  unique_ptr<FileLogger> glbLog (NULL);

  config.reset (new ifstream (configFile,
                              ios_base::in | ios_base::binary));
  if (! config->good ())
    {
      errOut << "Could not open the configuration file ";
      errOut << '\'' << configFile << "'.\n";

      return false;
    }

  if (! whs_init ())
    {
      errOut << "Could not initialize the network socket framework.\n";
      return false;
    }

  try
  {
      uint_t sectionLine = 0;
      if (SeekAtConfigurationSection (*config, sectionLine) == false)
        {
          errOut << "Cannot find the CONFIG section in configuration file!\n";
          return false;
        }

      assert (sectionLine > 0);

      if (ParseConfigurationSection (*config, sectionLine, errOut) == false)
        return false;

      glbLog.reset (new FileLogger (GetAdminSettings ().mLogFile.c_str ()));

  }
  catch (ios_base::failure& e)
  {
      errOut << "Unexpected error during configuration read:\n";
      errOut << e.what () << endl;

      return false;
  }
  catch (...)
  {
      errOut << "Unknown error encountered during main "
                "configuration file reading!\n";
    return false;
  }

  svc_report_info ("Main configuration section loaded. Please look onto "
                     "configured files for the rest of information logs.",
                   false);
  try
  {
    vector<DBSDescriptors>::iterator dbsIterator;

    if ( ! PrepareConfigurationSection (*glbLog))
      return false;

    uint_t configLine = 0;
    config->clear ();
    config->seekg (0);
    while (FindNextContextSection (*config, configLine))
      {
        svc_report_status (SERVICE_START_PENDING, NO_ERROR, 1000);

        DBSDescriptors dbs (configLine);

        //Inherit some global settings from the server configuration area in
        //case are not set in the context configuration section.
        dbs.mWaitReqTmo   = GetAdminSettings ().mWaitReqTmo;
        dbs.mSyncInterval = GetAdminSettings ().mSyncInterval;

        if ( ! ParseContextSection (*glbLog, *config, configLine, dbs))
          return false;

        if ( ! PrepareContextSection (*glbLog, dbs))
          return false;

        ostringstream   logEntry;

        for (dbsIterator = databases.begin ();
             dbsIterator != databases.end ();
             ++dbsIterator)
          {
            if (dbsIterator->mDbsName == dbs.mDbsName)
              {
                logEntry << "Duplicate entry '" << dbs.mDbsName << "'. ";
                logEntry << "Ignoring the last configuration entry.\n";
                glbLog->Log (LT_ERROR, logEntry.str ());
                continue;
              }
          }

        if (dbs.mDbsName == GlobalContextDatabase ())
          databases.insert (databases.begin (), dbs);

        else
          databases.push_back (dbs);
      }

    if (databases.size () == 0)
      {
        glbLog->Log (LT_CRITICAL, "No session were configured.");
        return false;
      }
    else if (databases[0].mDbsName != GlobalContextDatabase ())
      {
        ostringstream noGlbMsg;

        noGlbMsg << "No entry for global section '";
        noGlbMsg << GlobalContextDatabase ();
        noGlbMsg << "' was found.";

        glbLog->Log (LT_CRITICAL, noGlbMsg.str ());
      }

    const ServerSettings& confSettings = GetAdminSettings ();

    DBSSettings dbsSettings;
    dbsSettings.mTableCacheBlkCount   = confSettings.mTableCacheBlockCount;
    dbsSettings.mTableCacheBlkSize    = confSettings.mTableCacheBlockSize;
    dbsSettings.mWorkDir              = confSettings.mWorkDirectory;
    dbsSettings.mTempDir              = confSettings.mTempDirectory;
    dbsSettings.mVLStoreCacheBlkCount = confSettings.mVLBlockCount;
    dbsSettings.mVLStoreCacheBlkSize  = confSettings.mVLBlockSize;
    dbsSettings.mVLValueCacheSize     = confSettings.mTempValuesCache;

    DBSInit (dbsSettings);
    sDbsInited = true;

    InitInterpreter (databases[0].mDbsDirectory.c_str ());
    sInterpreterInited = true;

    for (dbsIterator = databases.begin ();
         dbsIterator != databases.end ();
         ++dbsIterator)
      {
        svc_report_status (SERVICE_START_PENDING, NO_ERROR, 6000);
        LoadDatabase (*glbLog, *dbsIterator);
      }

    svc_report_status(SERVICE_RUNNING, NO_ERROR, 0);
    StartServer (*glbLog, databases);
    svc_report_status(SERVICE_STOP_PENDING, NO_ERROR, 60000);
  }
  catch (Exception& e)
  {
    svc_report_status(SERVICE_STOP_PENDING, NO_ERROR, 60000);

    ostringstream logEntry;

    logEntry << "Unable to deal with error condition.\n";
    if (e.Description ())
      logEntry << "Description:\n\t" << e.Description () << endl;

    if ( ! e.Message ().empty ())
      logEntry << "Message:\n\t" << e.Message () << endl;

    logEntry <<"Extra: " << e.Code () << " (";
    logEntry << e.File () << ':' << e.Line () << ").\n";

    glbLog->Log (LT_CRITICAL, logEntry.str ());

    clean_frameworks (*glbLog);
    whs_clean ();

    return false;
  }
  catch (std::bad_alloc&)
  {
    svc_report_status(SERVICE_STOP_PENDING, NO_ERROR, 60000);

    glbLog->Log (LT_CRITICAL, "OUT OF MEMORY!!!");

    clean_frameworks (*glbLog);
    whs_clean ();

    return false;
  }
  catch (std::exception& e)
  {
    svc_report_status(SERVICE_STOP_PENDING, NO_ERROR, 60000);

    ostringstream logEntry;

    logEntry << "General system failure: " << e.what () << endl;

    glbLog->Log (LT_CRITICAL, logEntry.str ());

    clean_frameworks (*glbLog);
    whs_clean ();

    return false;
  }
  catch (...)
  {
    assert (false);

    svc_report_status(SERVICE_STOP_PENDING, NO_ERROR, 60000);

    glbLog->Log (LT_CRITICAL, "Unknown exception!");

    return false;
  }

  clean_frameworks (*glbLog);
  whs_clean ();

  return true;
}


static void
print_usage ()
{
  displayBanner (cout, sProgramName, WVER_MAJ, WVER_MIN);
  cout << "Use:\n"
          "  /r file Register the service and use 'file' for configuration.\n"
          "  /u      Unregister the service.\n"
          "  /h      Print this help message.\n";
          "  /v      Print the version of this program.\n"
          "  /l      Print informations about the license.\n";
}


static bool
install_service (const char* configFile)
{
  HKEY hKey;
  char path[MAX_PATH];

  if (GetFullPathName (configFile, sizeof path, path, NULL) == 0)
    {
      cerr << "GetFullPathName failed (" << GetLastError () << ").\n";
      return false;
    }

  LONG errCode = RegCreateKeyEx (HKEY_LOCAL_MACHINE,
                                 sSubKeyName,
                                 0,
                                 NULL,
                                 REG_OPTION_NON_VOLATILE,
                                 KEY_ALL_ACCESS | KEY_WOW64_64KEY,
                                 NULL,
                                 &hKey,
                                 NULL);
  if (errCode != ERROR_SUCCESS)
    {
      cerr << "RegCreateKeyEx failed (" << errCode << ").\n";
      return false;
    }

  errCode = RegSetValueEx (hKey,
                           sConfigValue,
                           0,
                           REG_SZ,
                           _RC (const BYTE*, path),
                           strlen (path) + 1);
  if (errCode != ERROR_SUCCESS)
    {
      cerr << "RegSetValueEx failed (" << errCode << ").\n";

      RegCloseKey (hKey);
      return false;
    }

  RegCloseKey (hKey);

  if (!GetModuleFileName(NULL, path, MAX_PATH))
    {
      cerr << "GetModuleFileName failed (" << GetLastError () << ").\n";
      return false;
    }

  SC_HANDLE schSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
    {
      cerr << "OpenSCManager failed (" << GetLastError () << ").\n";
      return false;
    }

  SC_HANDLE schService = CreateService (schSCManager,
                                        sServiceName,
                                        sServiceDesc,
                                        SERVICE_ALL_ACCESS,
                                        SERVICE_WIN32_OWN_PROCESS,
                                        SERVICE_AUTO_START,
                                        SERVICE_ERROR_NORMAL,
                                        path,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL);
  if (schService == NULL)
    {
      cerr << "CreateService failed (" << GetLastError () << ").\n";
      CloseServiceHandle (schSCManager);

      return false;
    }

  SERVICE_DESCRIPTION srvCconfig = { sServiceDescExt, };
  ChangeServiceConfig2A (schService, SERVICE_CONFIG_DESCRIPTION, &srvCconfig);

  CloseServiceHandle (schService);
  CloseServiceHandle (schSCManager);

  return true;
}

static bool
remove_sevice ()
{
  SC_HANDLE schSCManager;
  SC_HANDLE schService;
  SERVICE_STATUS srvStatus = {0, };

  schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
    {
      cerr << "OpenSCManager failed (" << GetLastError () << ").\n";
      return false;
    }

  schService = OpenService(schSCManager, sServiceName, SERVICE_ALL_ACCESS);
  if (schService == NULL)
    {
      cerr << "OpenService failed (" << GetLastError () << ").\n";

      CloseServiceHandle(schSCManager);

      return false;
    }

  int timeOut = 30000;
  while (srvStatus.dwCurrentState != SERVICE_STOPPED)
    {
      if (timeOut <= 0)
        {
          cerr << "It takes takes too long to stop the service.\n"
                  "Either try again or remove it manually.\n";

          CloseServiceHandle(schService);
          CloseServiceHandle(schSCManager);

          return false;
        }
      else
        {
          timeOut -= 100;
          Sleep (100);
        }

      if ( ! QueryServiceStatus (schService, &srvStatus))
        {
          cerr << "QueryServiceStatus failed (" << GetLastError () << ").\n";

          CloseServiceHandle(schService);
          CloseServiceHandle(schSCManager);

          return false;
        }

      if ((srvStatus.dwCurrentState == SERVICE_RUNNING)
           && ! ControlService (schService, SERVICE_CONTROL_STOP, &srvStatus))
        {
          cerr << "ControlService failed (" << GetLastError () << ").\n";

          CloseServiceHandle(schService);
          CloseServiceHandle(schSCManager);

          return false;
        }
    }

  if ( ! DeleteService (schService))
    {
      cerr << "DeleteService failed (" << GetLastError () << ").\n";

      CloseServiceHandle(schService);
      CloseServiceHandle(schSCManager);

      return false;
    }

  CloseServiceHandle(schService);
  CloseServiceHandle(schSCManager);


  RegDeleteKeyEx (HKEY_LOCAL_MACHINE, sSubKeyName, KEY_WOW64_64KEY, 0);

  return true;
}


static VOID WINAPI
whais_scv_ctrl_handler (DWORD dwCtrl)
{
  switch (dwCtrl)
    {
  case SERVICE_CONTROL_SHUTDOWN:
  case SERVICE_CONTROL_STOP:

    svc_report_status (SERVICE_STOP_PENDING, NO_ERROR, 30000);
    StopServer ();

    return;

  case SERVICE_CONTROL_INTERROGATE:
    svc_report_status (sSvcStatus.dwCurrentState, NO_ERROR, 0);
    break;

  default:
    break;
    }
}

VOID WINAPI
whais_main (DWORD argc, LPTSTR *argv )
{
  char path[MAX_PATH] = {0, };

  if ( ! GetModuleFileName (NULL, path, MAX_PATH))
    {
      svc_report_api_fail ("GetModuleFileName");
      return ;
    }

  for (int i = strlen (path); i > 0; )
    {
      if (path[--i] == '\\')
        {
          path[i + 1] = 0;
          if ( ! SetCurrentDirectory (path))
            {
              svc_report_api_fail ("SetCurrentDirectory");
              return ;
            }
          break ;
        }
    }

  sSvcStatusHandle = RegisterServiceCtrlHandler (sServiceName,
                                                 whais_scv_ctrl_handler);
  if ( ! sSvcStatusHandle)
    {
      svc_report_api_fail("RegisterServiceCtrlHandler");
      return;
    }

  sSvcStatus.dwServiceType             = SERVICE_WIN32_OWN_PROCESS;
  sSvcStatus.dwServiceSpecificExitCode = 0;

  svc_report_status(SERVICE_START_PENDING, NO_ERROR, 60000);


  DWORD configFileLen = sizeof path;
  HKEY hKey;

  LONG errCode = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                               sSubKeyName,
                               0,
                               KEY_READ | KEY_WOW64_64KEY,
                               &hKey);
  if (errCode != ERROR_SUCCESS)
    {
      svc_report_api_fail ("RegOpenKeyEx");
      svc_report_status (SERVICE_STOPPED, NO_ERROR, 0);
      return ;
    }

  errCode = RegGetValue (hKey,
                         NULL,
                         sConfigValue,
                         RRF_RT_REG_SZ,
                         NULL,
                         path,
                         &configFileLen);
  if (errCode != ERROR_SUCCESS)
    {
      svc_report_api_fail ("RegGetValue");
      svc_report_status (SERVICE_STOPPED, NO_ERROR, 0);
      return ;
    }

  ostringstream errBuffer;
  if ( ! whf_file_exists (path))
    {
      errBuffer << "The configuration file '" << path << "' does not exists!";
      svc_report_info (errBuffer.str ().c_str (), true);

      return;
    }

  if ( ! boot_server (path, errBuffer))
    svc_report_info (errBuffer.str ().c_str (), true);

  svc_report_status (SERVICE_STOPPED, NO_ERROR, 0);
}

int
main (int argc, char** argv)
{
  bool registerSrv       = false;
  bool removeSrv         = false;
  const char* configFile = NULL;

  for (int i = 1; i < argc; ++i)
    {
      if (strcmp (argv[i], "/r") == 0)
        {
          registerSrv = true;
          if (++i >= argc)
            {
              cerr << "A configuration file has to be provided for service "
                      "registration.";
              return EINVAL;
            }
           configFile = argv[i];
        }
      else if (strcmp (argv[i], "/u") == 0)
        removeSrv = true;

      else if ((strcmp (argv[i], "/h") == 0)
               || (strcmp (argv[i], "/?") == 0))
        {
          print_usage ();
          return 0;
        }
      else
        {
          cerr << "I don't know what to do with '" << argv[i] << "'.\n";
          return EINVAL;
        }
    }

  if (registerSrv & removeSrv)
    {
      cerr << "Provided parameters are confusing!\n"
              "Use '" << argv[0] << " /h' for help.\n";
      return EINVAL;
    }
  else if (registerSrv & (configFile == NULL))
    {
      cerr << "A configuration file has to be provided to register the service "
              "register the service to use it.";

      return EINVAL;
    }
  else if (registerSrv && ! whf_file_exists (configFile))
    {
      cerr << "Cannot find the specified configuration file ('"
           << configFile << "').\n";

      return EINVAL;
    }

  if (removeSrv)
    {
      if ( ! remove_sevice ())
        {
          cerr << "Failed to remove service '" << sServiceName << "'.\n";
          return EINVAL;
        }
      cout << "Service '" << sServiceName << "' removed successfully.\n";
      return 0;
    }
  else if (registerSrv)
    {
      if ( ! install_service (configFile))
        {
          cerr << "Failed to register service '" << sServiceName << "'.\n";
          return EINVAL;
        }

      cout << "Service '" << sServiceName << "' registered successfully.\n";
      return 0;
    }

  SERVICE_TABLE_ENTRY dispatchTable[] =
  {
    { sServiceName, (LPSERVICE_MAIN_FUNCTION) whais_main },
    { NULL, NULL }
  };

  StartServiceCtrlDispatcher (dispatchTable);

  return 0;
}
