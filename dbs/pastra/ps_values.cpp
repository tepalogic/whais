/******************************************************************************
 PASTRA - A light database one file system and more.
 Copyright(C) 2008  Iulian Popa

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
#include "utils/wthread.h"

#include "ps_textstrategy.h"
#include "ps_arraystrategy.h"
#include "ps_serializer.h"


using namespace std;
using namespace whais;
using namespace pastra;

static const uint_t  MAX_VALUE_RAW_STORAGE = Serializer::MAX_VALUE_RAW_SIZE;
static const uint_t  MNTH_DAYS[]           = MNTH_DAYS_A;

static const UTF8_CU_COUNTER _cuCache;



static bool
is_valid_date(const int year, const uint_t month, const uint_t day)
{
  if ((month == 0) || (month > 12))
    return false;

  else if (day == 0)
    return false;

  if (month == 2)
    {
      const bool leapYear = is_leap_year(year);

      if (day > (leapYear ? MNTH_DAYS[1] + 1 : MNTH_DAYS[1]))
        return false;
    }
  else if (day > MNTH_DAYS [month - 1])
    return false;

  return true;
}


static bool
is_valid_datetime(const int          year,
                   const uint_t       month,
                   const uint_t       day,
                   const uint_t       hour,
                   const uint_t       min,
                   const uint_t       sec)
{
  if (is_valid_date(year, month, day) == false)
    return false;

  else if ((hour > 23) || (min > 59) || (sec > 59))
      return false;

  return true;
}


static bool
is_valid_hiresdate(const int       year,
                    const uint_t    month,
                    const uint_t    day,
                    const uint_t    hour,
                    const uint_t    min,
                    const uint_t    sec,
                    const uint_t    microsec)
{
  if (is_valid_datetime(year, month, day, hour, min, sec) == false)
    return false;

  else if (microsec > 999999)
      return false;

  return true;
}


bool
DChar::operator< (const DChar& second) const
{
  if (IsNull())
    return second.IsNull() ?  false : true;

  else if (second.IsNull())
    return false;

  return wh_cmp_alphabetically(mValue, second.mValue) < 0;
}


DChar
DChar::Prev() const
{
  if (mIsNull || (mValue == 1))
    return DChar();

  return DChar(wh_prev_char(mValue));
}


DChar
DChar::Next() const
{
  if (mIsNull || (mValue == UTF_LAST_CODEPOINT))
      return DChar();

  return DChar(wh_next_char(mValue));
}



DDate::DDate(const int32_t year, const uint8_t month, const uint8_t day)
  : mYear(year),
    mMonth(month),
    mDay(day),
    mIsNull(false)
{
  if ((mIsNull == false)
      && ! is_valid_date(year, month, day))
    {
      throw DBSException(_EXTRA(DBSException::INVALID_DATE));
    }
}


DDate
DDate::Min()
{
  return DDate(-32768, 1, 1);
}


DDate
DDate::Max()
{
  return DDate(32767, 12, 31);
}


DDate
DDate::Prev() const
{
  if (mIsNull || (*this == Min()))
    return DDate();

  uint16_t year = mYear;
  uint8_t  mnth = mMonth;
  uint8_t  day  = mDay;

  if (--day == 0)
    {
      if (--mnth == 0)
          --year, mnth = 12;

      if ((mnth == 2) && is_leap_year(year))
        day = MNTH_DAYS[mnth - 1] + 1;

      else
        day = MNTH_DAYS[mnth - 1];
    }

  return DDate(year, mnth, day);
}


DDate
DDate::Next() const
{
  if (mIsNull || (*this == Max()))
    return DDate();

  uint16_t year = mYear;
  uint8_t  mnth = mMonth;
  uint8_t  day  = mDay;

  uint8_t  mnthDays = MNTH_DAYS[mnth - 1];

  if ((mnth == 2) && is_leap_year(year))
    mnthDays++;

  if (++day > mnthDays)
    {
      day = 1;

      if (++mnth > 12)
          ++year, mnth = 1;
    }

  return DDate(year, mnth, day);
}



DDateTime::DDateTime(const int32_t     year,
                      const uint8_t     month,
                      const uint8_t     day,
                      const uint8_t     hour,
                      const uint8_t     minutes,
                      const uint8_t     seconds)
  : mYear(year),
    mMonth(month),
    mDay(day),
    mHour(hour),
    mMinutes(minutes),
    mSeconds(seconds),
    mIsNull(false)
{
  if ((mIsNull == false)
       && ! is_valid_datetime(year, month, day, hour, minutes, seconds))
    {
      throw DBSException(_EXTRA(DBSException::INVALID_DATETIME));
    }
}


DDateTime
DDateTime::Min()
{
  return DDateTime(-32768, 1, 1, 0, 0, 0);
}


DDateTime
DDateTime::Max()
{
  return DDateTime(32767, 12, 31, 23, 59, 59);
}


DDateTime
DDateTime::Prev() const
{
  if (mIsNull || (*this == Min()))
    return DDateTime();

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

                  if ((mnth == 2) && is_leap_year(year))
                    day = MNTH_DAYS[mnth - 1] + 1;

                  else
                    day = MNTH_DAYS[mnth - 1];
                }
            }
        }
    }

  return DDateTime(year, mnth, day, hour, mins, secs);
}


DDateTime
DDateTime::Next() const
{
  if (mIsNull || (*this == Max()))
    return DDateTime();

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
              if ((mnth == 2) && is_leap_year(year))
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

  return DDateTime(year, mnth, day, hour, mins, secs);
}



DHiresTime::DHiresTime(const int32_t    year,
                        const uint8_t    month,
                        const uint8_t    day,
                        const uint8_t    hour,
                        const uint8_t    minutes,
                        const uint8_t    seconds,
                        const uint32_t   microsec)
  : mMicrosec(microsec),
    mYear(year),
    mMonth(month),
    mDay(day),
    mHour(hour),
    mMinutes(minutes),
    mSeconds(seconds),
    mIsNull(false)
{
  if ((mIsNull == false)
      && ! is_valid_hiresdate(year,
                               month,
                               day,
                               hour,
                               minutes,
                               seconds,
                               microsec))
    {
      throw DBSException(_EXTRA(DBSException::INVALID_DATETIME));
    }
}


DHiresTime
DHiresTime::Min()
{
  return DHiresTime(-32768, 1, 1, 0, 0, 0, 0);
}


DHiresTime
DHiresTime::Max()
{
  return DHiresTime(32767, 12, 31, 23, 59, 59, 999999);
}


DHiresTime
DHiresTime::Prev() const
{
  if (mIsNull || (*this == Min()))
    {
      return DHiresTime();
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

                      if ((mnth == 2) && is_leap_year(year))
                        day = MNTH_DAYS[mnth - 1] + 1;

                      else
                        day = MNTH_DAYS[mnth - 1];
                    }
                }
            }
        }
    }

  return DHiresTime(year, mnth, day, hour, mins, secs, usecs);
}


DHiresTime
DHiresTime::Next() const
{
  if (mIsNull || (*this == Max()))
    return DHiresTime();

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
                  if ((mnth == 2) && is_leap_year(year))
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

  return DHiresTime(year, mnth, day, hour, mins, secs, usecs);
}



DReal
DReal::Min()
{
  const int64_t intPart  = 0xFFFFFF8000000000ll;
  const int64_t fracPart = -1 * (DBS_REAL_PREC - 1);

  return DReal(DBS_REAL_T(intPart, fracPart, DBS_REAL_PREC));
}


DReal
DReal::Max()
{
  const int64_t intPart  = 0x0000007FFFFFFFFFll;
  const int64_t fracPart = (DBS_REAL_PREC - 1);

  return DReal(DBS_REAL_T(intPart, fracPart, DBS_REAL_PREC));
}


DReal
DReal::Prev() const
{
  if (mIsNull || (*this == Min()))
    return DReal();

  return DReal(mValue - DBS_REAL_T(0, 1, DBS_REAL_PREC));
}


DReal
DReal::Next() const
{
  if (mIsNull || (*this == Max()))
    return DReal();

  return DReal(mValue + DBS_REAL_T(0, 1, DBS_REAL_PREC));
}



DRichReal
DRichReal::Min()
{
  const int64_t intPart  = 0x8000000000000000ull;
  const int64_t fracPart = -1 * (DBS_RICHREAL_PREC - 1);

  return DRichReal(DBS_RICHREAL_T(intPart, fracPart, DBS_RICHREAL_PREC));
}


DRichReal
DRichReal::Max()
{
  const int64_t intPart  = 0x7FFFFFFFFFFFFFFFull;
  const int64_t fracPart = (DBS_RICHREAL_PREC - 1);

  return DRichReal(DBS_RICHREAL_T(intPart, fracPart, DBS_RICHREAL_PREC));
}


DRichReal
DRichReal::Prev() const
{
  if (mIsNull || (*this == Min()))
    return DRichReal();

  return DRichReal(mValue - DBS_RICHREAL_T(0, 1, DBS_RICHREAL_PREC));
}


DRichReal
DRichReal::Next() const
{
  if (mIsNull || (*this == Max()))
    return DRichReal();

  return DRichReal(mValue + DBS_RICHREAL_T(0, 1, DBS_RICHREAL_PREC));
}



DText::DText(const char* text)
  : mText(&pastra::NullText::GetSingletoneInstace()),
    mTextRefs(0)

{
  if ((text != NULL) && (text[0] != 0))
    mText = new TemporalText(_RC(const uint8_t*, text));
}


DText::DText(const uint8_t *utf8Src, uint_t unitsCount)
  : mText(& NullText::GetSingletoneInstace()),
    mTextRefs(0)

{
  if ((utf8Src != NULL) && (utf8Src[0] != 0) && (unitsCount > 0))
    mText = new TemporalText(utf8Src, unitsCount);
}


DText::DText(ITextStrategy& text)
  : mText(&text),
    mTextRefs(0)
{
}


DText::DText(const DText& source)
  : mText(NULL),
    mTextRefs(0)
{
  StrategyRAII s = source.GetStrategyRAII();

  mText = _SC(ITextStrategy&, s).MakeClone();

  assert(mText == source.mText);
}


DText::~DText()
{
  assert(mTextRefs == 0);

  mText->ReleaseReference();
}


bool
DText::IsNull() const
{
  return _SC(ITextStrategy&, GetStrategyRAII()).CharsCount() == 0;
}


DText&
DText::operator= (const DText& source)
{
  if (this == &source)
    return *this;

  StrategyRAII s = source.GetStrategyRAII();
  StrategyRAII t = GetStrategyRAII();

  if (&_SC(ITextStrategy&, s) != &_SC(ITextStrategy&, t))
    {
      t.Release();
      ReplaceStrategy(_SC(ITextStrategy&, s).MakeClone());
    }

  return *this;
}


uint64_t
DText::Count() const
{
  return _SC(ITextStrategy&, GetStrategyRAII()).CharsCount();
}


uint64_t
DText::RawSize() const
{
  return _SC(ITextStrategy&, GetStrategyRAII()).Utf8Count();
}


void
DText::RawRead(uint64_t        offset,
                uint64_t        count,
                uint8_t* const  dest) const
{
  return _SC(ITextStrategy&, GetStrategyRAII()).ReadUtf8(offset, count, dest);
}


uint64_t
DText::OffsetOfChar(const uint64_t chIndex) const
{
  return _SC(ITextStrategy&, GetStrategyRAII()).OffsetOfChar(chIndex);
}


uint64_t
DText::CharsUntilOffset(const uint64_t offset) const
{
  return _SC(ITextStrategy&, GetStrategyRAII()).CharsUntilOffset(offset);
}


void
DText::Append(const DChar& ch)
{
  if (ch.IsNull())
    return ;

  StrategyRAII    t = GetStrategyRAII();
  ITextStrategy&  s = t;

  ITextStrategy* const newText = s.Append(ch.mValue);

  if (newText != &s)
    {
      t.Release();
      ReplaceStrategy(newText);
    }
}


void
DText::Append(const DText& text)
{
  if (text.IsNull())
    return ;

  StrategyRAII    t   = GetStrategyRAII();
  StrategyRAII    src = text.GetStrategyRAII();
  ITextStrategy&  s   = t;

  ITextStrategy* const newText = s.Append(src);
  src.Release();
  if (newText != &s)
    {
      t.Release();
      ReplaceStrategy(newText);
    }
}


DChar
DText::CharAt(const uint64_t index) const
{
  return _SC(ITextStrategy&, GetStrategyRAII()).CharAt(index);
}


void
DText::CharAt(const uint64_t index, const DChar& ch)
{
  StrategyRAII    t = GetStrategyRAII();
  ITextStrategy&  s = t;

  ITextStrategy* const newText = s.UpdateCharAt(ch.mValue, index);

  if (newText != &s)
    {
      t.Release();
      ReplaceStrategy(newText);
    }
}


DUInt64
DText::FindInText(const DText&      text,
                   const bool        ignoreCase,
                   const uint64_t    fromCh,
                   const uint64_t    toCh)
{
  StrategyRAII    s = GetStrategyRAII();
  ITextStrategy&  t = s;

  return t.FindMatch(text.GetStrategyRAII(), fromCh, toCh, ignoreCase);
}


DUInt64
DText::FindSubstring(const DText&      substr,
                      const bool        ignoreCase,
                      const uint64_t    fromCh,
                      const uint64_t    toCh)
{
  return _CC(DText&, substr).FindInText( *this, ignoreCase, fromCh, toCh);
}


DText
DText::ReplaceSubstring(const DText&    substr,
                         const DText&    newSubstr,
                         const bool      ignoreCase,
                         const uint64_t  fromCh,
                         const uint64_t  toCh)
{
  StrategyRAII    s = substr.GetStrategyRAII();
  ITextStrategy&  t = s;

  ITextStrategy* const result = t.Replace(GetStrategyRAII(),
                                           newSubstr.GetStrategyRAII(),
                                           fromCh,
                                           toCh,
                                           ignoreCase);
  return DText(*result);
}


DText
DText::LowerCase() const
{
  return DText(*_SC(ITextStrategy&, GetStrategyRAII()).ToCase(true));
}


DText
DText::UpperCase() const
{
  return DText(*_SC(ITextStrategy&, GetStrategyRAII()).ToCase(false));
}


void
DText::MakeMirror(DText& inoutText)
{
  if (this == &inoutText)
    return ;

  StrategyRAII    s = GetStrategyRAII();
  ITextStrategy&  t = s;

  StrategyRAII    s2 = inoutText.GetStrategyRAII();
  ITextStrategy&  t2 = s2;

  if ((&t != &NullText::GetSingletoneInstace())
      && (&t == &t2))
    {
      return ;
    }

  ITextStrategy* const newText = t.MakeMirrorCopy();

  if (newText != &t)
    {
      s.Release();
      ReplaceStrategy(newText);
    }
  s.Release();

  if (newText != &t2)
    {
      s2.Release();
      inoutText.ReplaceStrategy(newText);
    }
}


ITextStrategy&
DText::GetStrategy()
{
  LockRAII<SpinLock> _l(mLock);

  ++mTextRefs;
  return *mText;
}

DText::StrategyRAII
DText::GetStrategyRAII() const
{
  return StrategyRAII( _CC(DText&, *this));
}


void
DText::ReleaseStrategy()
{
  LockRAII<SpinLock> _l(mLock);

  assert(mTextRefs > 0);

  --mTextRefs;
}


void
DText::ReplaceStrategy(ITextStrategy* const strategy)
{
  assert(strategy != NULL);
  assert(mText != strategy);

  LockRAII<SpinLock> _l(mLock);

  do
    {
      if (mTextRefs == 0)
        {
          mText->ReleaseReference();
          mText = strategy;
          return ;
        }

      _l.Release();
      wh_yield();
      _l.Acquire();
    }
  while(true);
}


int
DText::CompareTo(const DText& second) const
{
  if (this == &second)
    return 0;

  StrategyRAII    s = GetStrategyRAII();
  ITextStrategy&  t = s;

  StrategyRAII    s2 = second.GetStrategyRAII();
  ITextStrategy&  t2 = s2;

  if (&t == &t2)
    return 0;

  return t.CompareTo(t2);
}



template <class T> void
wh_array_init(const T* const       array,
               const uint64_t       count,
               IArrayStrategy* volatile *     outStrategy)
{
  if (count == 0)
    {
      assert(array == NULL);

      *outStrategy = &NullArray::GetSingletoneInstace(array[0].DBSType());

      return;
    }
  else if (array == NULL)
    throw DBSException(_EXTRA(DBSException::BAD_PARAMETERS));

  unique_ptr<IArrayStrategy> s(new TemporalArray(array[0].DBSType()));

  for (uint64_t index = 0; index < count; ++index)
    {
      uint64_t dummy;

      if (array[index].IsNull())
        continue;

      uint8_t rawValue [MAX_VALUE_RAW_STORAGE];

      assert(sizeof rawValue <= Serializer::Size(array[0].DBSType(), false));

      Serializer::Store(rawValue, array[index]);
      s->Add(array[index].DBSType(), rawValue, &dummy);
    }

  *outStrategy = s.release();
}


DArray::DArray()
  : mArray(&NullArray::GetSingletoneInstace(T_UNDETERMINED)),
    mArrayRefs(0)
{
}


DArray::DArray(const DBool* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DChar* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DDate* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DDateTime* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DHiresTime* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DUInt8* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DUInt16* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DUInt32* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DUInt64* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DReal* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DRichReal* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DInt8* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DInt16* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DInt32* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DInt64* const array, const uint64_t count)
  : mArray(NULL),
    mArrayRefs(0)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(IArrayStrategy& array)
  : mArray(&array),
    mArrayRefs(0)
{
}


DArray::DArray(const DArray& source)
  : mArray(NULL),
    mArrayRefs(0)
{
  StrategyRAII s = source.GetStrategyRAII();
  mArray = _SC(IArrayStrategy&, s).MakeClone();

  assert(mArray == source.mArray);
}


DArray::~DArray()
{
  assert(mArrayRefs == 0);

  mArray->ReleaseReference();
}


DArray &
DArray::operator= (const DArray& source)
{
  if (this == &source)
    return *this;

  StrategyRAII s = source.GetStrategyRAII();
  StrategyRAII t = GetStrategyRAII();

  if (&_SC(IArrayStrategy&, s) != &_SC(IArrayStrategy&, t))
    {
      t.Release();
      ReplaceStrategy(_SC(IArrayStrategy&, s).MakeClone());
    }

  return *this;
}


uint64_t
DArray::Count() const
{
  return _SC(IArrayStrategy&, GetStrategyRAII()).Count();
}


DBS_FIELD_TYPE
DArray::Type() const
{
  return _SC(IArrayStrategy&, GetStrategyRAII()).Type();
}

template <class T> inline uint64_t
add_array_element(const T& value, DArray& array)
{
  uint64_t result;
  uint8_t  rawValue[MAX_VALUE_RAW_STORAGE];

  assert(Serializer::Size(value.DBSType(), false) <= sizeof rawValue);

  Serializer::Store(rawValue, value);

  DArray::StrategyRAII a = array.GetStrategyRAII();
  IArrayStrategy&      s = a;

  IArrayStrategy* const newArray = s.Add(value.DBSType(),
                                          rawValue,
                                          &result);
  if (newArray != &s)
    {
      a.Release();
      array.ReplaceStrategy(newArray);
    }

  return result;
}


uint64_t
DArray::Add(const DBool& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DChar& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DDate& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DDateTime& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DHiresTime& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DUInt8& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DUInt16& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DUInt32& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DUInt64& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DReal& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DRichReal& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DInt8& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DInt16& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DInt32& value)
{
  return add_array_element(value, *this);
}


uint64_t
DArray::Add(const DInt64& value)
{
  return add_array_element(value, *this);
}


template <class T> void
get_array_element(const DArray&          array,
                   const uint64_t         index,
                   T&                     outElement)
{
  DArray::StrategyRAII a = array.GetStrategyRAII();
  IArrayStrategy&      s = a;

  uint8_t      rawValue[MAX_VALUE_RAW_STORAGE];
  const uint_t elSize = s.Get(index, rawValue);
  if (elSize == 0)
    {
      outElement = T();
      return ;
    }

  Serializer::Load(rawValue, &outElement);
}


void
DArray::Get(const uint64_t index, DBool& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DChar& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DDate& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DDateTime& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DHiresTime& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DUInt8& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DUInt16& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DUInt32& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DUInt64& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DReal& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DRichReal& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DInt8& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DInt16& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DInt32& outValue) const
{
  get_array_element(*this, index, outValue);
}


void
DArray::Get(const uint64_t index, DInt64& outValue) const
{
  get_array_element(*this, index, outValue);
}


template<class T> inline void
set_array_element(const T&          value,
                   const uint64_t    index,
                   DArray&           array)
{
  if (value.IsNull())
    {
      array.Remove(index);
      return ;
    }

  uint8_t  rawValue[MAX_VALUE_RAW_STORAGE];

  assert(Serializer::Size(value.DBSType(), false) <= sizeof rawValue);

  Serializer::Store(rawValue, value);

  DArray::StrategyRAII a = array.GetStrategyRAII();
  IArrayStrategy&      s = a;

  IArrayStrategy* const newArray = s.Set(value.DBSType(),
                                          rawValue,
                                          index);
  if (newArray != &s)
    {
      a.Release();
      array.ReplaceStrategy(newArray);
    }
}


void
DArray::Set(const uint64_t index, const DBool& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DChar& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DDate& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DDateTime& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DHiresTime& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index,  const DUInt8& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DUInt16& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DUInt32& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DUInt64& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DReal& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DRichReal& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DInt8& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DInt16& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DInt32& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Set(const uint64_t index, const DInt64& newValue)
{
  set_array_element(newValue, index, *this);
}


void
DArray::Remove(const uint64_t index)
{
  DArray::StrategyRAII a = GetStrategyRAII();
  IArrayStrategy&      s = a;

  IArrayStrategy* const newArray = s.Remove(index);
  if (newArray != &s)
    {
      a.Release();
      ReplaceStrategy(newArray);
    }
}


void
DArray::Sort(bool reverse)
{

  DArray::StrategyRAII a = GetStrategyRAII();
  IArrayStrategy&      s = a;

  IArrayStrategy* const newArray = s.Sort(reverse);
  if (newArray != &s)
    {
      a.Release();
      ReplaceStrategy(newArray);
    }
}


void
DArray::MakeMirror(DArray& inoutArray) const
{
  if (this == &inoutArray)
    return ;

  StrategyRAII    s = GetStrategyRAII();
  IArrayStrategy& t = s;

  StrategyRAII    s2 = inoutArray.GetStrategyRAII();
  IArrayStrategy& t2 = s2;


  if ((&t != &NullArray::GetSingletoneInstace(t.Type()))
      && (&t == &t2))
    {
      return ;
    }

  IArrayStrategy* const newArray = t.MakeMirrorCopy();

  if (newArray != &t)
    {
      s.Release();
      _CC(DArray&, *this).ReplaceStrategy(newArray);
    }
  s.Release();

  if (newArray != &t2)
    {
      s2.Release();
      inoutArray.ReplaceStrategy(newArray);
    }
}


IArrayStrategy&
DArray::GetStrategy()
{
  LockRAII<SpinLock> _l(mLock);

  ++mArrayRefs;
  return *mArray;
}


DArray::StrategyRAII
DArray::GetStrategyRAII() const
{
  return StrategyRAII( _CC(DArray&, *this));
}


void
DArray::ReleaseStrategy()
{
  LockRAII<SpinLock> _l(mLock);

  assert(mArrayRefs > 0);

  --mArrayRefs;
}


void
DArray::ReplaceStrategy(IArrayStrategy* const strategy)
{
  assert(strategy != NULL);
  assert(mArray != strategy);

  LockRAII<SpinLock> _l(mLock);

  do
    {
      if (mArrayRefs == 0)
        {
          mArray->ReleaseReference();
          mArray = strategy;
          return ;
        }

      _l.Release();
      wh_yield();
      _l.Acquire();
    }
  while(true);
}


