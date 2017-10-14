/*
 * test_dbstext.cpp
 *
 *  Created on: Oct 2, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "utils/endianness.h"
#include "utils/wrandom.h"
#include "utils/wunicode.h"
#include "utils/wutf.h"

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"
#include "dbs/dbs_values.h"

#include "../pastra/ps_varstorage.h"
#include "../pastra/ps_textstrategy.h"

using namespace whais;
using namespace pastra;

static bool
test_nulliness()
{
  std::cout << "Testing for nulliness ... "; std::cout.flush();
  bool result = true;
  uint8_t nullUtf8[] = {0x00};

  {
    DText textSingleton;
    DText textTemporal(nullUtf8);

    if (textSingleton.IsNull() == false)
      result = false;
    else if (textSingleton.Count() != 0)
      result = false;
    else if (textSingleton.RawSize() != 0)
      result = false;
    else if (textTemporal.IsNull() == false)
      result = false;
    else if (textTemporal.Count() != 0)
      result = false;
    else if (textTemporal.RawSize() != 0)
      result = false;
    else if (textSingleton.CharAt(0).mIsNull == false)
      return false;
    else if (textTemporal.CharAt(0).mIsNull == false)
      return false;
  }

  if (result)
    {
      DText textSingleton;
      textSingleton.Append(DChar(0x42));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.Count() != 1)
        result = false;
      else if (textSingleton.RawSize() != 1)
        result = false;
      else if (textSingleton.CharAt(0).mIsNull != false)
        result = false;
      else if (textSingleton.CharAt(0).mValue != 0x42)
        result = false;
    }

  if (result)
    {
      DText textTemporal(nullUtf8);
      textTemporal.Append(DChar(0x281));

      if (textTemporal.IsNull() != false)
        result = false;
      else if (textTemporal.Count() != 1)
        result = false;
      else if (textTemporal.RawSize() != 2)
        result = false;
      else if (textTemporal.CharAt(0).mIsNull != false)
        result = false;
      else if (textTemporal.CharAt(0).mValue != 0x281)
        result = false;
    }

  if (result)
    {
      DText textSingleton;
      textSingleton.Append(DChar(0x942));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.Count() != 1)
        result = false;
      else if (textSingleton.RawSize() != 3)
        result = false;
      else if (textSingleton.CharAt(0).mIsNull != false)
        result = false;
      else if (textSingleton.CharAt(0).mValue != 0x942)
        result = false;
    }

  if (result)
    {
      DText textSingleton;
      textSingleton.Append(DChar(0x10942));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.Count() != 1)
        result = false;
      else if (textSingleton.RawSize() != 4)
        result = false;
      else if (textSingleton.CharAt(0).mIsNull != false)
        result = false;
      else if (textSingleton.CharAt(0).mValue != 0x10942)
        result = false;
    }
#if 0
  if (result)
    {
      DText textSingleton;
      textSingleton.Append(DChar(0x3010942));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.Count() != 1)
        result = false;
      else if (textSingleton.RawSize() != 5)
        result = false;
      else if (textSingleton.CharAt(0).mIsNull != false)
        result = false;
      else if (textSingleton.CharAt(0).mValue != 0x3010942)
        result = false;
    }
#endif

  if (result)
    {
      std::string temp_file_base = DBSGetSeettings().mWorkDir;
      temp_file_base += "t_cont_1";

      VariableSizeStoreSPtr storage = shared_make(VariableSizeStore);
      storage->Init(temp_file_base.c_str(), 0, 713);
      storage->MarkForRemoval();

      uint64_t allocated_entry = storage->AddRecord(nullptr, 0);

      std::shared_ptr<ITextStrategy> s = shared_make(RowFieldText, storage, allocated_entry, 0);
      s->SetSelfReference(s);
      DText textVarRaw(s);

      if (textVarRaw.IsNull() == false)
        return false;

      textVarRaw.Append(DChar(0x10FFFE));

      if (textVarRaw.IsNull() != false)
        result = false;
      else if (textVarRaw.Count() != 1)
        result = false;
      else if (textVarRaw.RawSize() != 4)
        result = false;
      else if (textVarRaw.CharAt(0).mIsNull != false)
        result = false;
      else if (textVarRaw.CharAt(0).mValue != 0x10FFFE)
        result = false;
      else if (textVarRaw.CharAt(1).mIsNull == false)
        result = false;

      storage->Flush();
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}


static const uint32_t charValues[] = { 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x74,
                                       0x65, 0x78, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x74, 0x65, 0x73, 0x74,
                                       0x21, 0x20, 0x4c, 0x65, 0x74, 0x27, 0x73, 0x20, 0x68, 0x6f, 0x70,
                                       0x65, 0x20, 0x69, 0x74, 0x20, 0x77, 0x69, 0x6c, 0x6c, 0x20, 0x77,
                                       0x6f, 0x72, 0x6b, 0x20, 0x6a, 0x75, 0x73, 0x74, 0x20, 0x66, 0x69,
                                       0x6e, 0x65, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x77, 0x65, 0x20, 0x77,
                                       0x6f, 0x6e, 0x27, 0x74, 0x20, 0x68, 0x61, 0x76, 0x65, 0x20, 0x61,
                                       0x6e, 0x79, 0x20, 0x70, 0x72, 0x6f, 0x62, 0x6c, 0x65, 0x6d, 0x73,
                                       0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x69, 0x74, 0x2e };

static const char* pOriginalText = "This is a text to test! Let's hope it will work just fine and we "
                                     "won't have any problems with it.";

static bool
test_text_append()
{
  std::cout << "Testing for text appending ... "; std::cout.flush();
  bool result = true;

  if (result)
    {
      DText destinationText;
      const DText originalText(_RC(const uint8_t*, pOriginalText));
      const uint_t charsCount = sizeof(charValues) / sizeof(uint32_t);

      if (originalText.Count() != charsCount)
        result = false;
      else
        {
          destinationText.Append(originalText);
          if (destinationText.Count() != originalText.Count())
            result = false;
          else if (destinationText.RawSize() != originalText.RawSize())
            result = false;
          else
            {
              for (uint_t index = 0; index < charsCount; ++index)
                if ( destinationText.CharAt(index).mIsNull ||
                    (destinationText.CharAt(index).mValue != originalText.CharAt(index).mValue) ||
                    (destinationText.CharAt(index).mValue != charValues[index]))
                    result = false;

            }
        }
    }

  if (result)
    {
      std::string temp_file_base = DBSGetSeettings().mWorkDir;;
      temp_file_base += "ps_t_text";

      uint64_t allocated_entry = 0;

      const DText originalText(_RC(const uint8_t*, pOriginalText));
      const uint_t charsCount = sizeof(charValues) / sizeof(uint32_t);

      {
        uint8_t tempBuff[1024] = {0, };
        strcpy(_RC(char*, tempBuff) + 12, pOriginalText);
        store_le_int32((sizeof charValues / sizeof(uint32_t)), tempBuff);

        VariableSizeStore storage;
        storage.Init(temp_file_base.c_str(), 0, 713);

        allocated_entry = storage.AddRecord(
                                  tempBuff,
                                  (sizeof charValues / sizeof(uint32_t)) + 12
                                            );

        storage.Flush();
      }

      if (originalText.Count() != charsCount)
        result = false;
    else
    {
      VariableSizeStoreSPtr storage = shared_make(VariableSizeStore);

      storage->Init(temp_file_base.c_str(), 1, 713);
      storage->MarkForRemoval();

      {
        std::shared_ptr<ITextStrategy> s = shared_make(RowFieldText,
                                                       storage,
                                                       allocated_entry,
                                                       originalText.RawSize() + 12);
        s->SetSelfReference(s);
        DText destinationText(s);

        if (destinationText.Count() != originalText.Count())
          result = false;
        else if (destinationText.RawSize() != originalText.RawSize())
          result = false;
        else
        {
          for (uint_t index = 0; index < charsCount; ++index)
            if (destinationText.CharAt(index).mIsNull
                || (destinationText.CharAt(index).mValue != originalText.CharAt(index).mValue)
                || (destinationText.CharAt(index).mValue != charValues[index]))
              result = false;
        }
      }
      storage->Flush();
    }
  }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

static bool
test_character_insertion()
{
  std::cout << "Testing for text insertion ... ";
  std::cout.flush();
  bool result = true;

  if (result)
  {
    DText originalText(_RC(const uint8_t*, pOriginalText));
    const uint_t charsCount = sizeof(charValues) / sizeof(uint32_t);

    if (originalText.Count() != charsCount)
      result = false;
    else
    {
      DChar test_char = DChar('A');
      originalText.CharAt(0, test_char);

      if (originalText.Count() != charsCount)
        result = false;
      else if (originalText.CharAt(0).mValue != 'A')
        result = false;

      if (result)
      {
        DChar test_char = DChar(0x3412);
        originalText.CharAt(charsCount - 1, test_char);

        if (originalText.Count() != charsCount)
          result = false;
        else if (originalText.CharAt(charsCount - 1).mValue != 0x3412)
          result = false;
      }

      if (result)
      {
        std::string temp_file_base = DBSGetSeettings().mWorkDir;
        temp_file_base += "ps_t_text";
        uint64_t allocated_entry = 0;

        VariableSizeStoreSPtr storage = shared_make(VariableSizeStore);
        storage->Init(temp_file_base.c_str(), 0, 713);
        storage->MarkForRemoval();

        uint8_t tempBuff[1024] = { 0, };
        strcpy(_RC(char*, tempBuff) + 12, pOriginalText);
        store_le_int32((sizeof charValues / sizeof(uint32_t)), tempBuff);

        allocated_entry = storage->AddRecord(tempBuff, (sizeof charValues / sizeof(uint32_t)) + 12);
        std::shared_ptr<ITextStrategy> s = shared_make(RowFieldText,
                                                       storage,
                                                       allocated_entry,
                                                       sizeof(charValues) / sizeof(uint32_t) + 12);
        s->SetSelfReference(s);
        DText originalText(s);
        s.reset();

        DChar test_char = DChar(0x21135);
        originalText.CharAt(charsCount / 2, test_char);

        if (originalText.Count() != charsCount)
          result = false;
        else if (originalText.CharAt(charsCount / 2).mValue != 0x21135)
          result = false;

        storage->Flush();
      }
    }
  }

  std::cout << (result ? "OK" : "FALSE") << std::endl;
  return result;
}


static DText
build_random_text(const int charsCount)
{
  DText result;

  if (charsCount > 0)
    result.Append(DChar('A'));

  for (int i = 1; i < charsCount; ++i)
    {
      uint32_t cp;

      if (i % 2)
        cp = wh_rnd() % 0x10FFFE + 1;

      else
        cp = wh_rnd() % 0x80 + 1;

      if ((0xD800 <= cp) && (cp <= 0xDFFF))
        {
          --i;
          continue ;
        }

      result.Append(DChar(cp));
    }

  return result;
}


static DText
text_to_lower(const DText& src)
{
  DText result;

  for (uint64_t i = 0; i < src.Count(); ++i)
    result.Append(DChar(wh_to_lowercase(src.CharAt(i).mValue)));

  return result;
}


static bool
check_text_match(const DText&      src,
                  const DText&      pattern,
                  const uint64_t    matchIndex,
                  const bool        ignoreCase)
{
  if (matchIndex + pattern.Count() > src.Count())
    return false;

  for (uint64_t i = 0; i < pattern.Count(); ++i)
    {
      const uint32_t ch1 = src.CharAt(matchIndex + i).mValue;
      const uint32_t ch2 = pattern.CharAt(i).mValue;

      if (ignoreCase)
        {
          if (wh_to_lowercase(ch1) != wh_to_lowercase(ch2))
            return false;
        }
      else if (ch1 != ch2)
        return false;
    }

  return true;
}



static bool
test_text_limit_matches(const uint_t  patternSize,
                         const uint_t  textSize)
{
  DText nullText;
  DText pattern            = build_random_text(patternSize);
  DText lowerPattern       = text_to_lower(pattern);

  const DText text         = build_random_text(textSize);
  const DText lowerText    = text_to_lower(text);

  assert(pattern != lowerPattern);
  assert(text != lowerText);

  DText temp;

  if ((temp.FindSubstring(pattern).IsNull() == false)
      || (temp.FindSubstring(pattern, true) != DUInt64()))
    {
      return false;
    }

  temp.Append(pattern);

  if ((temp.FindSubstring(nullText).IsNull() == false)
      || (temp.FindSubstring(pattern, false, 1).IsNull() == false)
      || (temp.FindSubstring(pattern,
                              false,
                              0,
                              temp.Count() - 1).IsNull() == false)
      || (temp.FindSubstring(pattern) != DUInt64(0))
      || (temp.FindSubstring(pattern, true) != DUInt64(0))
      || (temp.FindSubstring(lowerPattern, true) != DUInt64(0))
      || (check_text_match(temp, pattern, 0, false) == false)
      || (check_text_match(temp, lowerPattern, 0, true) == false))
    {
      return false;
    }

  temp = DText();
  temp.Append(DChar(wh_rnd() % 0xFF + 1));
  temp.Append(pattern);

  if ((temp.FindSubstring(pattern, false, 1) != DUInt64(1))
      || (temp.FindSubstring(pattern,
                              false,
                              0,
                              temp.Count() - 1).IsNull() == false)
      || (temp.FindSubstring(pattern) != DUInt64(1))
      || (temp.FindSubstring(pattern, true) != DUInt64(1))
      || (temp.FindSubstring(lowerPattern, true) != DUInt64(1))
      || (check_text_match(temp, pattern, 1, false) == false)
      || (check_text_match(temp, lowerPattern, 1, true) == false))
    {
      return false;
    }

  temp = DText();
  temp.Append(text);
  temp.Append(pattern);

  if ((temp.FindSubstring(pattern) != DUInt64(text.Count()))
      || (temp.FindSubstring(pattern,
                              false,
                              0,
                              text.Count() + pattern.Count() - 1).IsNull() == false)
      || (temp.FindSubstring(pattern) != DUInt64(text.Count()))
      || (temp.FindSubstring(pattern, true) != DUInt64(text.Count()))
      || (temp.FindSubstring(lowerPattern, true) != DUInt64(text.Count())
      || (check_text_match(temp, pattern, text.Count(), false) == false)
      || (check_text_match(temp, lowerPattern, text.Count(), true) == false)))
    {
      return false;
    }

  return true;
}


static bool
test_text_substrings_matches(const uint_t  patternSize,
                              const uint_t  textSize)
{
  static const uint_t ITERATIONS = 64;

  DText pattern      = build_random_text(patternSize);
  DText lowerPattern = text_to_lower(pattern);

  assert(pattern != lowerPattern);

  std::vector<DUInt64> pattOffsets(ITERATIONS);
  std::vector<DUInt64> pattOffsetsLowerCases(ITERATIONS);

  DText temp;

  for (uint_t i = 0; i < ITERATIONS; ++i)
    {
      pattOffsets[i] = DUInt64(temp.Count());
      temp.Append(pattern);

      if (textSize > 0)
        temp.Append(build_random_text(wh_rnd() % textSize));
    }

  if ((temp.FindSubstring(pattern) != pattOffsets[0])
      || (check_text_match(temp, pattern, pattOffsets[0].mValue, false) == false))
    {
      return false;
    }
  else
    {
      for (uint_t i = 1; i < pattOffsets.size(); ++i)
        {
          const uint64_t next = pattOffsets[i -1].mValue + 1;
          if ((temp.FindSubstring(pattern, false, next) != pattOffsets[i])
              || (check_text_match(temp, pattern, pattOffsets[i].mValue, false) == false))
            {
              return false;
            }
        }
    }

  if ((temp.FindSubstring(lowerPattern, true, 1) != pattOffsets[1])
      || (check_text_match(temp, lowerPattern, pattOffsets[1].mValue, true) == false))
    {
      return false;
    }
  else
    {
      for (uint_t i = 2; i < pattOffsets.size(); ++i)
        {
          const uint64_t next = pattOffsets[i -1].mValue + 1;
          if ((temp.FindSubstring(lowerPattern, true, next) != pattOffsets[i])
              || (check_text_match(temp, lowerPattern, pattOffsets[i].mValue, true) == false))
            {
              return false;
            }
        }
    }

  temp = DText();

  for (uint_t i = 0; i < ITERATIONS; ++i)
    {
      pattOffsets[i] = DUInt64(temp.Count());
      temp.Append(pattern);

      if (textSize > 0)
        temp.Append(build_random_text(wh_rnd() % textSize));

      pattOffsetsLowerCases [i] = DUInt64(temp.Count());
      temp.Append(lowerPattern);

      if (textSize > 0)
        temp.Append(build_random_text(wh_rnd() % textSize));
    }

  if ((temp.FindSubstring(pattern) != DUInt64(pattOffsets[0]))
      || (check_text_match(temp, pattern, pattOffsets[0].mValue, false) == false))
    {
      return false;
    }
  else
    {
      for (uint_t i = 1; i < pattOffsets.size(); ++i)
        {
          const uint64_t next = pattOffsets[i -1].mValue + 1;
          if ((temp.FindSubstring(pattern, false, next) != pattOffsets[i])
              || (check_text_match(temp, pattern, pattOffsets[i].mValue, false) == false))
            {
              return false;
            }
        }
    }

  if ((temp.FindSubstring(lowerPattern, true) != pattOffsets [0])
      || (check_text_match(temp, pattern, pattOffsets[0].mValue, false) == false))
    {
      return false;
    }
  else if ((temp.FindSubstring(lowerPattern) != pattOffsetsLowerCases [0])
           || (check_text_match(temp, lowerPattern, pattOffsetsLowerCases[0].mValue, false) == false))
    {
      return false;
    }
  else
    {
      uint64_t next = pattOffsetsLowerCases[0].mValue + 1;
      for (uint_t i = 1; i < pattOffsets.size(); ++i)
        {
          if ((temp.FindSubstring(lowerPattern, true, next) != pattOffsets[i])
              || (check_text_match(temp, pattern, pattOffsets[0].mValue, false) == false))
            {
              return false;
            }

          next = pattOffsets[i].mValue + 1;

          if ((temp.FindSubstring(lowerPattern, true, next) != pattOffsetsLowerCases[i])
              || (check_text_match(temp, lowerPattern, pattOffsetsLowerCases[i].mValue, false) == false))
            {
              return false;
            }

          next = pattOffsetsLowerCases[i].mValue + 1;
        }
    }

  return true;
}



static bool
test_text_substrings_replace(const uint_t patternSize, const uint_t textSize)
{
  static const uint_t ITERATIONS = 64;

  DText pattern            = build_random_text(patternSize);
  const DText lowerPattern = text_to_lower(pattern);

  const DText newSubstr    = build_random_text(wh_rnd() % patternSize + 1);

  assert(pattern != lowerPattern);

  DText temp, temp2, temp3;

  for (uint_t i = 0; i < ITERATIONS; ++i)
  {
    temp.Append(pattern);
    temp2.Append(newSubstr);

    if ((i == 0) || ((i == ITERATIONS - 1) && (textSize == 0)))
      temp3.Append(pattern);

    else
      temp3.Append(newSubstr);

    if (textSize > 0)
    {
      const DText t = (build_random_text(wh_rnd() % textSize + 1));

      temp.Append(t);
      temp2.Append(t);
      temp3.Append(t);
    }
  }

  DText t = temp;
  if (t != temp)
    return false;

  if (temp == temp2)
    return false;

  if (temp.ReplaceSubstring( pattern, newSubstr) != temp2)
    return false;

  temp = t;
  if (temp.ReplaceSubstring( pattern, newSubstr, false, 1, temp.Count() - 1) != temp3)
    return false;

  temp = temp2 = temp3 = DText();

  for (uint_t i = 0; i < ITERATIONS; ++i)
    {
      temp.Append(pattern);
      temp2.Append(newSubstr);

      if (i == 0)
        temp3.Append(pattern);

      else
        temp3.Append(newSubstr);

      if (textSize > 0)
        {
          const DText t = (build_random_text(wh_rnd() % textSize));

          temp.Append(t);
          temp2.Append(t);
          temp3.Append(t);
        }

      temp.Append(lowerPattern);
      temp2.Append(newSubstr);
      temp3.Append(newSubstr);

      if (textSize > 0)
        {
          const DText t = (build_random_text(wh_rnd() % textSize));

          temp.Append(t);
          temp2.Append(t);
          temp3.Append(t);
        }
    }

  t = temp;
  if (temp.ReplaceSubstring(pattern, newSubstr, true) != temp2)
    return false;

  temp = t;
  if (temp.ReplaceSubstring(pattern, newSubstr, true, 1) != temp3)
    return false;

  return true;
}


static bool
test_text_substrings()
{
  std::cout << "Testing for text pattern matching ... "; std::cout.flush();
  bool result = true;

  result = result && test_text_limit_matches(19, 256);
  result = result && test_text_limit_matches(24, 4096);
  result = result && test_text_limit_matches(323, 400);
  result = result && test_text_limit_matches(543, 10231);

  result = result && test_text_substrings_matches(17, 0);
  result = result && test_text_substrings_matches(61, 3);
  result = result && test_text_substrings_matches(15, 1024);
  result = result && test_text_substrings_matches(271, 0);
  result = result && test_text_substrings_matches(567, 1024);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

static bool
test_text_substrings_2()
{
  std::cout << "Testing for text pattern replace ... "; std::cout.flush();
  bool result = true;

  result = result && test_text_substrings_replace(15, 0);
  result = result && test_text_substrings_replace(11, 3);
  result = result && test_text_substrings_replace(23, 1024);
  result = result && test_text_substrings_replace(1024, 0);
  result = result && test_text_substrings_replace(512, 1024);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}


static bool
test_text_char_offsets()
{
  std::cout << "Preparing for text character offsets ... ";
  std::cout.flush();

  bool result = true;

  const int charsCount = 30000;

  DArray  arrayChars, arrayOffsets;
  DText   text1, text2;
  for (int i = 0; i < charsCount; ++i)
    {
      uint32_t ch = wh_rnd() % (UTF_LAST_CODEPOINT - 1) + 1;
      switch(wh_rnd() % 4)
      {
        case 0:
          ch &= 0xFF;
          break;

        case 1:
          ch &= 0xFFF;
          break;

        case 2:
          ch &= 0xFFFF;
          break;
      }

      if (((UTF16_EXTRA_BYTE_MIN <= ch) && (ch <= UTF16_EXTRA_BYTE_MAX))
          || (ch == 0))
        {
          --i;
          continue;
        }
      arrayOffsets.Add(DUInt32(text1.RawSize()));
      arrayChars.Add(DChar(ch));

      text1.Append(DChar(ch));
      text2.Append(DChar(ch));
    }

  DChar   tch;
  DUInt32 toff;

  arrayChars.Get(text1.Count() - 1, tch);
  arrayOffsets.Get(text1.Count() - 1, toff);
  if ((text1.CharAt(text1.Count() - 1) != tch)
      || (text1.OffsetOfChar(text1.Count() - 1) !=  toff.mValue)
      || (text1.OffsetOfChar(text1.Count() - 1) !=  text2.OffsetOfChar(text2.Count() - 1))
      || (text2.CharAt(text2.Count() - 1) != tch))
    {
      result &= false;
    }

  arrayChars.Get(0, tch);
  arrayOffsets.Get(0, toff);
  if ((text1.CharAt(0) != tch)
      || (text1.OffsetOfChar(0) !=  toff.mValue)
      || (text1.OffsetOfChar(0) !=  text1.OffsetOfChar(0))
      || (text2.CharAt(0) != tch))
    {
      result &= false;
    }

  arrayChars.Get(text1.Count() - 1, tch);
  arrayOffsets.Get(text1.Count() - 1, toff);
  if ((text1.CharAt(text1.Count() - 1) != tch)
      || (text1.OffsetOfChar(text1.Count() - 1) !=  toff.mValue)
      || (text1.OffsetOfChar(text1.Count() - 1) !=  text2.OffsetOfChar(text2.Count() - 1))
      || (text2.CharAt(text2.Count() - 1) != tch))
    {
      result &= false;
    }

  arrayChars.Get(0, tch);
  arrayOffsets.Get(0, toff);
  if ((text1.CharAt(0) != tch)
      || (text1.OffsetOfChar(0) !=  toff.mValue)
      || (text1.OffsetOfChar(0) !=  text1.OffsetOfChar(0))
      || (text2.CharAt(0) != tch))
    {
      result &= false;
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  if (!result)
    return result;

  std::cout << "\tIn order access testing ...  ";
  std::cout.flush();
  for (int i = 0 ; (i < charsCount) && result; ++i)
    {
      arrayChars.Get(i, tch);
      arrayOffsets.Get(i, toff);
      if ((text1.CharAt(i) != tch)
          || (text1.OffsetOfChar(i) !=  toff.mValue)
          || (text1.OffsetOfChar(i) !=  text2.OffsetOfChar(i))
          || (text2.CharAt(i) != tch))
        {
          result &= false;
        }
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  if (!result)
    return result;

  std::cout << "\tReverse order access testing ...  ";
  std::cout.flush();
  for (int i = charsCount - 1 ; (0 <= i) && result; --i)
    {
      arrayChars.Get(i, tch);
      arrayOffsets.Get(i, toff);
      if ((text1.CharAt(i) != tch)
          || (text1.OffsetOfChar(i) !=  toff.mValue)
          || (text1.OffsetOfChar(i) !=  text2.OffsetOfChar(i))
          || (text2.CharAt(i) != tch))
        {
          result &= false;
        }
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  if (!result)
    return result;

  std::cout << "\tRandom order access testing ...  ";
  std::cout.flush();
  for (int i = 0 ; (i < 3 * charsCount) && result; ++i)
    {
      uint32_t offset = wh_rnd() % arrayChars.Count();

      arrayChars.Get(offset, tch);
      arrayOffsets.Get(offset, toff);
      if ((text1.CharAt(offset) != tch)
          || (text1.OffsetOfChar(offset) !=  toff.mValue)
          || (text1.OffsetOfChar(offset) !=  text2.OffsetOfChar(offset))
          || (text2.CharAt(offset) != tch))
        {
          result &= false;
        }
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

int
main()
{
  bool success = true;

  DBSInit(DBSSettings());

  success = success && test_nulliness();
  success = success && test_text_append();
  success = success && test_character_insertion();
  success = success && test_text_substrings();
  success = success && test_text_substrings_2();
  success = success && test_text_char_offsets();

  DBSShoutdown();

  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
