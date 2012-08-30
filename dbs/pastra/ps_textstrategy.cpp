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

#include "dbs_mgr.h"
#include "dbs_exception.h"

#include "ps_textstrategy.h"

using namespace std;
using namespace pastra;

static const D_UINT8 UTF8_7BIT_MASK = 0x00;
static const D_UINT8 UTF8_11BIT_MASK = 0xC0;
static const D_UINT8 UTF8_16BIT_MASK = 0xE0;
static const D_UINT8 UTF8_21BIT_MASK = 0xF0;
static const D_UINT8 UTF8_26BIT_MASK = 0xF8;
static const D_UINT8 UTF8_31BIT_MASK = 0xFC;
static const D_UINT8 UTF8_37BIT_MASK = 0xFE;

static const D_UINT8 UTF8_EXTRA_BYTE_SIG = 0x80;
static const D_UINT8 UTF8_EXTRA_BYTE_MASK  = 0xC0;
static const D_UINT8 UTF8_MAX_BYTES_COUNT = 0x8;

static D_UINT
get_utf8_char_size (D_UINT8 firstUtf8Byte)
{
  if ((firstUtf8Byte & UTF8_EXTRA_BYTE_SIG) == UTF8_7BIT_MASK)
    return 1;
  else if ((firstUtf8Byte & UTF8_16BIT_MASK) == UTF8_11BIT_MASK)
    return 2;
  else if ((firstUtf8Byte & UTF8_21BIT_MASK) == UTF8_16BIT_MASK)
    return 3;
  else if ((firstUtf8Byte & UTF8_26BIT_MASK) == UTF8_21BIT_MASK)
    return 4;
  else if ((firstUtf8Byte & UTF8_31BIT_MASK) == UTF8_26BIT_MASK)
    return 5;
  else if ((firstUtf8Byte & UTF8_37BIT_MASK) == UTF8_31BIT_MASK)
    return 6;

  return 0;
}

static D_UINT
decode_utf8_char (const D_UINT8 *pSource, D_UINT32 &outChar)
{
  outChar = 0;
  if ((pSource[0] & UTF8_EXTRA_BYTE_SIG) == UTF8_7BIT_MASK)
    {
      outChar |= pSource[0] & ~UTF8_7BIT_MASK;
      return 1;
    }
  else if ((pSource[0] & UTF8_16BIT_MASK) == UTF8_11BIT_MASK)
    {
      outChar |= pSource[0] & ~UTF8_11BIT_MASK;
      outChar <<= 6; outChar |= pSource[1] & ~UTF8_EXTRA_BYTE_SIG;
      return 2;
    }
  else if ((pSource[0] & UTF8_21BIT_MASK) == UTF8_16BIT_MASK)
    {
      outChar |= pSource[0] & ~UTF8_16BIT_MASK;
      outChar <<= 6; outChar |= pSource[1] & ~UTF8_EXTRA_BYTE_SIG;
      outChar <<= 6; outChar |= pSource[2] & ~UTF8_EXTRA_BYTE_SIG;
      return 3;
    }
  else if ((pSource[0] & UTF8_26BIT_MASK) == UTF8_21BIT_MASK)
    {
      outChar |= pSource[0] & ~UTF8_21BIT_MASK;
      outChar <<= 6; outChar |= pSource[1] & ~UTF8_EXTRA_BYTE_SIG;
      outChar <<= 6; outChar |= pSource[2] & ~UTF8_EXTRA_BYTE_SIG;
      outChar <<= 6; outChar |= pSource[3] & ~UTF8_EXTRA_BYTE_SIG;
      return 4;
    }
  else if ((pSource[0] & UTF8_31BIT_MASK) == UTF8_26BIT_MASK)
    {
      outChar |= pSource[0] & ~UTF8_26BIT_MASK;
      outChar <<= 6; outChar |= pSource[1] & ~UTF8_EXTRA_BYTE_SIG;
      outChar <<= 6; outChar |= pSource[2] & ~UTF8_EXTRA_BYTE_SIG;
      outChar <<= 6; outChar |= pSource[3] & ~UTF8_EXTRA_BYTE_SIG;
      outChar <<= 6; outChar |= pSource[4] & ~UTF8_EXTRA_BYTE_SIG;
      return 5;
    }
  else if ((pSource[0] & UTF8_37BIT_MASK) == UTF8_31BIT_MASK)
    {
        outChar |= pSource[0] & ~UTF8_31BIT_MASK;
        outChar <<= 6; outChar |= pSource[1] & ~UTF8_EXTRA_BYTE_SIG;
        outChar <<= 6; outChar |= pSource[2] & ~UTF8_EXTRA_BYTE_SIG;
        outChar <<= 6; outChar |= pSource[3] & ~UTF8_EXTRA_BYTE_SIG;
        outChar <<= 6; outChar |= pSource[4] & ~UTF8_EXTRA_BYTE_SIG;
        outChar <<= 6; outChar |= pSource[5] & ~UTF8_EXTRA_BYTE_SIG;
        return 6;
    }

  return 0;
}

static D_UINT
encode_utf8_char (D_UINT32 ch, D_UINT8 *pDest)
{
  if (ch < 0x80)
    {
      pDest[0] = _SC(D_UINT8, ch & 0xFF);
      return 1;
    }
  else if (ch < 0x800)
    {
      pDest[0] = _SC (D_UINT8, (ch >> 6) & 0xFF) | UTF8_11BIT_MASK;
      pDest[1] = _SC (D_UINT8, ch & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      return 2;
    }
  else if (ch < 0x10000)
    {
      pDest[0] = _SC (D_UINT8, (ch >> 12) & 0xFF) | UTF8_16BIT_MASK;
      pDest[1] = _SC (D_UINT8, (ch >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[2] = _SC (D_UINT8, ch & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      return 3;
    }
  else if (ch < 0x200000)
    {
      pDest[0] = _SC (D_UINT8, (ch >> 18) & 0xFF) | UTF8_21BIT_MASK;
      pDest[1] = _SC (D_UINT8, (ch >> 12) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[2] = _SC (D_UINT8, (ch >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[3] = _SC (D_UINT8, ch & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      return 4;
    }
  else if (ch < 0x4000000)
    {
      pDest[0] = _SC (D_UINT8, (ch >> 24) & 0xFF) | UTF8_26BIT_MASK;
      pDest[1] = _SC (D_UINT8, (ch >> 18) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[2] = _SC (D_UINT8, (ch >> 12) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[3] = _SC (D_UINT8, (ch >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[4] = _SC (D_UINT8, ch & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      return 5;
    }
  else if (ch < 0x80000000)
    {
      pDest[0] = _SC (D_UINT8, (ch >> 30) & 0xFF) | UTF8_31BIT_MASK;
      pDest[1] = _SC (D_UINT8, (ch >> 24) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[2] = _SC (D_UINT8, (ch >> 18) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[3] = _SC (D_UINT8, (ch >> 12) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[4] = _SC (D_UINT8, (ch >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[5] = _SC (D_UINT8, ch & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      return 6;
    }

  return 0;
}

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

  D_UINT64 offset = 0;
  D_UINT64 result = 0;

  while (offset < m_BytesSize)
    {
      D_UINT8 aUtf8Char;
      ReadUtf8 (offset, 1, &aUtf8Char);
      const D_UINT charSize = get_utf8_char_size (aUtf8Char);

      if ((charSize == 0) || (charSize + offset > m_BytesSize))
        throw DBSException (NULL, _EXTRA (DBSException::INVALID_UTF8_STRING));

      ++result, offset += charSize;
    }

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

  D_UINT64 offset = 0;
  D_UINT64 count = source.BytesCount();

  Truncate (0);
  assert (m_BytesSize == 0);

  while (count > 0)
    {
        D_UINT8 dataChunk[512];
        const D_UINT64 chunkToTansfer = MIN (sizeof dataChunk, count);
        source.ReadUtf8 (offset, chunkToTansfer, dataChunk);
        WriteUtf8 (offset, chunkToTansfer, dataChunk);

        m_BytesSize += chunkToTansfer,
        offset += chunkToTansfer,
        count -= chunkToTansfer;
    }
}

DBSChar
GenericText::CharAt (D_UINT64 index)
{
  if (index >= CharsCount() )
    return DBSChar (); //Null char!

  D_UINT64 offset = 0;
  D_UINT32 charValue = 0;

  while (index > 0)
    {
      D_UINT8 utf8Char;
      ReadUtf8 (offset, 1, &utf8Char);
      offset += get_utf8_char_size (utf8Char);
      -- index;

      assert (offset < m_BytesSize);
    }

  D_UINT8 aUtf8Char[UTF8_MAX_BYTES_COUNT];
  ReadUtf8 (offset,
                   MIN (m_BytesSize - offset, sizeof aUtf8Char),
                   aUtf8Char);

  const D_UINT charSize = decode_utf8_char (aUtf8Char, charValue);
  assert ((offset + charSize) <= m_BytesSize);

  return DBSChar (charValue);
}

void
GenericText::Append (const D_UINT32 charValue)
{
  assert (m_ReferenceCount == 1);

  D_UINT8 aUtf8Encoding[UTF8_MAX_BYTES_COUNT];
  D_UINT encodeSize = encode_utf8_char (charValue, aUtf8Encoding);

  WriteUtf8 (m_BytesSize, encodeSize, aUtf8Encoding);
  m_BytesSize += encodeSize;

}

void
GenericText::Append (I_TextStrategy& text)
{
  D_UINT64 toAppend = text.BytesCount ();
  D_UINT64 appendOffset = 0;

  while (toAppend > 0)
    {
      D_UINT8 chunkData [512];
      D_UINT64 chunkSize = MIN (sizeof chunkData, toAppend);

      text.ReadUtf8 (appendOffset, chunkSize, chunkData);
      WriteUtf8 (m_BytesSize, chunkSize, chunkData);

      m_BytesSize  += chunkSize;
      appendOffset += chunkSize;
      toAppend     -= chunkSize;
    }
}

void
GenericText::Truncate (D_UINT64 newCharCount)
{
  if (newCharCount >= CharsCount ())
    return;

  D_UINT offset = 0;
  while (newCharCount > 0)
    {
      D_UINT8 utf8Char;
      ReadUtf8 (offset, 1, &utf8Char);
      offset += get_utf8_char_size (utf8Char);
      --newCharCount;

      assert (offset < m_BytesSize);
    }

  TruncateUtf8 (offset);
  m_BytesSize = offset;
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

