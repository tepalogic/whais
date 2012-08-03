/*
 * test_dbstext.cpp
 *
 *  Created on: Oct 2, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "test/test_fmw.h"


#include "../include/dbs_mgr.h"
#include "../include/dbs_exception.h"
#include "../include/dbs_values.h"

#include "../pastra/ps_varstorage.h"
#include "../pastra/ps_textstrategy.h"

using namespace pastra;

static bool
test_nulliness()
{
  std::cout << "Testing for nulliness ... ";
  bool result = true;
  D_UINT8 nullUtf8[] = {0x00};

  {
    DBSText textSingleton;
    DBSText textTemporal (nullUtf8);

    if (textSingleton.IsNull() == false)
      result = false;
    else if (textSingleton.GetCharactersCount() != 0)
      result = false;
    else if (textSingleton.GetRawUtf8Count() != 0)
      result = false;
    else if (textTemporal.IsNull() == false)
      result = false;
    else if (textTemporal.GetCharactersCount() != 0)
      result = false;
    else if (textTemporal.GetRawUtf8Count() != 0)
      result = false;
    else if (textSingleton.GetCharAtIndex(0).m_IsNull == false)
      return false;
    else if (textTemporal.GetCharAtIndex(0).m_IsNull == false)
      return false;
  }

  if (result)
    {
      DBSText textSingleton;
      textSingleton.Append (DBSChar (0x42));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.GetCharactersCount() != 1)
        result = false;
      else if (textSingleton.GetRawUtf8Count() != 1)
        result = false;
      else if (textSingleton.GetCharAtIndex(0).m_IsNull != false)
        result = false;
      else if (textSingleton.GetCharAtIndex(0).m_Value != 0x42)
        result = false;
    }

  if (result)
    {
      DBSText textTemporal (nullUtf8);
      textTemporal.Append (DBSChar (0x281));

      if (textTemporal.IsNull() != false)
        result = false;
      else if (textTemporal.GetCharactersCount() != 1)
        result = false;
      else if (textTemporal.GetRawUtf8Count() != 2)
        result = false;
      else if (textTemporal.GetCharAtIndex(0).m_IsNull != false)
        result = false;
      else if (textTemporal.GetCharAtIndex(0).m_Value != 0x281)
        result = false;
    }

  if (result)
    {
      DBSText textSingleton;
      textSingleton.Append (DBSChar (0x942));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.GetCharactersCount() != 1)
        result = false;
      else if (textSingleton.GetRawUtf8Count() != 3)
        result = false;
      else if (textSingleton.GetCharAtIndex(0).m_IsNull != false)
        result = false;
      else if (textSingleton.GetCharAtIndex(0).m_Value != 0x942)
        result = false;
    }

  if (result)
    {
      DBSText textSingleton;
      textSingleton.Append (DBSChar (0x10942));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.GetCharactersCount() != 1)
        result = false;
      else if (textSingleton.GetRawUtf8Count() != 4)
        result = false;
      else if (textSingleton.GetCharAtIndex(0).m_IsNull != false)
        result = false;
      else if (textSingleton.GetCharAtIndex(0).m_Value != 0x10942)
        result = false;
    }
  if (result)
    {
      DBSText textSingleton;
      textSingleton.Append (DBSChar (0x3010942));

      if (textSingleton.IsNull() != false)
        result = false;
      else if (textSingleton.GetCharactersCount() != 1)
        result = false;
      else if (textSingleton.GetRawUtf8Count() != 5)
        result = false;
      else if (textSingleton.GetCharAtIndex(0).m_IsNull != false)
        result = false;
      else if (textSingleton.GetCharAtIndex(0).m_Value != 0x3010942)
        result = false;
    }

  if (result)
    {
      std::string temp_file_base = DBSGetWorkingDir();
      temp_file_base += "t_cont_1";

      VLVarsStore storage;
      storage.Init(temp_file_base.c_str(), 0, 713);
      storage.RegisterReference ();
      storage.MarkForRemoval();

      D_UINT64 allocated_entry = storage.AddRecord (NULL, 0);

      DBSText textVarRaw  (*(new RowFieldText(storage, allocated_entry, 0)));

      if (textVarRaw.IsNull() == false)
        return false;

      textVarRaw.Append (DBSChar (0x71201200));

      if (textVarRaw.IsNull() != false)
        result = false;
      else if (textVarRaw.GetCharactersCount() != 1)
        result = false;
      else if (textVarRaw.GetRawUtf8Count() != 6)
        result = false;
      else if (textVarRaw.GetCharAtIndex(0).m_IsNull != false)
        result = false;
      else if (textVarRaw.GetCharAtIndex(0).m_Value != 0x71201200)
        result = false;
      else if (textVarRaw.GetCharAtIndex(1).m_IsNull == false)
        result = false;

      storage.Flush ();
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}


static const D_UINT32 charValues[] = { 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x74,
                                       0x65, 0x78, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x74, 0x65, 0x73, 0x74,
                                       0x21, 0x20, 0x4c, 0x65, 0x74, 0x27, 0x73, 0x20, 0x68, 0x6f, 0x70,
                                       0x65, 0x20, 0x69, 0x74, 0x20, 0x77, 0x69, 0x6c, 0x6c, 0x20, 0x77,
                                       0x6f, 0x72, 0x6b, 0x20, 0x6a, 0x75, 0x73, 0x74, 0x20, 0x66, 0x69,
                                       0x6e, 0x65, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x77, 0x65, 0x20, 0x77,
                                       0x6f, 0x6e, 0x27, 0x74, 0x20, 0x68, 0x61, 0x76, 0x65, 0x20, 0x61,
                                       0x6e, 0x79, 0x20, 0x70, 0x72, 0x6f, 0x62, 0x6c, 0x65, 0x6d, 0x73,
                                       0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x69, 0x74, 0x2e };

static const D_CHAR* pOriginalText = "This is a text to test! Let's hope it will work just fine and we "
                                     "won't have any problems with it.";

static bool
test_text_append ()
{
  std::cout << "Testing for text appending ... ";
  bool result = true;

  if (result)
    {
      DBSText destinationText;
      const DBSText originalText (_RC(const D_UINT8*, pOriginalText));
      const D_UINT charsCount = sizeof (charValues) / sizeof (D_UINT32);

      if (originalText.GetCharactersCount() != charsCount)
        result = false;
      else
        {
          destinationText.Append (originalText);
          if (destinationText.GetCharactersCount() != originalText.GetCharactersCount())
            result = false;
          else if (destinationText.GetRawUtf8Count() != originalText.GetRawUtf8Count())
            result = false;
          else
            {
              for (D_UINT index = 0; index < charsCount; ++index)
                if ( destinationText.GetCharAtIndex(index).m_IsNull ||
                    (destinationText.GetCharAtIndex(index).m_Value != originalText.GetCharAtIndex(index).m_Value) ||
                    (destinationText.GetCharAtIndex(index).m_Value != charValues[index]))
                    result = false;

            }
        }
    }

  if (result)
    {
      std::string temp_file_base = DBSGetWorkingDir();
      temp_file_base += "ps_t_text";

      D_UINT64 allocated_entry = 0;

      const DBSText originalText (_RC(const D_UINT8*, pOriginalText));
      const D_UINT charsCount = sizeof (charValues) / sizeof (D_UINT32);

      {
        VLVarsStore storage;
        storage.Init(temp_file_base.c_str(), 0, 713);
        storage.RegisterReference ();

        allocated_entry = storage.AddRecord (_RC(const D_UINT8*, pOriginalText),
                                             (sizeof charValues / sizeof (D_UINT32)));

        storage.Flush ();
      }

      if (originalText.GetCharactersCount() != charsCount)
        result = false;
      else
        {
          VLVarsStore storage;
          storage.Init(temp_file_base.c_str(), (originalText.GetRawUtf8Count() + 713 - 1) / 713, 713);
          storage.RegisterReference ();
          storage.MarkForRemoval();

          {
            DBSText destinationText (*(new RowFieldText(storage, allocated_entry, originalText.GetRawUtf8Count())));

            if (destinationText.GetCharactersCount() != originalText.GetCharactersCount())
              result = false;
            else if (destinationText.GetRawUtf8Count() != originalText.GetRawUtf8Count())
              result = false;
            else
              {
                for (D_UINT index = 0; index < charsCount; ++index)
                  if ( destinationText.GetCharAtIndex(index).m_IsNull ||
                      (destinationText.GetCharAtIndex(index).m_Value != originalText.GetCharAtIndex(index).m_Value) ||
                      (destinationText.GetCharAtIndex(index).m_Value != charValues[index]))
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
      DBSText originalText (_RC(const D_UINT8*, pOriginalText));
      const D_UINT charsCount = sizeof (charValues) / sizeof (D_UINT32);

      if (originalText.GetCharactersCount() != charsCount)
        result = false;
      else
        {
          DBSChar test_char = DBSChar ('A');
          originalText.SetCharAtIndex (test_char, 0);

          if (originalText.GetCharactersCount() != charsCount)
            result = false;
          else if (originalText.GetCharAtIndex(0).m_Value != 'A')
            result = false;

          if (result)
            {
              DBSChar test_char = DBSChar(0x3412);
              originalText.SetCharAtIndex (test_char, charsCount - 1);

              if (originalText.GetCharactersCount() != charsCount)
                result = false;
              else if (originalText.GetCharAtIndex(charsCount - 1).m_Value != 0x3412)
                result = false;
            }

          if (result)
            {
              std::string temp_file_base = DBSGetWorkingDir();
              temp_file_base += "ps_t_text";
              D_UINT64 allocated_entry = 0;

              VLVarsStore storage;
              storage.Init(temp_file_base.c_str(), 0, 713);
              storage.RegisterReference ();

              storage.MarkForRemoval();
              allocated_entry = storage.AddRecord (_RC(const D_UINT8*, pOriginalText), sizeof (charValues) / sizeof (D_UINT32));
              DBSText originalText (*(new RowFieldText(storage, allocated_entry, sizeof (charValues) / sizeof (D_UINT32))));

              DBSChar test_char = DBSChar(0x211356);
              originalText.SetCharAtIndex (test_char, charsCount / 2);

              if (originalText.GetCharactersCount() != charsCount)
                result = false;
              else if (originalText.GetCharAtIndex(charsCount / 2).m_Value != 0x211356)
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

  const DBSText arbiter (_RC(const D_UINT8*, "Love is all you need!"));
  const DBSText arbiter2 (_RC(const D_UINT8*, "B"));

  DBSText firstText (_RC(const D_UINT8*, "A"));
  DBSText secondText (firstText);

  if (firstText != secondText)
    result = false;

  secondText = arbiter;
  if ((firstText == secondText) || (secondText != arbiter))
    result = false;

  firstText.SetMirror (secondText);
  if (firstText != secondText)
    result = false;


  secondText.SetCharAtIndex (DBSChar ('B'), 0);
  if ((secondText != firstText) || (secondText != arbiter2))
    result = false;

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}


int
main ()
{
  // VC++ allocates memory when the C++ runtime is initialized
  // We need not to test against it!
  D_UINT prealloc_mem = test_get_mem_used ();
  bool success = true;

  {
    std::string dir = ".";
    dir += whc_get_directory_delimiter ();

    DBSInit (dir.c_str (), dir.c_str ());
  }

  success = success && test_nulliness ();
  success = success && test_text_append ();
  success = success && test_character_insertion ();
  success = success && test_text_mirroring ();

  DBSShoutdown ();
  D_UINT mem_usage = test_get_mem_used () - prealloc_mem;

  if (mem_usage)
    {
      success = false;
      test_print_unfree_mem();
    }

  std::cout << "Memory peak (no prealloc): " <<
            test_get_mem_peak () - prealloc_mem << " bytes." << std::endl;
  std::cout << "Preallocated mem: " << prealloc_mem << " bytes." << std::endl;
  std::cout << "Current memory usage: " << mem_usage << " bytes." << std::endl;
  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return -1;
    }
  else
    std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}
