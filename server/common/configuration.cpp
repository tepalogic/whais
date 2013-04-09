/******************************************************************************
WHISPER - An advanced database system
Copyright (C) 2008  Iulian Popa

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

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "utils/include/tokenizer.h"

#include "server_protocol.h"
#include "configuration.h"



using namespace std;

static const char COMMENT_CHAR          = '#';
static const char DEFAULT_LISTEN_PORT[] = "1761";
static const char CLEAR_LOG_STREAM[]    = "";

static const char CIPHER_PLAIN[]        = "plain";
static const char CIPHER_3K[]           = "3k";

static const uint_t MIN_TABLE_CACHE_BLOCK_SIZE  = 1024;
static const uint_t MIN_TABLE_CACHE_BLOCK_COUNT = 128;
static const uint_t MIN_VL_BLOCK_SIZE           = 1024;
static const uint_t MIN_VL_BLOCK_COUNT          = 128;
static const uint_t MIN_TEMP_CACHE              = 128;

static const uint_t DEFAULT_MAX_CONNS               = 64;
static const uint_t DEFAULT_TABLE_CACHE_BLOCK_SIZE  = 4098;
static const uint_t DEFAULT_TABLE_CACHE_BLOCK_COUNT = 1024;
static const uint_t DEFAULT_VL_BLOCK_SIZE           = 1024;
static const uint_t DEFAULT_VL_BLOCK_COUNT          = 4098;
static const uint_t DEFAULT_TEMP_CACHE              = 512;

static const string gEntPort ("listen");
static const string gEntMaxConnections ("max_connections");
static const string gEntMaxFrameSize("max_frame_size");
static const string gEntEncryption("cipher");
static const string gEntTableBlkSize ("table_block_cache_size");
static const string gEntTableBlkCount ("table_block_cache_count");
static const string gEntVlBlkSize ("vl_values_block_size");
static const string gEntVlBlkCount ("vl_values_block_count");
static const string gEntTempCache ("temporals_cache");
static const string gEntLogFile ("log_file");
static const string gEntDBSName ("name");
static const string gEntWorkDir ("directory");
static const string gEntTempDir ("temp_directory");
static const string gEntShowDbg ("show_debug");
static const string gEntObjectLib ("load_object");
static const string gEntNativeLib ("load_native");
static const string gEntRootPasswrd("root_password");
static const string gEntUserPasswrd("user_password");


static ServerSettings gMainSettings;

static bool
get_enclose_entry (ostream&      os,
                   const string& line,
                   const char  encChar,
                   string&       output)
{
  assert (line.at (0) == encChar);

  bool encEnded = false;

  for (uint_t i = 1; i < line.length (); ++i)
    {
      char ch = line.at (i);
      if (ch == encChar)
        {
          encEnded = true;
          break;
        }
      output.append (1, ch);
    }

  return encEnded;
}

const string&
GlobalContextDatabase ()
{
  static const string dbsName ("administrator");

  return dbsName;
}

const ServerSettings&
GetAdminSettings ()
{
  return gMainSettings;
}

bool
SeekAtConfigurationSection (ifstream& config, uint_t& oConfigLine)
{
  static const string identifier = "[CONFIG]";
  static const string delimiters = " \t";

  oConfigLine = 0;
  config.clear ();
  config.seekg (0);
  while (! config.eof ())
    {
      string line;
      getline (config, line);

      ++oConfigLine;

      size_t pos = 0;
      string token = NextToken (line, pos, delimiters);

      if ((token.length () > 0) && (token.at (0) == COMMENT_CHAR))
        continue;

      if (token == identifier)
        return true;
    }

  return false;
}

bool
FindNextContextSection (std::ifstream& config, uint_t& ioConfigLine)
{
  static const string identifier = "[SESSION]";
  static const string delimiters = " \t";

  while (! config.eof ())
    {
      assert (config.good());

      string line;
      getline (config, line);

      ++ioConfigLine;

      size_t pos = 0;
      string token = NextToken (line, pos, delimiters);

      if ((token.length () > 0) && (token.at (0) == COMMENT_CHAR))
        continue;

      if (token == identifier)
        return true;
    }

  return false;
}

bool
ParseConfigurationSection (ifstream& config, uint_t& ioConfigLine)
{
  static const string delimiters = " \t=";

  while ( !  config.eof ())
    {
      const streampos lastPos = config.tellg ();

      string line;
      getline (config, line);

      ++ioConfigLine;

      size_t pos   = 0;
      string token = NextToken (line, pos, delimiters);

      if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
        continue;

      if (token.at (0) == '[')
        {
          //Another configuration section starts from here.
          config.clear ();
          config.seekg (lastPos);
          break;
        }

      if (token == gEntPort)
        {
          ListenEntry entry;

          token = NextToken (line, pos, " \t=@#");
          entry.m_Interface = token;

          token = NextToken (line, pos, " \t@#");
          entry.m_Service = token;

          if (entry.m_Interface == "*")
            entry.m_Interface = "";
          if (entry.m_Service == "")
            entry.m_Service = DEFAULT_LISTEN_PORT;

          gMainSettings.m_Listens.push_back (entry);
        }
      else if (token == gEntMaxConnections)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_MaxConnections = atoi (token.c_str ());
        }
      else if (token == gEntMaxFrameSize)
       {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_MaxFrameSize = atoi (token.c_str ());
       }
      else if (token == gEntEncryption)
        {
          token = NextToken (line, pos, delimiters);
          std::transform(token.begin(), token.end(), token.begin(), ::tolower);

          if (token == CIPHER_PLAIN)
            gMainSettings.m_Cipher = FRAME_ENCTYPE_PLAIN;
          else if (token == CIPHER_3K)
            gMainSettings.m_Cipher = FRAME_ENCTYPE_3K;
          else
            {
              cerr << "The cipher '" << token << "' is not supported. ";
              cerr << "Allowed ciphers are '" << CIPHER_PLAIN << "' and '";
              cerr << CIPHER_3K <<"'.\n";

              return false;
            }
        }
      else if (token == gEntTableBlkCount)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TableCacheBlockCount = atoi (token.c_str ());

          if (gMainSettings.m_TableCacheBlockCount == 0)
            {
              cerr << "Configuration error at line " << ioConfigLine << ".\n";
              return false;
            }
        }
      else if (token == gEntTableBlkSize)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TableCacheBlockSize = atoi (token.c_str ());

          if (gMainSettings.m_TableCacheBlockSize == 0)
            {
              cerr << "Configuration error at line " << ioConfigLine << ".\n";
              return false;
            }
        }
      else if (token == gEntVlBlkCount)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_VLBlockCount = atoi (token.c_str ());

          if (gMainSettings.m_VLBlockCount == 0)
            {
              cerr << "Configuration error at line " << ioConfigLine << ".\n";
              return false;
            }
        }
      else if (token == gEntVlBlkSize)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_VLBlockSize = atoi (token.c_str ());

          if (gMainSettings.m_VLBlockSize == 0)
            {
              cerr << "Configuration error at line " << ioConfigLine << ".\n";
              return false;
            }
        }
      else if (token == gEntTempCache)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TempValuesCache = atoi (token.c_str ());

          if (gMainSettings.m_TempValuesCache == 0)
            {
              cerr << "Configuration error at line " << ioConfigLine << ".\n";
              return false;
            }
        }
      else if (token == gEntLogFile)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line " << ioConfigLine << ".\n";
              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
              const string entry = line.c_str () + pos - token.length ();

              if (get_enclose_entry (cerr,
                                     entry,
                                     token.at (0),
                                     gMainSettings.m_LogFile) == false)
                {
                  cerr << "Unmatched "<< token.at (0);
                  cerr << " in configuration file at line ";
                  cerr << ioConfigLine << ".\n";
                  return false;
                }

            }
          else
            gMainSettings.m_LogFile = token;
        }
      else if (token == gEntTempDir)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line " << ioConfigLine << ".\n";
              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
              const string entry = line.c_str () + pos - token.length ();

              if (get_enclose_entry (cerr,
                                     entry,
                                     token.at (0),
                                     gMainSettings.m_TempDirectory) == false)
                {
                  cerr << "Unmatched "<< token.at (0);
                  cerr << " in configuration file at line ";
                  cerr << ioConfigLine << ".\n";
                  return false;
                }
            }
          else
            gMainSettings.m_TempDirectory = token;

          string& dir = gMainSettings.m_WorkDirectory;

          if ((dir.size () != 0)
              && (dir[dir.size () - 1] != whc_get_directory_delimiter()[0]))
            {
              dir += whc_get_directory_delimiter ();
            }
        }
      else if (token == gEntWorkDir)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line " << ioConfigLine << ".\n";
              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
              const string entry = line.c_str () + pos - token.length ();

              if (get_enclose_entry (cerr,
                                     entry,
                                     token.at (0),
                                     gMainSettings.m_WorkDirectory) == false)
                {
                  cerr << "Unmatched "<< token.at (0);
                  cerr << " in configuration file at line ";
                  cerr << ioConfigLine << ".\n";
                  return false;
                }
            }
          else
            gMainSettings.m_WorkDirectory = token;

          string& dir = gMainSettings.m_TempDirectory;

          if ((dir.size () != 0)
              && (dir[dir.size () - 1] != whc_get_directory_delimiter()[0]))
            {
              dir += whc_get_directory_delimiter ();
            }
        }
      else if (token == gEntShowDbg)
        {
          token = NextToken (line, pos, delimiters);
          if (token == "false")
            gMainSettings.m_ShowDebugLog = false;
          else if (token == "true")
            gMainSettings.m_ShowDebugLog = true;
          else
            {
              cerr << "Unkown to assign '" << token << "\' to 'show_debug' ";
              cerr << "at line " << ioConfigLine << ". ";
              cerr << "Valid value are only 'true' or 'false'.\n";

              return false;
            }
        }
      else
        {
          cerr << "At line " << ioConfigLine << ": Don't know what to do ";
          cerr << "with '" << token <<"'.\n";
          return false;
        }
    }

  if (gMainSettings.m_WorkDirectory.length () == 0)
    {
      cerr << "The configuration main section does not ";
      cerr << "have a '"<< gEntWorkDir << "' entry.\n";

      return false;
    }

  if (gMainSettings.m_TempDirectory.length () == 0)
    gMainSettings.m_TempDirectory = gMainSettings.m_WorkDirectory;

  if (gMainSettings.m_LogFile.length () == 0)
    {
      cerr << "The configuration main section does not ";
      cerr << "have a '"<< gEntLogFile << "' entry.\n";

      return false;
    }

  return true;
}

bool
ParseContextSection (I_Logger&        log,
                     ifstream&        config,
                     uint_t&          ioConfigLine,
                     DBSDescriptors&  output)
{
  ostringstream logEntry;
  static const string delimiters = " \t=";

  while ( ! config.eof ())
    {
      const streamoff lastPos = config.tellg ();

      string line;
      getline (config, line);

      ++ioConfigLine;

      size_t pos   = 0;
      string token = NextToken (line, pos, delimiters);

      if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
        continue;

      if (token.at (0) == '[')
        {
          //Another configuration section starts from here.
          //Another configuration section starts from here.
          config.clear ();
          config.seekg (lastPos);
          break;
        }

       if (token == gEntDBSName)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              logEntry << "Configuration error at line ";
              logEntry << ioConfigLine << ".\n";
              log.Log (LOG_CRITICAL, logEntry.str ());
              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
              const string entry = line.c_str () + pos - token.length ();

              if (get_enclose_entry (logEntry,
                                     entry,
                                     token.at (0),
                                     output.m_DbsName) == false)
                {
                  logEntry << "Unmatched "<< token.at (0);
                  logEntry << " in configuration file at line ";
                  logEntry << ioConfigLine << ".\n";
                  log.Log (LOG_CRITICAL, logEntry.str ());
                  return false;
                }

            }
          else
            output.m_DbsName = token;
        }
       else if (token == gEntWorkDir)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line " << ioConfigLine << ".\n";
              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
             const string entry = line.c_str () + pos - token.length ();

             if (get_enclose_entry (cerr,
                                    entry,
                                    token.at (0),
                                    output.m_DbsDirectory) == false)
               {
                 cerr << "Unmatched "<< token.at (0);
                 cerr << " in configuration file at line ";
                 cerr << ioConfigLine << ".\n";
                 return false;
               }
            }
          else
            output.m_DbsDirectory = token;

          string& dir = output.m_DbsDirectory;

          if ((dir.size () != 0)
              && (dir[dir.size () - 1] != whc_get_directory_delimiter()[0]))
            {
              dir += whc_get_directory_delimiter ();
            }
        }
       else if (token == gEntLogFile)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line " << ioConfigLine << ".\n";
              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
              const string entry = line.c_str () + pos - token.length ();

              if (get_enclose_entry (cerr,
                                     entry,
                                     token.at (0),
                                     output.m_DbsLogFile) == false)
                {
                  cerr << "Unmatched "<< token.at (0);
                  cerr << " in configuration file at line ";
                  cerr << ioConfigLine << ".\n";
                  return false;
                }

            }
          else
            output.m_DbsLogFile = token;
        }
       else if (token==gEntObjectLib)
        {
           token = NextToken (line, pos, delimiters);

           if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
             {
               logEntry << "Configuration error at line ";
               logEntry << ioConfigLine << ".\n";
               log.Log (LOG_CRITICAL, logEntry.str ());
               return false;
             }

           string libEntry;
           if ((token.at (0) == '\'') || (token.at (0) == '"'))
             {
               const string entry = line.c_str () + pos - token.length ();

               if (get_enclose_entry (logEntry,
                                      entry,
                                      token.at (0),
                                      libEntry) == false)
                 {
                   logEntry << "Unmatched "<< token.at (0);
                   logEntry << " in configuration file at line ";
                   logEntry << ioConfigLine << ".\n";
                   log.Log (LOG_CRITICAL, logEntry.str ());
                   return false;
                 }

             }
           else
             libEntry = token;

           if ( ! whc_is_path_absolute (libEntry.c_str ()))
             {
               libEntry = gMainSettings.m_WorkDirectory + libEntry;
             }
           output.m_ObjectLibs.push_back (libEntry);
        }
       else if (token==gEntNativeLib)
        {
           token = NextToken (line, pos, delimiters);

           if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
             {
               logEntry << "Configuration error at line ";
               logEntry << ioConfigLine << ".\n";
               log.Log (LOG_CRITICAL, logEntry.str ());
               return false;
             }

           string libEntry;
           if ((token.at (0) == '\'') || (token.at (0) == '"'))
             {
               const string entry = line.c_str () + pos - token.length ();

               if (get_enclose_entry (logEntry,
                                      entry,
                                      token.at (0),
                                      libEntry) == false)
                 {
                   logEntry << "Unmatched "<< token.at (0);
                   logEntry << " in configuration file at line ";
                   logEntry << ioConfigLine << ".\n";
                   log.Log (LOG_CRITICAL, logEntry.str ());
                   return false;
                 }

             }
           else
             libEntry = token;

           if ( ! whc_is_path_absolute (libEntry.c_str ()))
             {
               libEntry = gMainSettings.m_WorkDirectory + libEntry;
             }
           output.m_NativeLibs.push_back (libEntry);
        }
       else if (token == gEntRootPasswrd)
        {
           if (output.m_RootPass.size () > 0)
             {
               logEntry << "Configuration error at line ";
               logEntry << ioConfigLine << ". ";
               logEntry << "The root password for this database was ";
               logEntry << "already set.\n";
               log.Log (LOG_CRITICAL, logEntry.str ());
               return false;
             }
           token = NextToken (line, pos, delimiters);

           if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
             {
               logEntry << "Configuration error at line ";
               logEntry << ioConfigLine << ".\n";
               log.Log (LOG_CRITICAL, logEntry.str ());
               return false;
             }

           if ((token.at (0) == '\'') || (token.at (0) == '"'))
             {
               const string entry = line.c_str () + pos - token.length ();

               if (get_enclose_entry (logEntry,
                                      entry,
                                      token.at (0),
                                      output.m_RootPass) == false)
                 {
                   logEntry << "Unmatched "<< token.at (0);
                   logEntry << " in configuration file at line ";
                   logEntry << ioConfigLine << ".\n";
                   log.Log (LOG_CRITICAL, logEntry.str ());
                   return false;
                 }

             }
           else
             output.m_RootPass = token;
        }
       else if (token == gEntUserPasswrd)
        {
           if (output.m_UserPasswd.size () > 0)
             {
               logEntry << "Configuration error at line ";
               logEntry << ioConfigLine << ". ";
               logEntry << "The user password for this database was ";
               logEntry << "already set.\n";
               log.Log (LOG_CRITICAL, logEntry.str ());
               return false;
             }

           token = NextToken (line, pos, delimiters);

           if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
             {
               logEntry << "Configuration error at line ";
               logEntry << ioConfigLine << ".\n";
               log.Log (LOG_CRITICAL, logEntry.str ());
               return false;
             }

           if ((token.at (0) == '\'') || (token.at (0) == '"'))
             {
               const string entry = line.c_str () + pos - token.length ();

               if (get_enclose_entry (logEntry,
                                      entry,
                                      token.at (0),
                                      output.m_UserPasswd) == false)
                 {
                   logEntry << "Unmatched "<< token.at (0);
                   logEntry << " in configuration file at line ";
                   logEntry << ioConfigLine << ".\n";
                   log.Log (LOG_CRITICAL, logEntry.str ());
                   return false;
                 }

             }
           else
             output.m_UserPasswd = token;
        }
      else
        {
          logEntry << "At line " << ioConfigLine << ": Don't know what to do ";
          logEntry << "with '" << token <<"'.\n";
          log.Log (LOG_CRITICAL, logEntry.str ());
          return false;
        }
    }

  return true;
}

bool
PrepareConfigurationSection (I_Logger& log)
{
  ostringstream logStream;

  if (gMainSettings.m_Listens.size () == 0)
    {
      ListenEntry defaultEnt = {"", DEFAULT_LISTEN_PORT};
      gMainSettings.m_Listens.push_back (defaultEnt);
    }

  if (gMainSettings.m_MaxConnections == 0)
    {
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (LOG_DEBUG,
                     "The number of maximum simultaneous connections per "
                     "interface set to default.");
        }
      gMainSettings.m_MaxConnections = DEFAULT_MAX_CONNS;
    }

  logStream << "Maximum simultaneous connections per interface set at ";
  logStream << gMainSettings.m_MaxConnections << ".";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  if (gMainSettings.m_Cipher == 0)
    {
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (LOG_DEBUG, "The communication cipher is set to default.");
        }
      gMainSettings.m_Cipher = FRAME_ENCTYPE_PLAIN;
    }
  logStream << "Communication cipher is set to '";
  switch (gMainSettings.m_Cipher)
  {
  case FRAME_ENCTYPE_PLAIN:
    logStream << CIPHER_PLAIN;
    break;
  case FRAME_ENCTYPE_3K:
    logStream << CIPHER_3K;
    break;
  default:
    assert (false);
  }
  logStream << "'.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  if (gMainSettings.m_MaxFrameSize == 0)
    {
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (LOG_DEBUG,
                   "The maximum communication frame size set to default.");
        }
      gMainSettings.m_MaxFrameSize = DEFAULT_FRAME_SIZE;
    }
  else if ((gMainSettings.m_MaxFrameSize < MIN_FRAME_SIZE)
            || (MAX_FRAME_SIZE < gMainSettings.m_MaxFrameSize))
    {
      logStream << "The maximum frame size set to a invalid value. ";
      logStream << "The value should be set between " << MIN_FRAME_SIZE;
      logStream << " and " << MAX_FRAME_SIZE << " bytes.";

      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  logStream << "Maximum communication frame size set to ";
  logStream << gMainSettings.m_MaxFrameSize <<" bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  if (gMainSettings.m_TableCacheBlockSize == 0)
    {
      gMainSettings.m_TableCacheBlockSize = DEFAULT_TABLE_CACHE_BLOCK_SIZE;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (LOG_DEBUG,
                   "The table cache block size is set to default value.");
        }
    }

  if (gMainSettings.m_TableCacheBlockSize < MIN_TABLE_CACHE_BLOCK_SIZE)
    {
      gMainSettings.m_TableCacheBlockSize = MIN_TABLE_CACHE_BLOCK_SIZE;
      log.Log (LOG_INFO,
               "The table cache block size was set to less than minimum. ");
    }
  logStream << "Table cache block size set at ";
  logStream << gMainSettings.m_TableCacheBlockSize << " bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  if (gMainSettings.m_TableCacheBlockCount == 0)
    {
      gMainSettings.m_TableCacheBlockCount = DEFAULT_TABLE_CACHE_BLOCK_COUNT;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (LOG_DEBUG,
                   "The table cache block count is set to default values.");
        }
    }
  if (gMainSettings.m_TableCacheBlockCount < MIN_TABLE_CACHE_BLOCK_COUNT)
    {
      gMainSettings.m_TableCacheBlockCount = MIN_TABLE_CACHE_BLOCK_COUNT;
      log.Log (LOG_INFO,
               "The table cache block count was set to less than minimum. ");
    }
  logStream << "Table cache block count set at ";
  logStream << gMainSettings.m_TableCacheBlockCount << '.';
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  //VLS
  if (gMainSettings.m_VLBlockSize == 0)
    {
      gMainSettings.m_VLBlockSize = DEFAULT_VL_BLOCK_SIZE;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The table VL store cache block size is set "
                     "to default value."
                  );
        }
    }
  if (gMainSettings.m_VLBlockSize < MIN_VL_BLOCK_SIZE)
    {
      gMainSettings.m_VLBlockSize = MIN_VL_BLOCK_SIZE;
      log.Log (
               LOG_INFO,
                 "The table cache block size was "
                 " set to less than minimum. "
              );
    }
  logStream << "Table VL store cache block size set at ";
  logStream << gMainSettings.m_VLBlockSize << " bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  if (gMainSettings.m_VLBlockCount == 0)
    {
      gMainSettings.m_VLBlockCount = DEFAULT_VL_BLOCK_COUNT;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The table VL store cache block count is set "
                     "to default value."
                   );
        }
    }

  if (gMainSettings.m_VLBlockCount < MIN_VL_BLOCK_COUNT)
    {
      gMainSettings.m_VLBlockCount = MIN_VL_BLOCK_COUNT;
      log.Log (
               LOG_INFO,
                 "The table VL store he block count was set "
                 "to less than minimum."
               );
    }
  logStream << "Table VL store cache block count set at ";
  logStream << gMainSettings.m_VLBlockCount << '.';
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  //Temporal values
  if (gMainSettings.m_TempValuesCache == 0)
    {
      gMainSettings.m_TempValuesCache = DEFAULT_TEMP_CACHE;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The temporal values cache is set "
                     "to default value."
                   );
        }
    }
  if (gMainSettings.m_TempValuesCache < MIN_TEMP_CACHE)
    {
      gMainSettings.m_TempValuesCache = MIN_TEMP_CACHE;
      log.Log (
               LOG_INFO,
                 "The temporal values cache was set "
                 "to less than minimum."
               );
    }
  logStream << "The temporal values cache set at ";
  logStream << gMainSettings.m_TempValuesCache << " bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  return true;
}

bool
PrepareContextSection (I_Logger& log, DBSDescriptors& ioDesc)
{
  assert (ioDesc.m_ConfigLine != 0);

  ostringstream logStream;

  if (ioDesc.m_UserPasswd.size () == 0)
    {
      logStream << "A user password for the database ";
      logStream << "section starting at line ";
      logStream << ioDesc.m_ConfigLine;
      logStream << " was not set.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  if (ioDesc.m_RootPass.size () == 0)
    {
      logStream << "A root password for the database ";
      logStream << "section starting at line ";
      logStream << ioDesc.m_ConfigLine;
      logStream << " was not set.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  if (ioDesc.m_DbsName.length () == 0)
    {
      logStream << "Database section starting line ";
      logStream << ioDesc.m_ConfigLine;
      logStream << " does not have a '";
      logStream << gEntDBSName << "' entry.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  if (ioDesc.m_DbsDirectory.length () == 0)
    {
      logStream << "Database section starting line ";
      logStream << ioDesc.m_ConfigLine;
      logStream << " does not have a '";
      logStream << gEntWorkDir << "' entry.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  if (ioDesc.m_DbsLogFile.length () == 0)
    {
      logStream << "Database section starting line ";
      logStream << ioDesc.m_ConfigLine;
      logStream << " does not have a '";
      logStream << gEntLogFile << "' entry.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  if ( ! whc_is_path_absolute (ioDesc.m_DbsLogFile.c_str ()))
    ioDesc.m_DbsLogFile = ioDesc.m_DbsDirectory + ioDesc.m_DbsLogFile;

  return true;
}
