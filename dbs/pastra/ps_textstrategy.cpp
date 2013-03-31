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

#include "utils/include/le_converter.h"
#include "utils/include/utf8.h"

#include "dbs_mgr.h"
#include "dbs_exception.h"

#include "ps_textstrategy.h"

using namespace std;
using namespace pastra;


static D_UINT64
get_utf8_string_size (const D_UINT8 *pUtf8Str, D_UINT64 maxLength)
{
  D_UINT64 result = 0;

  while ((maxLength > 0) && (pUtf8Str[0] != 0))
    {
      const D_UINT charSize = get_utf8_char_size(pUtf8Str[0]);
      bool validChar = true;

      if ((charSize == 0) || (charSize > MIN (6, maxLength)))
        validChar = false;
      else
        for (D_UINT index = 1; (index < charSize) && validChar; ++index)
          if ((pUtf8Str[index] & UTF8_EXTRA_BYTE_MASK) != UTF8_EXTRA_BYTE_SIG)
            validChar = false;

      if (!validChar)
        throw DBSException (NULL, _EXTRA(DBSException::INVALID_UTF8_STRING));

      result += charSize, maxLength -= charSize, pUtf8Str += charSize;
    }

  return result;
}

GenericText::GenericText (D_UINT64 bytesSize)
  : m_BytesSize (bytesSize),
    m_CachedCharCount (INVALID_CACHE_VALUE),
    m_CachedCharIndex (0),
    m_CachedCharIndexOffset (0),
    m_ReferenceCount (0),
    m_ShareCount (0)

{
}

GenericText::~GenericText ()
{
}

D_UINT
GenericText::ReferenceCount () const
{
  return m_ReferenceCount;
}

void
GenericText::IncreaseReferenceCount ()
{
  assert (m_ShareCount == 0);
  ++m_ReferenceCount;
}

void
GenericText::DecreaseReferenceCount ()
{
  assert (m_ShareCount == 0);
  assert (m_ReferenceCount > 0);
  -- m_ReferenceCount;

  if (m_ReferenceCount  == 0)
    ClearMyself ();
}

D_UINT
GenericText::ShareCount () const
{
  return m_ShareCount;
}

void
GenericText::IncreaseShareCount ()
{
  assert (m_ReferenceCount == 1);
  ++m_ShareCount;
}

void
GenericText::DecreaseShareCount ()
{
  assert (m_ShareCount > 0);
  assert (m_ReferenceCount == 1);

  -- m_ShareCount;
}

D_UINT64
GenericText::CharsCount()
{
  if (m_CachedCharCount != INVALID_CACHE_VALUE)
    return m_CachedCharCount;

  D_UINT64 offset = m_CachedCharIndexOffset;
  D_UINT64 result = m_CachedCharIndex;

  while (offset < m_BytesSize)
    {
      D_UINT8 aUtf8Char;
      ReadUtf8 (offset, 1, &aUtf8Char);
      const D_UINT charSize = get_utf8_char_size (aUtf8Char);

      if ((charSize == 0) || (charSize + offset > m_BytesSize))
        throw DBSException (NULL, _EXTRA (DBSException::INVALID_UTF8_STRING));

      ++result, offset += charSize;
    }

  m_CachedCharCount = result;

  return result;
}

D_UINT64
GenericText::BytesCount() const
{
  return m_BytesSize;
}

void
GenericText::Duplicate (I_TextStrategy& source, const D_UINT64 newMaxUtf8Size)
{
  D_UINT64 index = 0;

  Truncate (0);
  assert (m_BytesSize == 0);
  assert (m_CachedCharCount == 0);

  DBSChar temp;
  while (((temp = source.CharAt (index)).IsNull () == false)
         && (m_BytesSize < newMaxUtf8Size))
    {
      Append (temp.m_Value);
      ++index;
    }
  assert (m_BytesSize <= newMaxUtf8Size);
  assert (m_CachedCharCount == index);

  m_CachedCharIndex       = 0;
  m_CachedCharIndexOffset = 0;
}

DBSChar
GenericText::CharAt (D_UINT64 index)
{
  D_UINT64 chIndex  = 0;
  D_UINT64 chOffset = 0;
  D_UINT32 chValue  = 0;

  if (m_CachedCharIndex <= index)
    {
      chIndex   = m_CachedCharIndex;
      chOffset  = m_CachedCharIndexOffset;
    }

  while ((chIndex < index)
          && (chOffset < m_BytesSize))
    {
      D_UINT8 utf8CharCodeUnit;
      ReadUtf8 (chOffset, 1, &utf8CharCodeUnit);
      chOffset += get_utf8_char_size (utf8CharCodeUnit);

      ++chIndex;
    }

  assert (chOffset <= m_BytesSize);
  if (chOffset >= m_BytesSize)
    {
      assert (chOffset == m_BytesSize);
      assert ((m_CachedCharCount == INVALID_CACHE_VALUE)
              || (m_CachedCharCount == chIndex));

      m_CachedCharCount = chIndex;

      return DBSChar ();
    }

  assert (chIndex == index);

  m_CachedCharIndex       = chIndex;
  m_CachedCharIndexOffset = chOffset;

  D_UINT8 aUtf8Char[UTF8_MAX_BYTES_COUNT];
  ReadUtf8 (chOffset,
            MIN (m_BytesSize - chOffset, sizeof aUtf8Char),
            aUtf8Char);

  const D_UINT charSize = decode_utf8_char (aUtf8Char, &chValue);
  assert ((chOffset + charSize) <= m_BytesSize);

  return DBSChar (chValue);
}

void
GenericText::Append (const D_UINT32 charValue)
{
  assert (m_ReferenceCount == 1);
  assert (charValue != 0);

  D_UINT8 aUtf8Encoding[UTF8_MAX_BYTES_COUNT];
  D_UINT  encodeSize = encode_utf8_char (charValue, aUtf8Encoding);

  WriteUtf8 (m_BytesSize, encodeSize, aUtf8Encoding);

  if (m_CachedCharCount != INVALID_CACHE_VALUE)
    ++m_CachedCharCount;
}

void
GenericText::Append (I_TextStrategy& text)
{
  D_UINT64 index = 0;
  DBSChar  temp;

  while ((temp = text.CharAt (index)).IsNull () == false)
    {
      Append (temp.m_Value);
      ++index;
    }
}

void
GenericText::Truncate (D_UINT64 newCharCount)
{
  if ((m_CachedCharCount != INVALID_CACHE_VALUE)
      && (m_CachedCharCount <= newCharCount))
    {
      return ;
    }

  m_CachedCharCount = 0;

  D_UINT64 offset = 0;
  if (m_CachedCharIndex < newCharCount)
    {
      offset             = m_CachedCharIndexOffset;
      m_CachedCharCount  = m_CachedCharIndex;
      newCharCount      -= m_CachedCharIndex + 1;
    }
  else
    {
      m_CachedCharIndex       = 0;
      m_CachedCharIndexOffset = 0;
    }

  while ((newCharCount > 0)
          && (offset < m_BytesSize))
    {
      D_UINT8 utf8Char;
      ReadUtf8 (offset, 1, &utf8Char);
      offset += get_utf8_char_size (utf8Char);

      --newCharCount;
      ++m_CachedCharCount;

      assert (offset <= m_BytesSize);
    }

   TruncateUtf8 (offset);
 }

void
GenericText::UpdateCharAt (const D_UINT32   charValue,
                           const D_UINT64   index,
                           I_TextStrategy** pIOStrategy)
{
  assert (this == *pIOStrategy);
  assert (charValue != 0);

  auto_ptr<I_TextStrategy> newText (new TemporalText(NULL));
  newText->IncreaseReferenceCount();

  D_UINT64 it  = 0;
  DBSChar  temp;
  while ((temp = CharAt (it)).IsNull () == false)
    {
      if (index == it)
        newText->Append (charValue);
      else
        newText->Append (temp.m_Value);

      ++it;
    }

  if (m_CachedCharCount == INVALID_CACHE_VALUE)
    m_CachedCharCount = it;
  else
    {
      assert (m_CachedCharCount == it);
    }

  if (ShareCount () > 0)
    {
      assert (ReferenceCount() == 1);
      Duplicate (*newText.get (), newText->BytesCount ());
    }
  else
    {
      *pIOStrategy = newText.release ();
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


D_UINT
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

D_UINT
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
NullText::ReadUtf8 (const D_UINT64 offset,
                    const D_UINT64 count,
                    D_UINT8* const pBuffDest)
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}

void
NullText::WriteUtf8 (const D_UINT64       offset,
                     const D_UINT64       count,
                     const D_UINT8* const pBuffSrc)
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}

void
NullText::TruncateUtf8 (const D_UINT64 newSize)
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

RowFieldText::RowFieldText (VLVarsStore& storage,
                            D_UINT64 firstEntry,
                            D_UINT64 bytesSize)
  : GenericText ((bytesSize == 0) ? 0 : (bytesSize - CACHE_META_DATA_SIZE)),
    m_FirstEntry (firstEntry),
    m_Storage (storage),
    m_TempText (NULL)
{
  if (bytesSize > 0)
    {
      assert (bytesSize > CACHE_META_DATA_SIZE);

      m_Storage.RegisterReference ();
      m_Storage.IncrementRecordRef (m_FirstEntry);

      assert (bytesSize > CACHE_META_DATA_SIZE);

      D_UINT8 cachedMetaData[CACHE_META_DATA_SIZE];
      m_Storage.GetRecord (m_FirstEntry,
                           0,
                           CACHE_META_DATA_SIZE,
                           cachedMetaData);

      m_CachedCharCount       = from_le_int32 (cachedMetaData);
      m_CachedCharIndex       = from_le_int32 (
                                          cachedMetaData + sizeof (D_UINT32)
                                              );
      m_CachedCharIndexOffset = from_le_int32 (
                                        cachedMetaData + 2 * sizeof (D_UINT32)
                                              );
    }
  else
    {
      m_TempText = new TemporalText (NULL);
      m_TempText->IncreaseReferenceCount ();
    }
}

RowFieldText::~RowFieldText ()
{
  assert ((m_ReferenceCount == 0) && (m_ShareCount == 0));

  assert (m_CachedCharCount <= MAX_CHARS_COUNT);
  assert (m_CachedCharIndex <= MAX_CHARS_COUNT);
  assert (m_CachedCharIndexOffset <= MAX_BYTES_COUNT);

  if (m_TempText == NULL)
    {
      D_UINT8 cachedMetaData[CACHE_META_DATA_SIZE];
      m_Storage.GetRecord (m_FirstEntry,
                           0,
                           CACHE_META_DATA_SIZE,
                           cachedMetaData);

      //Do not update the elements count if we have not modified ours
      //Someone else might did it in the mean time, so we will do our
      //best to avoid conflicts.
      if (from_le_int32 (cachedMetaData) < m_CachedCharCount)
        store_le_int32 (m_CachedCharCount, cachedMetaData);

      //Char index and offset would in the worst case trigger only
      //cache miss hits, but their values should stay valid, as they are
      //protected if the string is truncated or modified.
      store_le_int32 (m_CachedCharIndex,
                      cachedMetaData + sizeof (D_UINT32));
      store_le_int32 (m_CachedCharIndexOffset,
                      cachedMetaData + 2 * sizeof (D_UINT32));

      m_Storage.UpdateRecord (m_FirstEntry,
                              0,
                              CACHE_META_DATA_SIZE,
                              cachedMetaData);

      m_Storage.DecrementRecordRef (m_FirstEntry);
      m_Storage.Flush ();
      m_Storage.ReleaseReference ();
    }
  else
    {
      assert (m_BytesSize == m_TempText->BytesCount ());
      assert ((m_CachedCharCount == INVALID_CACHE_VALUE)
              || (m_CachedCharCount == m_TempText->CharsCount ()));

      m_TempText->DecreaseReferenceCount ();
    }
}

void
RowFieldText::ReadUtf8 (const D_UINT64 offset,
                        const D_UINT64 count,
                        D_UINT8* const pBuffDest)
{
  if (m_TempText != NULL)
    {
      m_TempText->ReadUtf8 (offset, count, pBuffDest);
      return ;
    }

  assert (m_FirstEntry > 0);
  m_Storage.GetRecord (m_FirstEntry,
                       offset + CACHE_META_DATA_SIZE,
                       count,
                       pBuffDest);
}

void
RowFieldText::WriteUtf8 (const D_UINT64      offset,
                         const D_UINT64      count,
                         const D_UINT8*const pBuffSrc)
{
  if ((offset + count) > MAX_BYTES_COUNT)
    throw DBSException (NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED));

  if (m_TempText != NULL)
    {
      m_TempText->WriteUtf8 (offset, count, pBuffSrc);
      m_BytesSize = m_TempText->BytesCount ();

      return ;
    }

  assert (m_FirstEntry > 0);
  assert (m_CachedCharCount <= MAX_CHARS_COUNT);
  assert (m_CachedCharIndex <= MAX_CHARS_COUNT);
  assert (m_CachedCharIndexOffset <= MAX_BYTES_COUNT);

  D_UINT8 cachedMetaData[CACHE_META_DATA_SIZE];
  store_le_int32 (m_CachedCharCount, cachedMetaData);
  store_le_int32 (m_CachedCharIndex, cachedMetaData + sizeof (D_UINT32));
  store_le_int32 (m_CachedCharIndexOffset,
                  cachedMetaData + 2 * sizeof (D_UINT32));

  m_Storage.UpdateRecord (m_FirstEntry,
                          0,
                          CACHE_META_DATA_SIZE,
                          cachedMetaData);

  m_Storage.UpdateRecord (m_FirstEntry,
                          offset + CACHE_META_DATA_SIZE,
                          count,
                          pBuffSrc);

  if (offset + count > m_BytesSize)
    m_BytesSize = offset + count;
}

void
RowFieldText::TruncateUtf8 (const D_UINT64 newSize)
{
  if (newSize >= m_BytesSize)
    return;

  if (m_TempText == NULL)
  {
    std::auto_ptr<TemporalText> temp (new TemporalText (NULL));
    temp->Duplicate (*this, newSize);

    m_TempText  = temp.release ();

    m_Storage.DecrementRecordRef (m_FirstEntry);
    m_Storage.Flush ();
    m_Storage.ReleaseReference ();
  }
 else
  {
    m_TempText->TruncateUtf8 (newSize);
  }

  m_BytesSize             = m_TempText->BytesCount ();
  m_CachedCharIndex       = 0;
  m_CachedCharIndexOffset = 0;

  if (m_BytesSize == 0)
    m_CachedCharCount = 0;
  else
    m_CachedCharCount = INVALID_CACHE_VALUE;
}

void
RowFieldText::UpdateCharAt (const D_UINT32   charValue,
                            const D_UINT64   index,
                            I_TextStrategy** pIOStrategy)
{
  assert (charValue != 0);

  const D_UINT32 utf8CodeUnitsCount = utf8_encode_size (charValue);
  assert ((utf8CodeUnitsCount > 0)
          && (utf8CodeUnitsCount < UTF8_MAX_BYTES_COUNT));

  const D_UINT32 oldChar = CharAt (index).m_Value;
  assert (oldChar != 0);

  if ((ReferenceCount () > 1)
      || utf8CodeUnitsCount != utf8_encode_size (oldChar))
    {
      this->GenericText::UpdateCharAt (charValue, index, pIOStrategy);
      return;
    }

  D_UINT8 utf8CodeUnits[UTF8_MAX_BYTES_COUNT];
  encode_utf8_char (charValue, utf8CodeUnits);
  assert (m_CachedCharIndex == index);
  WriteUtf8 (m_CachedCharIndexOffset,
             utf8CodeUnitsCount,
             utf8CodeUnits);
}


bool
RowFieldText::IsRowValue () const
{
  return m_TempText == NULL;
}

TemporalText&
RowFieldText::GetTemporal ()
{
  assert (m_TempText != NULL);

  return *m_TempText;
}

RowFieldText&
RowFieldText::GetRow ()
{
  assert (m_TempText == NULL);
  return *this;
}


void
RowFieldText::ClearMyself ()
{
  delete this;
}

TemporalText::TemporalText (const D_UINT8 *pUtf8String, D_UINT64 bytesCount)
  : GenericText (0),
    m_Storage (DBSGetSeettings ().m_TempDir.c_str(),
               DBSGetSeettings ().m_VLValueCacheSize)
{
  if (pUtf8String == NULL)
    return;

  m_BytesSize = get_utf8_string_size (pUtf8String, bytesCount);
  m_Storage.Write (0, m_BytesSize, pUtf8String);
}

TemporalText::~TemporalText()
{
}

void
TemporalText::ReadUtf8 (const D_UINT64 offset,
                        const D_UINT64 count,
                        D_UINT8* const pBuffDest)
{
  assert (m_BytesSize == m_Storage.Size ());
  m_Storage.Read (offset, count, pBuffDest);
}

void
TemporalText::WriteUtf8 (const D_UINT64 offset,
                         const D_UINT64 count,
                         const D_UINT8* const pBuffSrc)
{
  assert (m_BytesSize == m_Storage.Size ());
  m_Storage.Write (offset, count, pBuffSrc);

  m_BytesSize = m_Storage.Size ();
}

void
TemporalText::TruncateUtf8 (const D_UINT64 newSize)
{
  assert (m_BytesSize == m_Storage.Size ());
  m_Storage.Colapse (newSize, m_BytesSize);

  m_BytesSize = newSize;
}

void
TemporalText::UpdateCharAt (const D_UINT32   charValue,
                            const D_UINT64   index,
                            I_TextStrategy** pIOStrategy)
{
  assert (this == *pIOStrategy);
  assert (charValue != 0);

  const D_UINT32 utf8CodeUnitsCount = utf8_encode_size (charValue);
  assert ((utf8CodeUnitsCount > 0)
          && (utf8CodeUnitsCount < UTF8_MAX_BYTES_COUNT));

  const D_UINT32 oldChar = CharAt (index).m_Value;
  assert (oldChar != 0);

  if ((ReferenceCount () > 1)
      || utf8CodeUnitsCount != utf8_encode_size (oldChar))
    {
      this->GenericText::UpdateCharAt (charValue, index, pIOStrategy);
      return;
    }

  D_UINT8 utf8CodeUnits[UTF8_MAX_BYTES_COUNT];
  encode_utf8_char (charValue, utf8CodeUnits);
  assert (m_CachedCharIndex == index);
  WriteUtf8 (m_CachedCharIndexOffset,
             utf8CodeUnitsCount,
             utf8CodeUnits);
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

