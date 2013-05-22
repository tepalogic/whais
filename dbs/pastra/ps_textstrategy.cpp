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

#include "utils/le_converter.h"
#include "utils/utf8.h"

#include "dbs/dbs_mgr.h"
#include "dbs_exception.h"

#include "ps_textstrategy.h"

using namespace std;

namespace whisper {
namespace pastra {


/* Return the numbers of characters encoded bu this string. The string is
   either null terminated or 'maxBytesCount' bounded. */
static uint64_t
get_utf8_string_length (const uint8_t* utf8Str, uint64_t maxBytesCount)
{
  uint64_t result = 0;

  while ((maxBytesCount > 0) && (utf8Str[0] != 0))
    {
      const uint_t charSize = wh_utf8_cu_count(utf8Str[0]);

      bool validChar = true;

      if ((charSize == 0) || (charSize > MIN (6, maxBytesCount)))
        validChar = false;

      else
        {
          for (uint_t i = 1; (i < charSize) && validChar; ++i)
            {
              if ((utf8Str[i] & UTF8_EXTRA_BYTE_MASK) != UTF8_EXTRA_BYTE_SIG)
                validChar = false;
            }
        }

      if ( ! validChar)
        throw DBSException (NULL, _EXTRA(DBSException::INVALID_UTF8_STRING));

      result += charSize, maxBytesCount -= charSize, utf8Str += charSize;
    }

  return result;
}



GenericText::GenericText (uint64_t bytesSize)
  : mBytesSize (bytesSize),
    mCachedCharCount (INVALID_CACHE_VALUE),
    mCachedCharIndex (0),
    mCachedCharIndexOffset (0),
    mReferenceCount (0),
    mShareCount (0)

{
}


GenericText::~GenericText ()
{
}


uint_t
GenericText::ReferenceCount () const
{
  return mReferenceCount;
}


void
GenericText::IncreaseReferenceCount ()
{
  assert (mShareCount == 0);

  ++mReferenceCount;
}


void
GenericText::DecreaseReferenceCount ()
{
  assert (mShareCount == 0);
  assert (mReferenceCount > 0);

  -- mReferenceCount;

  if (mReferenceCount  == 0)
    ClearMyself ();
}


uint_t
GenericText::ShareCount () const
{
  return mShareCount;
}


void
GenericText::IncreaseShareCount ()
{
  assert (mReferenceCount == 1);

  ++mShareCount;
}


void
GenericText::DecreaseShareCount ()
{
  assert (mShareCount > 0);
  assert (mReferenceCount == 1);

  -- mShareCount;
}


uint64_t
GenericText::CharsCount()
{
  if (mCachedCharCount != INVALID_CACHE_VALUE)
    return mCachedCharCount;

  uint64_t offset = mCachedCharIndexOffset;
  uint64_t result = mCachedCharIndex;

  while (offset < mBytesSize)
    {
      uint8_t aUtf8Char;

      ReadUtf8 (offset, 1, &aUtf8Char);
      const uint_t charSize = wh_utf8_cu_count (aUtf8Char);

      if ((charSize == 0) || (charSize + offset > mBytesSize))
        throw DBSException (NULL, _EXTRA (DBSException::INVALID_UTF8_STRING));

      ++result, offset += charSize;
    }

  mCachedCharCount = result;

  return mCachedCharCount;
}


uint64_t
GenericText::BytesCount() const
{
  return mBytesSize;
}


void
GenericText::Duplicate (ITextStrategy&    source,
                        const uint64_t    maxCharsCount)
{

  Truncate (0);

  assert (mBytesSize == 0);
  assert (mCachedCharCount == 0);

  uint64_t index = 0;
  DChar    temp;
  while (((temp = source.CharAt (index)).IsNull () == false)
         && (mBytesSize < maxCharsCount))
    {
      Append (temp.mValue);
      ++index;
    }

  assert (mBytesSize <= maxCharsCount);
  assert (mCachedCharCount == index);

  mCachedCharIndex       = 0;
  mCachedCharIndexOffset = 0;
}


DChar
GenericText::CharAt (const uint64_t index)
{
  uint64_t chIndex  = 0;
  uint64_t chOffset = 0;
  uint32_t chValue  = 0;

  if (mCachedCharIndex <= index)
    {
      chIndex   = mCachedCharIndex;
      chOffset  = mCachedCharIndexOffset;
    }

  while ((chIndex < index)
         && (chOffset < mBytesSize))
    {
      uint8_t utf8CharCodeUnit;

      ReadUtf8 (chOffset, 1, &utf8CharCodeUnit);

      chOffset += wh_utf8_cu_count (utf8CharCodeUnit), ++chIndex;
    }

  assert (chOffset <= mBytesSize);

  if (chOffset >= mBytesSize)
    {
      assert (chOffset == mBytesSize);
      assert ((mCachedCharCount == INVALID_CACHE_VALUE)
              || (mCachedCharCount == chIndex));

      mCachedCharCount = chIndex;

      return DChar ();
    }

  assert (chIndex == index);

  mCachedCharIndex       = chIndex;
  mCachedCharIndexOffset = chOffset;

  uint8_t aUtf8Char[UTF8_MAX_BYTES_COUNT];

  ReadUtf8 (chOffset, MIN (mBytesSize - chOffset, sizeof aUtf8Char), aUtf8Char);

  const uint_t charSize = wh_load_utf8_cp (aUtf8Char, &chValue);

  assert ((chOffset + charSize) <= mBytesSize);

  return DChar (chValue);
}


void
GenericText::Append (const uint32_t ch)
{
  assert (mReferenceCount == 1);
  assert (ch != 0);

  uint8_t aUtf8Encoding[UTF8_MAX_BYTES_COUNT];

  const uint_t encodeSize = wh_store_utf8_cp (ch, aUtf8Encoding);

  WriteUtf8 (mBytesSize, encodeSize, aUtf8Encoding);

  if (mCachedCharCount != INVALID_CACHE_VALUE)
    ++mCachedCharCount;
}


void
GenericText::Append (ITextStrategy& text)
{
  uint64_t index = 0;
  DChar    temp;

  while ((temp = text.CharAt (index)).IsNull () == false)
    {
      Append (temp.mValue);
      ++index;
    }
}


void
GenericText::Truncate (uint64_t newCharCount)
{
  if ((mCachedCharCount != INVALID_CACHE_VALUE)
      && (mCachedCharCount <= newCharCount))
    {
      return ;
    }

  mCachedCharCount = 0;

  uint64_t offset = 0;
  if (mCachedCharIndex < newCharCount)
    {
      offset            = mCachedCharIndexOffset;
      mCachedCharCount  = mCachedCharIndex;
      newCharCount     -= mCachedCharIndex + 1;
    }
  else
    {
      mCachedCharIndex       = 0;
      mCachedCharIndexOffset = 0;
    }

  while ((newCharCount > 0)
         && (offset < mBytesSize))
    {
      uint8_t utf8Char;

      ReadUtf8 (offset, 1, &utf8Char);
      offset += wh_utf8_cu_count (utf8Char);

      --newCharCount, ++mCachedCharCount;

      assert (offset <= mBytesSize);
    }

   TruncateUtf8 (offset);
 }


void
GenericText::UpdateCharAt (const uint32_t   ch,
                           const uint64_t   index,
                           ITextStrategy**  inoutStrategy)
{
  assert (this == *inoutStrategy);
  assert (ch != 0);

  auto_ptr<ITextStrategy> newText (new TemporalText(NULL));
  newText->IncreaseReferenceCount();

  uint64_t i  = 0;
  DChar    temp;

  while ((temp = CharAt (i)).IsNull () == false)
    {
      if (index == i)
        newText->Append (ch);

      else
        newText->Append (temp.mValue);

      ++i;
    }

  if (mCachedCharCount == INVALID_CACHE_VALUE)
    mCachedCharCount = i;

  else
    {
      assert (mCachedCharCount == i);
    }

  if (ShareCount () > 0)
    {
      assert (ReferenceCount() == 1);
      Duplicate (*newText.get (), newText->BytesCount ());
    }
  else
    {
      *inoutStrategy = newText.release ();
      DecreaseReferenceCount ();
    }
}


bool
GenericText::IsRowValue() const
{
  return false;
}


TemporalText&
GenericText::GetTemporal()
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}


RowFieldText&
GenericText::GetRow ()
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}



NullText::NullText () :
    GenericText (0)
{
}


NullText::~NullText ()
{
}


uint_t
NullText::ReferenceCount () const
{
  return ~0; //Do not allow one to change us!
}


void
NullText::IncreaseReferenceCount ()
{
  //This is a singleton. Do nothing!
}


void
NullText::DecreaseReferenceCount ()
{
  //This is a singleton. Do nothing!
}


uint_t
NullText::ShareCount () const
{
  return 0; //Do not allow one to change us!
}


void
NullText::IncreaseShareCount ()
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}


void
NullText::DecreaseShareCount ()
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}


void
NullText::ReadUtf8 (const uint64_t offset,
                    const uint64_t count,
                    uint8_t* const buffer)
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}


void
NullText::WriteUtf8 (const uint64_t       offset,
                     const uint64_t       count,
                     const uint8_t* const buffer)
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}


void
NullText::TruncateUtf8 (const uint64_t newSize)
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}


void
NullText::ClearMyself ()
{
  //This is a singleton. Do nothing!
}


NullText&
NullText::GetSingletoneInstace ()
{
  static NullText nullTextInstance;

  return nullTextInstance;
}



RowFieldText::RowFieldText (VariableSizeStore& storage,
                            const uint64_t     firstEntry,
                            const uint64_t     bytesSize)
  : GenericText ((bytesSize == 0) ? 0 : (bytesSize - CACHE_META_DATA_SIZE)),
    mFirstEntry (firstEntry),
    mStorage (storage),
    mTempText (NULL)
{
  if (bytesSize > 0)
    {
      assert (bytesSize > CACHE_META_DATA_SIZE);

      mStorage.RegisterReference ();
      mStorage.IncrementRecordRef (mFirstEntry);

      assert (bytesSize > CACHE_META_DATA_SIZE);

      uint8_t cachedMetaData[CACHE_META_DATA_SIZE];
      mStorage.GetRecord (mFirstEntry,
                          0,
                          CACHE_META_DATA_SIZE,
                          cachedMetaData);

      mCachedCharCount = load_le_int32 (cachedMetaData);
      mCachedCharIndex = load_le_int32 (cachedMetaData + sizeof (uint32_t));

      mCachedCharIndexOffset = load_le_int32 (
                                        cachedMetaData + 2 * sizeof (uint32_t)
                                              );
    }
  else
    {
      mTempText = new TemporalText (NULL);
      mTempText->IncreaseReferenceCount ();
    }
}


RowFieldText::~RowFieldText ()
{
  assert ((mReferenceCount == 0) && (mShareCount == 0));
  assert (mCachedCharCount <= MAX_CHARS_COUNT);
  assert (mCachedCharIndex <= MAX_CHARS_COUNT);
  assert (mCachedCharIndexOffset <= MAX_BYTES_COUNT);

  if (mTempText == NULL)
    {
      uint8_t cachedMetaData[CACHE_META_DATA_SIZE];

      mStorage.GetRecord (mFirstEntry, 0, CACHE_META_DATA_SIZE, cachedMetaData);

      //Do not update the elements count if we have not modified ours
      //Someone else might did it in the mean time, so we will do our
      //best to avoid conflicts.
      if (load_le_int32 (cachedMetaData) < mCachedCharCount)
        store_le_int32 (mCachedCharCount, cachedMetaData);

      //Char index and offset would in the worst case trigger only
      //cache miss hits, but their values should stay valid, as they are
      //protected if the string is truncated or modified.
      store_le_int32 (mCachedCharIndex, cachedMetaData + sizeof (uint32_t));
      store_le_int32 (mCachedCharIndexOffset,
                      cachedMetaData + 2 * sizeof (uint32_t));

      mStorage.UpdateRecord (mFirstEntry,
                             0,
                             CACHE_META_DATA_SIZE,
                             cachedMetaData);

      mStorage.DecrementRecordRef (mFirstEntry);
      mStorage.Flush ();
      mStorage.ReleaseReference ();
    }
  else
    {
      assert (mBytesSize == mTempText->BytesCount ());
      assert ((mCachedCharCount == INVALID_CACHE_VALUE)
              || (mCachedCharCount == mTempText->CharsCount ()));

      mTempText->DecreaseReferenceCount ();
    }
}


void
RowFieldText::ReadUtf8 (const uint64_t offset,
                        const uint64_t count,
                        uint8_t* const buffer)
{
  if (mTempText != NULL)
    {
      mTempText->ReadUtf8 (offset, count, buffer);
      return ;
    }

  assert (mFirstEntry > 0);

  mStorage.GetRecord (mFirstEntry,
                      offset + CACHE_META_DATA_SIZE,
                      count,
                      buffer);
}


void
RowFieldText::WriteUtf8 (const uint64_t       offset,
                         const uint64_t       count,
                         const uint8_t* const buffer)
{
  if ((offset + count) > MAX_BYTES_COUNT)
    throw DBSException (NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED));

  if (mTempText != NULL)
    {
      mTempText->WriteUtf8 (offset, count, buffer);
      mBytesSize = mTempText->BytesCount ();

      return ;
    }

  assert (mFirstEntry > 0);
  assert (mCachedCharCount <= MAX_CHARS_COUNT);
  assert (mCachedCharIndex <= MAX_CHARS_COUNT);
  assert (mCachedCharIndexOffset <= MAX_BYTES_COUNT);

  uint8_t cachedMetaData[CACHE_META_DATA_SIZE];

  store_le_int32 (mCachedCharCount, cachedMetaData);
  store_le_int32 (mCachedCharIndex, cachedMetaData + sizeof (uint32_t));
  store_le_int32 (mCachedCharIndexOffset,
                  cachedMetaData + 2 * sizeof (uint32_t));

  mStorage.UpdateRecord (mFirstEntry, 0, CACHE_META_DATA_SIZE, cachedMetaData);
  mStorage.UpdateRecord (mFirstEntry,
                         offset + CACHE_META_DATA_SIZE,
                         count,
                         buffer);

  if (offset + count > mBytesSize)
    mBytesSize = offset + count;
}


void
RowFieldText::TruncateUtf8 (const uint64_t newSize)
{
  if (newSize >= mBytesSize)
    return;

  if (mTempText == NULL)
  {
    auto_ptr<TemporalText> newText (new TemporalText (NULL));
    newText->Duplicate (*this, newSize);

    mTempText = newText.release ();

    mStorage.DecrementRecordRef (mFirstEntry);
    mStorage.Flush ();
    mStorage.ReleaseReference ();
  }

 else
  mTempText->TruncateUtf8 (newSize);

  mBytesSize             = mTempText->BytesCount ();
  mCachedCharIndex       = 0;
  mCachedCharIndexOffset = 0;

  if (mBytesSize == 0)
    mCachedCharCount = 0;

  else
    mCachedCharCount = INVALID_CACHE_VALUE;
}


void
RowFieldText::UpdateCharAt (const uint32_t   ch,
                            const uint64_t   index,
                            ITextStrategy**  inoutStrategy)
{
  assert (ch != 0);

  const uint32_t utf8CodeUnitsCount = wh_utf8_store_size (ch);

  assert ((utf8CodeUnitsCount > 0)
          && (utf8CodeUnitsCount < UTF8_MAX_BYTES_COUNT));

  const uint32_t oldChar = CharAt (index).mValue;

  assert (oldChar != 0);

  if ((ReferenceCount () > 1)
      || utf8CodeUnitsCount != wh_utf8_store_size (oldChar))
    {
      this->GenericText::UpdateCharAt (ch, index, inoutStrategy);
      return;
    }

  assert (mCachedCharIndex == index);

  uint8_t utf8CodeUnits[UTF8_MAX_BYTES_COUNT];

  wh_store_utf8_cp (ch, utf8CodeUnits);
  WriteUtf8 (mCachedCharIndexOffset, utf8CodeUnitsCount, utf8CodeUnits);
}


bool
RowFieldText::IsRowValue () const
{
  return mTempText == NULL;
}


TemporalText&
RowFieldText::GetTemporal ()
{
  assert (mTempText != NULL);

  return *mTempText;
}


RowFieldText&
RowFieldText::GetRow ()
{
  assert (mTempText == NULL);
  return *this;
}


void
RowFieldText::ClearMyself ()
{
  delete this;
}


TemporalText::TemporalText (const uint8_t* const utf8Str,
                            const uint64_t       bytesCount)
  : GenericText (0),
    mStorage ()
{
  if (utf8Str == NULL)
    return;

  mBytesSize = get_utf8_string_length (utf8Str, bytesCount);
  mStorage.Write (0, mBytesSize, utf8Str);
}


void
TemporalText::ReadUtf8 (const uint64_t offset,
                        const uint64_t count,
                        uint8_t* const buffer)
{
  assert (mBytesSize == mStorage.Size ());

  mStorage.Read (offset, count, buffer);
}


void
TemporalText::WriteUtf8 (const uint64_t       offset,
                         const uint64_t       count,
                         const uint8_t* const buffer)
{
  assert (mBytesSize == mStorage.Size ());

  mStorage.Write (offset, count, buffer);

  mBytesSize = mStorage.Size ();
}


void
TemporalText::TruncateUtf8 (const uint64_t newSize)
{
  assert (mBytesSize == mStorage.Size ());

  mStorage.Colapse (newSize, mBytesSize);

  mBytesSize = newSize;
}


void
TemporalText::UpdateCharAt (const uint32_t   ch,
                            const uint64_t   index,
                            ITextStrategy**  inoutStrategy)
{
  assert (this == *inoutStrategy);
  assert (ch != 0);

  const uint32_t utf8CodeUnitsCount = wh_utf8_store_size (ch);

  assert ((utf8CodeUnitsCount > 0)
          && (utf8CodeUnitsCount < UTF8_MAX_BYTES_COUNT));

  const uint32_t oldChar = CharAt (index).mValue;

  assert (oldChar != 0);

  if ((ReferenceCount () > 1)
      || utf8CodeUnitsCount != wh_utf8_store_size (oldChar))
    {
      this->GenericText::UpdateCharAt (ch, index, inoutStrategy);
      return;
    }

  assert (mCachedCharIndex == index);

  uint8_t utf8CodeUnits[UTF8_MAX_BYTES_COUNT];

  wh_store_utf8_cp (ch, utf8CodeUnits);
  WriteUtf8 (mCachedCharIndexOffset, utf8CodeUnitsCount, utf8CodeUnits);
}


TemporalText&
TemporalText::GetTemporal ()
{
  return *this;
}


void
TemporalText::ClearMyself ()
{
  delete this;
}


} //namespace pastra
} //namespace whisper


