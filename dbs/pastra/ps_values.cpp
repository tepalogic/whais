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
#include <vector>

#include "dbs/dbs_values.h"
#include "dbs/dbs_mgr.h"
#include "utils/wrandom.h"
#include "utils/utf8.h"

#include "ps_textstrategy.h"
#include "ps_arraystrategy.h"
#include "ps_serializer.h"


using namespace std;
using namespace whisper;
using namespace pastra;


class StringMatcher
{
private:
  static const int       ALPHABET_SIZE           = 256;
  static const int       MAX_TEXT_CACHE_SIZE     = 1024;
  static const int       MAX_PATTERN_SIZE        = 255;
  static const int64_t   PATTERN_NOT_FOUND       = -1;
  static const uint64_t  DEFAULT_LAST_CHAR       = 0xFFFFFFFFFFFFFFFFull;


public:
  StringMatcher (const DText&   text,
                 const DText&   pattern);

  int64_t FindMatch (const uint64_t fromChar,
                     const uint64_t toChar,
                     const bool     ignoreCase);

  int64_t FindMatchRaw (const uint64_t fromChar,
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


  const DText&        mText;
  const DText&        mPattern;

  uint8_t             mPatternSize;
  bool                mIgnoreCase;

  uint64_t            mLastChar;
  uint64_t            mCurrentChar;
  uint64_t            mCurrentRawOffet;
  uint64_t            mCacheStartPos;

  uint8_t* const      mPatternRaw;
  uint8_t* const      mTextRawCache;

  uint16_t            mAvailableCache;
  uint16_t            mCacheValid;

  uint8_t             mShiftTable[ALPHABET_SIZE];
  uint8_t             mCache[MAX_PATTERN_SIZE + MAX_TEXT_CACHE_SIZE];
};

static const uint_t MAX_VALUE_RAW_STORAGE = 0x20;

static uint_t MNTH_DAYS[12] = { 31, 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31 };



static bool
is_leap_year (const int year)
{
  if ((year % 4) == 0)
    {
      if ((year % 100) == 0)
        {
          if ((year % 400) == 0)
            return true;

          else
            return false;
        }
      else
        return true;
    }
  else
    return false;
}


static bool
is_valid_date (const int year, const uint_t month, const uint_t day)
{
  const uint_t mnth = month - 1;

  if (mnth > 11)
    return false;

  else if (day == 0)
    return false;

  if ((mnth != 1) && (day > MNTH_DAYS [mnth]))
    return false;

  else if (mnth == 1)
    {
      const bool leapYear = is_leap_year (year);

      if (leapYear && (day > (MNTH_DAYS[1] + 1)))
        return false;

      else if (! leapYear && (day > MNTH_DAYS[1]))
        return false;
    }

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




DDate::DDate (const int32_t year, const uint8_t month, const uint8_t day)
  : mYear (year),
    mMonth (month),
    mDay (day),
    mIsNull (false)
{
  if ((mIsNull == false)
      && ! is_valid_date (year, month, day))
    {
      throw DBSException (NULL, _EXTRA (DBSException::INVALID_DATE));
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
      throw DBSException (NULL, _EXTRA (DBSException::INVALID_DATETIME));
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
      throw DBSException (NULL, _EXTRA (DBSException::INVALID_DATETIME));
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
  uint8_t  first[64];
  uint8_t  second[64];

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

  count = min (count, mText->BytesCount());

  mText->ReadUtf8 (offset, count, dest);
}


uint64_t
DText::BytesUntilChar (const uint64_t chIndex) const
{
  if (chIndex >= Count ())
    return RawSize ();

  uint64_t  rawSize  = RawSize ();
  uint64_t  currChar = 0;
  uint64_t  currOff  = 0;


  uint8_t   buffer[64];
  uint_t    buffValid = 0;
  uint_t    buffOff   = 0;

  while (currChar < chIndex)
    {
      if (buffValid <= buffOff)
        {
	  assert (buffOff - buffValid <= rawSize);

          rawSize   -= buffOff - buffValid;
          buffValid  = MIN (sizeof buffer, rawSize);
          rawSize   -= buffValid;

          if (buffValid == 0)
            break;

          RawRead (currOff, buffValid, buffer);
          buffOff = 0;
        }
      else
        {
          const uint_t bytesCount = wh_utf8_cu_count (buffer[buffOff]);

          assert (bytesCount > 0);

          currOff += bytesCount, buffOff += bytesCount;

          ++currChar;
        }
    }

  assert (currOff <= RawSize ());

  return currOff;
}


uint64_t
DText::CharsUntilByte (const uint64_t offset) const
{
  uint64_t  rawSize  = RawSize ();
  uint64_t  currChar = 0;
  uint64_t  currOff  = 0;

  uint8_t   buffer[64];
  uint_t    buffValid = 0;
  uint_t    buffOff   = 0;

  while (currOff < offset)
    {
      if (buffValid <= buffOff)
        {
	  assert (buffOff - buffValid <= rawSize);

          rawSize   -= buffOff - buffValid;
          buffValid  = MIN (sizeof buffer, rawSize);
          rawSize   -= buffValid;

          if (buffValid == 0)
            break;

          RawRead (currOff, buffValid, buffer);
          buffOff = 0;
        }
      else
        {
          const uint_t bytesCount = wh_utf8_cu_count (buffer[buffOff]);

          currOff += bytesCount, buffOff += bytesCount;

          if (currOff <= offset)
            ++currChar;
        }
    }

  return currChar;
}


void
DText::Append (const DChar& ch)
{
  delete _SC (StringMatcher*, mStringMatcher);
  mStringMatcher = NULL;

  if (ch.IsNull ())
    return ;

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
  delete _SC (StringMatcher*, mStringMatcher);
  mStringMatcher = NULL;

  if (text.IsNull ())
    return;

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

  mText->Append (*text.mText);
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
    throw DBSException (NULL, _EXTRA (DBSException::STRING_INDEX_TOO_BIG));

  if (ch.IsNull ())
    mText->Truncate (index);

  else
    mText->UpdateCharAt (ch.mValue, index, &mText);
}


DUInt64
DText::FindSubstr (const DText&      pattern,
                   const DUInt64     from,
                   const DUInt64     to,
                   const DBool       ignoreCase)
{
  const uint64_t fromCh = from.IsNull () ?  0 : from.mValue;
  const uint64_t endCh  = to.IsNull () ? Count () : MIN (Count (), to.mValue);

  delete _SC (StringMatcher*, mStringMatcher);
  mStringMatcher = NULL;

  if (pattern.IsNull ()
      || (endCh <= fromCh)
      || (endCh - fromCh < pattern.Count ()))
    {
      return DUInt64 ();
    }

  mStringMatcher = new StringMatcher (*this, pattern);

  const int64_t result = _SC (StringMatcher*, mStringMatcher)->FindMatch (
                                             fromCh,
                                             endCh,
                                             ignoreCase == DBool (true)
                                                                         );
  assert ((result < 0)
          || ((fromCh <= (uint64_t)result)
              && ((uint64_t)result <= endCh - pattern.Count ())));

  if (result >= 0)
    return DUInt64 (result);

  return DUInt64 ();
}


DUInt64
DText::FindSubstrNext () const
{
  if (mStringMatcher == NULL)
    return DUInt64 ();

  const int64_t result = _SC (StringMatcher*, mStringMatcher)->NextMatch ();
  if (result >= 0)
    return DUInt64 (result);

  return DUInt64 ();
}


DUInt64
DText::FindSubstrRaw (const DText&      pattern,
                      const DUInt64     from,
                      const DUInt64     to,
                      const DBool       ignoreCase)
{
  const uint64_t fromCh = from.IsNull () ?  0 : from.mValue;
  const uint64_t endCh  = to.IsNull () ? Count () : MIN (Count (), to.mValue);

  delete _SC (StringMatcher*, mStringMatcher);
  mStringMatcher = NULL;

  if (pattern.IsNull ()
      || (endCh <= fromCh)
      || (endCh - fromCh < pattern.Count ()))
    {
      return DUInt64 ();
    }


  mStringMatcher = new StringMatcher (*this, pattern);

  const int64_t result = _SC (StringMatcher*, mStringMatcher)->FindMatchRaw (
                                             fromCh,
                                             endCh,
                                             ignoreCase == DBool (true)
                                                                            );
  assert ((result < 0)
          || ((fromCh <= (uint64_t)result)
              && ((uint64_t)result <= endCh - pattern.Count ())));

  if (result >= 0)
    return DUInt64 (result);

  return DUInt64 ();
}


DUInt64
DText::FindSubstrNextRaw () const
{
  if (mStringMatcher == NULL)
    return DUInt64 ();

  const int64_t result = _SC (StringMatcher*, mStringMatcher)->NextMatchRaw ();
  if (result >= 0)
    return DUInt64 (result);

  return DUInt64 ();
}


DText
DText::ReplaceSubstr (const DText&      substr,
                      const DText&      newSubstr,
                      const DUInt64     from,
                      const DUInt64     to,
                      const DBool       ignoreCase)
{
  const uint64_t substrCount = substr.Count ();

  DText     result;
  DUInt64   matchPos;
  DUInt64   lastMatchPos (0);

  matchPos = FindSubstr (substr, from, to, ignoreCase);

  while (! matchPos.IsNull ())
    {
      for (uint64_t i = lastMatchPos.mValue; i < matchPos.mValue; ++i)
        {
          const DChar ch = CharAt (i);

          assert (ch.IsNull () == 0);

          result.Append (ch);
        }

      result.Append (newSubstr);

      lastMatchPos = DUInt64 (matchPos.mValue + substrCount);
      assert (lastMatchPos.mValue <= Count ());

      matchPos = FindSubstrNext ();
    }

  if (matchPos.IsNull ())
    matchPos = DUInt64 (Count ());

  for (uint64_t i = lastMatchPos.mValue; i < matchPos.mValue; ++i)
    {
      const DChar ch = CharAt (i);

      assert (ch.IsNull () == 0);

      result.Append (ch);
    }

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
    throw DBSException (NULL, _EXTRA (DBSException::INVALID_PARAMETERS));

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
      throw DBSException (NULL, _EXTRA (DBSException::INVALID_ARRAY_TYPE));
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
        throw DBSException (NULL, _EXTRA (DBSException::INVALID_ARRAY_TYPE));

      assert ((*inoutStrategy) ==
                &NullArray::GetSingletoneInstace (T_UNDETERMINED));

      *inoutStrategy = &NullArray::GetSingletoneInstace (element.DBSType ());

      return add_array_element (element, inoutStrategy);
    }
  else if (element.IsNull ())
    throw DBSException (NULL, _EXTRA (DBSException::NULL_ARRAY_ELEMENT));

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
    throw DBSException (NULL, _EXTRA (DBSException::ARRAY_INDEX_TOO_BIG));

  else if (strategy.Type() != outElement.DBSType ())
    throw DBSException (NULL, _EXTRA(DBSException::INVALID_ARRAY_TYPE));

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
      add_array_element (value, inoutStrategy);
      return ;
    }
  else if (index > (*inoutStrategy)->Count())
    throw DBSException(NULL, _EXTRA (DBSException::ARRAY_INDEX_TOO_BIG));

  prepare_array_strategy (inoutStrategy);
  static const uint_t storageSize = get_aligned_elem_size (
                                                    (*inoutStrategy)->Type()
                                                          );
  if (value.IsNull())
      (*inoutStrategy)->CollapseRaw (index * storageSize, storageSize);

  else
    {
      uint8_t rawElement[MAX_VALUE_RAW_STORAGE];

      Serializer::Store (rawElement, value);
      (*inoutStrategy)->WriteRaw (storageSize * index, storageSize, rawElement);
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
    throw DBSException(NULL, _EXTRA(DBSException::ARRAY_INDEX_TOO_BIG));

  prepare_array_strategy (&mArray);

  const uint_t storageSize = get_aligned_elem_size (mArray->Type());

  mArray->CollapseRaw (index * storageSize, storageSize);
}


template<class DBS_T>
int64_t
partition (int64_t            from,
           int64_t            to,
           DArray&            inoutArray,
           bool* const        outAlreadySorted)
{
  assert (from < to);
  assert (to < _SC (int64_t, inoutArray.Count ()));

  const int64_t pivotIndex = (from + to) / 2;
  DBS_T         leftEl;
  DBS_T         pivot;
  DBS_T         rightEl;
  DBS_T         temp;

  inoutArray.Get (pivotIndex, pivot);
  inoutArray.Get (from, leftEl);

  *outAlreadySorted = false;
  if (leftEl <= pivot)
    {
      temp = leftEl;
      while (from < to)
        {
          inoutArray.Get (from + 1, leftEl);
          if ((leftEl <= pivot) && (temp <= leftEl))
            {
              ++from;
              temp = leftEl;
            }
          else
            break;
        }
      if (from == to)
        *outAlreadySorted = true;
    }

  while (from < to)
    {
      while (from <= to)
        {
          inoutArray.Get (from, leftEl);
          if (leftEl < pivot)
            ++from;

          else
            break;
        }

      if (leftEl == pivot)
        {
          while (from < to)
            {
              inoutArray.Get (from + 1, temp);
              if (temp == pivot)
                ++from;

              else
                break;
            }
        }

      assert ((from < to) || (leftEl == pivot));

      while (from <= to)
        {
          inoutArray.Get (to, rightEl);
          if (pivot < rightEl)
            --to;

          else
            break;
        }

      assert ((from < to) || (rightEl == pivot));

      if (from < to)
        {
          if (leftEl == rightEl)
            {
              assert (leftEl == pivot);
              ++from;
            }
          else
            {
              inoutArray.Set (to, leftEl);
              inoutArray.Set (from, rightEl);
            }
        }
    }

  return from;
}


template<class DBS_T>
int64_t
partition_reverse (int64_t            from,
                   int64_t            to,
                   DArray&            inoutArray,
                   bool* const        outAlreadySorted)
{
  assert (from < to);
  assert (to < _SC (int64_t, inoutArray.Count()));

  const int64_t pivotIndex = (from + to) / 2;
  DBS_T         leftEl;
  DBS_T         pivot;
  DBS_T         rightEl;
  DBS_T         temp;

  inoutArray.Get (pivotIndex, pivot);
  inoutArray.Get (from, leftEl);

  *outAlreadySorted = false;
  if (leftEl >= pivot)
    {
      temp = leftEl;
      while (from < to)
        {
          inoutArray.Get (from + 1, leftEl);
          if ((leftEl >= pivot) && (temp >= leftEl))
            {
              ++from;
              temp = leftEl;
            }
          else
            break;
        }

      if (from == to)
        *outAlreadySorted = true;
    }

  while (from < to)
    {
      while (from <= to)
        {
          inoutArray.Get (from, leftEl);
          if (pivot < leftEl)
            ++from;

          else
            break;
        }

      if (leftEl == pivot)
        {
          while (from < to)
            {
              inoutArray.Get (from + 1, temp);
              if (temp == pivot)
                ++from;

              else
                break;
            }
        }

      assert ((from < to) || (leftEl == pivot));

      while (from <= to)
        {
          inoutArray.Get (to, rightEl);
          if (rightEl < pivot)
            --to;

          else
            break;
        }

      assert ((from < to) || (rightEl == pivot));

      if (from < to)
        {
          if (leftEl == rightEl)
            {
              assert (leftEl == pivot);
              ++from;
            }
          else
            {
              inoutArray.Set (to, leftEl);
              inoutArray.Set (from, rightEl);
            }
        }
    }

  return from;
}

struct _partition_t
{
  _partition_t (uint64_t from, uint64_t to)
    : mFrom (from),
      mTo (to)
  {
  }
  int64_t mFrom;
  int64_t mTo;
};

template<class DBS_T>
void
quick_sort (int64_t           from,
            int64_t           to,
            const bool        reverse,
            DArray&           inoutArray)
{
  assert (from <= to);

  vector<_partition_t> partStack;

  partStack.push_back ( _partition_t (from, to));

  do
    {
      _partition_t current = partStack[partStack.size () - 1];
      partStack.pop_back();

      if (current.mFrom >= current.mTo)
        continue;

      int64_t pivot;
      bool    alreadySorted;

      if (reverse)
        {
          pivot = partition_reverse<DBS_T> (current.mFrom,
                                            current.mTo,
                                            inoutArray,
                                            &alreadySorted);
        }
      else
        {
          pivot = partition<DBS_T> (current.mFrom,
                                    current.mTo,
                                    inoutArray,
                                    &alreadySorted);
        }

      if (alreadySorted == false)
        {
          if ((pivot + 1) < current.mTo)
            partStack.push_back (_partition_t (pivot + 1, current.mTo));

          if (current.mFrom < (pivot - 1))
            partStack.push_back (_partition_t (current.mFrom, pivot - 1));
        }
    }
  while (partStack.size () > 0);
}


void
DArray::Sort (bool reverse)
{
  switch (Type ())
  {
  case T_BOOL:
    quick_sort<DBool> (0, Count () - 1, reverse, *this);
    break;

  case T_CHAR:
    quick_sort<DChar> (0, Count () - 1, reverse, *this);
    break;

  case T_DATE:
    quick_sort<DDate> (0, Count () - 1, reverse, *this);
    break;

  case T_DATETIME:
    quick_sort<DDateTime> (0, Count () - 1, reverse, *this);
    break;

  case T_HIRESTIME:
    quick_sort<DHiresTime> (0, Count () - 1, reverse, *this);
    break;

  case T_UINT8:
    quick_sort<DUInt8> (0, Count () - 1, reverse, *this);
    break;

  case T_UINT16:
    quick_sort<DUInt16> (0, Count () - 1, reverse, *this);
    break;

  case T_UINT32:
    quick_sort<DUInt32> (0, Count () - 1, reverse, *this);
    break;

  case T_UINT64:
    quick_sort<DUInt64> (0, Count () - 1, reverse, *this);
    break;

  case T_REAL:
    quick_sort<DReal> (0, Count () - 1, reverse, *this);
    break;

  case T_RICHREAL:
    quick_sort<DRichReal> (0, Count () - 1, reverse, *this);
    break;

  case T_INT8:
    quick_sort<DInt8> (0, Count () - 1, reverse, *this);
    break;

  case T_INT16:
    quick_sort<DInt16> (0, Count () - 1, reverse, *this);
    break;

  case T_INT32:
    quick_sort<DInt32> (0, Count () - 1, reverse, *this);
    break;

  case T_INT64:
    quick_sort<DInt64> (0, Count () - 1, reverse, *this);
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



StringMatcher::StringMatcher (const DText&      text,
                              const DText&      pattern)
  : mText (text),
    mPattern (pattern),
    mPatternSize (MIN (pattern.RawSize (), MAX_PATTERN_SIZE)),
    mIgnoreCase (false),
    mLastChar (text.Count ()),
    mCurrentChar (mLastChar),
    mCurrentRawOffet (mText.RawSize ()),
    mCacheStartPos (0),
    mPatternRaw (mCache),
    mTextRawCache (mPatternRaw + mPatternSize),
    mAvailableCache (sizeof mCache - mPatternSize),
    mCacheValid (0)
{
  assert (mPatternSize > 0);
  assert (mText.RawSize () >= mPatternSize);

  mPattern.RawRead (0, mPatternSize, mPatternRaw);

  if (mPatternSize < pattern.RawSize ())
    {
      uint_t chOffset = 0;
      while (chOffset < mPatternSize)
        {
          const uint32_t codeUnits = wh_utf8_cu_count (mPatternRaw[chOffset]);

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
StringMatcher::FindMatch (const uint64_t     fromChar,
                          const uint64_t     toChar,
                          const bool         ignoreCase)
{
  bool patternRefreshed = false;

  if ((fromChar >= toChar)
      || (toChar - fromChar < mPattern.Count ()))
    {
      return PATTERN_NOT_FOUND;
    }

  mLastChar           = MIN (toChar, mText.Count ());
  mCurrentChar        = fromChar;
  mCurrentRawOffet    = mText.BytesUntilChar (mCurrentChar);
  mCacheValid         = 0;
  mCacheStartPos      = 0;

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
          chOffset += wh_store_utf8_cp (tolower (codePoint),
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

  assert (mText.CharsUntilByte (mCurrentRawOffet) == mCurrentChar);
  assert (mText.BytesUntilChar (mCurrentChar) == mCurrentRawOffet);

  return mCurrentChar;
}


int64_t
StringMatcher::FindMatchRaw (const uint64_t     fromChar,
                             const uint64_t     toChar,
                             const bool         ignoreCase)
{
  if (FindMatch (fromChar, toChar, ignoreCase) < 0)
    return PATTERN_NOT_FOUND;

  return mCurrentRawOffet;
}


int64_t
StringMatcher::NextMatch ()
{
  const uint64_t patternCount = mPattern.Count ();

  assert (mLastChar <= mText.Count ());
  assert (mCurrentChar + patternCount <= mLastChar);

  if ((mLastChar <= mCurrentChar + patternCount)
      || (mLastChar - (mCurrentChar + patternCount) < patternCount))
    {
      return PATTERN_NOT_FOUND;
    }

  mCurrentChar     += patternCount;
  mCurrentRawOffet += mPattern.RawSize ();

  assert (mText.CharsUntilByte (mCurrentRawOffet) == mCurrentChar);
  assert (mText.BytesUntilChar (mCurrentChar) == mCurrentRawOffet);

  if (FindSubstr () < 0)
    return PATTERN_NOT_FOUND;

  return mCurrentChar;
}


int64_t
StringMatcher::NextMatchRaw ()
{
  if (NextMatch () < 0)
    return PATTERN_NOT_FOUND;

  return mCurrentRawOffet;
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


bool
StringMatcher::SuffixesMatch () const
{
  const uint64_t chCount = mPattern.Count ();

  if (mLastChar < mCurrentChar + chCount)
    return false;

  uint64_t chIndex = mPattern.CharsUntilByte (mPatternSize);

  assert (chIndex < chCount);

  bool result = true;
  for (; result && (chIndex < chCount); ++chIndex)
    {
      const DChar ch1 = mText.CharAt (mCurrentChar + chIndex);
      const DChar ch2 = mPattern.CharAt (chIndex);

      assert (ch1.IsNull () == false);
      assert (ch2.IsNull () == false);

      if (mIgnoreCase)
        result = (tolower (ch1.mValue) == tolower (ch2.mValue));

      else
        result = (ch1.mValue == ch2.mValue);
    }

  return result;
}


uint_t
StringMatcher::FindInCache () const
{
  assert ((mTextRawCache[0] & UTF8_EXTRA_BYTE_MASK) != UTF8_EXTRA_BYTE_SIG);
  assert ((mCacheStartPos <= mCurrentRawOffet)
          && (mCurrentRawOffet < (mCacheStartPos + mCacheValid)));

  uint_t windowsPos = mCurrentRawOffet - mCacheStartPos + mPatternSize - 1;

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
  uint_t chOffset = mCurrentRawOffet - mCacheStartPos;

  assert (offset <= mCacheValid);
  assert ((mCacheStartPos <= mCurrentRawOffet)
          && (mCurrentRawOffet < (mCacheStartPos + mCacheValid)));

  while (chOffset < offset)
    {
      const uint_t codeUnits = wh_utf8_cu_count (mTextRawCache[chOffset]);

      assert (codeUnits > 0);

      ++mCurrentChar;

      mCurrentRawOffet += codeUnits;
      chOffset         += codeUnits;
    }
}


bool
StringMatcher::FillTextCache ()
{
  assert (mText.BytesUntilChar (mCurrentChar) == mCurrentRawOffet);

  if (mLastChar < mCurrentChar + mPattern.Count ())
    return false;

  if ((mCacheStartPos <= mCurrentRawOffet)
      && (mCurrentRawOffet + mPatternSize < (mCacheStartPos + mCacheValid)))
    {
      return true;
    }

  mCacheValid = MIN (mAvailableCache, mText.RawSize () - mCurrentRawOffet);
  if (mCacheValid < mPatternSize)
    return PATTERN_NOT_FOUND;

  mText.RawRead (mCurrentRawOffet, mCacheValid, mTextRawCache);
  mCacheStartPos = mCurrentRawOffet;

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
              wh_store_utf8_cp (tolower (codePoint), mTextRawCache + chOffset);
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
  assert (mLastChar <= mText.Count ());
  assert (mCurrentChar <= mLastChar);

  int cacheOffset = 0;

  while (true)
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
              mCurrentRawOffet += wh_utf8_cu_count (
                              mTextRawCache[mCurrentRawOffet - mCacheStartPos]
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

  if ((mLastChar <= mCurrentChar)
      || (mLastChar - mCurrentChar < mPattern.Count ()))
    {
      return PATTERN_NOT_FOUND;
    }

  return mCurrentChar;
}

