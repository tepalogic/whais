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
GenericText::Duplicate (I_TextStrategy& source)
{
  D_UINT64 index = 0;

  Truncate (0);
  assert (m_BytesSize == 0);

  DBSChar temp;
  while ((temp = source.CharAt (index)).IsNull () == false)
    {
      Append (temp.m_Value);
      ++index;
    }

  m_CachedCharCount       = index;
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
  if (chOffset == m_BytesSize)
    {
      assert ((m_CachedCharCount == INVALID_CACHE_VALUE)
              || (m_CachedCharCount == chIndex));

      if (m_CachedCharCount == INVALID_CACHE_VALUE)
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
  m_BytesSize += encodeSize;

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
  m_CachedCharCount = newCharCount;

  D_UINT64 offset = 0;
  if (m_CachedCharIndex <= newCharCount)
    {
      offset        = m_CachedCharIndexOffset;
      newCharCount -= m_CachedCharIndex;
    }

  while ((newCharCount > 0)
          && (offset < m_BytesSize))
    {
      D_UINT8 utf8Char;
      ReadUtf8 (offset, 1, &utf8Char);
      offset += get_utf8_char_size (utf8Char);
      --newCharCount;

      assert (offset <= m_BytesSize);
    }

  if (offset == m_BytesSize)
    m_CachedCharCount -= newCharCount;
  else
    {
      assert (offset < m_BytesSize);

      TruncateUtf8 (offset);
      m_BytesSize = offset;
    }

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

  if (ShareCount () > 0)
    {
      assert (ReferenceCount() == 1);
      Duplicate (*newText.get ());
    }
  else
    {
      DecreaseReferenceCount ();
      *pIOStrategy = newText.release ();
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

  //Avoid compiler warning! But never will execute!
  return *_RC(TemporalText*, this);
}

RowFieldText&
GenericText::GetRowValue()
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));

  //Avoid compiler warning! But never will execute!
  return *_RC(RowFieldText*, this);
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
  : GenericText (bytesSize),
    m_FirstEntry (firstEntry),
    m_Storage (storage)
{
  m_Storage.IncrementRecordRef (m_FirstEntry);
  m_Storage.RegisterReference ();
}

RowFieldText::~RowFieldText ()
{
  assert ((m_ReferenceCount == 0) && (m_ShareCount == 0));
  m_Storage.DecrementRecordRef (m_FirstEntry);
  m_Storage.Flush ();
  m_Storage.ReleaseReference ();
}

D_UINT
RowFieldText::ReferenceCount () const
{
  return ~0; //Do not allow one to change us!
}

void
RowFieldText::IncreaseReferenceCount ()
{
  GenericText::IncreaseReferenceCount ();
}

void
RowFieldText::DecreaseReferenceCount ()
{
  GenericText::DecreaseReferenceCount ();
}

D_UINT
RowFieldText::ShareCount () const
{
  return 0; //Do not allow one to change us!
}

void
RowFieldText::IncreaseShareCount ()
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}

void
RowFieldText::DecreaseShareCount ()
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}


void
RowFieldText::ReadUtf8 (const D_UINT64 offset,
                        const D_UINT64 count,
                        D_UINT8* const pBuffDest)
{
  assert (m_FirstEntry > 0);
  m_Storage.GetRecord (m_FirstEntry, offset, count, pBuffDest);
}

void
RowFieldText::WriteUtf8 (const D_UINT64      offset,
                         const D_UINT64      count,
                         const D_UINT8*const pBuffSrc)
{
  assert (m_FirstEntry > 0);
  m_Storage.UpdateRecord (m_FirstEntry, offset, count, pBuffSrc);
}

void
RowFieldText::TruncateUtf8 (const D_UINT64 newSize)
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}

bool
RowFieldText::IsRowValue () const
{
  return true;
}

RowFieldText&
RowFieldText::GetRowValue ()
{
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
}

void
TemporalText::TruncateUtf8 (const D_UINT64 newSize)
{
  assert (m_BytesSize == m_Storage.Size ());
  m_Storage.Colapse (newSize, m_BytesSize);
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
      || (utf8CodeUnitsCount != utf8_encode_size (oldChar)))
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

