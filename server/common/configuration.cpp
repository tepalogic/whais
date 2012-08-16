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

#include "configuration.h"

#include "utils/include/tokenizer.h"

using namespace std;

static const D_CHAR COMMENT_CHAR = '#';

static const D_UINT DEFAULT_LISTEN_PORT             = 1761;
static const D_UINT DEFAULT_TABLE_CACHE_BLOCK_SIZE  = 4098;
static const D_UINT DEFAULT_TABLE_CACHE_BLOCK_COUNT = 1024;
static const D_UINT DEFAULT_VL_BLOCK_SIZE           = 1024;
static const D_UINT DEFAULT_VL_BLOCK_COUNT          = 4098;
static const D_UINT DEFAULT_TEMP_CACHE              = 512;

static AdminSettings gMainSettings;

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
ParseMainSection (ifstream& config, D_UINT& ioSectionLine)
{
  static const string delimiters = " \t=";

  while ( !  config.eof ())
    {
      string line;
      getline (config, line);

      ++ioSectionLine;

      size_t pos = 0;
      string token = NextToken (line, pos, delimiters);

      if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
        continue;

      if (token.at (0) == '[')
        {
          //Another configuration section starts from here.
          config.seekg (- line.length (), ios::cur);
          break;
        }

      if (token == "port")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_ListenPort = atoi (token.c_str ());

          if (gMainSettings.m_ListenPort == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "table_block_cache_count")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TableCacheBlockCount = atoi (token.c_str ());

          if (gMainSettings.m_TableCacheBlockCount == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "table_block_cache_size")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TableCacheBlockSize = atoi (token.c_str ());

          if (gMainSettings.m_TableCacheBlockSize == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "vl_values_block_count")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_VLBlockCount = atoi (token.c_str ());

          if (gMainSettings.m_VLBlockCount == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "vl_values_block_size")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_VLBlockSize = atoi (token.c_str ());

          if (gMainSettings.m_VLBlockSize == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "temporals_cache")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TempValuesCache = atoi (token.c_str ());

          if (gMainSettings.m_TempValuesCache == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "max_table_block_cache_count")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_MaxTableCacheBlockCount = atoi (token.c_str ());

          if (gMainSettings.m_MaxTableCacheBlockCount == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "max_table_block_cache_size")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_MaxTableCacheBlockSize = atoi (token.c_str ());

          if (gMainSettings.m_MaxTableCacheBlockSize == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "vl_values_block_count")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_MaxVLBlockCount = atoi (token.c_str ());

          if (gMainSettings.m_MaxVLBlockCount == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "max_vl_values_block_size")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_MaxVLBlockSize = atoi (token.c_str ());

          if (gMainSettings.m_MaxVLBlockSize == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "max_temporals_cache")
        {
          token = NextToken (line, pos, delimiters);
          gMainSettings.m_TempValuesCache = atoi (token.c_str ());

          if (gMainSettings.m_MaxTempValuesCache == 0)
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }
        }
      else if (token == "log_file")
        {
          token = NextToken (line, pos, delimiters);

          if ((token.length () == 0) || (token.at (0) == COMMENT_CHAR))
            {
              cerr << "Configuration error at line " << ioSectionLine << ".\n";
              return false;
            }

          if (token.at (0) == '\'')
            {
              bool pathEnded = false;
              gMainSettings.m_LogFile = token.c_str () + 1;
              for (D_UINT lineIndex = pos;
                   lineIndex < line.length ();
                   ++lineIndex)
                {
                  if (line.at (lineIndex) == '\'')
                    {
                      pathEnded = true;
                      break;
                    }
                  gMainSettings.m_LogFile.append (1, line.at (lineIndex));
                }
              if (! pathEnded)
                {
                  cerr << "Unmatched ' in configuration file at line ";
                  cerr << ioSectionLine << ".\n";
                  return false;
                }

            }
          else if (token.at (0) == '\"')
            {
              bool pathEnded = false;
              gMainSettings.m_LogFile = token.c_str () + 1;
              for (D_UINT lineIndex = pos;
                   lineIndex < line.length ();
                   ++lineIndex)
                {
                  if (line.at (lineIndex) == '\"')
                    {
                      pathEnded = true;
                      break;
                    }
                  gMainSettings.m_LogFile.append (1, line.at (lineIndex));
                }
              if (! pathEnded)
                {
                  cerr << "Unmatched \" in configuration file at line ";
                  cerr << ioSectionLine << ".\n";
                  return false;
                }
            }
          else
            gMainSettings.m_LogFile = token;
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
