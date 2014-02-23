/******************************************************************************
 PASTRA - A light database one file system and more.
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

#include <assert.h>
//#include <vector>
#include <algorithm>

#include "dbs/dbs_values.h"
#include "dbs/dbs_mgr.h"
#include "utils/wrandom.h"
#include "utils/date.h"
#include "utils/wutf.h"
#include "utils/wsort.h"
#include "utils/wunicode.h"

#include "ps_textstrategy.h"
#include "ps_arraystrategy.h"
#include "ps_serializer.h"


using namespace std;
using namespace whisper;
using namespace pastra;

static const uint_t  MAX_VALUE_RAW_STORAGE = 0x20;
static const uint8_t MNTH_DAYS[]           = MNTH_DAYS_A;

static const UTF8_CU_COUNTER _cuCache;

static const uint64_t  DEFAULT_LAST_CHAR   = 0xFFFFFFFFFFFFFFFFull;


class StringMatcher
{
private:
  static const int       ALPHABET_SIZE           = 256;
  static const int       MAX_TEXT_CACHE_SIZE     = 1024;
  static const int       MAX_PATTERN_SIZE        = 255;
  static const int64_t   PATTERN_NOT_FOUND       = -1;


public:
  explicit StringMatcher (const DText& pattern);

  int64_t FindMatch (const DText&   text,
                     const uint64_t fromChar,
                     const uint64_t toChar,
                     const bool     ignoreCase);

  int64_t FindMatchRaw (const DText&   text,
                        const uint64_t fromChar,
                        const uint64_t toChar,
                        const bool     ignoreCase);

  int64_t NextMatch ();

  int64_t NextMatchRaw ();

private:
  uint_t ComparingWindowShift (uint_t position) const;

  bool SuffixesMatch () const;

  uint_t FindInCache () const;

  void CountCachedChars (const uint_t offset);

  bool FillTextCache ();

  int64_t FindSubstr ();


  const DText*        mText;
  const DText&        mPattern;

  uint8_t             mPatternSize;
  bool                mIgnoreCase;

  uint64_t            mLastChar;
  uint64_t            mCurrentChar;
  uint64_t            mCurrentRawOffset;
  uint64_t            mCacheStartPos;

  uint8_t* const      mPatternRaw;
  uint8_t* const      mTextRawCache;

  uint16_t            mAvailableCache;
  uint16_t            mCacheValid;

  uint8_t             mShiftTable[ALPHABET_SIZE];
  uint8_t             mCache[MAX_PATTERN_SIZE + MAX_TEXT_CACHE_SIZE];
};



static bool
is_valid_date (const int year, const uint_t month, const uint_t day)
{

  if (month > 12)
    return false;

  else if (day == 0)
    return false;

  if (month == 2)
    {
      const bool leapYear = is_leap_year (year);

      if (day > (leapYear ? MNTH_DAYS[1] + 1 : MNTH_DAYS[1]))
        return false;
    }
  else if (day > MNTH_DAYS [month - 1])
    return false;

  return true;
}


static bool
is_valid_datetime (const int          year,
                   const uint_t       month,
                   const uint_t       day,
                   const uint_t       hour,
                   const uint_t       min,
                   const uint_t       sec)
{
  if (is_valid_date (year, month, day) == false)
    return false;

  else if ((hour > 23) || (min > 59) || (sec > 59))
      return false;

  return true;
}


static bool
is_valid_hiresdate (const int       year,
                    const uint_t    month,
                    const uint_t    day,
                    const uint_t    hour,
                    const uint_t    min,
                    const uint_t    sec,
                    const uint_t    microsec)
{
  if (is_valid_datetime (year, month, day, hour, min, sec) == false)
    return false;

  else if (microsec > 999999)
      return false;

  return true;
}


bool
DChar::operator< (const DChar& second) const
{
  if (IsNull ())
    return second.IsNull () ?  false : true;

  else if (second.IsNull ())
    return false;

  return wh_cmp_alphabetically (mValue, second.mValue) < 0;
}


DChar
DChar::Prev () const
{
  if (mIsNull || (mValue == 1))
    return DChar ();

  return DChar (wh_prev_char (mValue));
}


DChar
DChar::Next () const
{
  if (mIsNull || (mValue == UTF_LAST_CODEPOINT))
      return DChar ();

  return DChar (wh_next_char(mValue));
}



DDate::DDate (const int32_t year, const uint8_t month, const uint8_t day)
  : mYear (year),
    mMonth (month),
    mDay (day),
    mIsNull (false)
{
  if ((mIsNull == false)
      && ! is_valid_date (year, month, day))
    {
      throw DBSException (_EXTRA (DBSException::INVALID_DATE));
    }
}


DDate
DDate::Min ()
{
  return DDate (-32768, 1, 1);
}


DDate
DDate::Max ()
{
  return DDate (32767, 12, 31);
}


DDate
DDate::Prev () const
{
  if (mIsNull || (*this == Min ()))
    return DDate ();

  uint16_t year = mYear;
  uint8_t  mnth = mMonth;
  uint8_t  day  = mDay;

  if (--day == 0)
    {
      if (--mnth == 0)
          --year, mnth = 12;

      if ((mnth == 2) && is_leap_year (year))
        day = MNTH_DAYS[mnth - 1] + 1;

      else
        day = MNTH_DAYS[mnth - 1];
    }

  return DDate (year, mnth, day);
}


DDate
DDate::Next () const
{
  if (mIsNull || (*this == Max ()))
    return DDate ();

  uint16_t year = mYear;
  uint8_t  mnth = mMonth;
  uint8_t  day  = mDay;

  uint8_t  mnthDays = MNTH_DAYS[mnth - 1];

  if ((mnth == 2) && is_leap_year (year))
    mnthDays++;

  if (++day > mnthDays)
    {
      day = 1;

      if (++mnth > 12)
          ++year, mnth = 1;
    }

  return DDate (year, mnth, day);
}



DDateTime::DDateTime (const int32_t     year,
                      const uint8_t     month,
                      const uint8_t     day,
                      const uint8_t     hour,
                      const uint8_t     minutes,
                      const uint8_t     seconds)
  : mYear (year),
    mMonth (month),
    mDay (day),
    mHour (hour),
    mMinutes (minutes),
    mSeconds (seconds),
    mIsNull (false)
{
  if ((mIsNull == false)
       && ! is_valid_datetime (year, month, day, hour, minutes, seconds))
    {
      throw DBSException (_EXTRA (DBSException::INVALID_DATETIME));
    }
}


DDateTime
DDateTime::Min ()
{
  return DDateTime (-32768, 1, 1, 0, 0, 0);
}


DDateTime
DDateTime::Max ()
{
  return DDateTime (32767, 12, 31, 23, 59, 59);
}


DDateTime
DDateTime::Prev () const
{
  if (mIsNull || (*this == Min ()))
    return DDateTime ();

  uint16_t year  = mYear;
  uint8_t  mnth  = mMonth;
  uint8_t  day   = mDay;
  uint8_t  hour  = mHour;
  uint8_t  mins  = mMinutes;
  uint8_t  secs  = mSeconds;

  if (secs-- == 0)
    {
      secs = 59;
      if (mins-- == 0)
        {
          mins = 59;
          if (hour-- == 0)
            {
              hour = 23;
              if (--day == 0)
                {
                  if (--mnth == 0)
                      --year, mnth = 12;

                  if ((mnth == 2) && is_leap_year (year))
                    day = MNTH_DAYS[mnth - 1] + 1;

                  else
                    day = MNTH_DAYS[mnth - 1];
                }
            }
        }
    }

  return DDateTime (year, mnth, day, hour, mins, secs);
}


DDateTime
DDateTime::Next () const
{
  if (mIsNull || (*this == Max ()))
    return DDateTime ();

  uint16_t year  = mYear;
  uint8_t  mnth  = mMonth;
  uint8_t  day   = mDay;
  uint8_t  hour  = mHour;
  uint8_t  mins  = mMinutes;
  uint8_t  secs  = mSeconds;

  if (++secs > 59)
    {
      secs = 0;
      if (++mins > 59)
        {
          mins = 0;
          if (++hour > 23)
            {
              hour = 0;

              uint8_t mnthDays = MNTH_DAYS[mnth - 1];
              if ((mnth == 2) && is_leap_year (year))
                mnthDays++;

              if (++day > mnthDays)
                {
                  day = 1;

                  if (++mnth > 12)
                      ++year, mnth = 1;
                }
            }
        }
    }

  return DDateTime (year, mnth, day, hour, mins, secs);
}



DHiresTime::DHiresTime (const int32_t    year,
                        const uint8_t    month,
                        const uint8_t    day,
                        const uint8_t    hour,
                        const uint8_t    minutes,
                        const uint8_t    seconds,
                        const uint32_t   microsec)
  : mMicrosec (microsec),
    mYear (year),
    mMonth (month),
    mDay (day),
    mHour (hour),
    mMinutes (minutes),
    mSeconds (seconds),
    mIsNull (false)
{
  if ((mIsNull == false)
      && ! is_valid_hiresdate (year,
                               month,
                               day,
                               hour,
                               minutes,
                               seconds,
                               microsec))
    {
      throw DBSException (_EXTRA (DBSException::INVALID_DATETIME));
    }
}


DHiresTime
DHiresTime::Min ()
{
  return DHiresTime (-32768, 1, 1, 0, 0, 0, 0);
}


DHiresTime
DHiresTime::Max ()
{
  return DHiresTime (32767, 12, 31, 23, 59, 59, 999999);
}


DHiresTime
DHiresTime::Prev () const
{
  if (mIsNull || (*this == Min ()))
    {
      return DHiresTime ();
    }

  uint16_t year  = mYear;
  uint8_t  mnth  = mMonth;
  uint8_t  day   = mDay;
  uint8_t  hour  = mHour;
  uint8_t  mins  = mMinutes;
  uint8_t  secs  = mSeconds;
  uint32_t usecs = mMicrosec;

  if (usecs-- == 0)
    {
      usecs = 999999;
      if (secs-- == 0)
        {
          secs = 59;
          if (mins-- == 0)
            {
              mins = 59;
              if (hour-- == 0)
                {
                  hour = 23;
                  if (--day == 0)
                    {
                      if (--mnth == 0)
                          --year, mnth = 12;

                      if ((mnth == 2) && is_leap_year (year))
                        day = MNTH_DAYS[mnth - 1] + 1;

                      else
                        day = MNTH_DAYS[mnth - 1];
                    }
                }
            }
        }
    }

  return DHiresTime (year, mnth, day, hour, mins, secs, usecs);
}


DHiresTime
DHiresTime::Next () const
{
  if (mIsNull || (*this == Max ()))
    return DHiresTime ();

  uint16_t year  = mYear;
  uint8_t  mnth  = mMonth;
  uint8_t  day   = mDay;
  uint8_t  hour  = mHour;
  uint8_t  mins  = mMinutes;
  uint8_t  secs  = mSeconds;
  uint32_t usecs = mMicrosec;

  if (++usecs > 999999)
    {
      usecs = 0;
      if (++secs > 59)
        {
          secs = 0;
          if (++mins > 59)
            {
              mins = 0;
              if (++hour > 23)
                {
                  hour = 0;

                  uint8_t  mnthDays = MNTH_DAYS[mnth - 1];
                  if ((mnth == 2) && is_leap_year (year))
                    mnthDays++;

                  if (++day > mnthDays)
                    {
                      day = 1;

                      if (++mnth > 12)
                          ++year, mnth = 1;
                    }
                }
            }
        }
    }

  return DHiresTime (year, mnth, day, hour, mins, secs, usecs);
}



DReal
DReal::Min ()
{
  const int64_t intPart  = 0xFFFFFF8000000000ll;
  const int64_t fracPart = -1 * (DBS_REAL_PREC - 1);

  return DReal (DBS_REAL_T (intPart, fracPart, DBS_REAL_PREC));
}


DReal
DReal::Max ()
{
  const int64_t intPart  = 0x0000007FFFFFFFFFll;
  const int64_t fracPart = (DBS_REAL_PREC - 1);

  return DReal (DBS_REAL_T (intPart, fracPart, DBS_REAL_PREC));
}


DReal
DReal::Prev () const
{
  if (mIsNull || (*this == Min ()))
    return DReal ();

  return DReal (mValue - DBS_REAL_T (0, 1, DBS_REAL_PREC));
}


DReal
DReal::Next () const
{
  if (mIsNull || (*this == Max ()))
    return DReal ();

  return DReal (mValue + DBS_REAL_T (0, 1, DBS_REAL_PREC));
}



DRichReal
DRichReal::Min ()
{
  const int64_t intPart  = 0x8000000000000000ull;
  const int64_t fracPart = -1 * (DBS_RICHREAL_PREC - 1);

  return DRichReal (DBS_RICHREAL_T (intPart, fracPart, DBS_RICHREAL_PREC));
}


DRichReal
DRichReal::Max ()
{
  const int64_t intPart  = 0x7FFFFFFFFFFFFFFFull;
  const int64_t fracPart = (DBS_RICHREAL_PREC - 1);

  return DRichReal (DBS_RICHREAL_T (intPart, fracPart, DBS_RICHREAL_PREC));
}


DRichReal
DRichReal::Prev () const
{
  if (mIsNull || (*this == Min ()))
    return DRichReal ();

  return DRichReal (mValue - DBS_RICHREAL_T (0, 1, DBS_RICHREAL_PREC));
}


DRichReal
DRichReal::Next () const
{
  if (mIsNull || (*this == Max ()))
    return DRichReal ();

  return DRichReal (mValue + DBS_RICHREAL_T (0, 1, DBS_RICHREAL_PREC));
}



DText::DText (const char* text)
  : mText (&pastra::NullText::GetSingletoneInstace ()),
    mStringMatcher (NULL)

{
  if ((text != NULL) && (text[0] != 0))
    {
      auto_ptr<ITextStrategy> strategy (
                               new TemporalText (_RC (const uint8_t*, text))
                                       );
      strategy.get ()->IncreaseReferenceCount ();

      mText = strategy.release ();

      assert (mText->ReferenceCount () == 1);
      assert (mText->ShareCount () == 0);
    }
}


DText::DText (const uint8_t *utf8Src)
  : mText (& NullText::GetSingletoneInstace()),
    mStringMatcher (NULL)

{
  if ((utf8Src != NULL) && (utf8Src[0] != 0))
    {
      auto_ptr<ITextStrategy> strategy (new TemporalText (utf8Src));
      strategy.get ()->IncreaseReferenceCount ();

      mText = strategy.release ();

      assert (mText->ReferenceCount () == 1);
      assert (mText->ShareCount () == 0);
    }
}


DText::DText (ITextStrategy& text)
  : mText (NULL),
    mStringMatcher (NULL)
{
  if (text.ShareCount () == 0)
    text.IncreaseReferenceCount ();

  else
    {
      assert (text.ReferenceCount () == 1);

      text.IncreaseShareCount ();
    }

  mText = &text;
}


DText::DText (const DText& source)
  : mText (NULL),
    mStringMatcher (NULL)
{
  if (source.mText->ShareCount() > 0)
    {
      assert (source.mText->ReferenceCount () == 1);

      auto_ptr<ITextStrategy> newText (new TemporalText(NULL));
      newText->IncreaseReferenceCount();
      newText.get()->Duplicate (*source.mText,
                                 source.mText->BytesCount());

      mText = newText.release ();

      assert (mText->ShareCount () == 0);
      assert (mText->ReferenceCount () == 1);
    }
  else
    {
      source.mText->IncreaseReferenceCount ();
      mText = source.mText;
    }

  assert (mText != NULL);
}


DText::~DText ()
{
  delete _SC (StringMatcher*, mStringMatcher);

  if (mText->ShareCount () > 0)
    mText->DecreaseShareCount ();

  else
    mText->DecreaseReferenceCount();
}


bool
DText::IsNull () const
{
  return (mText->BytesCount () == 0);
}


DText &
DText::operator= (const DText& source)
{
  if (this == &source)
    return *this;

  delete _SC (StringMatcher*, mStringMatcher);
  mStringMatcher = NULL;

  ITextStrategy* const oldText = mText;

  if (source.mText->ShareCount() > 0)
    {
      assert (source.mText->ReferenceCount () == 1);

      auto_ptr<ITextStrategy> newText (new TemporalText(NULL));
      newText->IncreaseReferenceCount();
      newText.get()->Duplicate (*source.mText,
                                 source.mText->BytesCount ());

      mText = newText.release ();

      assert (mText->ShareCount () == 0);
      assert (mText->ReferenceCount () == 1);
    }
  else
    {
      source.mText->IncreaseReferenceCount();
      mText = source.mText;
    }

  if (oldText->ShareCount () > 0)
    oldText->DecreaseShareCount ();

  else
    oldText->DecreaseReferenceCount ();

  return *this;
}


bool
DText::operator== (const DText& text) const
{
  if (mText == text.mText)
    return true;

  if (IsNull () != text.IsNull ())
    return false;

  if (IsNull () == true)
    return true;

  uint64_t textSize = RawSize ();

  assert (textSize != 0);
  assert (text.RawSize () != 0);

  if (textSize != text.RawSize ())
    return false;

  uint64_t offset = 0;
  uint8_t  first[128];
  uint8_t  second[sizeof first];

  while (textSize > 0)
  {
    const uint_t chunkSize = MIN (sizeof first, textSize);

    RawRead (offset, chunkSize, first);
    text.RawRead (offset, chunkSize, second);

    if (memcmp (first, second, chunkSize) != 0)
      return false;

    offset += chunkSize, textSize -= chunkSize;
  }

  return true;
}


uint64_t
DText::Count () const
{
  return mText->CharsCount ();
}


uint64_t
DText::RawSize () const
{
  return mText->BytesCount ();
}


void
DText::RawRead (uint64_t        offset,
                uint64_t        count,
                uint8_t* const  dest) const
{
  if (IsNull())
    return;

  count = min<uint64_t> (count, mText->BytesCount());

  mText->ReadUtf8 (offset, count, dest);
}


uint64_t
DText::OffsetOfChar (const uint64_t chIndex) const
{
  return mText->OffsetOfChar (chIndex);
}


uint64_t
DText::CharsUntilOffset (const uint64_t offset) const
{
  return mText->CharsUntilOffset (offset);
}


void
DText::AppendRaw (const DText&   text,
                  const uint64_t fromOff,
                  const uint64_t toOff)
{

  if (text.IsNull ())
    return;

  delete _SC (StringMatcher*, mStringMatcher);
  mStringMatcher = NULL;

  if (mText->ReferenceCount () > 1)
    {
      auto_ptr<ITextStrategy> newText (new TemporalText(NULL));
      newText->IncreaseReferenceCount();
      newText.get()->Duplicate (*mText, mText->BytesCount ());

      mText->DecreaseReferenceCount();
      mText = newText.release ();

      assert (mText->ShareCount () == 0);
      assert (mText->ReferenceCount () == 1);
    }

  mText->Append (*text.mText, fromOff, min (text.RawSize (), toOff));
}

void
DText::Append (const DChar& ch)
{
  if (ch.IsNull ())
    return ;

  delete _SC (StringMatcher*, mStringMatcher);
  mStringMatcher = NULL;

  if (mText->ReferenceCount() > 1)
    {
      auto_ptr<ITextStrategy> newText (new TemporalText(NULL));
      newText->IncreaseReferenceCount();
      newText.get()->Duplicate (*mText, mText->BytesCount ());

      mText->DecreaseReferenceCount();
      mText = newText.release ();

      assert (mText->ShareCount () == 0);
      assert (mText->ReferenceCount () == 1);
    }

  assert (mText->ReferenceCount() != 0);

  mText->Append (ch.mValue);
}


void
DText::Append (const DText& text)
{
  AppendRaw (text, 0, 0xFFFFFFFFFFFFFFFFull);
}


DChar
DText::CharAt(const uint64_t index) const
{
  return mText->CharAt (index);
}


void
DText::CharAt (const uint64_t index, const DChar& ch)
{
  const uint64_t charsCount = mText->CharsCount();

  delete _SC (StringMatcher*, mStringMatcher);
  mStringMatcher = NULL;

  if (charsCount == index)
    {
      Append (ch);

      return ;
    }
  else if (charsCount < index)
    throw DBSException (_EXTRA (DBSException::STRING_INDEX_TOO_BIG));

  if (ch.IsNull ())
    mText->Truncate (index);

  else
    mText->UpdateCharAt (ch.mValue, index, &mText);
}


DUInt64
DText::FindInText (const DText&      text,
                   const bool        ignoreCase,
                   const uint64_t    fromCh,
                   const uint64_t    toCh)
{
  if (IsNull ()
      || text.IsNull ()
      || (toCh <= fromCh)
      || (toCh - fromCh < Count ()))
    {
      return DUInt64 ();
    }

  if (mStringMatcher == NULL)
    mStringMatcher = new StringMatcher (*this);

  const int64_t result = _SC (StringMatcher*,
                              mStringMatcher)->FindMatch (text,
                                                          fromCh,
                                                          toCh,
                                                          ignoreCase);
  assert ((result < 0)
          || ((fromCh <= (uint64_t)result)
              && ((uint64_t)result <= toCh - Count ())));

  if (result >= 0)
    return DUInt64 (result);

  return DUInt64 ();
}


DUInt64
DText::FindSubstring (DText&            substr,
                      const bool        ignoreCase,
                      const uint64_t    fromCh,
                      const uint64_t    toCh)
{
  return substr.FindInText (*this, ignoreCase, fromCh, toCh);
}


DText
DText::ReplaceSubstr (DText&             substr,
                      const DText&       newSubstr,
                      const bool         ignoreCase,
                      const uint64_t     fromCh,
                      const uint64_t     toCh)
{
  const uint64_t substrSize = substr.RawSize ();

  DText     result;
  DUInt64   matchPos;
  DUInt64   lastMatchPos (0);

  matchPos = substr.FindInTextUTF8 (*this, ignoreCase, fromCh, toCh);

  while (! matchPos.IsNull ())
    {
      result.AppendRaw (*this, lastMatchPos.mValue, matchPos.mValue);
      result.Append (newSubstr);

      lastMatchPos = DUInt64 (matchPos.mValue + substrSize);
      assert (lastMatchPos.mValue <= RawSize ());

      matchPos = substr.FindNextUTF8 ();
    }

  if (matchPos.IsNull ())
    matchPos = DUInt64 (RawSize ());

  result.AppendRaw (*this, lastMatchPos.mValue, matchPos.mValue);

  return result;
}


DText
DText::LowerCase () const
{
  DText result = *this;

  result.AllCharsToCase (true);
  return result;
}


DText
DText::UpperCase () const
{
  DText result = *this;

  result.AllCharsToCase (false);

  return result;
}


void
DText::MakeMirror (DText& inoutText) const
{
  if (mText->ReferenceCount() == 1)
    mText->IncreaseShareCount ();

  else
    {
      auto_ptr<ITextStrategy> newText (new TemporalText (NULL));
      newText->IncreaseReferenceCount();
      newText.get()->Duplicate (*mText, mText->BytesCount ());
      mText->DecreaseReferenceCount ();

      _CC (DText*, this)->mText = newText.release ();

      assert (mText->ShareCount () == 0);
      assert (mText->ReferenceCount () == 1);

      mText->IncreaseShareCount ();
    }

  assert (mText->ReferenceCount () == 1);

  if (this != &inoutText)
    {
      if (inoutText.mText->ShareCount () > 0)
        inoutText.mText->DecreaseShareCount ();

      else
        inoutText.mText->DecreaseReferenceCount ();

      inoutText.mText = mText;
    }
}


DUInt64
DText::FindInTextUTF8 (const DText&      text,
                       const bool        ignoreCase,
                       const uint64_t    fromCh,
                       const uint64_t    toCh)
{
  if (IsNull ()
      || text.IsNull ()
      || (toCh <= fromCh)
      || (toCh - fromCh < Count ()))
    {
      return DUInt64 ();
    }

  if (mStringMatcher == NULL)
    mStringMatcher = new StringMatcher (*this);

  const int64_t result = _SC (StringMatcher*,
                              mStringMatcher)->FindMatchRaw (text,
                                                             fromCh,
                                                             toCh,
                                                             ignoreCase);
  assert ((result < 0)
          || ((fromCh <= (uint64_t)result)
              && ((uint64_t)result <= toCh - Count ())));

  if (result >= 0)
    return DUInt64 (result);

  return DUInt64 ();
}


DUInt64
DText::FindNextUTF8 () const
{
  if (mStringMatcher == NULL)
    return DUInt64 ();

  const int64_t result = _SC (StringMatcher*, mStringMatcher)->NextMatchRaw ();
  if (result >= 0)
    return DUInt64 (result);

  return DUInt64 ();
}


void
DText::AllCharsToCase (const bool lowerCase)
{
  const uint64_t charsCount = Count ();

  for (uint64_t i = 0; i < charsCount; ++i)
    {
      DChar ch = CharAt (i);

      ch = DChar (lowerCase ?
                    wh_to_lowercase (ch.mValue) :
                    wh_to_uppercase (ch.mValue)
                 );

      CharAt (i, ch);
    }
}


template <class T> void
wh_array_init (const T* const       array,
               const uint64_t       count,
               IArrayStrategy**     outStrategy)
{
  if (count == 0)
    {
      assert (array == NULL);

      *outStrategy = &NullArray::GetSingletoneInstace (array[0].DBSType ());

      return;
    }

  if (array == NULL)
    throw DBSException (_EXTRA (DBSException::BAD_PARAMETERS));

  auto_ptr<TemporalArray> autoP (new TemporalArray (array[0].DBSType ()));
  *outStrategy = autoP.get ();

  uint64_t currentOffset  = 0;
  uint_t   valueIncrement = 0;

  while (valueIncrement <
           _SC(uint_t,  Serializer::Size (array[0].DBSType (), false)))
    {
      valueIncrement += Serializer::Alignment (array[0].DBSType (), false);
    }

  for (uint64_t index = 0; index < count; ++index)
    {
      if (array[index].IsNull ())
        continue;

      uint8_t rawStorage [MAX_VALUE_RAW_STORAGE];

      assert (valueIncrement <= (sizeof rawStorage));

      Serializer::Store (rawStorage, array[index]);
      (*outStrategy)->WriteRaw(currentOffset, valueIncrement, rawStorage);
      currentOffset += valueIncrement;
    }

  (*outStrategy)->IncrementReferenceCount();

  assert ((*outStrategy)->ShareCount () == 0);
  assert ((*outStrategy)->ReferenceCount () == 1);

  autoP.release ();
}


DArray::DArray ()
  : mArray (&NullArray::GetSingletoneInstace (T_UNDETERMINED))
{
}


DArray::DArray (const DBool* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DChar* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DDate* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DDateTime* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DHiresTime* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DUInt8* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DUInt16* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DUInt32* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DUInt64* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DReal* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DRichReal* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DInt8* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DInt16* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DInt32* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (const DInt64* const array, const uint64_t count)
  : mArray (NULL)
{
  wh_array_init (array, count, &mArray);
}


DArray::DArray (IArrayStrategy& array)
  : mArray (NULL)
{
  if (array.ShareCount () == 0)
    array.IncrementReferenceCount();

  else
   array.IncrementShareCount ();

  mArray = &array;
}


DArray::DArray (const DArray& source)
  : mArray (NULL)
{
  if (source.mArray->ShareCount () == 0)
    {
      mArray = source.mArray;
      mArray->IncrementReferenceCount ();
    }
  else
    {
      assert (source.mArray->ReferenceCount() == 1);

      auto_ptr<IArrayStrategy> newStrategy (
                                    new TemporalArray (source.mArray->Type ())
                                           );

      newStrategy->Clone (*source.mArray);
      newStrategy->IncrementReferenceCount ();
      mArray = newStrategy.release ();

      assert (mArray->ShareCount() == 0);
      assert (mArray->ReferenceCount() == 1);
    }
}


DArray::~DArray ()
{
  if (mArray->ShareCount () == 0)
    mArray->DecrementReferenceCount ();

  else
    mArray->DecrementShareCount ();
}


DArray &
DArray::operator= (const DArray& source)
{
  if (&source == this)
    return *this;

  if ((Type () != T_UNDETERMINED)
      && source.Type () != Type ())
    {
      throw DBSException (_EXTRA (DBSException::INVALID_ARRAY_TYPE));
    }

  IArrayStrategy* const oldArray = mArray;

  if (source.mArray->ShareCount () == 0)
    {
      mArray = source.mArray;
      mArray->IncrementReferenceCount ();
    }
  else
    {
      assert (source.mArray->ReferenceCount() == 1);

      auto_ptr<IArrayStrategy> newStrategy (
                                    new TemporalArray (source.mArray->Type ())
                                           );

      newStrategy->Clone (*source.mArray);
      newStrategy->IncrementReferenceCount ();
      mArray = newStrategy.release ();

      assert (mArray->ShareCount() == 0);
      assert (mArray->ReferenceCount() == 1);
    }

  if (oldArray->ShareCount () == 0)
    oldArray->DecrementReferenceCount ();

  else
    oldArray->DecrementShareCount ();

  return *this;
}


uint64_t
DArray::Count () const
{
  return mArray->Count ();
}


DBS_FIELD_TYPE
DArray::Type () const
{
  return mArray->Type ();
}


static int
get_aligned_elem_size (DBS_FIELD_TYPE type)
{
  int result = 0;

  while (result < Serializer::Size(type, false))
    result += Serializer::Alignment (type, false);

  return result;
}


static void
prepare_array_strategy (IArrayStrategy** inoutStrategy)
{
  assert ((*inoutStrategy)->ReferenceCount () > 0);
  assert (((*inoutStrategy)->ShareCount () == 0)
          || ((*inoutStrategy)->ReferenceCount () == 1));

  if ((*inoutStrategy)->ReferenceCount () == 1)
    return ; //Do not change anything!

  auto_ptr<IArrayStrategy> newStrategy (
                              new TemporalArray ((*inoutStrategy)->Type ())
                                       );

  newStrategy->Clone (*(*inoutStrategy));
  (*inoutStrategy)->DecrementReferenceCount ();
  *inoutStrategy = newStrategy.release ();
  (*inoutStrategy)->IncrementReferenceCount ();

  assert ((*inoutStrategy)->ShareCount () == 0);
  assert ((*inoutStrategy)->ReferenceCount () == 1);
}


template <class T> inline uint64_t
add_array_element (const T& element, IArrayStrategy** inoutStrategy)
{
  if ((*inoutStrategy)->Type () != element.DBSType ())
    {
      if ((*inoutStrategy)->Type () != T_UNDETERMINED)
        throw DBSException (_EXTRA (DBSException::INVALID_ARRAY_TYPE));

      assert ((*inoutStrategy) ==
                &NullArray::GetSingletoneInstace (T_UNDETERMINED));

      *inoutStrategy = &NullArray::GetSingletoneInstace (element.DBSType ());

      return add_array_element (element, inoutStrategy);
    }
  else if (element.IsNull ())
    throw DBSException (_EXTRA (DBSException::NULL_ARRAY_ELEMENT));

  static const uint_t storageSize = get_aligned_elem_size (
                                                    element.DBSType ()
                                                          );
  prepare_array_strategy (inoutStrategy);

  uint8_t rawElement[MAX_VALUE_RAW_STORAGE];

  assert (storageSize <= sizeof rawElement);

  Serializer::Store (rawElement, element);
  (*inoutStrategy)->WriteRaw ((*inoutStrategy)->RawSize (),
                              storageSize,
                              rawElement);

  assert (((*inoutStrategy)->RawSize() % storageSize) == 0);

  return ((*inoutStrategy)->Count () - 1);
}


uint64_t
DArray::Add (const DBool& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DChar& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DDate& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DDateTime& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DHiresTime& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DUInt8& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DUInt16& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DUInt32& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DUInt64& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DReal& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DRichReal& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DInt8& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DInt16& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DInt32& value)
{
  return add_array_element (value, &mArray);
}


uint64_t
DArray::Add (const DInt64& value)
{
  return add_array_element (value, &mArray);
}


template <class T> void
get_array_element (IArrayStrategy&        strategy,
                   const uint64_t         index,
                   T&                     outElement)
{
  if (strategy.Count() <= index)
    throw DBSException (_EXTRA (DBSException::ARRAY_INDEX_TOO_BIG));

  else if (strategy.Type() != outElement.DBSType ())
    throw DBSException (_EXTRA(DBSException::INVALID_ARRAY_TYPE));

  static const uint_t storageSize = get_aligned_elem_size (
                                                    outElement.DBSType ()
                                                          );
  uint8_t rawElement[MAX_VALUE_RAW_STORAGE];

  assert (sizeof rawElement >= storageSize);

  strategy.ReadRaw (index * storageSize, storageSize, rawElement);
  Serializer::Load (rawElement, &outElement);
}


void
DArray::Get (const uint64_t index, DBool& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DChar& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DDate& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DDateTime& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DHiresTime& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DUInt8& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DUInt16& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DUInt32& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DUInt64& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DReal& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DRichReal& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DInt8& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DInt16& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DInt32& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


void
DArray::Get (const uint64_t index, DInt64& outValue) const
{
  get_array_element (*mArray, index, outValue);
}


template<class T>
inline void
set_array_element (const T&          value,
                   const uint64_t    index,
                   IArrayStrategy**  inoutStrategy)
{
  if (index == (*inoutStrategy)->Count ())
    {
      if (value.IsNull ())
        return;

      add_array_element (value, inoutStrategy);
      return ;
    }
  else if (index > (*inoutStrategy)->Count())
    throw DBSException(_EXTRA (DBSException::ARRAY_INDEX_TOO_BIG));

  prepare_array_strategy (inoutStrategy);
  static const uint_t storageSize = get_aligned_elem_size (
                                                    (*inoutStrategy)->Type ()
                                                          );
  if (value.IsNull ())
      (*inoutStrategy)->CollapseRaw (index * storageSize, storageSize);

  else
    {
      uint8_t rawElement[MAX_VALUE_RAW_STORAGE];

      Serializer::Store (rawElement, value);
      (*inoutStrategy)->WriteRaw (storageSize * index,
                                  storageSize,
                                  rawElement);
    }
}


void
DArray::Set (const uint64_t index, const DBool& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DChar& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DDate& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DDateTime& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DHiresTime& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index,  const DUInt8& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DUInt16& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DUInt32& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DUInt64& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DReal& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DRichReal& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DInt8& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DInt16& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DInt32& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Set (const uint64_t index, const DInt64& newValue)
{
  set_array_element (newValue, index, &mArray);
}


void
DArray::Remove (const uint64_t index)
{
  if (index >= mArray->Count ())
    throw DBSException(_EXTRA(DBSException::ARRAY_INDEX_TOO_BIG));

  prepare_array_strategy (&mArray);

  const uint_t storageSize = get_aligned_elem_size (mArray->Type());

  mArray->CollapseRaw (index * storageSize, storageSize);
}


template<typename TE>
class ArrayContainer
{
public:
  ArrayContainer (DArray& array)
    : mArray (array)
    {
    }

  const TE operator[] (const int64_t position) const
    {
      TE value;
      mArray.Get (position, value);

      return value;
    }

  void Exchange (const int64_t pos1, const int64_t pos2)
    {
      TE val1, val2;

      mArray.Get (pos1, val1);
      mArray.Get (pos2, val2);

      mArray.Set (pos2, val1);
      mArray.Set (pos1, val2);
    }

  uint64_t Count () const
    {
      return mArray. Count ();
    }

  void Pivot (const int64_t from, const int64_t to)
  {
    mArray.Get ((from + to) / 2, mPivot);
  }

  const TE& Pivot () const
  {
    return mPivot;
  }

private:
  DArray&     mArray;
  TE          mPivot;
};


void
DArray::Sort (bool reverse)
{

  const int64_t arrayCount = Count ();

  if (arrayCount == 0)
    return ;

  switch (Type ())
  {
  case T_BOOL:
      {
        ArrayContainer<DBool> temp (*this);
        quick_sort<DBool, ArrayContainer<DBool> > (0,
                                                   arrayCount - 1,
                                                   reverse,
                                                   temp);
      }
    break;

  case T_CHAR:
     {
        ArrayContainer<DChar> temp (*this);
        quick_sort<DChar, ArrayContainer<DChar> > (0,
                                                   arrayCount - 1,
                                                   reverse,
                                                   temp);
      }
    break;

  case T_DATE:
     {
        ArrayContainer<DDate> temp (*this);
        quick_sort<DDate, ArrayContainer<DDate> > (0,
                                                   arrayCount - 1,
                                                   reverse,
                                                   temp);
      }
    break;

  case T_DATETIME:
     {
        ArrayContainer<DDateTime> temp (*this);
        quick_sort<DDateTime, ArrayContainer<DDateTime> > (0,
                                                           arrayCount - 1,
                                                           reverse,
                                                           temp);
      }
    break;

  case T_HIRESTIME:
     {
        ArrayContainer<DHiresTime> temp (*this);
        quick_sort<DHiresTime, ArrayContainer<DHiresTime> > (0,
                                                             arrayCount - 1,
                                                             reverse,
                                                             temp);
      }
    break;

  case T_UINT8:
     {
        ArrayContainer<DUInt8> temp (*this);
        quick_sort<DUInt8, ArrayContainer<DUInt8> > (0,
                                                     arrayCount - 1,
                                                     reverse,
                                                     temp);
      }
    break;

  case T_UINT16:
     {
        ArrayContainer<DUInt16> temp (*this);
        quick_sort<DUInt16, ArrayContainer<DUInt16> > (0,
                                                       arrayCount - 1,
                                                       reverse,
                                                       temp);
      }
    break;

  case T_UINT32:
     {
        ArrayContainer<DUInt32> temp (*this);
        quick_sort<DUInt32, ArrayContainer<DUInt32> > (0,
                                                       arrayCount - 1,
                                                       reverse,
                                                       temp);
      }
    break;

  case T_UINT64:
     {
        ArrayContainer<DUInt64> temp (*this);
        quick_sort<DUInt64, ArrayContainer<DUInt64> > (0,
                                                       arrayCount - 1,
                                                       reverse,
                                                       temp);
      }
    break;

  case T_REAL:
     {
        ArrayContainer<DReal> temp (*this);
        quick_sort<DReal, ArrayContainer<DReal> > (0,
                                                   arrayCount - 1,
                                                   reverse,
                                                   temp);
      }
    break;

  case T_RICHREAL:
     {
        ArrayContainer<DRichReal> temp (*this);
        quick_sort<DRichReal, ArrayContainer<DRichReal> > (0,
                                                           arrayCount - 1,
                                                           reverse,
                                                           temp);
      }
    break;

  case T_INT8:
     {
        ArrayContainer<DInt8> temp (*this);
        quick_sort<DInt8, ArrayContainer<DInt8> > (0,
                                                   arrayCount - 1,
                                                   reverse,
                                                   temp);
      }
    break;

  case T_INT16:
     {
        ArrayContainer<DInt16> temp (*this);
        quick_sort<DInt16, ArrayContainer<DInt16> > (0,
                                                     arrayCount - 1,
                                                     reverse,
                                                     temp);
      }
    break;

  case T_INT32:
     {
        ArrayContainer<DInt32> temp (*this);
        quick_sort<DInt32, ArrayContainer<DInt32> > (0,
                                                     arrayCount - 1,
                                                     reverse,
                                                     temp);
      }
    break;

  case T_INT64:
     {
        ArrayContainer<DInt64> temp (*this);
        quick_sort<DInt64, ArrayContainer<DInt64> > (0,
                                                     arrayCount - 1,
                                                     reverse,
                                                     temp);
      }
    break;

  default:
    assert (false);
  }
}


void
DArray::MakeMirror (DArray& inoutArray) const
{
  if (mArray->ReferenceCount() == 1)
    mArray->IncrementShareCount ();

  else
    {
      assert (mArray->ShareCount () == 0);

      auto_ptr<IArrayStrategy> newStrategy (
                                      new TemporalArray (mArray->Type ())
                                           );

      newStrategy->Clone (*mArray);
      newStrategy->IncrementReferenceCount ();
      mArray->DecrementReferenceCount ();
      _CC (DArray*, this)->mArray = newStrategy.release ();

      assert (mArray->ShareCount() == 0);
      assert (mArray->ReferenceCount() == 1);

      mArray->IncrementShareCount ();
    }

  assert (mArray->ReferenceCount () == 1);

  if (this != &inoutArray)
    {
      if (inoutArray.mArray->ShareCount () == 0)
        inoutArray.mArray->DecrementReferenceCount ();

      else
        inoutArray.mArray->DecrementShareCount ();

      inoutArray.mArray = mArray;
    }
}



StringMatcher::StringMatcher (const DText& pattern)
  : mText (NULL),
    mPattern (pattern),
    mPatternSize (min<uint64_t> (pattern.RawSize (), MAX_PATTERN_SIZE)),
    mIgnoreCase (false),
    mLastChar (DEFAULT_LAST_CHAR),
    mCurrentChar (0),
    mCurrentRawOffset (0),
    mCacheStartPos (0),
    mPatternRaw (mCache),
    mTextRawCache (mPatternRaw + mPatternSize),
    mAvailableCache (sizeof mCache - mPatternSize),
    mCacheValid (0)
{
  assert (mPatternSize > 0);

  mPattern.RawRead (0, mPatternSize, mPatternRaw);

  if (mPatternSize < pattern.RawSize ())
    {
      uint_t chOffset = 0;
      while (chOffset < mPatternSize)
        {
          const uint32_t codeUnits = _cuCache.Count (mPatternRaw[chOffset]);

          if (chOffset + codeUnits > mPatternSize)
            mPatternSize = chOffset;

          else
            chOffset += codeUnits;
        }
      assert (chOffset == mPatternSize);
    }

  assert (mPatternSize != 0);

  memset (mShiftTable, 0, sizeof (mShiftTable));
  for (uint_t i = 0; i < mPatternSize; ++i)
    mShiftTable[mPatternRaw[i]] = i;
}


int64_t
StringMatcher::FindMatch (const DText&       text,
                          const uint64_t     fromChar,
                          const uint64_t     toChar,
                          const bool         ignoreCase)
{
  bool patternRefreshed = false;

  if ((fromChar >= toChar)
      || (toChar - fromChar < mPattern.Count ()))
    {
      return PATTERN_NOT_FOUND;
    }

  mText               = &text;
  mLastChar           = toChar;
  mCurrentChar        = fromChar;
  mCurrentRawOffset   = mText->OffsetOfChar (mCurrentChar);
  mCacheValid         = 0;
  mCacheStartPos      = 0;

  if (mText->RawSize () < mCurrentRawOffset + mPattern.RawSize ())
    return PATTERN_NOT_FOUND;

  if (ignoreCase != mIgnoreCase)
    {
      memset (mShiftTable, 0, sizeof mShiftTable);
      mPattern.RawRead (0, mPatternSize, mPatternRaw);
      patternRefreshed = true;
    }

  if (ignoreCase && patternRefreshed)
    {
      uint_t chOffset = 0;

      while (chOffset < mPatternSize)
        {
          uint32_t codePoint;

          wh_load_utf8_cp (mPatternRaw + chOffset, &codePoint);
          chOffset += wh_store_utf8_cp (wh_to_lowercase (codePoint),
                                        mPatternRaw + chOffset);
        }
      assert (chOffset == mPatternSize);
    }

  mIgnoreCase = ignoreCase;

  if (patternRefreshed)
    {
      memset (mShiftTable, 0, sizeof (mShiftTable));
      for (uint_t i = 0; i < mPatternSize; ++i)
        mShiftTable[mPatternRaw[i]] = i;
    }

  if (FindSubstr () < 0)
    return PATTERN_NOT_FOUND;

  assert (mPattern.Count () <= toChar - mCurrentChar);

  assert (mText->CharsUntilOffset (mCurrentRawOffset) == mCurrentChar);
  assert (mText->OffsetOfChar (mCurrentChar) == mCurrentRawOffset);

  return mCurrentChar;
}


int64_t
StringMatcher::FindMatchRaw (const DText&       text,
                             const uint64_t     fromChar,
                             const uint64_t     toChar,
                             const bool         ignoreCase)
{
  if (FindMatch (text, fromChar, toChar, ignoreCase) < 0)
    return PATTERN_NOT_FOUND;

  return mCurrentRawOffset;
}


int64_t
StringMatcher::NextMatch ()
{
  if (mText == NULL)
    return PATTERN_NOT_FOUND;

  const uint64_t patternCount = mPattern.Count ();

  assert (mCurrentChar + patternCount <= mLastChar);

  if ((mLastChar <= mCurrentChar + patternCount)
      || (mLastChar - (mCurrentChar + patternCount) < patternCount))
    {
      return PATTERN_NOT_FOUND;
    }

  mCurrentChar      += patternCount;
  mCurrentRawOffset += mPattern.RawSize ();

  if (mText->RawSize () < mCurrentRawOffset + mPattern.RawSize ())
    return PATTERN_NOT_FOUND;

  assert (mText->CharsUntilOffset (mCurrentRawOffset) == mCurrentChar);
  assert (mText->OffsetOfChar (mCurrentChar) == mCurrentRawOffset);

  if (FindSubstr () < 0)
    return PATTERN_NOT_FOUND;

  return mCurrentChar;
}


int64_t
StringMatcher::NextMatchRaw ()
{
  if (NextMatch () < 0)
    return PATTERN_NOT_FOUND;

  return mCurrentRawOffset;
}


uint_t
StringMatcher::ComparingWindowShift (uint_t position) const
{
  assert ((mPatternSize - 1) <= position);
  assert (position < mCacheValid);

  for (int i = mPatternSize - 1; i >= 0; --i, --position)
    {
      if (mPatternRaw[i] != mTextRawCache[position])
        return i - mShiftTable[mTextRawCache[position]];
    }

  return 0;
}


static bool
compare_text_buffers (const uint8_t* buffer1,
                      const uint8_t* buffer2,
                      const bool     ignoreCase,
                      uint_t*        ioSize)
{
  const uint_t maxSize = *ioSize;

  uint_t offset = 0;
  while (offset < maxSize)
    {
      const uint_t chSize = _cuCache.Count (buffer1[offset]);

      assert (chSize != 0);

      if (offset + chSize <= maxSize)
        {
          uint32_t ch1;
          uint32_t ch2;

          wh_load_utf8_cp (buffer1 + offset, &ch1);
          wh_load_utf8_cp (buffer2 + offset, &ch2);

          if (ignoreCase)
            {
              if (wh_to_lowercase (ch1) != wh_to_lowercase (ch2))
                return false;
            }
          else if (ch1 != ch2)
            return false;

          offset += chSize;
        }
      else
        break;
    }

  *ioSize = offset;

  return offset != 0;
}


bool
StringMatcher::SuffixesMatch () const
{
  const uint64_t chCount = mPattern.Count ();

  if (mLastChar < mCurrentChar + chCount)
    return false;

  const uint_t buffSize   = mPattern.RawSize ();
  uint_t       buffOffset = mPatternSize;

  if (mText->RawSize () < (mCurrentRawOffset + buffSize))
    return false;

  while (buffOffset < buffSize)
    {
      uint8_t   buffer1[64];
      uint8_t   buffer2[sizeof buffer1];

      uint_t chunkSize = MIN (sizeof buffer1, buffSize - buffOffset);

      mPattern.RawRead (buffOffset, chunkSize, buffer1);
      mText->RawRead (mCurrentRawOffset + buffOffset,
                      chunkSize,
                      buffer2);
      if (! compare_text_buffers (buffer1, buffer2, mIgnoreCase, &chunkSize))
        return false;

      buffOffset += chunkSize;
    }

  assert (buffOffset == buffSize);

  return true;
}


uint_t
StringMatcher::FindInCache () const
{
  assert ((mTextRawCache[0] & UTF8_EXTRA_BYTE_MASK) != UTF8_EXTRA_BYTE_SIG);
  assert ((mCacheStartPos <= mCurrentRawOffset)
          && (mCurrentRawOffset < (mCacheStartPos + mCacheValid)));

  uint_t windowsPos = mCurrentRawOffset - mCacheStartPos + mPatternSize - 1;

  while (windowsPos < mCacheValid)
    {
      const uint_t shift = ComparingWindowShift (windowsPos);

      if (shift == 0)
        break;

      windowsPos += shift;
    }

  return windowsPos;
}


void
StringMatcher::CountCachedChars (const uint_t offset)
{
  uint_t chOffset = mCurrentRawOffset - mCacheStartPos;

  assert (offset <= mCacheValid);
  assert ((mCacheStartPos <= mCurrentRawOffset)
          && (mCurrentRawOffset < (mCacheStartPos + mCacheValid)));

  while (chOffset < offset)
    {
      const uint_t codeUnits = _cuCache.Count (mTextRawCache[chOffset]);

      assert (codeUnits > 0);

      ++mCurrentChar;

      mCurrentRawOffset += codeUnits;
      chOffset          += codeUnits;
    }
}


bool
StringMatcher::FillTextCache ()
{
  assert (mText->OffsetOfChar (mCurrentChar) == mCurrentRawOffset);

  if (mLastChar < mCurrentChar + mPattern.Count ())
    return false;

  if ((mCacheStartPos <= mCurrentRawOffset)
      && (mCurrentRawOffset + mPatternSize < (mCacheStartPos + mCacheValid)))
    {
      return true;
    }

  mCacheValid = min<uint64_t> (mAvailableCache,
                               mText->RawSize () - mCurrentRawOffset);
  if (mCacheValid < mPatternSize)
    {
      mCacheValid = 0;
      return false;
    }

  mText->RawRead (mCurrentRawOffset, mCacheValid, mTextRawCache);
  mCacheStartPos = mCurrentRawOffset;

  if (mIgnoreCase)
    {
      uint_t chOffset = 0;

      while (chOffset < mCacheValid)
        {
          uint32_t codePoint;

          const uint_t codeUnits = wh_load_utf8_cp (mTextRawCache + chOffset,
                                                    &codePoint);
          if (chOffset + codeUnits <= mCacheValid)
            {
              wh_store_utf8_cp (wh_to_lowercase (codePoint),
                                mTextRawCache + chOffset);
              chOffset += codeUnits;
            }
          else
            mCacheValid = chOffset;
        }
      assert (chOffset == mCacheValid);
    }

  if (mCacheValid < mPatternSize)
    return false;

  return true;
}


int64_t
StringMatcher::FindSubstr ()
{
  assert (mCurrentChar <= mLastChar);

  const uint64_t textSize = mText->RawSize ();

  int cacheOffset = 0;
  while (mCurrentRawOffset < textSize)
    {
      if ( ! FillTextCache ())
        return PATTERN_NOT_FOUND;

      cacheOffset = FindInCache ();
      if (cacheOffset < mCacheValid)
        {
          assert ((mPatternSize - 1) <= cacheOffset);

          cacheOffset -= mPatternSize - 1;

          CountCachedChars (cacheOffset);

          if (mPatternSize == mPattern.RawSize ())
            break; // Match  found.

          else if (! SuffixesMatch ())
            {
              //A prefix match was found but the rest of string don't match.
              ++mCurrentChar;
              mCurrentRawOffset += _cuCache.Count (
                              mTextRawCache[mCurrentRawOffset - mCacheStartPos]
                                                  );
            }
          else
            break; //Match found (including the suffix).
        }
      else
        {
          assert (mPatternSize <= mCacheValid);

          CountCachedChars (mCacheValid - (mPatternSize - 1));
          mCacheValid = 0;
        }
    }

  assert (mCurrentRawOffset <= textSize);

  if ((textSize <= mCurrentRawOffset)
      || (mLastChar <= mCurrentChar)
      || (mLastChar - mCurrentChar < mPattern.Count ()))
    {
      return PATTERN_NOT_FOUND;
    }

  return mCurrentChar;
}

