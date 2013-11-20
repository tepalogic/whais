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

#ifndef PS_TEXTSTRATEGY_H_
#define PS_TEXTSTRATEGY_H_

#include "whisper.h"

#include "ps_container.h"
#include "ps_varstorage.h"


namespace whisper {

namespace pastra {

//Justr forward declarations for now!
class TemporalText;
class RowFieldText;

}

class ITextStrategy
{
public:
  ITextStrategy () {};
  virtual ~ITextStrategy() {};

  virtual uint_t ReferenceCount () const = 0;

  virtual void   IncreaseReferenceCount () = 0;

  virtual void   DecreaseReferenceCount () = 0;

  virtual uint_t ShareCount () const = 0;

  virtual void   IncreaseShareCount () = 0;

  virtual void   DecreaseShareCount () = 0;

  virtual uint64_t CharsCount () = 0;

  virtual uint64_t CharsUntilOffset (const uint64_t offset) = 0;

  virtual uint64_t OffsetOfChar (const uint64_t index) = 0;

  virtual uint64_t BytesCount () const = 0;

  virtual void Duplicate (ITextStrategy&  source,
                          const uint64_t  maxCharsCount) = 0;

  virtual DChar   CharAt (const uint64_t index) = 0;

  virtual void    Append (const uint32_t ch) = 0;

  virtual void    Append (ITextStrategy& text,
                          const uint64_t fromOff,
                          const uint64_t toOff) = 0;

  virtual void    Truncate (uint64_t newCharCount) = 0;

  virtual void    UpdateCharAt (const uint32_t   ch,
                                const uint64_t   index,
                                ITextStrategy**  inoutStrategy) = 0;

  virtual void ReadUtf8 (const uint64_t offset,
                         const uint64_t count,
                         uint8_t *const buffer) = 0;

  virtual void WriteUtf8 (const uint64_t       offset,
                          const uint64_t       count,
                          const uint8_t* const buffer) = 0;

  virtual void TruncateUtf8 (const uint64_t newSize) = 0;

  virtual bool                  IsRowValue() const = 0;

  virtual pastra::TemporalText& GetTemporal () = 0;

  virtual pastra::RowFieldText& GetRow () = 0;

protected:
};



namespace pastra
{



class GenericText : public ITextStrategy
{
public:
  GenericText (uint64_t bytesSize);

  //Implementations of public ITextStrategy
  virtual uint_t ReferenceCount () const;

  virtual void   IncreaseReferenceCount ();

  virtual void   DecreaseReferenceCount ();

  virtual uint_t ShareCount () const;

  virtual void   IncreaseShareCount ();

  virtual void   DecreaseShareCount ();

  virtual uint64_t CharsCount ();

  virtual uint64_t CharsUntilOffset (const uint64_t offset);

  virtual uint64_t OffsetOfChar (const uint64_t index);

  virtual uint64_t BytesCount () const;

  virtual void Duplicate (ITextStrategy&  source,
                          const uint64_t  bytesCount);

  virtual DChar   CharAt (const uint64_t index);

  virtual void    Append (const uint32_t ch);

  virtual void    Append (ITextStrategy& text,
                          const uint64_t fromOff,
                          const uint64_t toOff);

  virtual void    Truncate (uint64_t newCharCount);

  virtual void    UpdateCharAt (const uint32_t   ch,
                                const uint64_t   index,
                                ITextStrategy**  inoutStrategy);

  virtual bool IsRowValue() const;

  virtual TemporalText& GetTemporal ();

  virtual RowFieldText& GetRow ();

protected:
  static const uint64_t INVALID_CACHE_VALUE     = ~0ull;
  static const uint_t   GENERIC_CACHE_BUFF_SIZE = 512;


  virtual ~GenericText ();

  virtual void ClearMyself () = 0;

  uint64_t    mBytesSize;
  uint64_t    mCachedCharCount;
  uint64_t    mCachedCharIndex;
  uint64_t    mCachedCharIndexOffset;
  uint64_t    mCacheStartOff;
  uint_t      mCacheValid;
  uint_t      mReferenceCount;
  uint_t      mShareCount;
  uint8_t     mCacheBuffer[GENERIC_CACHE_BUFF_SIZE];


};


class NullText : public GenericText
{
public:
  //Implementations of ITextStrategy
  virtual uint_t ReferenceCount () const;

  virtual void   IncreaseReferenceCount ();

  virtual void   DecreaseReferenceCount ();

  virtual uint_t ShareCount () const;

  virtual void   IncreaseShareCount ();

  virtual void   DecreaseShareCount ();

  virtual void ReadUtf8 (const uint64_t offset,
                         const uint64_t count,
                         uint8_t *const buffer);

  virtual void WriteUtf8 (const uint64_t       offset,
                          const uint64_t       count,
                          const uint8_t* const buffer);

  virtual void TruncateUtf8 (const uint64_t newSize);

  static NullText& GetSingletoneInstace ();

protected:
  NullText ();
  ~NullText ();

  //Implementations of GenericText
  virtual void ClearMyself ();

};


class TemporalText : public GenericText
{
  friend class PrototypeTable;
public:
  TemporalText (const uint8_t* const utf8Str,
                const uint64_t       bytesCount = ~0);

  //Implementations of ITextStrategy
  virtual void ReadUtf8 (const uint64_t offset,
                         const uint64_t count,
                         uint8_t *const buffer);

  virtual void WriteUtf8 (const uint64_t       offset,
                          const uint64_t       count,
                          const uint8_t* const buffer);

  virtual void TruncateUtf8 (const uint64_t   newSize);

  virtual void UpdateCharAt (const uint32_t   ch,
                             const uint64_t   index,
                             ITextStrategy**  inoutStrategy);

  virtual TemporalText& GetTemporal();

protected:
  virtual void ClearMyself ();

  TemporalContainer mStorage;
};


class RowFieldText : public GenericText
{
  friend class PrototypeTable;

public:
  RowFieldText (VariableSizeStore& storage,
                const uint64_t     firstEntry,
                const uint64_t     bytesSize);

  //Implementations of ITextStrategy
  virtual void ReadUtf8 (const uint64_t offset,
                         const uint64_t count,
                         uint8_t *const buffer);

  virtual void WriteUtf8 (const uint64_t       offset,
                          const uint64_t       count,
                          const uint8_t* const buffer);

  virtual void TruncateUtf8 (const uint64_t newSize);

  virtual void UpdateCharAt (const uint32_t   ch,
                             const uint64_t   index,
                             ITextStrategy**  inoutStrategy);

  virtual bool IsRowValue() const;

  virtual TemporalText& GetTemporal ();

  virtual RowFieldText& GetRow ();

protected:
  virtual ~RowFieldText ();

  virtual void ClearMyself ();

  static const uint64_t CACHE_META_DATA_SIZE = 3 * sizeof (uint32_t);
  static const uint32_t MAX_CHARS_COUNT      = 0xFFFFFFFF;
  static const uint32_t MAX_BYTES_COUNT      = 0xFFFFFFFF;

  const uint64_t        mFirstEntry;
  VariableSizeStore&    mStorage;
  TemporalText*         mTempText;

private:
  RowFieldText (const RowFieldText&);
  RowFieldText operator= (const RowFieldText&);
};


} //namespace pastra
} //namespace whisper


#endif /* PS_TEXTSTRATEGY_H_ */
