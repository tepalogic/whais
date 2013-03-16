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

namespace pastra
{
  class TemporalText;
  class RowFieldText;
}

class I_TextStrategy
{
public:
  I_TextStrategy () {};
  virtual ~I_TextStrategy() {};

  virtual D_UINT ReferenceCount () const = 0;
  virtual void   IncreaseReferenceCount () = 0;
  virtual void   DecreaseReferenceCount () = 0;

  virtual D_UINT ShareCount () const = 0;
  virtual void   IncreaseShareCount () = 0;
  virtual void   DecreaseShareCount () = 0;

  virtual D_UINT64 CharsCount () = 0;
  virtual D_UINT64 BytesCount () const = 0;

  virtual void Duplicate (I_TextStrategy& source,
                          const D_UINT64  newMaxUtf8Size) = 0;

  virtual DBSChar CharAt (D_UINT64 index) = 0;
  virtual void    Append (const D_UINT32 charValue) = 0;
  virtual void    Append (I_TextStrategy& text) = 0;
  virtual void    Truncate (D_UINT64 newCharCount) = 0;
  virtual void    UpdateCharAt (const D_UINT32   charValue,
                                const D_UINT64   index,
                                I_TextStrategy** pIOStrategy) = 0;

  virtual void ReadUtf8 (const D_UINT64 offset,
                         const D_UINT64 count,
                         D_UINT8 *const pBuffDest) = 0;
  virtual void WriteUtf8 (const D_UINT64 offset,
                          const D_UINT64 count,
                          const D_UINT8 *const pBuffSrc) = 0;
  virtual void TruncateUtf8 (const D_UINT64 newSize) = 0;

  virtual bool                  IsRowValue() const = 0;
  virtual pastra::TemporalText& GetTemporal () = 0;
  virtual pastra::RowFieldText& GetRow () = 0;

protected:
};

namespace pastra
{

class GenericText : public I_TextStrategy
{
public:
  GenericText (D_UINT64 bytesSize);

  //Implementations of public I_TextStrategy
  virtual D_UINT ReferenceCount () const;
  virtual void   IncreaseReferenceCount ();
  virtual void   DecreaseReferenceCount ();

  virtual D_UINT ShareCount () const;
  virtual void   IncreaseShareCount ();
  virtual void   DecreaseShareCount ();

  virtual D_UINT64 CharsCount ();
  virtual D_UINT64 BytesCount () const;

  virtual void Duplicate (I_TextStrategy& source,
                          const D_UINT64  newMaxUtf8Size);

  virtual DBSChar CharAt (D_UINT64 index);
  virtual void    Append (const D_UINT32 charValue);
  virtual void    Append (I_TextStrategy& text);
  virtual void    Truncate (D_UINT64 newCharCount);
  virtual void    UpdateCharAt (const D_UINT32   charValue,
                                const D_UINT64   index,
                                I_TextStrategy** pIOStrategy);

  virtual bool          IsRowValue() const;
  virtual TemporalText& GetTemporal ();
  virtual RowFieldText& GetRow ();

protected:
  virtual ~GenericText ();

  virtual void ClearMyself () = 0;

  D_UINT64 m_BytesSize;
  D_UINT64 m_CachedCharCount;
  D_UINT64 m_CachedCharIndex;
  D_UINT64 m_CachedCharIndexOffset;
  D_UINT   m_ReferenceCount;
  D_UINT   m_ShareCount;


  static const D_UINT64 INVALID_CACHE_VALUE = ~0ull;
};

class NullText : public GenericText
{
public:
  //Implementations of I_TextStrategy
  virtual D_UINT ReferenceCount () const;
  virtual void   IncreaseReferenceCount ();
  virtual void   DecreaseReferenceCount ();

  virtual D_UINT ShareCount () const;
  virtual void   IncreaseShareCount ();
  virtual void   DecreaseShareCount ();

  virtual void ReadUtf8 (const D_UINT64 offset,
                         const D_UINT64 count,
                         D_UINT8 *const pBuffDest);
  virtual void WriteUtf8 (const D_UINT64 offset,
                          const D_UINT64 count,
                          const D_UINT8 *const pBuffSrc);
  virtual void TruncateUtf8 (const D_UINT64 newSize);

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
  TemporalText (const D_UINT8 *pUtf8String, D_UINT64 bytesCount = ~0);
  virtual ~TemporalText ();

  //Implementations of I_TextStrategy
  virtual void ReadUtf8 (const D_UINT64 offset,
                         const D_UINT64 count,
                         D_UINT8 *const pBuffDest);
  virtual void WriteUtf8 (const D_UINT64 offset,
                          const D_UINT64 count,
                          const D_UINT8 *const pBuffSrc);
  virtual void TruncateUtf8 (const D_UINT64 newSize);
  virtual void UpdateCharAt (const D_UINT32   charValue,
                             const D_UINT64   index,
                             I_TextStrategy** pIOStrategy);

  virtual TemporalText& GetTemporal();

protected:


  virtual void ClearMyself ();

  TempContainer m_Storage;
};

class RowFieldText : public GenericText
{
  friend class PrototypeTable;
public:
  RowFieldText (VLVarsStore& storage, D_UINT64 firstEntry, D_UINT64 bytesSize);

  //Implementations of I_TextStrategy
  virtual void ReadUtf8 (const D_UINT64 offset,
                         const D_UINT64 count,
                         D_UINT8 *const pBuffDest);
  virtual void WriteUtf8 (const D_UINT64 offset,
                          const D_UINT64 count,
                          const D_UINT8 *const pBuffSrc);

  virtual void TruncateUtf8 (const D_UINT64 newSize);

  virtual void UpdateCharAt (const D_UINT32   charValue,
                             const D_UINT64   index,
                             I_TextStrategy** pIOStrategy);

  virtual bool          IsRowValue() const;
  virtual TemporalText& GetTemporal ();
  virtual RowFieldText& GetRow ();

protected:
  virtual ~RowFieldText ();

  virtual void ClearMyself ();

  static const D_UINT64 CACHE_META_DATA_SIZE = 3 * sizeof (D_UINT32);
  static const D_UINT32 MAX_CHARS_COUNT      = 0xFFFFFFFF;
  static const D_UINT32 MAX_BYTES_COUNT      = 0xFFFFFFFF;

  const D_UINT64  m_FirstEntry;
  VLVarsStore&    m_Storage;
  TemporalText*   m_TempText;

private:
  RowFieldText (const RowFieldText&);
  RowFieldText operator= (const RowFieldText&);
};

};

#endif /* PS_TEXTSTRATEGY_H_ */
