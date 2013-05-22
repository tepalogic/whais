/*
 * test_dbstext.cpp
 *
 *  Created on: Oct 2, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "utils/le_converter.h"

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"
#include "dbs/dbs_values.h"

#include "../pastra/ps_varstorage.h"
#include "../pastra/ps_textstrategy.h"

using namespace whisper;
using namespace pastra;

static bool
test_nulliness()
{
  std::cout << "Testing for nulliness ... ";
  bool result = true;
  uint8_t nullUtf8[] = {0x00};

  {
    DText textSingleton;
    DText textTemporal (nullUtf8);

    if (textSingleton.IsNull() == false)
      result = false;
    else if (textSingleton.Count() != 0)
      result = false;
    else if (textSingleton.RawSize () != 0)
      result = false;
    else if (textTemporal.IsNull() == false)
      result = false;
    else if (textTemporal.Count() != 0)
      result = false;
    else if (textTemporal.RawSize () != 0)
      result = false;
    else if (textSingleton.CharAt(0).mIsNull == false)
      return false;
    else if (textTemporal.CharAt(0).mIsNull == false)
      return false;
  }

  if (result)
    {
      DText textSingleton;
      textSingleton.Append (DChar (0x42));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.Count() != 1)
        result = false;
      else if (textSingleton.RawSize () != 1)
        result = false;
      else if (textSingleton.CharAt(0).mIsNull != false)
        result = false;
      else if (textSingleton.CharAt(0).mValue != 0x42)
        result = false;
    }

  if (result)
    {
      DText textTemporal (nullUtf8);
      textTemporal.Append (DChar (0x281));

      if (textTemporal.IsNull() != false)
        result = false;
      else if (textTemporal.Count() != 1)
        result = false;
      else if (textTemporal.RawSize () != 2)
        result = false;
      else if (textTemporal.CharAt(0).mIsNull != false)
        result = false;
      else if (textTemporal.CharAt(0).mValue != 0x281)
        result = false;
    }

  if (result)
    {
      DText textSingleton;
      textSingleton.Append (DChar (0x942));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.Count() != 1)
        result = false;
      else if (textSingleton.RawSize () != 3)
        result = false;
      else if (textSingleton.CharAt(0).mIsNull != false)
        result = false;
      else if (textSingleton.CharAt(0).mValue != 0x942)
        result = false;
    }

  if (result)
    {
      DText textSingleton;
      textSingleton.Append (DChar (0x10942));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.Count() != 1)
        result = false;
      else if (textSingleton.RawSize () != 4)
        result = false;
      else if (textSingleton.CharAt(0).mIsNull != false)
        result = false;
      else if (textSingleton.CharAt(0).mValue != 0x10942)
        result = false;
    }
  if (result)
    {
      DText textSingleton;
      textSingleton.Append (DChar (0x3010942));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.Count() != 1)
        result = false;
      else if (textSingleton.RawSize () != 5)
        result = false;
      else if (textSingleton.CharAt(0).mIsNull != false)
        result = false;
      else if (textSingleton.CharAt(0).mValue != 0x3010942)
        result = false;
    }

  if (result)
    {
      std::string temp_file_base = DBSGetSeettings ().mWorkDir;
      temp_file_base += "t_cont_1";

      VariableSizeStore storage;
      storage.Init(temp_file_base.c_str(), 0, 713);
      storage.RegisterReference ();
      storage.MarkForRemoval();

      uint64_t allocated_entry = storage.AddRecord (NULL, 0);

      DText textVarRaw  (*(new RowFieldText(storage, allocated_entry, 0)));

      if (textVarRaw.IsNull() == false)
        return false;

      textVarRaw.Append (DChar (0x71201200));

      if (textVarRaw.IsNull() != false)
        result = false;
      else if (textVarRaw.Count() != 1)
        result = false;
      else if (textVarRaw.RawSize () != 6)
        result = false;
      else if (textVarRaw.CharAt(0).mIsNull != false)
        result = false;
      else if (textVarRaw.CharAt(0).mValue != 0x71201200)
        result = false;
      else if (textVarRaw.CharAt(1).mIsNull == false)
        result = false;

      storage.Flush ();
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
test_text_append ()
{
  std::cout << "Testing for text appending ... ";
  bool result = true;

  if (result)
    {
      DText destinationText;
      const DText originalText (_RC(const uint8_t*, pOriginalText));
      const uint_t charsCount = sizeof (charValues) / sizeof (uint32_t);

      if (originalText.Count() != charsCount)
        result = false;
      else
        {
          destinationText.Append (originalText);
          if (destinationText.Count() != originalText.Count())
            result = false;
          else if (destinationText.RawSize () != originalText.RawSize ())
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
      std::string temp_file_base = DBSGetSeettings ().mWorkDir;;
      temp_file_base += "ps_t_text";

      uint64_t allocated_entry = 0;

      const DText originalText (_RC(const uint8_t*, pOriginalText));
      const uint_t charsCount = sizeof (charValues) / sizeof (uint32_t);

      {
        uint8_t tempBuff[1024] = {0, };
        strcpy (_RC (char*, tempBuff) + 12, pOriginalText);
        store_le_int32 ((sizeof charValues / sizeof (uint32_t)), tempBuff);

        VariableSizeStore storage;
        storage.Init(temp_file_base.c_str(), 0, 713);
        storage.RegisterReference ();

        allocated_entry = storage.AddRecord (
                                  tempBuff,
                                  (sizeof charValues / sizeof (uint32_t)) + 12
                                            );

        storage.Flush ();
      }

      if (originalText.Count() != charsCount)
        result = false;
      else
        {
          VariableSizeStore storage;

          storage.Init(temp_file_base.c_str(), 1, 713);
          storage.RegisterReference ();
          storage.MarkForRemoval();

          {
            DText destinationText (
                      *(new RowFieldText(storage,
                                        allocated_entry,
                                        originalText.RawSize () + 12))
                                    );

            if (destinationText.Count() != originalText.Count())
              result = false;
            else if (destinationText.RawSize () != originalText.RawSize ())
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
          storage.Flush ();
        }
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

static bool
test_character_insertion ()
{
  std::cout << "Testing for text insertion ... ";
  bool result = true;

  if (result)
    {
      DText originalText (_RC(const uint8_t*, pOriginalText));
      const uint_t charsCount = sizeof (charValues) / sizeof (uint32_t);

      if (originalText.Count() != charsCount)
        result = false;
      else
        {
          DChar test_char = DChar ('A');
          originalText.CharAt (0, test_char);

          if (originalText.Count() != charsCount)
            result = false;
          else if (originalText.CharAt(0).mValue != 'A')
            result = false;

          if (result)
            {
              DChar test_char = DChar(0x3412);
              originalText.CharAt (charsCount - 1, test_char);

              if (originalText.Count() != charsCount)
                result = false;
              else if (originalText.CharAt(charsCount - 1).mValue != 0x3412)
                result = false;
            }

          if (result)
            {
              std::string temp_file_base = DBSGetSeettings ().mWorkDir;
              temp_file_base += "ps_t_text";
              uint64_t allocated_entry = 0;

              VariableSizeStore storage;
              storage.Init(temp_file_base.c_str(), 0, 713);
              storage.RegisterReference ();
              storage.MarkForRemoval();

               uint8_t tempBuff[1024] = {0, };
              strcpy (_RC (char*, tempBuff) + 12, pOriginalText);
              store_le_int32 ((sizeof charValues / sizeof (uint32_t)), tempBuff);

              allocated_entry = storage.AddRecord (
                                  tempBuff,
                                  (sizeof charValues / sizeof (uint32_t)) + 12
                                                  );

              DText originalText (
                    *(new RowFieldText(
                              storage,
                              allocated_entry,
                              sizeof (charValues) / sizeof (uint32_t) + 12
                                      ))
                                   );

              DChar test_char = DChar(0x211356);
              originalText.CharAt (charsCount / 2, test_char);

              if (originalText.Count() != charsCount)
                result = false;
              else if (originalText.CharAt(charsCount / 2).mValue != 0x211356)
                result = false;

              storage.Flush ();
            }
        }
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_text_mirroring ()
{
  std::cout << "Testing for text mirroring ... ";
  bool result = true;

  const DText arbiter (_RC(const uint8_t*, "Love is all you need!"));
  const DText arbiter2 (_RC(const uint8_t*, "B"));

  DText firstText (_RC(const uint8_t*, "A"));
  DText secondText (firstText);

  if (firstText != secondText)
    result = false;

  secondText = arbiter;
  if ((firstText == secondText) || (secondText != arbiter))
    result = false;

  firstText.MakeMirror (secondText);
  if (firstText != secondText)
    result = false;


  secondText.CharAt (0, DChar ('B'));
  if ((secondText != firstText) || (secondText != arbiter2))
    result = false;

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}


int
main ()
{
  bool success = true;

  DBSInit (DBSSettings ());

  success = success && test_nulliness ();
  success = success && test_text_append ();
  success = success && test_character_insertion ();
  success = success && test_text_mirroring ();

  DBSShoutdown ();

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
