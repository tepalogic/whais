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

#include "configuration.h"

#include "utils/include/tokenizer.h"

using namespace std;

static const D_CHAR COMMENT_CHAR = '#';

static const D_UINT MIN_TABLE_CACHE_BLOCK_SIZE  = 1024;
static const D_UINT MIN_TABLE_CACHE_BLOCK_COUNT = 128;
static const D_UINT MIN_VL_BLOCK_SIZE           = 1024;
static const D_UINT MIN_VL_BLOCK_COUNT          = 128;
static const D_UINT MIN_TEMP_CACHE              = 128;

static const D_UINT DEFAULT_LISTEN_PORT             = 1761;
static const D_UINT DEFAULT_TABLE_CACHE_BLOCK_SIZE  = 4098;
static const D_UINT DEFAULT_TABLE_CACHE_BLOCK_COUNT = 1024;
static const D_UINT DEFAULT_VL_BLOCK_SIZE           = 1024;
static const D_UINT DEFAULT_VL_BLOCK_COUNT          = 4098;
static const D_UINT DEFAULT_TEMP_CACHE              = 512;

static const string gEntPort ("port");
static const string gEntTableBlkSize ("table_block_cache_size");
static const string gEntTableBlkCount ("table_block_cache_count");
static const string gEntVlBlkSize ("vl_values_block_size");
static const string gEntVlBlkCount ("vl_values_block_count");
static const string gEntMaxTableBlkSize ("max_table_block_cache_size");
static const string gEntMaxTableBlkCount ("max_table_block_cache_count");
static const string gEntMaxVlBlkSize ("max_vl_values_block_size");
static const string gEntMaxVlBlkCount ("max_vl_values_block_count");
static const string gEntMaxTempCache ("max_temporals_cache");
static const string gEntTempCache ("temporals_cache");
static const string gEntLogFile ("log_file");
static const string gEntDBSName ("name");
static const string gEntDBSDir ("directory");
static const string gEntTmpDir ("temp_directory");
static const string gEntShowDbg ("show_debug");

static AdminSettings gMainSettings;

static bool
get_enclose_entry (ostream&      os,
                   const string& line,
                   const D_CHAR  encChar,
                   string&       output)
{
  assert (line.at (0) == encChar);

  bool encEnded = false;

  for (D_UINT i = 1; i < line.length (); ++i)
    {
      D_CHAR ch = line.at (i);
      if (ch == encChar)
        {
          encEnded = true;
          break;
        }
      output.append (1, ch);
    }

  return encEnded;
}

const AdminSettings&
GetAdminSettings ()
{
  return gMainSettings;
}

bool
SeekAtGlobalSection (ifstream& config, D_UINT& oSectionLine)
{
  static const string identifier = "[ADMIN]";
  static const string delimiters = " \t";

  oSectionLine = 0;
  config.seekg (0, ios::beg);
  while (! config.eof ())
    {
      string line;
      getline (config, line);

      ++oSectionLine;

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
FindNextSessionSection (std::ifstream& config, D_UINT& ioSectionLine)
{
  static const string identifier = "[SESSION]";
  static const string delimiters = " \t";

  while (! config.eof ())
    {
      string line;
      getline (config, line);

      ++ioSectionLine;

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
ParseMainSection (ifstream& config, D_UINT& ioSectionLine)
{
  static const string delimiters = " \t=";

  while ( !  config.eof ())
    {
      string line;
      getline (config, line);

      ++ioSectionLine;

      size_t pos   = 0;
      string token = NextToken (line, pos, delimiters);

      if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
        continue;

      if (token.at (0) == '[')
        {
          //Another configuration section starts from here.
          config.seekg (- line.length (), ios::cur);
          break;
        }

      if (token == gEntPort)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_ListenPort = atoi (token.c_str ());

          if (gMainSettings.m_ListenPort == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntTableBlkCount)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TableCacheBlockCount = atoi (token.c_str ());

          if (gMainSettings.m_TableCacheBlockCount == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntTableBlkSize)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TableCacheBlockSize = atoi (token.c_str ());

          if (gMainSettings.m_TableCacheBlockSize == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntVlBlkCount)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_VLBlockCount = atoi (token.c_str ());

          if (gMainSettings.m_VLBlockCount == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntVlBlkSize)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_VLBlockSize = atoi (token.c_str ());

          if (gMainSettings.m_VLBlockSize == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntTempCache)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TempValuesCache = atoi (token.c_str ());

          if (gMainSettings.m_TempValuesCache == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntMaxTableBlkCount)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_MaxTableCacheBlockCount = atoi (token.c_str ());

          if (gMainSettings.m_MaxTableCacheBlockCount == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntMaxTableBlkSize)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_MaxTableCacheBlockSize = atoi (token.c_str ());

          if (gMainSettings.m_MaxTableCacheBlockSize == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntMaxVlBlkCount)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_MaxVLBlockCount = atoi (token.c_str ());

          if (gMainSettings.m_MaxVLBlockCount == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntMaxVlBlkSize)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_MaxVLBlockSize = atoi (token.c_str ());

          if (gMainSettings.m_MaxVLBlockSize == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntMaxTempCache)
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TempValuesCache = atoi (token.c_str ());

          if (gMainSettings.m_MaxTempValuesCache == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == gEntLogFile)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
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
                  cerr << ioSectionLine << ".\n";
                  return false;
                }

            }
          else
            gMainSettings.m_LogFile = token;
        }
      else if (token == gEntTmpDir)
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
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
                  cerr << ioSectionLine << ".\n";
                  return false;
                }

            }
          else
            gMainSettings.m_TempDirectory = token;
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
              cerr << "at line " << ioSectionLine << ". ";
              cerr << "Valid value are only 'true' or 'false'.\n";

              return -1;
            }
        }
      else
        {
          cerr << "At line " << ioSectionLine << ": Don't know what to do ";
          cerr << "with '" << token <<"'.\n";
          return false;
        }
    }

  if (gMainSettings.m_LogFile == "")
    {
      cerr << "The configuration main section ";
      cerr << "does not have a 'log_file' entry.\n";

      return false;
    }

  return true;
}

bool
ParseSessionSection (Logger&          log,
                     ifstream&        config,
                     D_UINT&          ioConfigLine,
                     SessionSettings& output)
{
  ostringstream logEntry;
  static const string delimiters = " \t=";

  while ( !  config.eof ())
    {
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
          config.seekg (- line.length (), ios::cur);
          break;
        }

      if (token == gEntTableBlkCount)
        {
          token = NextToken (line, pos, delimiters);
          output.m_TableCacheBlockCount = atoi (token.c_str ());

          if (output.m_TableCacheBlockCount == 0)
            {
              logEntry << "Configuration error at line ";
              logEntry << ioConfigLine << ".\n";
              log.Log (LOG_CRITICAL, logEntry.str ());
              return false;
            }
        }
      else if (token == gEntTableBlkSize)
        {
          token = NextToken (line, pos, delimiters);
          output.m_TableCacheBlockSize = atoi (token.c_str ());

          if (output.m_TableCacheBlockSize == 0)
            {
              logEntry << "Configuration error at line ";
              logEntry << ioConfigLine << ".\n";
              log.Log (LOG_CRITICAL, logEntry.str ());
              return false;
            }
        }
      else if (token == gEntVlBlkCount)
        {
          token = NextToken (line, pos, delimiters);
          output.m_VLBlockCount = atoi (token.c_str ());

          if (output.m_VLBlockCount == 0)
            {
              logEntry << "Configuration error at line ";
              logEntry << ioConfigLine << ".\n";
              log.Log (LOG_CRITICAL, logEntry.str ());
              return false;
            }
        }
      else if (token == gEntVlBlkSize)
        {
          token = NextToken (line, pos, delimiters);
          output.m_VLBlockSize = atoi (token.c_str ());

          if (output.m_VLBlockSize == 0)
            {
              logEntry << "Configuration error at line ";
              logEntry << ioConfigLine << ".\n";
              log.Log (LOG_CRITICAL, logEntry.str ());
              return false;
            }
        }
      else if (token == gEntTempCache)
        {
          token = NextToken (line, pos, delimiters);
          output.m_TempValuesCache = atoi (token.c_str ());

          if (output.m_TempValuesCache == 0)
            {
              logEntry << "Configuration error at line ";
              logEntry << ioConfigLine << ".\n";
              log.Log (LOG_CRITICAL, logEntry.str ());
              return false;
            }
        }
       else if (token == gEntDBSName)
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
                                     output.m_Name) == false)
                {
                  logEntry << "Unmatched "<< token.at (0);
                  logEntry << " in configuration file at line ";
                  logEntry << ioConfigLine << ".\n";
                  log.Log (LOG_CRITICAL, logEntry.str ());
                  return false;
                }

            }
          else
            output.m_Name = token;
        }
       else if (token == gEntDBSDir)
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
                                     output.m_DBSDirectory) == false)
                {
                  logEntry << "Unmatched "<< token.at (0);
                  logEntry << " in configuration file at line ";
                  logEntry << ioConfigLine << ".\n";
                  log.Log (LOG_CRITICAL, logEntry.str ());
                  return false;
                }

            }
          else
            output.m_DBSDirectory = token;
        }
       else if (token == gEntTmpDir)
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
                                     output.m_TempDirectory) == false)
                {
                  logEntry << "Unmatched "<< token.at (0);
                  logEntry << " in configuration file at line ";
                  logEntry << ioConfigLine << ".\n";
                  log.Log (LOG_CRITICAL, logEntry.str ());
                  return false;
                }

            }
          else
            output.m_TempDirectory = token;
        }
      else if (token == gEntShowDbg)
        {
          token = NextToken (line, pos, delimiters);
          if (token == "false")
            output.m_ShowDebugLog = false;
          else if (token == "true")
            output.m_ShowDebugLog = true;
          else
            {
              logEntry << "Unkown to assign '";
              logEntry << token << "\' to 'show_debug' ";
              logEntry << "at line " << ioConfigLine << ". ";
              logEntry << "Valid value are only 'true' or 'false'.\n";
              log.Log (LOG_CRITICAL, logEntry.str ());
              return false;
            }
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
FixMainSection (Logger& log)
{
  ostringstream logStream;

  if (gMainSettings.m_ListenPort == 0)
    {
      gMainSettings.m_ListenPort = DEFAULT_LISTEN_PORT;
      if (gMainSettings.m_ShowDebugLog)
        log.Log (LOG_DEBUG, "The listen port is set to default value.");
    }
  logStream << "Listen port set at " << gMainSettings.m_ListenPort << '.';
  log.Log (LOG_INFO, logStream.str ());
  logStream.str ("");

  if (gMainSettings.m_MaxTableCacheBlockSize == 0)
    {
      gMainSettings.m_MaxTableCacheBlockSize = DEFAULT_TABLE_CACHE_BLOCK_SIZE;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The maximum table cache block size is set "
                     "to default value."
                  );
        }
    }
  if (gMainSettings.m_MaxTableCacheBlockSize < MIN_TABLE_CACHE_BLOCK_SIZE)
    {
      gMainSettings.m_MaxTableCacheBlockSize = MIN_TABLE_CACHE_BLOCK_SIZE;
      log.Log (
                LOG_INFO,
                  "The maximum table cache block size "
                  "was set to less than minimum. "
              );
    }
  logStream << "Table maximum cache block size set at ";
  logStream << gMainSettings.m_MaxTableCacheBlockSize << " bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str ("");

  if (gMainSettings.m_TableCacheBlockSize == 0)
    {
      gMainSettings.m_TableCacheBlockSize = DEFAULT_TABLE_CACHE_BLOCK_SIZE;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (LOG_DEBUG,
                   "The table cache block size is set to default value.");
        }
    }
  if (gMainSettings.m_TableCacheBlockSize >
      gMainSettings.m_MaxTableCacheBlockSize)
    {
      gMainSettings.m_TableCacheBlockSize =
          gMainSettings.m_MaxTableCacheBlockSize;
      log.Log (LOG_INFO,
               "The table cache block size was set bigger than maximum. ");
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
  logStream.str ("");

  if (gMainSettings.m_MaxTableCacheBlockCount == 0)
    {
      gMainSettings.m_MaxTableCacheBlockCount =
                                    DEFAULT_TABLE_CACHE_BLOCK_COUNT;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The maximum table cache block count is set "
                     "to default value."
                  );
        }
    }
  if (gMainSettings.m_MaxTableCacheBlockCount < MIN_TABLE_CACHE_BLOCK_COUNT)
    {
      gMainSettings.m_MaxTableCacheBlockCount = MIN_TABLE_CACHE_BLOCK_COUNT;
      log.Log (
                LOG_INFO,
                  "The maximum table cache block count "
                  "was set to less than minimum. "
              );
    }
  logStream << "Table maximum cache block count set at ";
  logStream << gMainSettings.m_MaxTableCacheBlockCount << '.';
  log.Log (LOG_INFO, logStream.str ());
  logStream.str ("");

  if (gMainSettings.m_TableCacheBlockCount == 0)
    {
      gMainSettings.m_TableCacheBlockCount = DEFAULT_TABLE_CACHE_BLOCK_COUNT;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (LOG_DEBUG,
                   "The table cache block count is set to default values.");
        }
    }
  if (gMainSettings.m_TableCacheBlockCount >
      gMainSettings.m_MaxTableCacheBlockCount)
    {
      gMainSettings.m_TableCacheBlockCount =
          gMainSettings.m_MaxTableCacheBlockCount;
      log.Log (LOG_INFO,
               "The table cache block count was set bigger than maximum. ");
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
  logStream.str ("");


  //VLS

  if (gMainSettings.m_MaxVLBlockSize == 0)
    {
      gMainSettings.m_MaxVLBlockSize = DEFAULT_VL_BLOCK_SIZE;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The maximum table VL store cache block size is set "
                     "to default value."
                  );
        }
    }
  if (gMainSettings.m_MaxVLBlockSize < MIN_VL_BLOCK_SIZE)
    {
      gMainSettings.m_MaxVLBlockSize = MIN_VL_BLOCK_SIZE;
      log.Log (
                LOG_INFO,
                  "The maximum table VL store cache block size "
                  "was set to less than minimum. "
              );
    }
  logStream << "Table maximum table VL store cache block size set at ";
  logStream << gMainSettings.m_MaxVLBlockSize << " bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str ("");

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
  if (gMainSettings.m_VLBlockSize > gMainSettings.m_MaxVLBlockSize)
    {
      gMainSettings.m_VLBlockSize = gMainSettings.m_MaxVLBlockSize;
      log.Log (
               LOG_INFO,
                 "The table VL store cache block size was "
                 "set bigger than maximum. "
              );
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
  logStream.str ("");

  if (gMainSettings.m_MaxVLBlockCount == 0)
    {
      gMainSettings.m_MaxVLBlockCount = DEFAULT_VL_BLOCK_COUNT;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The maximum table VL store cache block count is set "
                     "to default value."
                  );
        }
    }
  if (gMainSettings.m_MaxVLBlockCount < MIN_VL_BLOCK_COUNT)
    {
      gMainSettings.m_MaxVLBlockCount = MIN_VL_BLOCK_COUNT;
      log.Log (
                LOG_INFO,
                  "The maximum table VL store cache block count "
                  "was set to less than minimum. "
              );
    }
  logStream << "Table maximum VL store cache block count set at ";
  logStream << gMainSettings.m_MaxVLBlockCount << '.';
  log.Log (LOG_INFO, logStream.str ());
  logStream.str ("");

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
  if (gMainSettings.m_VLBlockCount > gMainSettings.m_MaxVLBlockCount)
    {
      gMainSettings.m_VLBlockCount = gMainSettings.m_MaxVLBlockCount;
      log.Log (
                LOG_INFO,
                  "The table VL store cache block count was set "
                  "bigger than maximum."
              );
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
  logStream.str ("");

  //Temporal values

  if (gMainSettings.m_MaxTempValuesCache == 0)
    {
      gMainSettings.m_MaxTempValuesCache = DEFAULT_TEMP_CACHE;
      if (gMainSettings.m_ShowDebugLog)
        {
          log.Log (
                   LOG_DEBUG,
                     "The maximum temporal values cache is set "
                     "to default value."
                  );
        }
    }
  if (gMainSettings.m_MaxTempValuesCache < MIN_TEMP_CACHE)
    {
      gMainSettings.m_MaxTempValuesCache = MIN_TEMP_CACHE;
      log.Log (
               LOG_INFO,
                 "The maximum temporal values cache was set "
                 "to less than minimum."
               );
    }

  logStream << "The maximum temporal values cache set at ";
  logStream << gMainSettings.m_MaxTempValuesCache << " bytes.";
  log.Log (LOG_INFO, logStream.str ());
  logStream.str ("");

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
  if (gMainSettings.m_TempValuesCache > gMainSettings.m_MaxTempValuesCache)
    {
      gMainSettings.m_TempValuesCache = gMainSettings.m_MaxTempValuesCache;
      log.Log (
                LOG_INFO,
                  "The temporal values cache was set "
                  "bigger than maximum."
              );
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
  logStream.str ("");

  return true;
}

bool
FixSessionSection (Logger& log, SessionSettings& ioSession)
{
  if (ioSession.m_TableCacheBlockSize == 0)
    ioSession.m_TableCacheBlockSize = gMainSettings.m_TableCacheBlockSize;

  if (ioSession.m_TableCacheBlockSize > gMainSettings.m_MaxTableCacheBlockSize)
    ioSession.m_TableCacheBlockSize = gMainSettings.m_MaxTableCacheBlockSize;

  if (ioSession.m_TableCacheBlockCount == 0)
    ioSession.m_TableCacheBlockCount = gMainSettings.m_TableCacheBlockCount;

  if (ioSession.m_TableCacheBlockCount >
      gMainSettings.m_MaxTableCacheBlockCount)
    {
      ioSession.m_TableCacheBlockCount =
                                    gMainSettings.m_MaxTableCacheBlockCount;
    }

  if (ioSession.m_VLBlockSize == 0)
    ioSession.m_VLBlockSize = gMainSettings.m_VLBlockSize;

  if (ioSession.m_VLBlockSize > gMainSettings.m_MaxVLBlockSize)
    ioSession.m_VLBlockSize = gMainSettings.m_MaxVLBlockSize;

  if (ioSession.m_VLBlockCount == 0)
    ioSession.m_VLBlockCount = gMainSettings.m_VLBlockCount;

  if (ioSession.m_VLBlockCount > gMainSettings.m_MaxVLBlockCount)
    ioSession.m_VLBlockCount = gMainSettings.m_MaxVLBlockCount;

  if (ioSession.m_DBSDirectory.at (ioSession.m_DBSDirectory.length () - 1) !=
      whc_get_directory_delimiter ()[0])
    {
      ioSession.m_DBSDirectory.append (whc_get_directory_delimiter ());
    }

  if (ioSession.m_TempDirectory.length () == 0)
    ioSession.m_TempDirectory = gMainSettings.m_TempDirectory;

  if (ioSession.m_TempDirectory.at (ioSession.m_TempDirectory.length () - 1) !=
      whc_get_directory_delimiter ()[0])
    {
      ioSession.m_TempDirectory.append (whc_get_directory_delimiter ());
    }


  return true;
}
