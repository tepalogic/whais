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

#include "utils/tokenizer.h"

#include "server_protocol.h"
#include "configuration.h"

using namespace std;
using namespace whisper;

static const char COMMENT_CHAR                      = '#';
static const char DEFAULT_LISTEN_PORT[]             = "1761";
static const char CLEAR_LOG_STREAM[]                = "";

static const char CIPHER_PLAIN[]                    = "plain";
static const char CIPHER_3K[]                       = "3k";

static const uint_t MIN_TABLE_CACHE_BLOCK_SIZE      = 1024;
static const uint_t MIN_TABLE_CACHE_BLOCK_COUNT     = 128;
static const uint_t MIN_VL_BLOCK_SIZE               = 1024;
static const uint_t MIN_VL_BLOCK_COUNT              = 128;
static const uint_t MIN_TEMP_CACHE                  = 128;

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



// Helper function to retrieve '...' or "..." text entries.
static bool
get_enclose_entry (ostream&         os,
                   const string&    line,
                   const char       encChar,
                   string&          output)
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
SeekAtConfigurationSection (ifstream& config, uint_t& outConfigLine)
{
  static const string identifier("[CONFIG]");
  static const string delimiters(" \t");

  outConfigLine = 0;
  config.clear ();
  config.seekg (0);

  while (! config.eof ())
    {
      string line;

      getline (config, line);

      ++outConfigLine;

      size_t pos   = 0;
      string token = NextToken (line, pos, delimiters);

      if ((token.length () > 0) && (token.at (0) == COMMENT_CHAR))
        continue;

      if (token == identifier)
        return true;
    }
  return false;
}


bool
FindNextContextSection (std::ifstream& config, uint_t& inoutConfigLine)
{
  static const string identifier("[SESSION]");
  static const string delimiters(" \t");

  while (! config.eof ())
    {
      assert (config.good());

      string line;

      getline (config, line);

      ++inoutConfigLine;

      size_t pos   = 0;
      string token = NextToken (line, pos, delimiters);

      if ((token.length () > 0) && (token.at (0) == COMMENT_CHAR))
        continue;

      if (token == identifier)
        return true;
    }

  return false;
}


bool
ParseConfigurationSection (ifstream& config, uint_t& inoutConfigLine)
{
  static const string delimiters(" \t=");

  while ( !  config.eof ())
    {
      const streampos lastPos = config.tellg ();

      string line;
      getline (config, line);

      ++inoutConfigLine;

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
          entry.mInterface = token;

          token = NextToken (line, pos, " \t@#");
          entry.mService = token;

          if (entry.mInterface == "*")
            entry.mInterface = "";

          if (entry.mService == "")
            entry.mService = DEFAULT_LISTEN_PORT;

          gMainSettings.mListens.push_back (entry);
        }
      else if (token == gEntMaxConnections)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.mMaxConnections = atoi (token.c_str ());
        }
      else if (token == gEntMaxFrameSize)
       {
          token = NextToken (line, pos, delimiters);
          gMainSettings.mMaxFrameSize = atoi (token.c_str ());
       }
      else if (token == gEntEncryption)
        {
          token = NextToken (line, pos, delimiters);
          std::transform(token.begin(), token.end(), token.begin(), ::tolower);

          if (token == CIPHER_PLAIN)
            gMainSettings.mCipher = FRAME_ENCTYPE_PLAIN;

          else if (token == CIPHER_3K)
            gMainSettings.mCipher = FRAME_ENCTYPE_3K;

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
          gMainSettings.mTableCacheBlockCount = atoi (token.c_str ());

          if (gMainSettings.mTableCacheBlockCount == 0)
            {
              cerr << "Configuration error at line ";
              cerr << inoutConfigLine << ".\n";

              return false;
            }
        }
      else if (token == gEntTableBlkSize)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.mTableCacheBlockSize = atoi (token.c_str ());

          if (gMainSettings.mTableCacheBlockSize == 0)
            {
              cerr << "Configuration error at line ";
              cerr << inoutConfigLine << ".\n";

              return false;
            }
        }
      else if (token == gEntVlBlkCount)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.mVLBlockCount = atoi (token.c_str ());

          if (gMainSettings.mVLBlockCount == 0)
            {
              cerr << "Configuration error at line ";
              cerr << inoutConfigLine << ".\n";

              return false;
            }
        }
      else if (token == gEntVlBlkSize)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.mVLBlockSize = atoi (token.c_str ());

          if (gMainSettings.mVLBlockSize == 0)
            {
              cerr << "Configuration error at line ";
              cerr << inoutConfigLine << ".\n";

              return false;
            }
        }
      else if (token == gEntTempCache)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.mTempValuesCache = atoi (token.c_str ());

          if (gMainSettings.mTempValuesCache == 0)
            {
              cerr << "Configuration error at line ";
              cerr << inoutConfigLine << ".\n";

              return false;
            }
        }
      else if (token == gEntLogFile)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line ";
              cerr << inoutConfigLine << ".\n";

              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
              const string entry = line.c_str () + pos - token.length ();

              if (get_enclose_entry (cerr,
                                     entry,
                                     token.at (0),
                                     gMainSettings.mLogFile) == false)
                {
                  cerr << "Unmatched "<< token.at (0);
                  cerr << " in configuration file at line ";
                  cerr << inoutConfigLine << ".\n";

                  return false;
                }

            }
          else
            gMainSettings.mLogFile = token;
        }
      else if (token == gEntTempDir)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line ";
              cerr << inoutConfigLine << ".\n";

              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
              const string entry = line.c_str () + pos - token.length ();

              if (get_enclose_entry (cerr,
                                     entry,
                                     token.at (0),
                                     gMainSettings.mTempDirectory) == false)
                {
                  cerr << "Unmatched "<< token.at (0);
                  cerr << " in configuration file at line ";
                  cerr << inoutConfigLine << ".\n";
                  return false;
                }
            }
          else
            gMainSettings.mTempDirectory = token;

          string& dir = gMainSettings.mWorkDirectory;

          if ((dir.size () != 0)
              && (dir[dir.size () - 1] != whf_dir_delim()[0]))
            {
              dir += whf_dir_delim ();
            }
        }
      else if (token == gEntWorkDir)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line ";
              cerr << inoutConfigLine << ".\n";

              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
              const string entry = line.c_str () + pos - token.length ();

              if (get_enclose_entry (cerr,
                                     entry,
                                     token.at (0),
                                     gMainSettings.mWorkDirectory) == false)
                {
                  cerr << "Unmatched "<< token.at (0);
                  cerr << " in configuration file at line ";
                  cerr << inoutConfigLine << ".\n";
                  return false;
                }
            }
          else
            gMainSettings.mWorkDirectory = token;

          string& dir = gMainSettings.mTempDirectory;

          if ((dir.size () != 0)
              && (dir[dir.size () - 1] != whf_dir_delim()[0]))
            {
              dir += whf_dir_delim ();
            }
        }
      else if (token == gEntShowDbg)
        {
          token = NextToken (line, pos, delimiters);

          if (token == "false")
            gMainSettings.mShowDebugLog = false;

          else if (token == "true")
            gMainSettings.mShowDebugLog = true;

          else
            {
              cerr << "Unkown to assign '" << token << "\' to 'show_debug' ";
              cerr << "at line " << inoutConfigLine << ". ";
              cerr << "Valid value are only 'true' or 'false'.\n";

              return false;
            }
        }
      else
        {
          cerr << "At line " << inoutConfigLine << ": Don't know what to do ";
          cerr << "with '" << token <<"'.\n";
          return false;
        }
    }

  if (gMainSettings.mWorkDirectory.length () == 0)
    {
      cerr << "The configuration main section does not ";
      cerr << "have a '"<< gEntWorkDir << "' entry.\n";

      return false;
    }

  if (gMainSettings.mTempDirectory.length () == 0)
    gMainSettings.mTempDirectory = gMainSettings.mWorkDirectory;

  if (gMainSettings.mLogFile.length () == 0)
    {
      cerr << "The configuration main section does not ";
      cerr << "have a '"<< gEntLogFile << "' entry.\n";

      return false;
    }

  return true;
}


bool
ParseContextSection (Logger&          log,
                     ifstream&        config,
                     uint_t&          inoutConfigLine,
                     DBSDescriptors&  output)
{
  ostringstream logEntry;
  static const string delimiters = " \t=";

  while ( ! config.eof ())
    {
      const streamoff lastPos = config.tellg ();

      string line;
      getline (config, line);

      ++inoutConfigLine;

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

       if (token == gEntDBSName)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              logEntry << "Configuration error at line ";
              logEntry << inoutConfigLine << ".\n";
              log.Log (LOG_CRITICAL, logEntry.str ());

              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
              const string entry = line.c_str () + pos - token.length ();

              if (get_enclose_entry (logEntry,
                                     entry,
                                     token.at (0),
                                     output.mDbsName) == false)
                {
                  logEntry << "Unmatched "<< token.at (0);
                  logEntry << " in configuration file at line ";
                  logEntry << inoutConfigLine << ".\n";
                  log.Log (LOG_CRITICAL, logEntry.str ());

                  return false;
                }

            }
          else
            output.mDbsName = token;
        }
       else if (token == gEntWorkDir)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line ";
              cerr << inoutConfigLine << ".\n";

              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
             const string entry = line.c_str () + pos - token.length ();

             if (get_enclose_entry (cerr,
                                    entry,
                                    token.at (0),
                                    output.mDbsDirectory) == false)
               {
                 cerr << "Unmatched "<< token.at (0);
                 cerr << " in configuration file at line ";
                 cerr << inoutConfigLine << ".\n";

                 return false;
               }
            }
          else
            output.mDbsDirectory = token;

          string& dir = output.mDbsDirectory;

          if ((dir.size () != 0)
              && (dir[dir.size () - 1] != whf_dir_delim()[0]))
            {
              dir += whf_dir_delim ();
            }
        }
       else if (token == gEntLogFile)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line " << inoutConfigLine << ".\n";
              return false;
            }

          if ((token.at (0) == '\'') || (token.at (0) == '"'))
            {
              const string entry = line.c_str () + pos - token.length ();

              if (get_enclose_entry (cerr,
                                     entry,
                                     token.at (0),
                                     output.mDbsLogFile) == false)
                {
                  cerr << "Unmatched "<< token.at (0);
                  cerr << " in configuration file at line ";
                  cerr << inoutConfigLine << ".\n";

                  return false;
                }

            }
          else
            output.mDbsLogFile = token;
        }
       else if (token==gEntObjectLib)
        {
           token = NextToken (line, pos, delimiters);

           if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
             {
               logEntry << "Configuration error at line ";
               logEntry << inoutConfigLine << ".\n";
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
                   logEntry << inoutConfigLine << ".\n";
                   log.Log (LOG_CRITICAL, logEntry.str ());

                   return false;
                 }

             }
           else
             libEntry = token;

           if ( ! whf_is_absolute (libEntry.c_str ()))
             {
               libEntry = gMainSettings.mWorkDirectory + libEntry;
             }
           output.mObjectLibs.push_back (libEntry);
        }
       else if (token==gEntNativeLib)
        {
           token = NextToken (line, pos, delimiters);

           if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
             {
               logEntry << "Configuration error at line ";
               logEntry << inoutConfigLine << ".\n";
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
                   logEntry << inoutConfigLine << ".\n";
                   log.Log (LOG_CRITICAL, logEntry.str ());

                   return false;
                 }

             }
           else
             libEntry = token;

           if ( ! whf_is_absolute (libEntry.c_str ()))
             {
               libEntry = gMainSettings.mWorkDirectory + libEntry;
             }
           output.mNativeLibs.push_back (libEntry);
        }
       else if (token == gEntRootPasswrd)
        {
           if (output.mRootPass.size () > 0)
             {
               logEntry << "Configuration error at line ";
               logEntry << inoutConfigLine << ". ";
               logEntry << "The root password for this database was ";
               logEntry << "already set.\n";
               log.Log (LOG_CRITICAL, logEntry.str ());

               return false;
             }
           token = NextToken (line, pos, delimiters);

           if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
             {
               logEntry << "Configuration error at line ";
               logEntry << inoutConfigLine << ".\n";
               log.Log (LOG_CRITICAL, logEntry.str ());

               return false;
             }

           if ((token.at (0) == '\'') || (token.at (0) == '"'))
             {
               const string entry = line.c_str () + pos - token.length ();

               if (get_enclose_entry (logEntry,
                                      entry,
                                      token.at (0),
                                      output.mRootPass) == false)
                 {
                   logEntry << "Unmatched "<< token.at (0);
                   logEntry << " in configuration file at line ";
                   logEntry << inoutConfigLine << ".\n";
                   log.Log (LOG_CRITICAL, logEntry.str ());

                   return false;
                 }

             }
           else
             output.mRootPass = token;
        }
       else if (token == gEntUserPasswrd)
        {
           if (output.mUserPasswd.size () > 0)
             {
               logEntry << "Configuration error at line ";
               logEntry << inoutConfigLine << ". ";
               logEntry << "The user password for this database was ";
               logEntry << "already set.\n";
               log.Log (LOG_CRITICAL, logEntry.str ());

               return false;
             }

           token = NextToken (line, pos, delimiters);

           if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
             {
               logEntry << "Configuration error at line ";
               logEntry << inoutConfigLine << ".\n";
               log.Log (LOG_CRITICAL, logEntry.str ());

               return false;
             }

           if ((token.at (0) == '\'') || (token.at (0) == '"'))
             {
               const string entry = line.c_str () + pos - token.length ();

               if (get_enclose_entry (logEntry,
                                      entry,
                                      token.at (0),
                                      output.mUserPasswd) == false)
                 {
                   logEntry << "Unmatched "<< token.at (0);
                   logEntry << " in configuration file at line ";
                   logEntry << inoutConfigLine << ".\n";
                   log.Log (LOG_CRITICAL, logEntry.str ());

                   return false;
                 }

             }
           else
             output.mUserPasswd = token;
        }
      else
        {
          logEntry << "At line " << inoutConfigLine << ": Don't know what to do ";
          logEntry << "with '" << token <<"'.\n";
          log.Log (LOG_CRITICAL, logEntry.str ());

          return false;
        }
    }

  return true;
}

bool
PrepareConfigurationSection (Logger& log)
{
  ostringstream logStream;

  if (gMainSettings.mListens.size () == 0)
    {
      ListenEntry defaultEnt = {"", DEFAULT_LISTEN_PORT};
      gMainSettings.mListens.push_back (defaultEnt);
    }

  if (gMainSettings.mMaxConnections == 0)
    {
      if (gMainSettings.mShowDebugLog)
        {
          log.Log (LOG_DEBUG,
                     "The number of maximum simultaneous connections per "
                     "interface set to default.");
        }
      gMainSettings.mMaxConnections = DEFAULT_MAX_CONNS;
    }

  logStream << "Maximum simultaneous connections per interface set at ";
  logStream << gMainSettings.mMaxConnections << ".";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  if (gMainSettings.mCipher == 0)
    {
      if (gMainSettings.mShowDebugLog)
        {
          log.Log (LOG_DEBUG, "The communication cipher is set to default.");
        }
      gMainSettings.mCipher = FRAME_ENCTYPE_PLAIN;
    }

  logStream << "Communication cipher is set to '";
  switch (gMainSettings.mCipher)
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

  if (gMainSettings.mMaxFrameSize == 0)
    {
      if (gMainSettings.mShowDebugLog)
        {
          log.Log (LOG_DEBUG,
                   "The maximum communication frame size set to default.");
        }
      gMainSettings.mMaxFrameSize = DEFAULT_FRAME_SIZE;
    }
  else if ((gMainSettings.mMaxFrameSize < MIN_FRAME_SIZE)
            || (MAX_FRAME_SIZE < gMainSettings.mMaxFrameSize))
    {
      logStream << "The maximum frame size set to a invalid value. ";
      logStream << "The value should be set between " << MIN_FRAME_SIZE;
      logStream << " and " << MAX_FRAME_SIZE << " bytes.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  logStream << "Maximum communication frame size set to ";
  logStream << gMainSettings.mMaxFrameSize <<" bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  if (gMainSettings.mTableCacheBlockSize == 0)
    {
      gMainSettings.mTableCacheBlockSize = DEFAULT_TABLE_CACHE_BLOCK_SIZE;
      if (gMainSettings.mShowDebugLog)
        {
          log.Log (LOG_DEBUG,
                   "The table cache block size is set to default value.");
        }
    }

  if (gMainSettings.mTableCacheBlockSize < MIN_TABLE_CACHE_BLOCK_SIZE)
    {
      gMainSettings.mTableCacheBlockSize = MIN_TABLE_CACHE_BLOCK_SIZE;
      log.Log (LOG_INFO,
               "The table cache block size was set to less than minimum. ");
    }
  logStream << "Table cache block size set at ";
  logStream << gMainSettings.mTableCacheBlockSize << " bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  if (gMainSettings.mTableCacheBlockCount == 0)
    {
      gMainSettings.mTableCacheBlockCount = DEFAULT_TABLE_CACHE_BLOCK_COUNT;
      if (gMainSettings.mShowDebugLog)
        {
          log.Log (LOG_DEBUG,
                   "The table cache block count is set to default values.");
        }
    }
  if (gMainSettings.mTableCacheBlockCount < MIN_TABLE_CACHE_BLOCK_COUNT)
    {
      gMainSettings.mTableCacheBlockCount = MIN_TABLE_CACHE_BLOCK_COUNT;
      log.Log (LOG_INFO,
               "The table cache block count was set to less than minimum. ");
    }

  logStream << "Table cache block count set at ";
  logStream << gMainSettings.mTableCacheBlockCount << '.';
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  //VLS
  if (gMainSettings.mVLBlockSize == 0)
    {
      gMainSettings.mVLBlockSize = DEFAULT_VL_BLOCK_SIZE;
      if (gMainSettings.mShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The table VL store cache block size is set "
                     "to default value."
                  );
        }
    }

  if (gMainSettings.mVLBlockSize < MIN_VL_BLOCK_SIZE)
    {
      gMainSettings.mVLBlockSize = MIN_VL_BLOCK_SIZE;
      log.Log (
               LOG_INFO,
                 "The table cache block size was "
                 " set to less than minimum. "
              );
    }
  logStream << "Table VL store cache block size set at ";
  logStream << gMainSettings.mVLBlockSize << " bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  if (gMainSettings.mVLBlockCount == 0)
    {
      gMainSettings.mVLBlockCount = DEFAULT_VL_BLOCK_COUNT;
      if (gMainSettings.mShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The table VL store cache block count is set "
                     "to default value."
                   );
        }
    }

  if (gMainSettings.mVLBlockCount < MIN_VL_BLOCK_COUNT)
    {
      gMainSettings.mVLBlockCount = MIN_VL_BLOCK_COUNT;
      log.Log (
               LOG_INFO,
                 "The table VL store he block count was set "
                 "to less than minimum."
               );
    }
  logStream << "Table VL store cache block count set at ";
  logStream << gMainSettings.mVLBlockCount << '.';
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  //Temporal values
  if (gMainSettings.mTempValuesCache == 0)
    {
      gMainSettings.mTempValuesCache = DEFAULT_TEMP_CACHE;
      if (gMainSettings.mShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The temporal values cache is set "
                     "to default value."
                   );
        }
    }
  if (gMainSettings.mTempValuesCache < MIN_TEMP_CACHE)
    {
      gMainSettings.mTempValuesCache = MIN_TEMP_CACHE;
      log.Log (
               LOG_INFO,
                 "The temporal values cache was set "
                 "to less than minimum."
               );
    }
  logStream << "The temporal values cache set at ";
  logStream << gMainSettings.mTempValuesCache << " bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str (CLEAR_LOG_STREAM);

  return true;
}


bool
PrepareContextSection (Logger& log, DBSDescriptors& inoutDesc)
{
  assert (inoutDesc.mConfigLine != 0);

  ostringstream logStream;

  if (inoutDesc.mUserPasswd.size () == 0)
    {
      logStream << "A user password for the database ";
      logStream << "section starting at line ";
      logStream << inoutDesc.mConfigLine << " was not set.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  if (inoutDesc.mRootPass.size () == 0)
    {
      logStream << "A root password for the database ";
      logStream << "section starting at line ";
      logStream << inoutDesc.mConfigLine << " was not set.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  if (inoutDesc.mDbsName.length () == 0)
    {
      logStream << "Database section starting line ";
      logStream << inoutDesc.mConfigLine;
      logStream << " does not have a '" << gEntDBSName << "' entry.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  if (inoutDesc.mDbsDirectory.length () == 0)
    {
      logStream << "Database section starting line ";
      logStream << inoutDesc.mConfigLine;
      logStream << " does not have a '" << gEntWorkDir << "' entry.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  if (inoutDesc.mDbsLogFile.length () == 0)
    {
      logStream << "Database section starting line ";
      logStream << inoutDesc.mConfigLine;
      logStream << " does not have a '" << gEntLogFile << "' entry.";
      log.Log (LOG_ERROR, logStream.str ());

      return false;
    }

  if ( ! whf_is_absolute (inoutDesc.mDbsLogFile.c_str ()))
    inoutDesc.mDbsLogFile = inoutDesc.mDbsDirectory + inoutDesc.mDbsLogFile;

  return true;
}
