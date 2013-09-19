/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whisper.
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
 *****************************************************************************/

#ifndef BASE_TEXT_H_
#define BASE_TEXT_H_

#include <cctype>

#include "whisper.h"

#include "utils/wutf.h"

#include "base_types.h"



using namespace whisper;


typedef int (*IS_FUNC) (int);


WLIB_PROC_DESCRIPTION       gIsUpper;
WLIB_PROC_DESCRIPTION       gIsLower;
WLIB_PROC_DESCRIPTION       gIsDigit;
WLIB_PROC_DESCRIPTION       gIsAlpha;
WLIB_PROC_DESCRIPTION       gIsSpace;
WLIB_PROC_DESCRIPTION       gIsPunct;
WLIB_PROC_DESCRIPTION       gIsPrint;

WLIB_PROC_DESCRIPTION       gUnicodeCP;
WLIB_PROC_DESCRIPTION       gUpperChar;
WLIB_PROC_DESCRIPTION       gLowerChar;
WLIB_PROC_DESCRIPTION       gUpperText;
WLIB_PROC_DESCRIPTION       gLowerText;

WLIB_PROC_DESCRIPTION       gTextToUtf8;
WLIB_PROC_DESCRIPTION       gTextFromUtf8;

WLIB_PROC_DESCRIPTION       gTextToUtf16;
WLIB_PROC_DESCRIPTION       gTextFromUtf16;

WLIB_PROC_DESCRIPTION       gTextToCharArray;
WLIB_PROC_DESCRIPTION       gTextFromCharArray;

WLIB_PROC_DESCRIPTION       gTextCharsCount;

WLIB_PROC_DESCRIPTION       gCharFind;
WLIB_PROC_DESCRIPTION       gTextFind;

WLIB_PROC_DESCRIPTION       gCharCompare;
WLIB_PROC_DESCRIPTION       gTextCompare;

WLIB_PROC_DESCRIPTION       gTextLike;

WLIB_PROC_DESCRIPTION       gTextSubstr;


static int
compare_chars (const int   c1,
               const int   c2,
               const bool  ignoreCase,
               const bool  alphabetically)
{
  const int lc1 = (alphabetically || ignoreCase) ? tolower (c1) : c1;
  const int lc2 = (alphabetically || ignoreCase) ? tolower (c2) : c2;

  if (alphabetically && (lc1 == lc2))
    return c1 - c2;

  return lc1 - lc2;
}


static int
compare_chars (const DChar c1,
               const DChar c2,
               const bool  ignoreCase,
               const bool  alphabetically)
{
  if (c1.IsNull () && c2.IsNull ())
    return 0;

  else if (c1.IsNull ())
    return -1;

  else if (c2.IsNull ())
    return 1;

  return compare_chars (c1.mValue, c2.mValue, ignoreCase, alphabetically);
}


template <IS_FUNC F>
WLIB_STATUS
is_func_XX (SessionStack& stack, ISession&)
{
  DChar ch;

  stack[stack.Size () - 1].Operand ().GetValue (ch);
  stack.Pop (1);

  if (ch.IsNull ())
    {
      stack.Push (DBool ());
      return WOP_OK;
    }

  stack.Push (DBool (F (ch.mValue) != 0));
  return WOP_OK;
}

static WLIB_STATUS
get_char_cp (SessionStack& stack, ISession&)
{
  DChar ch;

  stack[stack.Size () - 1].Operand ().GetValue (ch);
  stack.Pop (1);

  if (ch.IsNull ())
    {
      stack.Push (DUInt32 ());
      return WOP_OK;
    }

  stack.Push (DUInt32 (ch.mValue));

  return WOP_OK;
}


static WLIB_STATUS
get_char_upper (SessionStack& stack, ISession&)
{
  DChar ch;

  stack[stack.Size () - 1].Operand ().GetValue (ch);
  stack.Pop (1);

  if (ch.IsNull ())
    {
      stack.Push (DChar ());
      return WOP_OK;
    }

  stack.Push (DChar (toupper (ch.mValue)));
  return WOP_OK;
}


static WLIB_STATUS
get_char_lower (SessionStack& stack, ISession&)
{
  DChar ch;

  stack[stack.Size () - 1].Operand ().GetValue (ch);
  stack.Pop (1);

  if (ch.IsNull ())
    {
      stack.Push (DChar ());
      return WOP_OK;
    }

  stack.Push (DChar (tolower (ch.mValue)));
  return WOP_OK;
}


static WLIB_STATUS
get_text_upper (SessionStack& stack, ISession&)
{
  DText input, result;

  stack[stack.Size () - 1].Operand ().GetValue (input);
  stack.Pop (1);

  const uint64_t inputCount = input.Count ();
  for (uint64_t i = 0; i < inputCount; ++i)
    {
      const DChar ch = input.CharAt (i);
      result.Append (DChar (toupper (ch.mValue)));
    }

  stack.Push (result);
  return WOP_OK;
}


static WLIB_STATUS
get_text_lower (SessionStack& stack, ISession&)
{
  DText input, result;

  stack[stack.Size () - 1].Operand ().GetValue (input);
  stack.Pop (1);

  const uint64_t inputCount = input.Count ();
  for (uint64_t i = 0; i < inputCount; ++i)
    {
      const DChar ch = input.CharAt (i);

      result.Append (DChar (tolower (ch.mValue)));
    }

  stack.Push (result);
  return WOP_OK;
}


static WLIB_STATUS
load_utf8_text (SessionStack& stack, ISession&)
{
  DArray      source (_SC (DBool*, NULL));
  DText       result;

  stack[stack.Size () - 1].Operand ().GetValue (source);
  stack.Pop (1);

  const uint64_t arrayCount = source.Count ();
  uint64_t       it = 0;
  while (it < arrayCount)
    {
      uint8_t temp[UTF8_MAX_BYTES_COUNT * 16];
      uint_t  tempCount;

      //Read into temporal buffer as much as we can.
      for (tempCount = 0;
           (tempCount < sizeof temp) && (it + tempCount < arrayCount);
           ++tempCount)
        {
          DUInt8 codeUnit;
          source.Get (it + tempCount, codeUnit);

          temp[tempCount] = codeUnit.mValue;
        }

      //Load the characters from the temporal buffer.
      uint_t i = 0;
      while (i < tempCount)
        {
          uint32_t ch;

          //Check if all characters' code units are in the temporal buffer.
          if (i + wh_utf8_cu_count (temp[i] >= tempCount))
            break;

          i += wh_load_utf8_cp (temp + i, &ch);
          result.Append (DChar (ch));
        }

      if (i == 0)
        return WOP_UNKNOW; //The array does not hold valid utf8 code units.

      //Count only what was used.
      it += i;
    }

    stack.Push (result);
    return WOP_OK;
}

static WLIB_STATUS
store_utf8_text (SessionStack& stack, ISession&)
{
  DText      source;
  DArray     result (_SC (DUInt8*, NULL));

  stack[stack.Size () - 1].Operand ().GetValue (source);
  stack.Pop (1);

  const uint64_t textCount = source.Count ();
  for (uint64_t c = 0; c < textCount; ++c)
    {
      const DChar ch = source.CharAt (c);

      uint8_t      temp[UTF8_MAX_BYTES_COUNT];
      const uint_t codeUnits = wh_store_utf8_cp (ch.mValue, temp);

      for (uint_t i = 0; i < codeUnits; ++i)
        result.Add (DUInt8 (temp[i]));
    }

  stack.Push (result);
  return WOP_OK;
}


static WLIB_STATUS
store_utf16_text (SessionStack& stack, ISession&)
{
  DText      source;
  DArray     result (_SC (DUInt16*, NULL));

  stack[stack.Size () - 1].Operand ().GetValue (source);
  stack.Pop (1);

  const uint64_t textCount = source.Count ();
  for (uint64_t c = 0; c < textCount; ++c)
    {
      const DChar ch = source.CharAt (c);

      uint16_t     temp[2];
      const uint_t codeUnits = wh_store_utf16_cp (ch.mValue, temp);

      for (uint_t i = 0; i < codeUnits; ++i)
        result.Add (DUInt16 (temp[i]));
    }

  stack.Push (result);
  return WOP_OK;
}


static WLIB_STATUS
load_utf16_text (SessionStack& stack, ISession&)
{
  DArray      source (_SC (DUInt16*, NULL));
  DText       result;

  stack[stack.Size () - 1].Operand ().GetValue (source);
  stack.Pop (1);

  const uint64_t arrayCount = source.Count ();
  uint64_t       it = 0;
  while (it < arrayCount)
    {
      uint16_t temp[16];
      uint_t   tempCount;

      //Read into temporal buffer as much as we can.
      for (tempCount = 0;
           (tempCount < (sizeof temp / sizeof (temp[0]))) &&
            (it + tempCount < arrayCount);
           ++tempCount)
        {
          DUInt16 codeUnit;
          source.Get (it + tempCount, codeUnit);

          temp[tempCount] = codeUnit.mValue;
        }

      //Load the characters from the temporal buffer.
      uint_t i = 0;
      while (i < tempCount)
        {
          uint32_t ch;

          //Check if all characters' code units are in the temporal buffer.
          if (i + wh_utf16_cu_count (temp[i] >= tempCount))
            break;

          i += wh_load_utf16_cp (temp + i, &ch);
          result.Append (DChar (ch));
        }

      if (i == 0)
        return WOP_UNKNOW; //The array does not hold valid utf8 code units.

      //Count only what was used.
      it += i;
    }

    stack.Push (result);
    return WOP_OK;
}


static WLIB_STATUS
store_achar_text (SessionStack& stack, ISession&)
{
  DText      source;
  DArray     result (_SC (DChar*, NULL));

  stack[stack.Size () - 1].Operand ().GetValue (source);
  stack.Pop (1);

  const uint64_t textCount = source.Count ();
  for (uint64_t c = 0; c < textCount; ++c)
    result.Add (source.CharAt (c));

  stack.Push (result);
  return WOP_OK;
}


static WLIB_STATUS
load_achar_text (SessionStack& stack, ISession&)
{
  DArray      source (_SC (DChar*, NULL));
  DText       result;

  stack[stack.Size () - 1].Operand ().GetValue (source);
  stack.Pop (1);

  const uint64_t arrayCount = source.Count ();
  for (uint64_t i = 0; i < arrayCount; ++i)
    {
      DChar ch;

      source.Get (i, ch);
      result.Append (ch);
    }

    stack.Push (result);
    return WOP_OK;
}


static WLIB_STATUS
get_chars_count (SessionStack& stack, ISession&)
{
  DText text;

  stack[stack.Size () - 1].Operand ().GetValue (text);
  stack.Pop (1);

  if (text.IsNull ())
    {
      stack.Push (DUInt64 ());
      return WOP_OK;
    }

  stack.Push (DUInt64 (text.Count ()));
  return WOP_OK;
}


WLIB_STATUS
base_text_init ()
{
  static const uint8_t* isFamilyLocals[] = { gBoolType, gCharType };

  gIsUpper.name              = "is_upper";
  gIsUpper.localsCount       = 2;
  gIsUpper.localsTypes       = isFamilyLocals;
  gIsUpper.code              = is_func_XX<isupper>;

  gIsLower.name              = "is_lower";
  gIsLower.localsCount       = 2;
  gIsLower.localsTypes       = isFamilyLocals;
  gIsLower.code              = is_func_XX<islower>;

  gIsDigit.name              = "is_digit";
  gIsDigit.localsCount       = 2;
  gIsDigit.localsTypes       = isFamilyLocals;
  gIsDigit.code              = is_func_XX<isdigit>;

  gIsAlpha.name              = "is_alpha";
  gIsAlpha.localsCount       = 2;
  gIsAlpha.localsTypes       = isFamilyLocals;
  gIsAlpha.code              = is_func_XX<isalpha>;

  gIsSpace.name              = "is_space";
  gIsSpace.localsCount       = 2;
  gIsSpace.localsTypes       = isFamilyLocals;
  gIsSpace.code              = is_func_XX<isspace>;

  gIsPunct.name              = "is_punct";
  gIsPunct.localsCount       = 2;
  gIsPunct.localsTypes       = isFamilyLocals;
  gIsPunct.code              = is_func_XX<ispunct>;

  gIsPrint.name              = "is_print";
  gIsPrint.localsCount       = 2;
  gIsPrint.localsTypes       = isFamilyLocals;
  gIsPrint.code              = is_func_XX<isprint>;

  static const uint8_t* unicodeConvLocal[] = { gUInt32Type, gCharType };

  gUnicodeCP.name            = "unicode_cp";
  gUnicodeCP.localsCount     = 2;
  gUnicodeCP.localsTypes     = unicodeConvLocal;
  gUnicodeCP.code            = get_char_cp;

  static const uint8_t* charConvLocals[] = { gCharType, gCharType };

  gUpperChar.name            = "to_upper_char";
  gUpperChar.localsCount     = 2;
  gUpperChar.localsTypes     = charConvLocals;
  gUpperChar.code            = get_char_upper;

  gLowerChar.name            = "to_lower_char";
  gLowerChar.localsCount     = 2;
  gLowerChar.localsTypes     = charConvLocals;
  gLowerChar.code            = get_char_lower;

  static const uint8_t* textLocals[] = { gTextType, gTextType };

  gUpperText.name            = "to_upper_text";
  gUpperText.localsCount     = 2;
  gUpperText.localsTypes     = textLocals;
  gUpperText.code            = get_text_upper;

  gLowerText.name            = "to_lower_text";
  gLowerText.localsCount     = 2;
  gLowerText.localsTypes     = textLocals;
  gLowerText.code            = get_text_lower;

  static const uint8_t* textToUtf8Locals[] = { gAInt8Type, gTextType };

  gTextToUtf8.name           = "to_utf8";
  gTextToUtf8.localsCount    = 2;
  gTextToUtf8.localsTypes    = textToUtf8Locals;
  gTextToUtf8.code           = store_utf8_text;

  static const uint8_t* textLoadUtf8Locals[] = { gTextType, gAInt8Type };

  gTextFromUtf8.name         = "from_utf8";
  gTextFromUtf8.localsCount  = 2;
  gTextFromUtf8.localsTypes  = textLoadUtf8Locals;
  gTextFromUtf8.code         = load_utf8_text;

  static const uint8_t* textToUtf16Locals[] = { gAInt16Type, gTextType };

  gTextToUtf16.name          = "to_utf16";
  gTextToUtf16.localsCount   = 2;
  gTextToUtf16.localsTypes   = textToUtf16Locals;
  gTextToUtf16.code          = store_utf16_text;

  static const uint8_t* textLoadUtf16Locals[] = { gTextType, gAInt16Type };

  gTextFromUtf16.name        = "from_utf16";
  gTextFromUtf16.localsCount = 2;
  gTextFromUtf16.localsTypes = textLoadUtf16Locals;
  gTextFromUtf16.code        = load_utf16_text;

  static const uint8_t* textToACharLocals[] = { gACharType, gTextType };

  gTextToUtf16.name          = "to_array";
  gTextToUtf16.localsCount   = 2;
  gTextToUtf16.localsTypes   = textToACharLocals;
  gTextToUtf16.code          = store_achar_text;

  static const uint8_t* textLoadACharLocals[] = { gTextType, gACharType };

  gTextFromUtf16.name        = "from_array";
  gTextFromUtf16.localsCount = 2;
  gTextFromUtf16.localsTypes = textLoadACharLocals;
  gTextFromUtf16.code        = load_achar_text;

  static const uint8_t* charsCountLocals[] = { gTextType };

  gTextFromUtf16.name        = "text_length";
  gTextFromUtf16.localsCount = 2;
  gTextFromUtf16.localsTypes = charsCountLocals;
  gTextFromUtf16.code        = get_chars_count;

  return WOP_OK;
}

#endif //BASE_TEXT_H_

