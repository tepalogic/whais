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

#ifndef PS_TEXTSTRATEGY_H_
#define PS_TEXTSTRATEGY_H_


#include "whais.h"

#include "ps_container.h"
#include "ps_varstorage.h"
#include "utils/wthread.h"


namespace whais {
namespace pastra {


//Just forward declarations for now!
class PrototypeTable;
class StringMatcher;


}


class ITextStrategy
{
  friend class std::unique_ptr<ITextStrategy>;
  friend class pastra::PrototypeTable;
  friend class pastra::StringMatcher;

public:
  virtual ~ITextStrategy();
  bool operator== (ITextStrategy& o);

  uint64_t CharsCount();
  uint64_t CharsUntilOffset(const uint64_t offset);
  uint64_t OffsetOfChar(const uint64_t index);
  DChar CharAt(const uint64_t index);

  std::shared_ptr<ITextStrategy> ToCase(const bool toLower);
  std::shared_ptr<ITextStrategy> Append(const uint32_t ch);
  std::shared_ptr<ITextStrategy> Append(ITextStrategy& text);
  std::shared_ptr<ITextStrategy> Append(ITextStrategy& text,
                                        const uint64_t fromOff,
                                        const uint64_t toOff);
  std::shared_ptr<ITextStrategy> UpdateCharAt(const uint32_t ch, const uint64_t index);

  int CompareTo(ITextStrategy& second);
  DUInt64 FindMatchInText(ITextStrategy& text,
                          const uint64_t fromCh,
                          const uint64_t toCh,
                          const bool ignoreCase);
  std::shared_ptr<ITextStrategy> ReplaceInText(std::shared_ptr<ITextStrategy> text,
                                               std::shared_ptr<ITextStrategy> newSubstr,
                                               const uint64_t fromCh,
                                               const uint64_t toCh,
                                               const bool ignoreCase);
  uint64_t Utf8Count();
  void ReadUtf8(const uint64_t offset, const uint64_t count, uint8_t * const buffer);
  void WriteUtf8(const uint64_t offset, const uint64_t count, const uint8_t* const buffer);
  void TruncateUtf8(const uint64_t offset);

  void SetSelfReference(std::shared_ptr<ITextStrategy>& self) { mSelfShare = self; }

  virtual pastra::TemporalContainer& GetTemporalContainer();
  virtual pastra::VariableSizeStore& GetRowStorage();

protected:
  ITextStrategy();

  uint64_t CharsUntilOffsetU(const uint64_t offset);
  uint64_t OffsetOfCharU(const uint64_t index);
  DChar CharAtU(const uint64_t index);

  std::shared_ptr<ITextStrategy> DuplicateU();
  std::shared_ptr<ITextStrategy> ToCaseU(const bool toLower);

  std::shared_ptr<ITextStrategy> AppendU(const uint32_t ch);
  std::shared_ptr<ITextStrategy> AppendU(ITextStrategy& text);
  std::shared_ptr<ITextStrategy> AppendU(ITextStrategy& text, const uint64_t fromOff, const uint64_t toOff);

  std::shared_ptr<ITextStrategy> UpdateCharAtU(const uint32_t ch, const uint64_t index);

  virtual bool IsShared() const = 0;
  virtual uint64_t Utf8CountU() = 0;
  virtual void ReadUtf8U(const uint64_t offset, const uint64_t count, uint8_t * const buffer) = 0;
  virtual void WriteUtf8U(const uint64_t offset,
                          const uint64_t count,
                          const uint8_t* const buffer) = 0;
  virtual void TruncateUtf8U(const uint64_t offset) = 0;

  pastra::StringMatcher* mMatcher;
  uint64_t mCachedCharsCount;
  uint64_t mCachedCharIndex;
  uint64_t mCachedCharIndexOffset;
  std::weak_ptr<ITextStrategy> mSelfShare;
  Lock mLock;
};


namespace pastra
{


class NullText : public ITextStrategy
{
  friend class PrototypeTable;

public:
  static std::shared_ptr<ITextStrategy> GetSingletoneInstace();

protected:
  bool IsShared() const override;
  virtual uint64_t Utf8CountU() override;
  virtual void ReadUtf8U(const uint64_t offset, const uint64_t count, uint8_t * const buffer) override;
  virtual void WriteUtf8U(const uint64_t offset, const uint64_t count, const uint8_t* const buffer) override;
  virtual void TruncateUtf8U(const uint64_t offset) override;
};


class TemporalText : public ITextStrategy
{
  friend class PrototypeTable;

public:
  TemporalText(const uint8_t* const utf8Str = nullptr, const uint64_t unitsCount = ~0);

  TemporalText(const TemporalText&) = delete;
  TemporalText operator=(const TemporalText&) = delete;

  virtual TemporalContainer& GetTemporalContainer() override;

protected:
  bool IsShared() const override;
  virtual uint64_t Utf8CountU() override;
  virtual void ReadUtf8U(const uint64_t offset, const uint64_t count, uint8_t * const buffer) override;
  virtual void WriteUtf8U(const uint64_t offset, const uint64_t count, const uint8_t* const buffer) override;
  virtual void TruncateUtf8U(const uint64_t offset) override;

  TemporalContainer mStorage;
};


class RowFieldText : public ITextStrategy
{
  friend class VariableSizeStore;
  friend class PrototypeTable;

public:
  RowFieldText(VariableSizeStore& storage, const uint64_t firstEntry, const uint64_t bytesSize);

  RowFieldText(const RowFieldText&) = delete;
  RowFieldText operator=(const RowFieldText&) = delete;

  virtual ~RowFieldText() override;

  virtual TemporalContainer& GetTemporalContainer() override;
  virtual VariableSizeStore& GetRowStorage() override;

protected:
  bool IsShared() const override;
  virtual uint64_t Utf8CountU() override;
  virtual void ReadUtf8U(const uint64_t offset, const uint64_t count, uint8_t * const buffer) override;
  virtual void WriteUtf8U(const uint64_t offset, const uint64_t count, const uint8_t* const buffer) override;
  virtual void TruncateUtf8U(const uint64_t offset) override;

  const uint64_t mFirstEntry;
  const uint64_t mUtf8Count;
  VariableSizeStore& mStorage;
  TemporalContainer mTempContainer;

  static const uint64_t CACHE_META_DATA_SIZE = 3 * sizeof(uint32_t);
  static const uint32_t MAX_CHARS_COUNT = 0xFFFFFFFF;
  static const uint32_t MAX_BYTES_COUNT = 0xFFFFFFFF;
};

class StringMatcher
{

public:
  explicit StringMatcher(ITextStrategy& pattern);
  int64_t FindMatch(ITextStrategy& text,
                    const uint64_t fromChar,
                    const uint64_t toChar,
                    const bool ignoreCase);
  int64_t FindMatchRaw(ITextStrategy& text,
                       const uint64_t fromChar,
                       const uint64_t toChar,
                       const bool ignoreCase);

private:
  static const int       ALPHABET_SIZE           = 256;
  static const int       MAX_TEXT_CACHE_SIZE     = 1024;
  static const int       MAX_PATTERN_SIZE        = 255;
  static const int64_t   PATTERN_NOT_FOUND       = -1;

  uint_t ComparingWindowShift(uint_t position) const;
  bool SuffixesMatch() const;
  uint_t FindInCache() const;
  void CountCachedChars(const uint_t offset);
  bool FillTextCache();
  int64_t FindSubstr();

  ITextStrategy*        mText;
  ITextStrategy&        mPattern;

  uint8_t               mPatternSize;
  bool                  mIgnoreCase;

  uint64_t              mLastChar;
  uint64_t              mCurrentChar;
  uint64_t              mCurrentRawOffset;
  uint64_t              mCacheStartPos;

  uint8_t* const        mPatternRaw;
  uint8_t* const        mTextRawCache;

  uint16_t              mAvailableCache;
  uint16_t              mCacheValid;

  uint8_t               mShiftTable[ALPHABET_SIZE];
  uint8_t               mCache[MAX_PATTERN_SIZE + MAX_TEXT_CACHE_SIZE];


};


} //namespace pastra
} //namespace whais


#endif /* PS_TEXTSTRATEGY_H_ */
