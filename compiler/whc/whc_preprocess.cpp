/******************************************************************************
WHAISC - A compiler for whais programs
Copyright (C) 2009  Iulian Popa

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

#include <cassert>
#include <cstring>
#include <cctype>
#include <iostream>
#include <sstream>
#include <cstdio>

#include "whc_preprocess.h"
#include "utils/wfile.h"

#include "msglog.h"


using namespace std;
using namespace whais;


static const int TOKEN_OK             =  0;
static const int TOKEN_NFOUND         =  1;
static const int TOKEN_NO_NEWLINE     = -1;

static const uint_t errPreprocessorBase      = 50;
static const uint_t errNoIncludeId           = errPreprocessorBase        + 1;
static const uint_t errMultipleIncludes      = errNoIncludeId             + 1;
static const uint_t errTagLineOffset         = errMultipleIncludes        + 1;
static const uint_t errTagMissingValue       = errTagLineOffset           + 1;
static const uint_t errTagMultipleGuard      = errTagMissingValue         + 1;
static const uint_t errTooNestedInclude      = errTagMultipleGuard        + 1;

static const char tagInclude[]      = "#@include";
static const char tagIncludeGuard[] = "#@guard";

static const uint_t MAX_INCLUDED_LEVELS = 256;



void
print_err_include_to_deep (vector<SourceCodeMark>&   codeMarks,
                           const string&             source)
{
  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size() - 1,
                 errTooNestedInclude,
                 0,
                 "Reached the maximum level of nested includes."
                 " This implementation supports a maximum of '%u' levels.",
                 MAX_INCLUDED_LEVELS);
}

void
print_err_no_include (vector<SourceCodeMark>&   codeMarks,
                      const string&             source,
                      const vector<string>&     inclusionPaths,
                      const string&             includedFile)
{
  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size()  - 1,
                 errPreprocessorBase,
                 0,
                 "Cannot find '%s' to include.",
                 includedFile.c_str ());

  if (inclusionPaths.size() > 0)
    {
      cerr << "\tI looked for it in:\n";

      for (uint_t i = 0; i < inclusionPaths.size(); ++i)
        cerr << "\t\t"  << inclusionPaths[i].c_str () << endl;
    }
  else
    cerr << "\tThere were no directories setup to search for it.\n";

}


void
print_err_multiple_include (vector<SourceCodeMark>&   codeMarks,
                            const string&             source,
                            const vector<string>&     foundFiles,
                            const string&             includedFile)
{
  assert (foundFiles.size() > 0);

  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size() - 1,
                 errMultipleIncludes,
                 0,
                 "There is more than one candidate for the inclusion of '%s'.",
                 includedFile.c_str ());

  cerr << "\tI found the following candidates:\n";
  for (uint_t i = 0; i < foundFiles.size(); ++i)
    cerr << "\t\t" << foundFiles[i].c_str () << endl;
}


void
print_err_tag_no_line_start (vector<SourceCodeMark>&   codeMarks,
                             const string&              source,
                             const string&             tag)
{
  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size() - 1,
                 errTagLineOffset,
                 0,
                 "The tag '%s' should be at the start of the line.",
                 tag.c_str ());
}


void
print_err_tag_no_value (vector<SourceCodeMark>&   codeMarks,
                        const string&             source,
                        const string&             tag)
{
  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size() - 1,
                 errTagMissingValue,
                 0,
                 "The tag '%s' has no value.",
                 tag.c_str ());
}


void
print_err_multiple_guard_entries (vector<SourceCodeMark>&   codeMarks,
                                  const string&             source,
                                  const string&             includedFile)
{
  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size(),
                 errTagMultipleGuard,
                 0,
                 "Tag '%s' defined multiple times inside included file '%s'.",
                 tagIncludeGuard,
                 includedFile.c_str ());
}


static void
add_file_to_deps (const string&         file,
                  vector<string>&       usedFiles)
{
  for (size_t i = 0; i < usedFiles.size(); ++i)
    {
      if (usedFiles[i] == file)
        return ;
    }

  usedFiles.push_back (file);
}


static string
get_tag_value (const string& line, const uint tagLength)
{
  uint_t from = tagLength;

  assert (from <= line.size());

  while (from < line.size())
    {
      if (line[from] == '\n' || line[from]=='\r')
        return string();

      else if ( ! isspace (line[from]))
        break;

      ++from;
    }

  if (line[from] == '#')
    return string();

  uint_t size = 0;
  while (from + size < line.size())
    {
      if ((line[from + size] == '#') || isspace (line[from + size]))
        break;

      ++size;
    }
  return line.substr (from, size);
}


static void
process_line (const string&                     file,
              const uint_t                      lineIndex,
              const vector<ReplacementTag>&     tagPairs,
              string&                           line)
{
  for (uint_t i = 0; i < tagPairs.size(); ++i)
    {
      const size_t offset = line.find (tagPairs[i].mTagName);

      if (offset != string::npos)
        {
          line.replace (offset,
                        tagPairs[i].mTagName.size(),
                        tagPairs[i].mTagValue);
        }
    }

  static const string fileTag ("%FILE%");
  const size_t fileOffset = line.find (fileTag);
  if (fileOffset != string::npos)
    line.replace (fileOffset, fileTag.size(), file);

  static const string lineTag ("%LINE%");
  const size_t lineOffset = line.find (lineTag);
  if (lineOffset != string::npos)
    {
      char temp[8];
      snprintf (temp, sizeof temp, "%u", lineIndex);

      line.replace (lineOffset, lineTag.size(), temp);
    }
}


static void
search_for_header_file (const vector<string>&    inclusionPaths,
                        string&                  fileName,
                        vector<string>&          foundFiles)
{
  foundFiles.clear ();

  const char* const dirDelim = whf_dir_delim();
  for (uint_t i = 0; i < fileName.size(); ++i)
    {
      if ((fileName[i] == '\\') || (fileName[i] == '/'))
        fileName[i] = dirDelim[0];
    }

  for (uint_t i = 0; i < inclusionPaths.size(); ++i)
    {
      const string file (inclusionPaths[i] + fileName);

      if (whf_file_exists (file.c_str ()))
        foundFiles.push_back (file);
    }
}


static bool
preprocess_include_directives (const string&                    file,
                               const vector<string>&            inclusionPaths,
                               vector<string>&                  includedGuards,
                               const vector<ReplacementTag>&    tagPairs,
                               const uint_t                     levelSize,
                               istringstream&                   includedSource,
                               ostringstream&                   sourceCode,
                               vector<SourceCodeMark>&          codeMarks,
                               vector<string>&                  usedFiles)

{
  if (levelSize >= MAX_INCLUDED_LEVELS)
    {
      print_err_include_to_deep (codeMarks, sourceCode.str ());
      return false;
    }

  add_file_to_deps (file, usedFiles);

  uint_t lineIndex    = 1;

  codeMarks.push_back (SourceCodeMark (sourceCode.tellp(),
                                       lineIndex,
                                       file,
                                       levelSize));
  while ( ! includedSource.eof ())
    {
      string line;
      getline (includedSource, line);

      process_line (file, lineIndex, tagPairs, line);

      if (strncmp (tagInclude, line.c_str (), sizeof tagInclude - 1) == 0)
        {
          string includeName = get_tag_value (line, strlen (tagInclude));

          if (includeName.size() == 0)
            {
              print_err_tag_no_value (codeMarks,
                                      sourceCode.str (),
                                      tagInclude);
              return false;
            }

          vector<string> foundFiles;
          search_for_header_file (inclusionPaths, includeName, foundFiles);

          if (foundFiles.size() > 1)
            {
              print_err_multiple_include (codeMarks,
                                          sourceCode.str (),
                                          foundFiles,
                                          includeName);
              return false;
            }
          else if (foundFiles.size() == 0)
            {
              print_err_no_include (codeMarks,
                                    sourceCode.str (),
                                    inclusionPaths,
                                    includeName);
              return false;
            }

          File includedFile (foundFiles[0].c_str (), WH_FILEREAD);
          string includeContent;

          includeContent.resize (includedFile.Size(), ' ');

          includedFile.Read (_CC (uint8_t*,
                                  _RC (const uint8_t*,
                                       includeContent.c_str ())),
                             includeContent.size());

          const char* const guard = strstr (includeContent.c_str (),
                                            tagIncludeGuard);

          string guardValue;
          bool ignoreInclusion = false;
          if (guard != NULL)
            {
              if (strstr (guard + strlen (tagIncludeGuard),
                          tagIncludeGuard) != NULL)
                {
                  print_err_multiple_guard_entries (codeMarks,
                                                    sourceCode.str (),
                                                    foundFiles[0]);
                  return 0;
                }

              guardValue = get_tag_value (guard, strlen (tagIncludeGuard));
              if (guardValue.size() > 0)
                {
                  for (uint i = 0; i < includedGuards.size(); ++i)
                    {
                      if (includedGuards[i] == guardValue)
                        {
                          ignoreInclusion = true;
                          break;
                        }
                    }
                }
            }

          if (ignoreInclusion)
            {
              line = string ("#Ignoring include of '") +
                     foundFiles[0] + "' due to guard '" + guardValue + "'.";

              sourceCode << line << endl;

              ++lineIndex;
            }
          else
            {
              codeMarks.push_back (SourceCodeMark (sourceCode.tellp(),
                                                   lineIndex,
                                                   file,
                                                   levelSize));
              line = string ("#Including '") +
                     foundFiles[0] + "' with guard '" + guardValue + "'.";

              sourceCode << line << endl;

              if (guardValue.size() > 0)
                includedGuards.push_back (guardValue);

              istringstream includedSource (includeContent);
              if (! preprocess_include_directives (foundFiles[0],
                                                   inclusionPaths,
                                                   includedGuards,
                                                   tagPairs,
                                                   levelSize + 1,
                                                   includedSource,
                                                   sourceCode,
                                                   codeMarks,
                                                   usedFiles))
                {
                  return false;
                }

              line = string ("#Finished including '") +
                     foundFiles[0] + "' with guard '" + guardValue + "'.";

              sourceCode << line << endl;

              ++lineIndex;

              codeMarks.push_back (SourceCodeMark (sourceCode.tellp(),
                                                  lineIndex,
                                                  file,
                                                  levelSize));
            }
        }
      else if (strstr (line.c_str (), tagInclude) != NULL)
        {
          sourceCode << line; //Added to properly compute the line number.

          print_err_tag_no_line_start (codeMarks,
                                       sourceCode.str (),
                                       tagInclude);
          return false;
        }
      else
        {
          sourceCode << line << endl;
          ++lineIndex;
        }

    }

  return true;
}


bool
preprocess_source (const string&                  sourceFile,
                   const vector<string>&          inclusionPaths,
                   const vector<ReplacementTag>&  tagPairs,
                   ostringstream&                 sourceCode,
                   vector<SourceCodeMark>&        codeMarks,
                   vector<string>&                usedFiles)
{
  usedFiles.resize (0);

  File source (sourceFile.c_str (), WH_FILEREAD);
  const uint_t fileSize = source.Size();

  string fileContent;
  fileContent.resize (fileSize, ' ');
  source.Read (_CC (uint8_t*,
                    _RC (const uint8_t*, fileContent.c_str ())), fileSize);


  vector<string> includedGuards;
  const char* const guard = strstr (fileContent.c_str (), tagIncludeGuard);
  if (guard != NULL)
    {
      if (strstr (guard + strlen (tagIncludeGuard),
                  tagIncludeGuard) != NULL)
        {
          print_err_multiple_guard_entries (codeMarks,
                                            sourceCode.str (),
                                            sourceFile);
          return 0;
        }

      const string guardValue = get_tag_value (guard, strlen (tagIncludeGuard));
      if (guardValue.size() > 0)
        includedGuards.push_back (guardValue);

    }

  istringstream includedSource (fileContent);

  return preprocess_include_directives (sourceFile,
                                        inclusionPaths,
                                        includedGuards,
                                        tagPairs,
                                        0,
                                        includedSource,
                                        sourceCode,
                                        codeMarks,
                                        usedFiles);
  return true;
}

