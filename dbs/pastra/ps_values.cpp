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
#include "dbs/dbs_valtranslator.h"
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

extern const UTF8_CU_COUNTER _cuCache;


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
is_valid_datetime(const int year,
                  const uint_t month,
                  const uint_t day,
                  const uint_t hour,
                  const uint_t min,
                  const uint_t sec)
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
  if (mIsNull == false
      && ! is_valid_hiresdate(year, month, day, hour, minutes, seconds, microsec))
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
  if (mIsNull || ( *this == Min()))
  {
    return DHiresTime();
  }

  uint16_t year = mYear;
  uint8_t mnth = mMonth;
  uint8_t day = mDay;
  uint8_t hour = mHour;
  uint8_t mins = mMinutes;
  uint8_t secs = mSeconds;
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
  if (mIsNull || ( *this == Max()))
    return DHiresTime();

  uint16_t year = mYear;
  uint8_t mnth = mMonth;
  uint8_t day = mDay;
  uint8_t hour = mHour;
  uint8_t mins = mMinutes;
  uint8_t secs = mSeconds;
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
{
  if (text != nullptr && text[0] != 0)
  {
    mText = shared_make(TemporalText, _RC(const uint8_t*, text));
    mText->SetSelfReference(mText);
  }
  else
    mText = NullText::GetSingletoneInstace();
}


DText::DText(const uint8_t *utf8Src, uint_t unitsCount)
{
  if ((utf8Src != nullptr) && (utf8Src[0] != 0) && (unitsCount > 0))
  {
    mText = shared_make(TemporalText, utf8Src, unitsCount);
    mText->SetSelfReference(mText);
  }
  else
    mText = NullText::GetSingletoneInstace();
}


DText::DText(shared_ptr<ITextStrategy> strategy)
  : mText(strategy)
{
}


DText::DText(const DText& source)
  : mText(source.GetStrategy())
{
}


bool
DText::IsNull() const
{
  return GetStrategy()->CharsCount() == 0;
}


DText&
DText::operator= (const DText& source)
{
  if (this != &source)
    ReplaceStrategy(source.GetStrategy());

  return *this;
}


uint64_t
DText::Count() const
{
  return GetStrategy()->CharsCount();
}


uint64_t
DText::RawSize() const
{
  return GetStrategy()->Utf8Count();
}


void
DText::RawRead(uint64_t offset, uint64_t count, uint8_t* const dest) const
{
  return GetStrategy()->ReadUtf8(offset, count, dest);
}


uint64_t
DText::OffsetOfChar(const uint64_t chIndex) const
{
  return GetStrategy()->OffsetOfChar(chIndex);
}


uint64_t
DText::CharsUntilOffset(const uint64_t offset) const
{
  return GetStrategy()->CharsUntilOffset(offset);
}


DText&
DText::Append(const DChar& ch)
{
  if (! ch.IsNull())
    ReplaceStrategy(GetStrategy()->Append(ch.mValue));

  return *this;
}


DText&
DText::Append(const DText& text)
{
  const auto s = GetStrategy();
  ReplaceStrategy(s->Append(*text.GetStrategy()));
  return *this;
}


DChar
DText::CharAt(const uint64_t index) const
{
  return GetStrategy()->CharAt(index);
}


void
DText::CharAt(const uint64_t index, const DChar& ch)
{
  const auto s = GetStrategy();
  ReplaceStrategy(s->UpdateCharAt(ch.mValue, index));
}


DUInt64
DText::FindInText(const DText&      text,
                  const bool        ignoreCase,
                  const uint64_t    fromCh,
                  const uint64_t    toCh)
{
  const auto s = GetStrategy();
  return s->FindMatchInText(*text.GetStrategy(), fromCh, toCh, ignoreCase);
}


DUInt64
DText::FindSubstring(const DText&      substr,
                     const bool        ignoreCase,
                     const uint64_t    fromCh,
                     const uint64_t    toCh)
{
  return _CC(DText&, substr).FindInText( *this, ignoreCase, fromCh, toCh);
}


DText&
DText::ReplaceSubstring(const DText&    substr,
                        const DText&    newSubstr,
                        const bool      ignoreCase,
                        const uint64_t  fromCh,
                        const uint64_t  toCh)
{
  ReplaceStrategy(substr.GetStrategy()->ReplaceInText(GetStrategy(),
                                                newSubstr.GetStrategy(),
                                                fromCh,
                                                toCh,
                                                ignoreCase));
  return *this;
}

DText&
DText::LowerCase()
{
  const auto s = GetStrategy();
  ReplaceStrategy(s->ToCase(true));
  return *this;
}

DText&
DText::UpperCase()
{
  const auto s = GetStrategy();
  ReplaceStrategy(s->ToCase(false));
  return *this;
}

shared_ptr<ITextStrategy>
DText::GetStrategy() const
{
  LockGuard<decltype(mLock)> _l(mLock);
  return mText;
}


void
DText::ReplaceStrategy(shared_ptr<ITextStrategy> s)
{
  LockGuard<decltype(mLock)> _l(mLock);

  if (mText != s)
      mText = s;
}


int
DText::CompareTo(const DText& second) const
{
  if (this == &second)
    return 0;


  shared_ptr<ITextStrategy> this_s = GetStrategy();
  shared_ptr<ITextStrategy> second_s = second.GetStrategy();

  if (this_s == second_s)
    return 0;

  return this_s->CompareTo(*second_s);
}


DText::operator string()
{
  string result;

  auto s = GetStrategy();
  for (uint32_t i = 0; i < s->CharsCount(); ++i)
  {
    uint8_t utfChar[8] = {0, };
    Utf8Translator::Write(utfChar, sizeof utfChar, false, s->CharAt(i));

    result += _RC(char*, utfChar);
  }

  return move(result);
}


template <class T> void
wh_array_init(const T* const array,
              const uint64_t count,
              shared_ptr<IArrayStrategy> *outStrategy)
{
  if (count == 0)
  {
    assert(array == nullptr);
    *outStrategy = NullArray::GetSingletoneInstace(array[0].DBSType());
    return;
  }
  else if (array == nullptr)
    throw DBSException(_EXTRA(DBSException::BAD_PARAMETERS));

  shared_ptr<IArrayStrategy> s = shared_make(TemporalArray, (array[0].DBSType()));

  for (uint64_t index = 0; index < count; ++index)
  {
    uint64_t dummy;

    if (array[index].IsNull())
      continue;

    uint8_t rawValue[MAX_VALUE_RAW_STORAGE];

    assert(sizeof rawValue <= Serializer::Size(array[0].DBSType(), false));

    Serializer::Store(rawValue, array[index]);
    s->Add(array[index].DBSType(), rawValue, &dummy);
  }
  *outStrategy = s;
}


DArray::DArray()
  : mArray(NullArray::GetSingletoneInstace(T_UNDETERMINED))
{
}


DArray::DArray(const DArray& source)
  : mArray(source.GetStrategy())
{
}

DArray&
DArray::operator= (const DArray& source)
{
  if (this != &source)
  {
    LockGuard<decltype(mLock)> _l(source.mLock);
    ReplaceStrategy(source.mArray);
  }

  return *this;
}


DArray::DArray(const DBool* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DChar* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DDate* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DDateTime* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DHiresTime* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DUInt8* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DUInt16* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DUInt32* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DUInt64* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DReal* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DRichReal* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DInt8* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DInt16* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DInt32* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(const DInt64* const array, const uint64_t count)
  : mArray(nullptr)
{
  wh_array_init(array, count, &mArray);
}


DArray::DArray(std::shared_ptr<IArrayStrategy> s)
  : mArray(s)
{
}


uint64_t
DArray::Count() const
{
  return GetStrategy()->Count();
}


DBS_FIELD_TYPE
DArray::Type() const
{
  return GetStrategy()->Type();
}

template <class T> inline uint64_t
DArray::add_array_element(const T& value)
{
  if (value.IsNull())
    throw DBSException(_EXTRA(DBSException::NULL_ARRAY_ELEMENT));

  uint64_t result;
  uint8_t rawValue[MAX_VALUE_RAW_STORAGE];

  assert(Serializer::Size(value.DBSType(), false) <= sizeof rawValue);

  Serializer::Store(rawValue, value);

  auto s = GetStrategy();
  ReplaceStrategy(s->Add(value.DBSType(), rawValue, &result));

  return result;
}

uint64_t
DArray::Add(const DBool& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DChar& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DDate& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DDateTime& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DHiresTime& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DUInt8& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DUInt16& value)
{
  return add_array_element(value);
}


uint64_t
DArray::Add(const DUInt32& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DUInt64& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DReal& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DRichReal& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DInt8& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DInt16& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DInt32& value)
{
  return add_array_element(value);
}

uint64_t
DArray::Add(const DInt64& value)
{
  return add_array_element(value);
}

template <class T> void
DArray::get_array_element(const uint64_t index, T& outElement) const
{
  auto s = GetStrategy();

  uint8_t rawValue[MAX_VALUE_RAW_STORAGE];
  const uint_t elSize = s->Get(index, rawValue);
  if (elSize == 0)
  {
    outElement = T();
    return;
  }

  if (outElement.DBSType() != Type())
    throw DBSException(_EXTRA(DBSException::INVALID_ARRAY_TYPE));

  Serializer::Load(rawValue, &outElement);
}

void
DArray::Get(const uint64_t index, DBool& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DChar& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DDate& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DDateTime& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DHiresTime& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DUInt8& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DUInt16& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DUInt32& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DUInt64& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DReal& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DRichReal& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DInt8& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DInt16& outValue) const
{
  get_array_element(index, outValue);
}


void
DArray::Get(const uint64_t index, DInt32& outValue) const
{
  get_array_element(index, outValue);
}

void
DArray::Get(const uint64_t index, DInt64& outValue) const
{
  get_array_element(index, outValue);
}


template<class T> inline void
DArray::set_array_element(const T& value, const uint64_t index)
{
  if (value.IsNull())
  {
    Remove(index);
    return;
  }

  uint8_t rawValue[MAX_VALUE_RAW_STORAGE];

  assert(Serializer::Size(value.DBSType(), false) <= sizeof rawValue);

  Serializer::Store(rawValue, value);
  auto s = GetStrategy();
  ReplaceStrategy(s->Set(value.DBSType(), rawValue, index));
}


void
DArray::Set(const uint64_t index, const DBool& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DChar& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DDate& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DDateTime& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DHiresTime& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index,  const DUInt8& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DUInt16& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DUInt32& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DUInt64& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DReal& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DRichReal& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DInt8& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DInt16& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DInt32& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Set(const uint64_t index, const DInt64& newValue)
{
  set_array_element(newValue, index);
}


void
DArray::Remove(const uint64_t index)
{
  const auto s = GetStrategy();
  ReplaceStrategy(s->Remove(index));
}


void
DArray::Sort(bool reverse)
{
  const auto s = GetStrategy();
  ReplaceStrategy(s->Sort(reverse));
}


shared_ptr<IArrayStrategy>
DArray::GetStrategy() const
{
  LockGuard<decltype(mLock)> _l(mLock);
  return mArray;
}

void
DArray::ReplaceStrategy(shared_ptr<IArrayStrategy> s)
{
  LockGuard<decltype(mLock)> _l(mLock);
  if (s != mArray)
    mArray = s;
}



