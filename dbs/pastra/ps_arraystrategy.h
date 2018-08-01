/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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
******************************************************************************/

#ifndef PS_ARRAYSTRATEGY_H_
#define PS_ARRAYSTRATEGY_H_


#include "whais.h"
#include "ps_container.h"
#include "ps_varstorage.h"
#include "ps_serializer.h"


namespace whais {
namespace pastra {


//Just forward class declarations
class TemporalArray;
class RowFieldArray;
class NullArray;
class PrototypeTable;


} //namespace pastra


class IArrayStrategy
{
  friend class std::unique_ptr<IArrayStrategy>;
  friend class pastra::PrototypeTable;

public:
  IArrayStrategy(const IArrayStrategy&) = delete;
  IArrayStrategy& operator= (const IArrayStrategy&) = delete;

  virtual ~IArrayStrategy() = default;

  uint64_t Count();
  DBS_BASIC_TYPE Type();

  uint_t Get(const uint64_t index, uint8_t* const dest);
  auto Set(const DBS_BASIC_TYPE type,
           const uint8_t* const rawValue,
           const uint64_t index) -> std::shared_ptr<IArrayStrategy>;
  auto Add(const DBS_BASIC_TYPE type,
           const uint8_t* const rawValue,
           uint64_t* const outIndex) -> std::shared_ptr<IArrayStrategy>;
  auto Remove(const uint64_t index) -> std::shared_ptr<IArrayStrategy>;
  auto Sort(const bool reverse) -> std::shared_ptr<IArrayStrategy>;

  void SetSelfReference(std::shared_ptr<IArrayStrategy>& self) { mSelfShare = self; }

  virtual pastra::TemporalContainer& GetTemporalContainer();
  virtual pastra::VariableSizeStore& GetRowStorage();


  template<typename TE>
  class ArrayContainer
  {
  public:
    ArrayContainer(IArrayStrategy& array)
      : mArray(array)
    {
      assert(mArray.mElementsCount > 0);
      assert(mArray.mElementRawSize > 0);
    }

    const TE operator[](const uint64_t index) const
    {
      assert(index < mArray.mElementsCount);

      uint8_t rawValue[pastra::Serializer::MAX_VALUE_RAW_SIZE];
      mArray.RawRead(index * mArray.mElementRawSize, mArray.mElementRawSize, rawValue);

      TE result;
      pastra::Serializer::Load(rawValue, &result);

      return result;
    }

    void Exchange(const int64_t pos1, const int64_t pos2)
    {
      uint8_t rawValue1[pastra::Serializer::MAX_VALUE_RAW_SIZE];
      uint8_t rawValue2[pastra::Serializer::MAX_VALUE_RAW_SIZE];

      mArray.RawRead(pos1 * mArray.mElementRawSize, mArray.mElementRawSize, rawValue1);
      mArray.RawRead(pos2 * mArray.mElementRawSize, mArray.mElementRawSize, rawValue2);

      mArray.RawWrite(pos2 * mArray.mElementRawSize, mArray.mElementRawSize, rawValue1);
      mArray.RawWrite(pos1 * mArray.mElementRawSize, mArray.mElementRawSize, rawValue2);
    }

    uint64_t Count() const { return mArray.mElementsCount; }

    void Pivot(const int64_t index) { mPivot = this->operator[] (index); }
    const TE& Pivot() const { return mPivot; }

  private:
    IArrayStrategy&   mArray;
    TE                mPivot;
  };

protected:
  IArrayStrategy(const DBS_BASIC_TYPE elemsType);
  virtual bool IsShared() const = 0;

  virtual std::shared_ptr<IArrayStrategy> Clone();
  virtual void RawRead(const uint64_t offset, const uint64_t size, uint8_t* const buffer) = 0;
  virtual void RawWrite(const uint64_t offset, const uint64_t size, const uint8_t* const buffer) = 0;
  virtual void ColapseRaw(const uint64_t offset, const uint64_t count) = 0;
  virtual uint64_t RawSize() const = 0;

  uint64_t mElementsCount;
  uint_t mElementRawSize;
  std::weak_ptr<IArrayStrategy> mSelfShare;
  const DBS_BASIC_TYPE mElementsType;
  Lock mLock;
};


namespace pastra
{


class NullArray : public IArrayStrategy
{
public:
  NullArray(const DBS_BASIC_TYPE elemsType);

  static auto GetSingletoneInstace(const DBS_BASIC_TYPE type) -> std::shared_ptr<IArrayStrategy>;
protected:
  bool IsShared() const override;
  void RawRead(const uint64_t offset, const uint64_t size, uint8_t* const buffer) override;
  void RawWrite(const uint64_t offset, const uint64_t size, const uint8_t* const buffer) override;
  void ColapseRaw(const uint64_t offset, const uint64_t count) override;
  uint64_t RawSize() const override;
};

class TemporalArray : public IArrayStrategy
{
  friend class PrototypeTable;

public:
  TemporalArray(const DBS_BASIC_TYPE elemsType);
  TemporalArray(const TemporalArray&) = delete;
  TemporalArray& operator= (const TemporalArray&) = delete;

protected:
  bool IsShared() const override;
  void RawRead(const uint64_t offset, const uint64_t size, uint8_t* const buffer) override;
  void RawWrite(const uint64_t offset, const uint64_t size, const uint8_t* const buffer) override;
  void ColapseRaw(const uint64_t offset, const uint64_t count) override;
  uint64_t RawSize() const override;

  pastra::TemporalContainer& GetTemporalContainer();

private:
  TemporalContainer mStorage;
};

class RowFieldArray : public IArrayStrategy
{
  friend class IArrayStrategy;
  friend class PrototypeTable;

public:
  RowFieldArray(VariableSizeStoreSPtr storage,
                const uint64_t firstRecordEntry,
                const DBS_FIELD_TYPE type);

  RowFieldArray(const RowFieldArray&) = delete;
  RowFieldArray& operator= (const RowFieldArray&) = delete;

  ~RowFieldArray();

protected:
  bool IsShared() const override;
  void RawRead(const uint64_t offset, const uint64_t size, uint8_t* const buffer) override;
  void RawWrite(const uint64_t offset, const uint64_t size, const uint8_t* const buffer) override;
  void ColapseRaw(const uint64_t offset, const uint64_t count) override;
  uint64_t RawSize() const override;

  pastra::TemporalContainer& GetTemporalContainer() override;
  pastra::VariableSizeStore& GetRowStorage() override;

private:
  uint64_t              mFirstRecordEntry;
  VariableSizeStoreSPtr mStorage;
  TemporalContainer     mTempStorage;

  static const uint_t METADATA_SIZE = sizeof(uint64_t);
};


} //namespace pastra
} //namespace whais


#endif /* PS_ARRAYSTRATEGY_H_ */
