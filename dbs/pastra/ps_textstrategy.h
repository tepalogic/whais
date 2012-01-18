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

  virtual D_UINT64 GetReferenceCount () const = 0;

  virtual void IncreaseReferenceCount () = 0;

  virtual void DecreaseReferenceCount () = 0;

  virtual D_UINT64 GetCharactersCount () = 0;

  virtual D_UINT64 GetBytesCount () const = 0;

  virtual void Duplicate (I_TextStrategy &source) = 0;

  virtual DBSChar GetCharacterAtIndex (D_UINT64 index) = 0;

  virtual void Append (const D_UINT32 charValue) = 0;

  virtual void Append (I_TextStrategy &text) = 0;

  virtual void Truncate (D_UINT64 newCharCount) = 0;

  virtual void RawReadUtf8Data (const D_UINT64 offset,
                                const D_UINT64 count,
                                D_UINT8 *const pBuffDest) = 0;

  virtual void RawWriteUtf8Data (const D_UINT64 offset,
                                 const D_UINT64 count,
                                 const D_UINT8 *const pBuffSrc) = 0;

  virtual void RawTruncateUtf8Data (const D_UINT64 newSize) = 0;

  virtual bool IsRowValue() const = 0;

  virtual pastra::TemporalText& GetTemporal () = 0;

  virtual pastra::RowFieldText& GetRowValue () = 0;

protected:
};

namespace pastra
{

class GenericText : public I_TextStrategy
{
public:
  GenericText (D_UINT64 bytesSize);

  //Implementations of public I_TextStrategy
  virtual D_UINT64 GetReferenceCount () const;

  virtual void IncreaseReferenceCount ();

  virtual void DecreaseReferenceCount ();

  virtual D_UINT64 GetCharactersCount ();

  virtual D_UINT64 GetBytesCount () const;

  virtual void Duplicate (I_TextStrategy &source);

  virtual DBSChar GetCharacterAtIndex (D_UINT64 index);

  virtual void Append (const D_UINT32 charValue);

  virtual void Append (I_TextStrategy& text);

  virtual void Truncate (D_UINT64 newCharCount);

  virtual bool IsRowValue() const;

  virtual TemporalText& GetTemporal ();

  virtual RowFieldText& GetRowValue ();

protected:

  virtual ~GenericText () {};
  virtual void ClearMyself () = 0;

  D_UINT64 m_BytesSize;
  D_UINT64 m_ReferenceCount;
};

class NullText : public GenericText
{
public:
  //Implementations of I_TextStrategy

  virtual D_UINT64 GetReferenceCount () const;
  virtual void IncreaseReferenceCount ();
  virtual void DecreaseReferenceCount ();

  virtual void RawReadUtf8Data (const D_UINT64 offset,
                                const D_UINT64 count,
                                D_UINT8 *const pBuffDest);
  virtual void RawWriteUtf8Data (const D_UINT64 offset,
                                 const D_UINT64 count,
                                 const D_UINT8 *const pBuffSrc);
  virtual void RawTruncateUtf8Data (const D_UINT64 newSize);

  static NullText& GetSingletoneInstace ();

protected:
  NullText ();
  ~NullText ();

  //Implementations of GenericText
  virtual void ClearMyself ();

};

class RowFieldText : public GenericText
{
  friend class PSTable;
public:
  RowFieldText (VaribaleLenghtStore &storage, D_UINT64 firstEntry, D_UINT64 bytesSize);

protected:
  virtual D_UINT64 GetReferenceCount () const;

  virtual void IncreaseReferenceCount ();

  virtual void DecreaseReferenceCount ();

  //Implementations of I_TextStrategy
  virtual void RawReadUtf8Data (const D_UINT64 offset,
                                const D_UINT64 count,
                                D_UINT8 *const pBuffDest);

  virtual void RawWriteUtf8Data (const D_UINT64 offset,
                                 const D_UINT64 count,
                                 const D_UINT8 *const pBuffSrc);

  virtual void RawTruncateUtf8Data (const D_UINT64 newSize);

  virtual bool IsRowValue() const;

  virtual RowFieldText& GetRowValue ();

protected:
  virtual ~RowFieldText ();

  virtual void ClearMyself ();

  const D_UINT64 m_FirstEntry;
  VaribaleLenghtStore &m_Storage;
};

class TemporalText : public GenericText
{
  friend class PSTable;
public:
  TemporalText (const D_UINT8 *pUtf8String, D_UINT64 bytesCount = ~0);

protected:
  //Implementations of I_TextStrategy
  virtual void RawReadUtf8Data (const D_UINT64 offset,
                                const D_UINT64 count,
                                D_UINT8 *const pBuffDest);
  virtual void RawWriteUtf8Data (const D_UINT64 offset,
                                 const D_UINT64 count,
                                 const D_UINT8 *const pBuffSrc);

  virtual void RawTruncateUtf8Data (const D_UINT64 newSize);

  virtual TemporalText& GetTemporal();


protected:
  virtual ~TemporalText ();

  virtual void ClearMyself ();

  TempContainer m_Storage;
};

};

#endif /* PS_TEXTSTRATEGY_H_ */
