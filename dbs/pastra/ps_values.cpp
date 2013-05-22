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

#include "ps_textstrategy.h"
#include "ps_arraystrategy.h"
#include "ps_serializer.h"

using namespace std;
using namespace whisper;
using namespace pastra;

static const uint_t MAX_VALUE_RAW_STORAGE = 0x20;

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
  const uint_t  mnth          = month - 1;
  static uint_t monthDays[12] = { 31, 28, 31, 30, 31, 30,
                                  31, 31, 30, 31, 30, 31 };
  if (mnth > 11)
    return false;

  else if (day == 0)
    return false;

  if ((mnth != 1) && (day > monthDays [mnth]))
    return false;

  else if (mnth == 1)
    {
      if (is_leap_year (year) && (day > (monthDays[1] + 1)))
        return false;

      else if (day > monthDays[1])
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

  else if (microsec > 999999999)
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



DText::DText (const char* text)
  : mText (&pastra::NullText::GetSingletoneInstace ())
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
  : mText (& NullText::GetSingletoneInstace())
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
  : mText (NULL)
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
  : mText (NULL)
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


DText &
DText::operator= (const DText& source)
{
  if (this == &source)
    return *this;

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


DText::~DText ()
{
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


void
DText::Append (const DChar& ch)
{
  if ((ch.mIsNull) || (ch.mValue == 0))
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
  if (text.IsNull())
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

