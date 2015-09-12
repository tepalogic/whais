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
#include <algorithm>

#include "utils/endianness.h"
#include "utils/wutf.h"
#include "utils/wunicode.h"

#include "dbs/dbs_mgr.h"
#include "dbs_exception.h"

#include "ps_textstrategy.h"

using namespace std;

namespace whais {

ITextStrategy::ITextStrategy (uint32_t charsCount)
  : mMatcher (NULL),
    mCachedCharsCount (charsCount),
    mCachedCharIndex (0),
    mCachedCharIndexOffset (0),
    mMirrorsCount (0),
    mCopyReferences (1)
{
}


ITextStrategy::~ITextStrategy ()
{
  delete mMatcher;
};

bool
ITextStrategy::operator== (ITextStrategy& o)
{
  if (this == &o)
    return true;

  DoubleLockRAII<Lock> _l (mLock, o.mLock);

  const uint64_t utf8Count = Utf8CountU ();
  if (utf8Count != o.Utf8CountU ())
    return false;

  uint8_t buff1[128];
  uint8_t buff2[sizeof buff1];

  uint64_t offset = 0;
  while (offset < utf8Count)
    {
      const uint_t chunkSize = MIN (sizeof buff1, utf8Count - offset);
      ReadUtf8U (offset, chunkSize, buff1);
      o.ReadUtf8U (offset, chunkSize, buff2);

      if (memcmp (buff1, buff2, chunkSize) != 0)
        return false;

      offset += chunkSize;
    }

  assert (offset == utf8Count);

  return true;
}

uint64_t
ITextStrategy::CharsCount ()
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  return mCachedCharsCount;
}

uint64_t
ITextStrategy::CharsUntilOffset (const uint64_t offset)
{
  LockRAII<Lock> _l (mLock);
  return CharsUntilOffsetU (offset);
}

uint64_t
ITextStrategy::CharsUntilOffsetU (const uint64_t offset)
{
  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  const uint64_t maxOffset = MIN (offset, Utf8CountU ());

  if (mCachedCharIndexOffset > offset)
    mCachedCharIndex = mCachedCharIndexOffset = 0;

  uint64_t buffOffset = 0, buffValid = 0;
  uint8_t tempBuffer[256];
  while (mCachedCharIndexOffset < maxOffset)
    {
      if (buffOffset >= buffValid)
        {
          buffOffset = 0;
          buffValid  = MIN (sizeof tempBuffer,
                            maxOffset - mCachedCharIndexOffset);
          ReadUtf8U (mCachedCharIndexOffset, buffValid, tempBuffer);
        }

      const uint_t cuCount = wh_utf8_cu_count (tempBuffer[buffOffset]);

      mCachedCharIndexOffset += cuCount;
      buffOffset             += cuCount;
      ++mCachedCharIndex;
    }

  assert (mCachedCharIndex <= mCachedCharsCount);

  return mCachedCharIndex;
}


uint64_t
ITextStrategy::OffsetOfChar (const uint64_t index)
{
  LockRAII<Lock> _l (mLock);
  return OffsetOfCharU (index);
}


uint64_t
ITextStrategy::OffsetOfCharU (const uint64_t index)
{
  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  if (index == mCachedCharsCount)
    return Utf8CountU ();

  else if (index > mCachedCharsCount)
    throw DBSException (_EXTRA (DBSException::STRING_INDEX_TOO_BIG));

  else if (index < mCachedCharIndex / 2)
    mCachedCharIndex = mCachedCharIndexOffset = 0;

  else if (index > (mCachedCharsCount + mCachedCharIndex) / 2)
    {
      mCachedCharIndex       = mCachedCharsCount;
      mCachedCharIndexOffset = Utf8CountU ();
    }
  else if (mCachedCharIndex == index)
    return mCachedCharIndexOffset;

  if (mCachedCharIndex < index)
    {
      const uint64_t maxOffset = Utf8CountU ();
      uint64_t buffOffset = 0, buffValid = 0;
      uint8_t tempBuffer[256];
      while (mCachedCharIndex < index)
        {
          if (buffOffset >= buffValid)
            {
              buffOffset = 0;
              buffValid  = MIN (sizeof tempBuffer,
                                maxOffset - mCachedCharIndexOffset);
              ReadUtf8U (mCachedCharIndexOffset, buffValid, tempBuffer);
            }

          const uint_t cuCount = wh_utf8_cu_count (tempBuffer[buffOffset]);

          mCachedCharIndexOffset += cuCount;
          buffOffset             += cuCount;
          ++mCachedCharIndex;
        }
    }
  else
    {
      uint64_t lastOffset  = mCachedCharIndexOffset;
      uint64_t startOffset = 0;
      uint_t   tempValid   = 0;
      uint8_t  tempBuffer[128];
      while (mCachedCharIndex > index)
        {
          startOffset = max<int64_t> (lastOffset - sizeof tempBuffer, 0);
          tempValid = lastOffset - startOffset;
          ReadUtf8U (startOffset, tempValid, tempBuffer);
          do
            {
              const uint64_t i = --lastOffset - startOffset;
              if ((tempBuffer[i] & UTF8_EXTRA_BYTE_MASK) != UTF8_EXTRA_BYTE_SIG)
                {
                  mCachedCharIndexOffset = lastOffset;
                  --mCachedCharIndex;

                  if (index == mCachedCharIndex)
                    break;
                }
            }
          while (startOffset < lastOffset);
        }
    }

  return mCachedCharIndexOffset;
}


DChar
ITextStrategy::CharAt (const uint64_t index)
{
  LockRAII<Lock> _l (mLock);

  if (index > mCachedCharsCount)
    throw DBSException (_EXTRA (DBSException::STRING_INDEX_TOO_BIG));

  return CharAtU (index);
}


DChar
ITextStrategy::CharAtU (const uint64_t index)
{
  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  assert (index <= mCachedCharsCount);

  if (index == mCachedCharsCount)
    return DChar ();

  else if (index < mCachedCharIndex / 2)
    mCachedCharIndex = mCachedCharIndexOffset = 0;

  else if (index > (mCachedCharsCount + mCachedCharIndex) / 2)
    {
      mCachedCharIndex       = mCachedCharsCount;
      mCachedCharIndexOffset = Utf8CountU ();
    }

  if (mCachedCharIndex <= index)
    {
      const uint64_t maxOffset = Utf8CountU ();
      uint64_t buffOffset = 0, buffValid = 0;
      uint8_t tempBuffer[256];
      while (true)
        {
          if (buffOffset >= buffValid)
            {
              buffOffset = 0;
              buffValid  = MIN (sizeof tempBuffer,
                                maxOffset - mCachedCharIndexOffset);
              ReadUtf8U (mCachedCharIndexOffset, buffValid, tempBuffer);
            }

          const uint_t cuCount = wh_utf8_cu_count (tempBuffer[buffOffset]);

          if (mCachedCharIndex == index)
            {
              uint32_t cp = 0;

              if (cuCount + buffOffset >= buffValid)
                {
                  buffOffset = 0;
                  ReadUtf8U (mCachedCharIndexOffset, cuCount, tempBuffer);
                }

              wh_load_utf8_cp (tempBuffer + buffOffset, &cp);

              assert (cp > 0);
              assert (mCachedCharIndexOffset < maxOffset);
              assert (mCachedCharIndex < mCachedCharsCount);

              return DChar (cp);
            }

          mCachedCharIndexOffset += cuCount;
          buffOffset             += cuCount;
          ++mCachedCharIndex;
        }
    }
  else
    {
      uint64_t lastOffset  = mCachedCharIndexOffset;
      uint64_t startOffset = 0;
      uint_t   tempValid   = 0;
      uint8_t  tempBuffer[128];
      while (true)
        {
          if (mCachedCharIndex == index)
            {
              assert (lastOffset == mCachedCharIndexOffset);
              assert (lastOffset - startOffset < tempValid);

              uint64_t i           = lastOffset - startOffset;
              const uint_t cuCount = wh_utf8_cu_count (tempBuffer[i]);
              if (cuCount + i > tempValid)
                {
                  i = 0;
                  ReadUtf8U (lastOffset, cuCount, tempBuffer);
                }

              uint32_t cp = 0;
              wh_load_utf8_cp (tempBuffer + i, &cp);

              assert (cp > 0);
              assert (mCachedCharIndex < mCachedCharsCount);
              assert (mCachedCharIndex == index);

              return DChar (cp);
            }

          startOffset = max<int64_t> (lastOffset - sizeof tempBuffer, 0);
          tempValid = lastOffset - startOffset;
          ReadUtf8U (startOffset, tempValid, tempBuffer);
          do
            {
              const uint64_t i = --lastOffset - startOffset;
              if ((tempBuffer[i] & UTF8_EXTRA_BYTE_MASK) != UTF8_EXTRA_BYTE_SIG)
                {
                  mCachedCharIndexOffset = lastOffset;
                  --mCachedCharIndex;

                  if (index == mCachedCharIndex)
                    break;
                }
            }
          while (startOffset < lastOffset);
        }
    }

  assert (false);

  throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


ITextStrategy*
ITextStrategy::DuplicateU ()
{
  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  auto_ptr<ITextStrategy> result (new pastra::TemporalText ());

  const uint64_t utf8Count = Utf8CountU ();
  uint64_t offset = 0;
  uint8_t tempBuffer[256];
  while (offset < utf8Count)
    {
      const uint_t chunkSize  = MIN (sizeof tempBuffer, utf8Count - offset);
      ReadUtf8U (offset, chunkSize, tempBuffer);
      result->WriteUtf8U (offset, chunkSize, tempBuffer);

      offset += chunkSize;
    }

  assert (offset == utf8Count);

  result->mCachedCharsCount      = mCachedCharsCount;
  result->mCachedCharIndexOffset = mCachedCharIndexOffset;
  result->mCachedCharIndex       = mCachedCharIndex;

  return result.release ();
}


ITextStrategy*
ITextStrategy::ToCase (const bool toLower)
{
  LockRAII<Lock> _l (mLock);
  return ToCaseU (toLower);
}


ITextStrategy*
ITextStrategy::ToCaseU (const bool toLower)
{
  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  auto_ptr<ITextStrategy> result (new pastra::TemporalText ());

  const uint64_t utf8Count = Utf8CountU ();
  uint64_t offset = 0, charsRemained = mCachedCharsCount;
  uint8_t tempBuffer[256];
  while (charsRemained > 0)
    {
      assert (offset < utf8Count);

      const uint_t chunkSize  = MIN (sizeof tempBuffer, utf8Count - offset);
      ReadUtf8U (offset, chunkSize, tempBuffer);

      uint_t buffValid = 0;
      while (charsRemained > 0)
      {
        uint32_t cp = 0;
        if (buffValid >= chunkSize)
          break;

        const uint8_t cuCount = wh_utf8_cu_count (tempBuffer[buffValid]);
        if (buffValid + cuCount > chunkSize)
          break;

        wh_load_utf8_cp (tempBuffer + buffValid, &cp);
        wh_store_utf8_cp (toLower
                            ? wh_to_lowercase (cp)
                            : wh_to_uppercase (cp),
                          tempBuffer + buffValid);
        buffValid += cuCount;
        --charsRemained;
      }

      assert (buffValid <= chunkSize);
      assert ((charsRemained == 0) || (buffValid > 0));

      result->WriteUtf8U (offset, buffValid, tempBuffer);
      offset += buffValid;
    }

  assert (offset == utf8Count);

  result->mCachedCharsCount      = mCachedCharsCount;
  result->mCachedCharIndexOffset = mCachedCharIndex;
  result->mCachedCharIndex       = mCachedCharIndexOffset;

  return result.release ();
}


ITextStrategy*
ITextStrategy::Append (const uint32_t ch)
{
  LockRAII<Lock> _l (mLock);
  return AppendU (ch);
}


ITextStrategy*
ITextStrategy::AppendU (const uint32_t ch)
{
  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);
  assert (ch != 0);

  auto_ptr<ITextStrategy> keeper (NULL);
  ITextStrategy* result = this;

  if (ReferenceCount () > 1)
    {
      keeper.reset (DuplicateU ());
      result = keeper.get ();
    }

  assert (result->mCachedCharsCount == mCachedCharsCount);
  assert (result->Utf8CountU () == Utf8CountU ());

  uint8_t buffer[16];
  const uint_t cuCount = wh_store_utf8_cp (ch, buffer);

  assert (cuCount < sizeof buffer);

  result->WriteUtf8U (result->Utf8CountU (), cuCount, buffer);
  result->mCachedCharsCount++;

  if (result == this)
    {
      delete mMatcher;
      mMatcher = NULL;
    }

  keeper.release ();
  return result;
}

ITextStrategy*
ITextStrategy::Append (ITextStrategy& text)
{
  DoubleLockRAII<Lock> _l (text.mLock, mLock);
  return AppendU (text);
}


ITextStrategy*
ITextStrategy::AppendU (ITextStrategy& text)
{
  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  if (text.Utf8CountU () == 0)
    return this;

  auto_ptr<ITextStrategy> keeper (NULL);
  ITextStrategy* result = this;

  if (ReferenceCount () > 1)
    {
      keeper.reset (DuplicateU ());
      result = keeper.get ();
    }

  assert (result->mCachedCharsCount == mCachedCharsCount);
  assert (result->Utf8CountU () == Utf8CountU ());

  result->mCachedCharIndex       = mCachedCharsCount;
  result->mCachedCharIndexOffset = result->Utf8CountU ();

  uint8_t tempBuffer[256];
  uint64_t offset = 0;
  const uint64_t utf8Count = text.Utf8CountU ();
  while (offset < utf8Count)
    {
      const uint_t chunkSize = MIN (sizeof tempBuffer, utf8Count - offset);

      text.ReadUtf8U (offset, chunkSize, tempBuffer);
      result->WriteUtf8U (result->Utf8CountU (), chunkSize, tempBuffer);

      offset += chunkSize;
    }
  result->mCachedCharsCount += text.mCachedCharsCount;

  assert (offset == utf8Count);

  if (result == this)
    {
      delete mMatcher;
      mMatcher = NULL;
    }

  keeper.release ();
  return result;
}


ITextStrategy*
ITextStrategy::Append (ITextStrategy&   text,
                       const uint64_t   utf8OffFrom,
                       const uint64_t   utf8OffTo)
{
  DoubleLockRAII<Lock> _l (text.mLock, mLock);
  return AppendU (text, utf8OffFrom, utf8OffTo);
}


ITextStrategy*
ITextStrategy::AppendU (ITextStrategy&   text,
                        const uint64_t   utf8OffFrom,
                        const uint64_t   utf8OffTo)
{
  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  if ((utf8OffFrom >= text.Utf8CountU ())
      || (utf8OffTo <= utf8OffFrom))
    {
      return this;
    }

  auto_ptr<ITextStrategy> keeper (NULL);
  ITextStrategy* result = this;

  if (ReferenceCount () > 1)
    {
      keeper.reset (DuplicateU ());
      result = keeper.get ();
    }

  assert (result->mCachedCharsCount == mCachedCharsCount);
  assert (result->Utf8CountU () == Utf8CountU ());

  result->mCachedCharIndex       = mCachedCharsCount;
  result->mCachedCharIndexOffset = result->Utf8CountU ();

  uint8_t tempBuffer[256];
  uint64_t offset = utf8OffFrom;
  const uint64_t utf8To = MIN (text.Utf8CountU (), utf8OffTo);
  while (offset < utf8To)
    {
      const uint_t chunkSize = MIN (sizeof tempBuffer, utf8To - offset);
      text.ReadUtf8U (offset, chunkSize, tempBuffer);

      uint_t buffValid = 0;
      while (buffValid < chunkSize)
        {
          const uint8_t cuCount = wh_utf8_cu_count (tempBuffer[buffValid]);

          if (buffValid + cuCount > chunkSize)
            break;

          buffValid += cuCount;
          result->mCachedCharsCount++;
        }

      assert (buffValid <= chunkSize);
      assert (buffValid > 0);

      result->WriteUtf8U (result->Utf8CountU (), buffValid, tempBuffer);
      offset += buffValid;
    }

  assert (offset == utf8To);

  if (result == this)
    {
      delete mMatcher;
      mMatcher = NULL;
    }

  keeper.release ();
  return result;
}



ITextStrategy*
ITextStrategy::UpdateCharAt (const uint32_t   newCh,
                             const uint64_t   index)
{
  LockRAII<Lock> _l (mLock);
  return UpdateCharAtU (newCh, index);
}


int
ITextStrategy::CompareTo (ITextStrategy& s)
{
  DoubleLockRAII<Lock> _l (mLock, s.mLock);

  const uint64_t charsCount = max (mCachedCharsCount, s.mCachedCharsCount);

  DChar c1, c2;
  for (uint64_t i = 0;  i < charsCount; ++i)
    {
      c1 = CharAtU (i);
      c2 = s.CharAtU (i);

      if (c1 != c2)
        break;
    }

  if (c1 < c2)
    return -1;

  else if (c1 == c2)
    return 0;

  return 1;
}


ITextStrategy*
ITextStrategy::UpdateCharAtU (const uint32_t   newCh,
                              const uint64_t   index)
{
  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  if (index == mCachedCharsCount)
    return AppendU (newCh);

  const DChar oldCh = CharAtU (index);

  if (DChar (newCh) == oldCh)
    return this;

  uint8_t newBuff[8];
  const uint_t oldCUCnt = wh_utf8_store_size (oldCh.mValue);
  const uint_t newCUCnt = wh_store_utf8_cp (newCh, newBuff);

  auto_ptr<ITextStrategy> keeper (NULL);
  ITextStrategy* result = this;

  if (ReferenceCount () > 1)
    {
      keeper.reset (DuplicateU ());
      result = keeper.get ();
    }

  assert (result->mCachedCharIndex == mCachedCharIndex);
  assert (result->mCachedCharIndexOffset == mCachedCharIndexOffset);
  assert (result->mCachedCharsCount == mCachedCharsCount);
  assert (mCachedCharIndexOffset == index);

  if (newCh == 0)
    {
      assert (result == this);
      assert (result->mCachedCharIndex == index);

      result->TruncateUtf8U (mCachedCharIndexOffset);

      result->mCachedCharsCount = mCachedCharIndex;
      result->mCachedCharIndex  = result->mCachedCharIndexOffset = 0;

    }
  else if ((oldCUCnt == newCUCnt)
           || (oldCh.IsNull ()))
    {
      if (oldCh.IsNull ())
        {
          assert (mCachedCharsCount == index);

          result->mCachedCharIndexOffset = result->Utf8CountU ();
          result->mCachedCharIndex       = result->mCachedCharsCount++;
        }

      assert (result->mCachedCharIndex < result->mCachedCharsCount);

      result->WriteUtf8U (result->mCachedCharIndexOffset, newCUCnt, newBuff);
    }
  else if (oldCUCnt < newCUCnt)
    {
        uint8_t buffer[256];
        uint64_t  offset = result->Utf8CountU ();
        const uint64_t from = mCachedCharIndexOffset + oldCUCnt;
        while (from < offset)
          {
            const uint_t chunkSize = MIN (sizeof buffer, offset - from);

            assert (offset - chunkSize >= mCachedCharIndexOffset);

            result->ReadUtf8U (offset - chunkSize, chunkSize, buffer);
            result->WriteUtf8U (offset - chunkSize + (newCUCnt - oldCUCnt),
                                chunkSize,
                                buffer);
            offset -= chunkSize;
          }

        assert (offset == from);

        result->WriteUtf8U (mCachedCharIndexOffset, newCUCnt, newBuff);
    }
  else
    {
      uint8_t buffer[256];

      result->WriteUtf8U (mCachedCharIndexOffset, newCUCnt, newBuff);
      const uint64_t utf8Count = result->Utf8CountU ();
      uint64_t offset = mCachedCharIndexOffset + oldCUCnt;
      while (offset < utf8Count)
        {
          const uint_t chunkSize = MIN (sizeof buffer, utf8Count - offset);
          result->ReadUtf8U (offset, chunkSize, buffer);
          result->WriteUtf8U (offset - (oldCUCnt - newCUCnt),
                              chunkSize,
                              buffer);
          offset += chunkSize;
        }

      assert (offset == utf8Count);

      result->TruncateUtf8U (result->Utf8CountU () - (oldCUCnt - newCUCnt));
    }

  if (result == this)
    {
      delete mMatcher;
      mMatcher = NULL;
    }

  keeper.release ();
  return result;
}


DUInt64
ITextStrategy::FindMatch (ITextStrategy&   text,
                          const uint64_t   fromCh,
                          const uint64_t   toCh,
                          const bool       ignoreCase)
{
  DoubleLockRAII<Lock> _l (mLock, text.mLock);

  if ((mCachedCharsCount == 0)
      || (toCh <= fromCh)
      || (toCh - fromCh < mCachedCharsCount))
    {
      return DUInt64 ();
    }

  if (mMatcher == NULL)
    mMatcher = new pastra::StringMatcher (*this);

  const int64_t res = mMatcher->FindMatch (text, fromCh, toCh, ignoreCase);

  assert ((res < 0)
          || ((fromCh <= (uint64_t)res)
              && ((uint64_t)res <= toCh - mCachedCharsCount)));

  return ((res < 0) ? DUInt64 () : DUInt64 (res));
}


ITextStrategy*
ITextStrategy::Replace (ITextStrategy&   text,
                        ITextStrategy&   newSubstr,
                        const uint64_t   fromCh,
                        const uint64_t   toCh,
                        const bool       ignoreCase)
{
  TripleLockRAII<Lock> _l (mLock, text.mLock, newSubstr.mLock);

  std::auto_ptr<ITextStrategy> _ns (NULL);
  ITextStrategy* ns = &newSubstr;

  if (this == ns)
    return text.DuplicateU ();

  else if (&text == ns)
    {
      _ns.reset (newSubstr.DuplicateU ());
      ns = _ns.get ();
    }

  const uint64_t subStrSize = Utf8CountU ();
  std::auto_ptr<ITextStrategy> result (new pastra::TemporalText ());

  if (mMatcher == NULL)
    mMatcher = new pastra::StringMatcher (*this);

  int64_t lastMatchPos = 0;
  int64_t  matchPos    = mMatcher->FindMatchRaw (text,
                                                 fromCh,
                                                 toCh,
                                                 ignoreCase);
  while (matchPos >= 0)
    {
      result->AppendU (text, lastMatchPos, matchPos);
      result->AppendU (*ns);

      lastMatchPos = matchPos + subStrSize;
      assert (lastMatchPos <= _SC (int64_t, text.Utf8CountU ()));

      matchPos = mMatcher->FindMatchRaw (text,
                                         text.CharsUntilOffsetU (lastMatchPos),
                                         toCh,
                                         ignoreCase);

      assert ((matchPos < 0) || (lastMatchPos <= matchPos));
    }

  if (matchPos < 0)
    matchPos = text.Utf8CountU ();

  result->AppendU (text, lastMatchPos, matchPos);

  return result.release ();
}

uint64_t
ITextStrategy::Utf8Count ()
{
  LockRAII<Lock> _l (mLock);
  return Utf8CountU ();
}


void
ITextStrategy::ReadUtf8 (const uint64_t offset,
                          const uint64_t count,
                          uint8_t* const dest)
{
  LockRAII<Lock> _l (mLock);
  return ReadUtf8U (offset, count, dest);
}


void
ITextStrategy::WriteUtf8 (const uint64_t       offset,
                          const uint64_t       count,
                          const uint8_t* const buffer)
{
  LockRAII<Lock> _l (mLock);

  delete mMatcher;
  mMatcher = NULL;

  return WriteUtf8U (offset, count, buffer);
}

void
ITextStrategy::TruncateUtf8 (const uint64_t offset)
{
  LockRAII<Lock> _l (mLock);
  return TruncateUtf8U (offset);
}



void
ITextStrategy::ReleaseReference ()
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  if (MirrorsCount () > 0)
    --mMirrorsCount;

  else
    --mCopyReferences;

  if (ReferenceCount () == 0)
    {
      _l.Release ();
      delete this;
    }
}

uint32_t
ITextStrategy::ReferenceCount () const
{
  return mCopyReferences;
}

uint32_t
ITextStrategy::MirrorsCount () const
{
  return mMirrorsCount;
}

ITextStrategy*
ITextStrategy::MakeMirrorCopy ()
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  if (ReferenceCount () > 1)
    return DuplicateU ()->MakeMirrorCopy ();

  ++mMirrorsCount;
  return this;
}

ITextStrategy*
ITextStrategy::MakeClone ()
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount () == 1));
  assert (ReferenceCount () > 0);

  if (MirrorsCount () > 0)
    {
      ++mMirrorsCount;
      return this;
    }

  ++mCopyReferences;
  return this;
}



pastra::TemporalContainer&
ITextStrategy::GetTemporalContainer ()
{
  throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


pastra::VariableSizeStore&
ITextStrategy::GetRowStorage ()
{
  throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


namespace pastra {


static const UTF8_CU_COUNTER _cuCache;



/* Return the numbers of characters encoded by this string. The string is
   either null terminated or 'maxBytesCount' bounded. */
static uint64_t
get_utf8_string_length (const uint8_t* utf8Str,
                        uint64_t       maxBytesCount,
                        uint64_t*      outCharsCount)
{
  uint64_t result = 0;

  *outCharsCount = 0;
  while ((maxBytesCount > 0) && (utf8Str[0] != 0))
    {
      const uint_t charSize = _cuCache.Count (utf8Str[0]);

      bool validChar = true;

      if ((charSize == 0) || (charSize > MIN (4, maxBytesCount)))
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
        throw DBSException (_EXTRA (DBSException::INVALID_UTF8_STRING));

      result += charSize, maxBytesCount -= charSize, utf8Str += charSize;
      *outCharsCount += 1;
    }

  return result;
}


NullText::NullText ()
{
}

uint64_t
NullText::Utf8CountU ()
{
  return 0;
}

void
NullText::ReadUtf8U (const uint64_t offset,
                     const uint64_t count,
                     uint8_t* const buffer)
{
  if (offset > 0)
    throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


void
NullText::WriteUtf8U (const uint64_t       offset,
                      const uint64_t       count,
                      const uint8_t* const buffer)
{
  throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


void
NullText::TruncateUtf8U (const uint64_t offset)
{
  if (offset > 0)
    throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}

uint32_t
NullText::ReferenceCount () const
{
  return 8; //Enough not to allow to change use
}

uint32_t
NullText::MirrorsCount () const
{
  return 0;
}


void
NullText::ReleaseReference ()
{
  //This is a singleton. Do nothing!
}


NullText&
NullText::GetSingletoneInstace ()
{
  static NullText nullTextInstance;

  return nullTextInstance;
}



TemporalText::TemporalText (const uint8_t* const utf8Str,
                            const uint64_t       unitsCount)
{
  if (utf8Str == NULL)
    return;

  const uint64_t bytesCount = get_utf8_string_length (utf8Str,
                                                      unitsCount,
                                                      &mCachedCharsCount);
  mStorage.Write (0, bytesCount, utf8Str);
}


uint64_t
TemporalText::Utf8CountU ()
{
  return mStorage.Size ();
}

void
TemporalText::ReadUtf8U (const uint64_t offset,
                         const uint64_t count,
                         uint8_t* const buffer)
{
  const uint64_t toRead = MIN (count, mStorage.Size () - offset);
  mStorage.Read (offset, toRead, buffer);
}


void
TemporalText::WriteUtf8U (const uint64_t       offset,
                          const uint64_t       count,
                          const uint8_t* const buffer)
{
  mStorage.Write (offset, count, buffer);
}


void
TemporalText::TruncateUtf8U (const uint64_t offset)
{
  mStorage.Colapse (0, offset);
}


TemporalContainer&
TemporalText::GetTemporalContainer ()
{
  return mStorage;
}



RowFieldText::RowFieldText (VariableSizeStore& storage,
                            const uint64_t     firstEntry,
                            const uint64_t     bytesSize)
  : mFirstEntry (firstEntry),
    mUtf8Count ((bytesSize == 0) ? 0 : bytesSize - CACHE_META_DATA_SIZE),
    mStorage (storage),
    mTempContainer ()
{
  if (mUtf8Count == 0)
    return ;

  assert (bytesSize > CACHE_META_DATA_SIZE);

  mStorage.RegisterReference ();
  mStorage.IncrementRecordRef (mFirstEntry);

  assert (bytesSize > CACHE_META_DATA_SIZE);

  uint8_t cachedMetaData[CACHE_META_DATA_SIZE];
  mStorage.GetRecord (mFirstEntry,
                      0,
                      CACHE_META_DATA_SIZE,
                      cachedMetaData);

  mCachedCharsCount = load_le_int32 (cachedMetaData);
  mCachedCharIndex  = load_le_int32 (cachedMetaData + sizeof (uint32_t));
  mCachedCharIndexOffset = load_le_int32 (
                                cachedMetaData + 2 * sizeof (uint32_t)
                                         );
}



RowFieldText::~RowFieldText ()
{
  assert (mCachedCharsCount <= MAX_CHARS_COUNT);
  assert (mCachedCharIndex <= MAX_CHARS_COUNT);
  assert (mCachedCharIndexOffset <= MAX_BYTES_COUNT);

  if (mUtf8Count != 0)
    {
      uint8_t cachedMetaData[CACHE_META_DATA_SIZE];

      mStorage.GetRecord (mFirstEntry, 0, CACHE_META_DATA_SIZE, cachedMetaData);

      //Do not update the elements count
      store_le_int32 (mCachedCharIndex, cachedMetaData + sizeof (uint32_t));
      store_le_int32 (mCachedCharIndexOffset,
                      cachedMetaData + 2 * sizeof (uint32_t));

      mStorage.UpdateRecord (mFirstEntry,
                             0,
                             CACHE_META_DATA_SIZE,
                             cachedMetaData);

      mStorage.DecrementRecordRef (mFirstEntry);
      mStorage.ReleaseReference ();
    }
}

uint64_t
RowFieldText::Utf8CountU ()
{
  return (mUtf8Count == 0) ? mTempContainer.Size () : mUtf8Count;
}

void
RowFieldText::ReadUtf8U (const uint64_t offset,
                         const uint64_t count,
                         uint8_t* const buffer)
{
  assert (offset <= ((mUtf8Count == 0) ? mTempContainer.Size () : mUtf8Count));

  const uint64_t containerSize = mTempContainer.Size ();
  if (containerSize > 0)
    {
      const uint64_t toRead = MIN (count, containerSize - offset);
      if (toRead > 0)
        mTempContainer.Read (offset, count, buffer);
      return ;
    }

  assert (mFirstEntry > 0);

  const uint64_t toRead = MIN (count, mUtf8Count - offset);
  if (toRead == 0)
    return ;

  mStorage.GetRecord (mFirstEntry,
                      offset + CACHE_META_DATA_SIZE,
                      toRead,
                      buffer);
}


void
RowFieldText::WriteUtf8U (const uint64_t       offset,
                          const uint64_t       count,
                          const uint8_t* const buffer)
{
  if ((offset + count) > MAX_BYTES_COUNT)
    {
      throw DBSException (_EXTRA (DBSException::OPER_NOT_SUPPORTED),
                          "This implementation does not support text fields"
                            " with more than %lu characters.",
                          _SC (long, MAX_BYTES_COUNT));
    }

  const uint64_t containerSize = mTempContainer.Size ();
  if ((containerSize == 0)
      && (mUtf8Count > 0))
    {
      uint8_t buffer[256];
      uint64_t offset = 0;

      while (offset < mUtf8Count)
        {
          const uint_t chunkSize = MIN (sizeof buffer, mUtf8Count - offset);
          mStorage.GetRecord (mFirstEntry,
                              offset + CACHE_META_DATA_SIZE,
                              chunkSize,
                              buffer);
          mTempContainer.Write (offset, chunkSize, buffer);
          offset += chunkSize;
        }

      assert (offset == mUtf8Count);
      assert (mTempContainer.Size () == mUtf8Count);

      mStorage.DecrementRecordRef (mFirstEntry);
      mStorage.ReleaseReference ();

      _CC(uint64_t&, mUtf8Count) = 0;

    }

  mTempContainer.Write (offset, count, buffer);
}

void
RowFieldText::TruncateUtf8U (const uint64_t atOffset)
{
  const uint64_t containerSize = mTempContainer.Size ();

  if ((containerSize == 0)
      && (mUtf8Count > 0))
    {
      uint8_t buffer[256];
      uint64_t offset = 0;

      while (offset < MIN (mUtf8Count, atOffset))
        {
          const uint_t chunkSize = MIN (sizeof buffer,
                                        MIN (mUtf8Count, atOffset) - offset);
          mStorage.GetRecord (mFirstEntry,
                              offset + CACHE_META_DATA_SIZE,
                              chunkSize,
                              buffer);
          mTempContainer.Write (offset, chunkSize, buffer);
          offset += chunkSize;
        }

      assert (offset == MIN (mUtf8Count, atOffset));
      assert (mTempContainer.Size () == mUtf8Count);

      mStorage.DecrementRecordRef (mFirstEntry);
      mStorage.ReleaseReference ();

      _CC(uint64_t&, mUtf8Count) = 0;
   }
  else
    mTempContainer.Colapse(0, atOffset);
}


TemporalContainer&
RowFieldText::GetTemporalContainer ()
{
  return mTempContainer;
}

VariableSizeStore&
RowFieldText::GetRowStorage ()
{
  return mStorage;
}




StringMatcher::StringMatcher (ITextStrategy& pattern)
  : mText (NULL),
    mPattern (pattern),
    mPatternSize (min<uint64_t> (pattern.Utf8CountU (), MAX_PATTERN_SIZE)),
    mIgnoreCase (false),
    mLastChar (0xFFFFFFFFFFFFFFFFull),
    mCurrentChar (0),
    mCurrentRawOffset (0),
    mCacheStartPos (0),
    mPatternRaw (mCache),
    mTextRawCache (mPatternRaw + mPatternSize),
    mAvailableCache (sizeof mCache - mPatternSize),
    mCacheValid (0)
{
  assert (mPatternSize > 0);

  mPattern.ReadUtf8U ( 0, mPatternSize, mPatternRaw);

  if (mPatternSize < pattern.Utf8CountU ())
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
StringMatcher::FindMatch (ITextStrategy&  text,
                          const uint64_t  fromChar,
                          const uint64_t  toChar,
                          const bool      ignoreCase)
{
  bool patternRefreshed = false;

  mText             = &text;
  mCurrentChar      = fromChar;
  mCurrentRawOffset = mText->OffsetOfCharU (mCurrentChar);
  mLastChar         = MIN (toChar, mText->mCachedCharsCount);

  if ((fromChar >= mLastChar)
      || (mLastChar - fromChar < mPattern.mCachedCharsCount))
    {
      return PATTERN_NOT_FOUND;
    }

  mCacheValid         = 0;
  mCacheStartPos      = 0;

  if (mText->OffsetOfCharU (mLastChar) <
        mCurrentRawOffset + mPattern.Utf8CountU ())
    {
      return PATTERN_NOT_FOUND;
    }

  if (ignoreCase != mIgnoreCase)
    {
      memset (mShiftTable, 0, sizeof mShiftTable);
      mPattern.ReadUtf8U ( 0, mPatternSize, mPatternRaw);
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

  assert (mPattern.mCachedCharsCount <= toChar - mCurrentChar);

  assert (mText->CharsUntilOffsetU (mCurrentRawOffset) == mCurrentChar);
  assert (mText->OffsetOfCharU (mCurrentChar) == mCurrentRawOffset);

  return mCurrentChar;
}


int64_t
StringMatcher::FindMatchRaw (ITextStrategy&   text,
                             const uint64_t   fromChar,
                             const uint64_t   toChar,
                             const bool       ignoreCase)
{
  if (FindMatch (text, fromChar, toChar, ignoreCase) < 0)
    return PATTERN_NOT_FOUND;

  return mCurrentRawOffset;
}


uint_t
StringMatcher::ComparingWindowShift (uint_t position) const
{
  assert (_SC (uint_t, (mPatternSize - 1)) <= position);
  assert (position < mCacheValid);

  for (int i = mPatternSize - 1; i >= 0; --i, --position)
    {
      if (mPatternRaw[i] != mTextRawCache[position])
        {
          if (i > mShiftTable[mTextRawCache[position]])
            return i - mShiftTable[mTextRawCache[position]];

          else
            return 1;
        }
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
  if (mLastChar < mCurrentChar + mPattern.mCachedCharsCount)
    return false;

  const uint_t buffSize   = mPattern.Utf8CountU ();
  uint_t       buffOffset = mPatternSize;

  if (mText->Utf8CountU () < (mCurrentRawOffset + buffSize))
    return false;

  while (buffOffset < buffSize)
    {
      uint8_t   buffer1[64];
      uint8_t   buffer2[sizeof buffer1];

      uint_t chunkSize = MIN (sizeof buffer1, buffSize - buffOffset);

      mPattern.ReadUtf8U (buffOffset, chunkSize, buffer1);
      mText->ReadUtf8U (mCurrentRawOffset + buffOffset, chunkSize, buffer2);
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
  assert (mText->OffsetOfCharU (mCurrentChar) == mCurrentRawOffset);

  if (mLastChar < mCurrentChar + mPattern.mCachedCharsCount)
    return false;

  if ((mCacheStartPos <= mCurrentRawOffset)
      && (mCurrentRawOffset + mPatternSize < (mCacheStartPos + mCacheValid)))
    {
      return true;
    }

  mCacheValid = min<uint64_t> (mAvailableCache,
                               mText->Utf8CountU () - mCurrentRawOffset);
  if (mCacheValid < mPatternSize)
    {
      mCacheValid = 0;
      return false;
    }

  mText->ReadUtf8U (mCurrentRawOffset, mCacheValid, mTextRawCache);
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
  assert (mCurrentRawOffset < mText->OffsetOfCharU (mLastChar));
  assert (mText->OffsetOfCharU (mLastChar) >= mPattern.Utf8CountU ());

  const uint64_t textSize = mText->OffsetOfCharU (mLastChar);

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

          if (mPatternSize == mPattern.Utf8CountU ())
            break; // Match  found.

          else if (! SuffixesMatch ())
            {
              //A prefix match was found but the rest of string don't match.
              ++mCurrentChar;
              mCurrentRawOffset += _cuCache.Count(
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

  if (mLastChar < mCurrentChar + mPattern.mCachedCharsCount)
    return PATTERN_NOT_FOUND;

  assert (mLastChar - mCurrentChar >= mPattern.mCachedCharsCount);
  assert (mCurrentRawOffset <=
            mText->OffsetOfCharU (mLastChar) - mPattern.Utf8CountU ());

  return mCurrentChar;
}


} //namespace pastra
} //namespace whais

uint8_t UTF8_CU_COUNTER::COUNTS[256];


