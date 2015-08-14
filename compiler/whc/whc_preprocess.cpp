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
#include "utils/tokenizer.h"

#include "msglog.h"


using namespace std;
using namespace whais;


static const int TOKEN_OK             =  0;
static const int TOKEN_NFOUND         =  1;
static const int TOKEN_NO_NEWLINE     = -1;

static const uint_t errPreprocessorBase      = 50;
static const uint_t errNoIncludeId           = errPreprocessorBase        + 1;
static const uint_t errMultipleIncludes      = errNoIncludeId             + 1;
static const uint_t errTagMissingValue       = errMultipleIncludes        + 1;
static const uint_t errTagMultipleGuard      = errTagMissingValue         + 1;
static const uint_t errTooNestedInclude      = errTagMultipleGuard        + 1;
static const uint_t errIncompleteDefinition  = errTooNestedInclude        + 1;
static const uint_t errTagDefinedAlready     = errIncompleteDefinition    + 1;
static const uint_t errTagInvalidChars       = errTagDefinedAlready       + 1;
static const uint_t errTagDefinedBuildin     = errTagInvalidChars         + 1;
static const uint_t errTagDefinedCmdline     = errTagDefinedBuildin       + 1;
static const uint_t errTagDefinedSrcOffset   = errTagDefinedCmdline       + 1;

static const char cmdInclude[]     = "#@include";
static const char cmdGuard[]       = "#@guard";
static const char cmdDefined[]     = "#@define";
static const char cmdUndefine[]    = "#@undefine";

static const uint_t MAX_INCLUDED_LEVELS = 256;



static void
print_err_include_to_deep (vector<SourceCodeMark>&   codeMarks,
                           const string&             source)
{
  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size () - 1,
                 errTooNestedInclude,
                 0,
                 "Reached the maximum level of nested includes."
                 " This implementation supports a maximum of '%u' levels.",
                 MAX_INCLUDED_LEVELS);
}

static void
print_err_no_include (vector<SourceCodeMark>&   codeMarks,
                      const string&             source,
                      const vector<string>&     inclusionPaths,
                      const string&             includedFile)
{
  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size ()  - 1,
                 errPreprocessorBase,
                 0,
                 "Cannot find '%s' to include.",
                 includedFile.c_str ());

  if (inclusionPaths.size () > 1)
    {
      //Ignore the first entry as is always the current directory;
      cerr << "\tI looked for it in:\n";

      for (uint_t i = 1; i < inclusionPaths.size (); ++i)
        cerr << "\t\t"  << inclusionPaths[i].c_str () << endl;
    }
}


static void
print_err_multiple_include (vector<SourceCodeMark>&   codeMarks,
                            const string&             source,
                            const vector<string>&     foundFiles,
                            const string&             includedFile)
{
  assert (foundFiles.size () > 0);

  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size () - 1,
                 errMultipleIncludes,
                 0,
                 "There is more than one candidate for the inclusion of '%s'.",
                 includedFile.c_str ());

  cerr << "\tI found the following candidates:\n";
  for (uint_t i = 0; i < foundFiles.size (); ++i)
    cerr << "\t\t" << foundFiles[i].c_str () << endl;
}


static void
print_err_cmd_no_value (vector<SourceCodeMark>&   codeMarks,
                        const string&             source,
                        const string&             cmd)
{
  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size () - 1,
                 errTagMissingValue,
                 0,
                 "The preprocessor command '%s' is incomplete.",
                 cmd.c_str ());
}


static void
print_err_multiple_guard_entries (vector<SourceCodeMark>&   codeMarks,
                                  const string&             source,
                                  const string&             includedFile)
{
  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (&ctxt,
                 source.size (),
                 errTagMultipleGuard,
                 0,
                 "Multiple preprocessor '%s' specified in file '%s'.",
                 cmdGuard,
                 includedFile.c_str ());
}


static void
print_err_def_no_value (vector<SourceCodeMark>&   codeMarks,
                        const string&             source,
                        const string&             tag)
{
  WHC_MESSAGE_CTX ctxt (codeMarks, _RC (const char*, &source[0]));

  whc_messenger (
        &ctxt,
        source.size () - 1,
        errIncompleteDefinition,
        0,
        "Definition of tag '%s' is incomplete. No replacement provided.",
        tag.c_str ()
                );
}

static bool
add_tag_definition (vector<SourceCodeMark>&     codeMarks,
                    ostringstream&              source,
                    vector<ReplacementTag>&     tags,
                    const ReplacementTag&       tag)
{
  for (size_t c = 0; c < tag.mTagName.length (); ++c)
    {
      if (((c == 0) && isdigit (tag.mTagName[c]))
          || ! (isalnum (tag.mTagName[c]) || (tag.mTagName[c] == '_')))
        {
          const string src = source.str ();
          WHC_MESSAGE_CTX ctxt (codeMarks, src.c_str ());

          whc_messenger (&ctxt,
                         tag.mDefinitionOffset,
                         errTagInvalidChars,
                         0,
                         "The defined contains invalid characters [a-zA-Z0-9_]"
                         " or it starts with a digit.");
          return false;
        }
    }

  bool alreadyDefined = false;
  for (size_t i = 0; i < tags.size (); ++i)
    {
      if (tags[i].mTagName == tag.mTagName)
        {
          const string src = source.str ();
          WHC_MESSAGE_CTX ctxt (codeMarks, src.c_str ());

          if ( ! alreadyDefined)
            {
              whc_messenger (&ctxt,
                             tag.mDefinitionOffset,
                             errTagDefinedAlready,
                             0,
                             "Redefinition of tag '%s'.",
                             tag.mTagName.c_str ());
              alreadyDefined = true;
            }

          switch (tags[i].mDefinitionOffset)
          {
          case ReplacementTag::BUILDIN_OFF:
            whc_messenger (
                  &ctxt,
                  WHC_IGNORE_BUFFER_POS,
                  errTagDefinedBuildin,
                  0,
                  "The compiler has already a tag definition for '%s' build in.",
                  tag.mTagName.c_str ()
                          );
            break;

          case ReplacementTag::CMDLINE_OFF:
            whc_messenger (
                  &ctxt,
                  WHC_IGNORE_BUFFER_POS,
                  errTagDefinedCmdline,
                  0,
                  "The tag '%s' has been previously defined at command line.",
                  tag.mTagName.c_str ()
                          );
            break;

          default:
            whc_messenger (&ctxt,
                           tags[i].mDefinitionOffset,
                           errTagDefinedSrcOffset,
                           0,
                           "The tag '%s' previously here.",
                           tags[i].mTagName.c_str ());
          }
        }
    }

  if ( ! alreadyDefined)
    tags.push_back (tag);

  return alreadyDefined == false;
}


static void
add_file_to_deps (const string&         file,
                  vector<string>&       usedFiles)
{
  for (size_t i = 0; i < usedFiles.size (); ++i)
    {
      if (usedFiles[i] == file)
        return ;
    }

  usedFiles.push_back (file);
}


static string
get_tag_value (const string& line, const char* const tag)
{
  assert (strstr (line.c_str (), tag) != NULL);

  size_t from = strstr (line.c_str (), tag) - line.c_str () + strlen (tag);

  assert (from <= line.size ());

  while (from < line.size ())
    {
      if (line[from] == '\n' || line[from]=='\r')
        return string ();

      else if ( ! isspace (line[from]))
        break;

      ++from;
    }

  if (line[from] == '#')
    return string ();

  size_t size = 0;
  while (from + size < line.size ())
    {
      if ((line[from + size] == '#') || isspace (line[from + size]))
        break;

      ++size;
    }
  return line.substr (from, size);
}


static void
process_line_tags (const string&                     file,
                   const uint_t                      lineIndex,
                   vector<ReplacementTag>&           tagPairs,
                   string&                           line)
{
  size_t inoutOff = 0, lastOff = 0;

  string token, newLine, tags;
  while ( ! (token = NextToken(line, inoutOff, "\t ")).empty ())
    {
      do {
          const char ch = token[0];
          if ( ! (isalpha (ch)
                  || (ch == '_') || (ch == '#' )
                  || (ch == '\'') || (ch == '\"')))
            {
              token.erase (0, 1);
            }

          else
            break;
      }
      while (! token.empty ());

      newLine += line.substr(lastOff, inoutOff - lastOff - token.length ());
      lastOff = inoutOff;
      if (token.empty ())
        continue;

      do
        {
          const char ch = token[token.length ()  - 1];
          if ( ! (isalpha (ch)
                  || (ch == '_') || (ch == '#' )
                  || (ch == '\'') || (ch == '\"')))
            {
              lastOff--;
              token.erase (token.length () - 1);
            }

          else
            break;
        }
      while ( ! token.empty ());

      if (token.empty ())
        {
          newLine += line.substr (lastOff, inoutOff - lastOff);
          lastOff = inoutOff;
          continue;
        }

      else if (token[0] == '#')
        {
          newLine += token;
          break;
        }

      for (size_t i = 0; i < tagPairs.size (); ++i)
        {
          if (token == tagPairs[i].mTagName)
            {
              tags  += ' '; tags  += tagPairs[i].mTagName;
              if ((token == "_FILE_")
                  && (tagPairs[i].mDefinitionOffset ==
                      ReplacementTag::BUILDIN_OFF))
                {
                  token = '"'; token += file; token += '"';
                }
              else if ((token == "_LINE_")
                       && (tagPairs[i].mDefinitionOffset ==
                           ReplacementTag::BUILDIN_OFF))
                {
                  char temp[8];
                  snprintf(temp, sizeof temp, "%u", lineIndex);
                  token = temp;
                }
              else
                token  = tagPairs[i].mTagValue;

              break;
            }
        }
      newLine += token;
    }

  newLine += line.substr(lastOff);
  line     = newLine;
  if ( ! tags.empty ())
    {
      line += "\t#Tags applied:";
      line += tags;
    }
}


static void
search_for_header_file (const vector<string>&    inclusionPaths,
                        string&                  fileName,
                        vector<string>&          foundFiles)
{
  foundFiles.clear ();

  for (uint_t i = 0; i < inclusionPaths.size (); ++i)
    {
      string file (inclusionPaths[i] + fileName);

      NormalizeFilePath (file, false);

      if (whf_file_exists (file.c_str ()))
        foundFiles.push_back (file);
    }
}


static bool
check_preprocess_tag (const char* line,
                      const char* tag,
                      const char* occurence)
{
  assert (occurence == strstr (line, tag));

  for (const char* p = line; p < occurence; ++p)
    {
      if ( ! isspace (*p))
        return false;
    }

  occurence += strlen (tag);
  if ( ! isspace (*occurence))
    return false;

  return true;
}

static bool
preprocess_directives (const string&                    file,
                       const vector<string>&            inclusionPaths,
                       vector<string>&                  includedGuards,
                       vector<ReplacementTag>&          tagPairs,
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

  codeMarks.push_back (SourceCodeMark (sourceCode.tellp (),
                                       lineIndex,
                                       file,
                                       levelSize));
  while ( includedSource.good ())
    {
      const char* occurence;
      string line;

      getline (includedSource, line);
      process_line_tags (file, lineIndex, tagPairs, line);

      if (((occurence = strstr (line.c_str (), cmdDefined)) != NULL)
          && check_preprocess_tag (line.c_str(), cmdDefined, occurence))
        {
          size_t inoutOffset = 0;
          string token = NextToken (line, inoutOffset, "\t ");

          assert (token == cmdDefined);

          string tag = NextToken (line, inoutOffset, "\t ");
          if (tag.empty ())
            {
              print_err_cmd_no_value (codeMarks, sourceCode.str (), cmdDefined);
              return false;
            }

          string value;
          do
            {
              token = NextToken (line, inoutOffset, "\t ");
              if ((token[0] == '#') || token.empty ())
                break;

              if ( ! value.empty ())
                value += ' ';

              value += token;
            }
          while ( ! token.empty ());

          if (value.empty ())
            {
              print_err_def_no_value (codeMarks, sourceCode.str (), tag);
              return false;
            }

          if ( ! add_tag_definition (
                          codeMarks,
                          sourceCode,
                          tagPairs,
                          ReplacementTag (tag, value, sourceCode.tellp ()))
                                    )
            {
              return false;
            }

          sourceCode << line << endl;
          ++lineIndex;
        }
      else if (((occurence = strstr (line.c_str (), cmdUndefine)) != NULL)
               && check_preprocess_tag (line.c_str(), cmdUndefine, occurence))
        {
          string tags;
          size_t inoutOffset = (occurence - line.c_str ())
                               + strlen (cmdUndefine);
          while (true)
            {
              string token = NextToken (line, inoutOffset, "\t ");
              if (token.empty ())
                break;

              tags += ' '; tags += token;
              for (vector<ReplacementTag>::iterator it = tagPairs.begin ();
                   it != tagPairs.end ();
                   ++it)
                {
                  if (it->mTagName == token)
                    {
                      tagPairs.erase (it);
                      break;
                    }
                }
            }

          sourceCode << line << endl;
          ++lineIndex;
        }
      else if (((occurence = strstr (line.c_str (), cmdInclude)) != NULL)
               && check_preprocess_tag (line.c_str(), cmdInclude, occurence))
        {
          string includeName = get_tag_value (line, cmdInclude);
          if (includeName.empty())
            {
              print_err_cmd_no_value (codeMarks, sourceCode.str(), cmdInclude);
              return false;
            }

          vector<string> foundFiles;
          search_for_header_file(inclusionPaths, includeName, foundFiles);

          if (foundFiles.size () > 1)
            {
              print_err_multiple_include (codeMarks,
                                          sourceCode.str (),
                                          foundFiles,
                                          includeName);
              return false;
            }
          else if (foundFiles.size () == 0)
            {
              print_err_no_include (codeMarks,
                                    sourceCode.str (),
                                    inclusionPaths,
                                    includeName);
              return false;
            }

          File includedFile (foundFiles[0].c_str (), WH_FILEREAD);
          string includeContent;

          includeContent.resize (includedFile.Size (), ' ');

          includedFile.Read (_CC (uint8_t*,
                                  _RC (const uint8_t*,
                                       includeContent.c_str ())),
                             includeContent.size ());

          const char* const guard = strstr (includeContent.c_str (),
                                            cmdGuard);

          string guardValue;
          bool ignoreInclusion = false;
          if (guard != NULL)
            {
              if (strstr (guard + strlen (cmdGuard),
                          cmdGuard) != NULL)
                {
                  print_err_multiple_guard_entries (codeMarks,
                                                    sourceCode.str (),
                                                    foundFiles[0]);
                  return 0;
                }

              guardValue = get_tag_value (guard, cmdGuard);
              if ( ! guardValue.empty ())
                {
                  for (uint_t i = 0; i < includedGuards.size (); ++i)
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
              codeMarks.push_back (SourceCodeMark (sourceCode.tellp (),
                                                   lineIndex,
                                                   file,
                                                   levelSize));
              line = string ("#Including '") +
                     foundFiles[0] + "' with guard '" + guardValue + "'.";

              sourceCode << line << endl;

              if (! guardValue.empty ())
                includedGuards.push_back (guardValue);

              istringstream includedSource (includeContent);
              if (! preprocess_directives (foundFiles[0],
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
              codeMarks.push_back (SourceCodeMark (sourceCode.tellp (),
                                                   lineIndex,
                                                   file,
                                                   levelSize));
            }
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
                   vector<ReplacementTag>&        tagPairs,
                   ostringstream&                 sourceCode,
                   vector<SourceCodeMark>&        codeMarks,
                   vector<string>&                usedFiles)
{
  usedFiles.resize (0);

  File source (sourceFile.c_str (), WH_FILEREAD);
  const uint_t fileSize = source.Size ();

  string fileContent;
  fileContent.resize (fileSize, ' ');
  source.Read (_CC (uint8_t*,
                    _RC (const uint8_t*, fileContent.c_str ())), fileSize);


  vector<string> includedGuards;
  const char* const guard = strstr (fileContent.c_str (), cmdGuard);
  if (guard != NULL)
    {
      if (strstr (guard + strlen (cmdGuard),
                  cmdGuard) != NULL)
        {
          print_err_multiple_guard_entries (codeMarks,
                                            sourceCode.str (),
                                            sourceFile);
          return 0;
        }

      const string guardValue = get_tag_value (guard, cmdGuard);
      if ( ! guardValue.empty ())
        includedGuards.push_back (guardValue);
    }

  istringstream includedSource (fileContent);

  return preprocess_directives (sourceFile,
                               inclusionPaths,
                               includedGuards,
                               tagPairs,
                               0,
                               includedSource,
                               sourceCode,
                               codeMarks,
                               usedFiles);
}

