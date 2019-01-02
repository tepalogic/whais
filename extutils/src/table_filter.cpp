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

#include "../include/table_filter.h"

#include <memory>

#include "dbs/dbs_valtranslator.h"
#include "ext_exception.h"
#include "arrays_ops.h"

using namespace whais;
using namespace std;

template<typename T>
T operator+ (const T& op, int i)
{
  T result = op;
  if (i < 0)
  {
    while (i++ < 0)
      result = result.Prev();
  }
  else
  {
    while (i-- > 0)
      result = result.Next();
  }

  return result;
}

template<typename T>
T operator- (const T& op, int i)
{
  T result = op;
  if (i < 0)
  {
    while (i++ < 0)
      result = result.Next();
  }
  else
  {
    while (i-- > 0)
      result = result.Prev();
  }

  return result;
}

template<typename T> void
exclude_interval(vector<tuple<T, T>>& dest, T from, T to)
{
  if (from > to)
    swap(from, to);

  auto startEntry = dest.begin();
  while (startEntry != dest.end())
  {
    if (from <= get<1>(*startEntry))
      break;
    ++startEntry;
  }
  if (startEntry == dest.end())
    return ;

  auto endEntry = startEntry;
  do
  {
    if (to <= get<1>(*endEntry))
      break;
    ++endEntry;
  } while (endEntry != dest.end());

  if (endEntry == dest.end())
  {
    get<1>(*startEntry) = from;

    if (get<0>(*startEntry) <= get<1>(*startEntry))
      ++startEntry;

    dest.erase(startEntry, endEntry);
    return ;
  }
  else if (startEntry != endEntry)
  {
    get<1>(*startEntry) = from - 1;
    get<0>(*endEntry) = to + 1;

    if (get<0>(*startEntry) <= get<1>(*startEntry))
      ++startEntry;

    if (get<1>(*startEntry) < get<0>(*startEntry))
      ++endEntry;

    dest.erase(startEntry, endEntry);
    return ;
  }

  if (to < get<0>(*startEntry))
    return ;

  assert (to <= get<1>(*startEntry));

  if (from < get<0>(*startEntry))
  {
    get<0>(*startEntry) = to + 1;
    if (get<1>(*startEntry) < get<0>(*startEntry))
      dest.erase(startEntry);

    return ;
  }

  auto interval = *startEntry;
  get<1>(*startEntry) = from - 1;
  get<0>(interval) = to + 1;


  if (get<1>(*startEntry) < get<0>(*startEntry))
    dest.erase (startEntry);

  else if (get<0>(interval) <= get<1>(interval))
      dest.insert(startEntry, interval);
}


template<typename T> void
insert_interval(vector<tuple<T, T>>& dest, T from, T to)
{
  if (from > to)
    swap(from, to);

  for (auto entry = dest.begin(); entry != dest.end(); ++entry)
  {
    if (from < (get<0>(*entry)))
    {
      if (to < (get<0>(*entry)))
      {
        dest.insert(entry, make_tuple(from, to));
        return;
      }

      auto startRemove = entry;
      while (entry != dest.end())
      {
        if (to <= get<1>(*entry))
        {
          get<0>(*entry) = from;
          dest.erase(startRemove, entry);
          return ;
        }
        ++entry;
      }
      dest.erase(startRemove, entry);
      dest.push_back(make_tuple(from, to));
      return ;
    }
    else if (from <= get<1>(*entry))
    {
      if (to <= get<1>(*entry))
        return ;

      auto startRemove = entry;
      while (entry != dest.end())
      {
        if (to <= get<1>(*entry))
        {
          get<0>(*entry) = get<0>(*startRemove);
          dest.erase (startRemove, entry);
          return ;
        }
        ++entry;
      }
      get<1>(*startRemove) = to;
      dest.erase(++startRemove, entry);
      return ;
    }
  }
  dest.push_back(make_tuple(from, to));
}

template<typename T> void
insert_string_value(vector<tuple<T, T>>& dest, const string& from, const string& to)
{
  T f, t;

  Utf8Translator::Read(_RC(const uint8_t*, from.c_str()), from.length(), &f);
  Utf8Translator::Read(_RC(const uint8_t*, to.c_str()), to.length(), &t);

  insert_interval(dest, f, t);
}


template<typename T> TableFieldValuesFilter::ValuesIntervalList
convert_to_interval_list(const vector<tuple<T,T>>& intervals)
{
  TableFieldValuesFilter::ValuesIntervalList result;

  for (auto& i : intervals)
  {
    char from[64], to[64];

    Utf8Translator::Write(_RC(uint8_t*, from), sizeof from, get<0>(i));
    Utf8Translator::Write(_RC(uint8_t*, to), sizeof to, get<1>(i));

    result.push_back(make_tuple(from, to));
  }

  return result;
}


template<typename T> vector<tuple<T,T>>
convert_from_interval_list(const TableFieldValuesFilter::ValuesIntervalList& intervals)
{
  vector<tuple<T,T>> result;

  for (auto& i : intervals)
  {
    T from, to;

    Utf8Translator::Read(_RC(const uint8_t*, get<0>(i).c_str()), get<0>(i).length(), &from);
    Utf8Translator::Read(_RC(const uint8_t*, get<1>(i).c_str()), get<1>(i).length(), &to);

    result.push_back(make_tuple(from, to));
  }

  return result;
}

template<typename T> void
add_values_to_intervals(TableFieldValuesFilter::ValuesIntervalList& list,
                        const string& from,
                        const string& to)
{
  auto vectorList = convert_from_interval_list<T>(list);
  insert_string_value(vectorList, from, to);
  list = convert_to_interval_list(vectorList);
}


bool
add_values_to_interval_list(const string& from,
                            const string& to,
                            const uint16_t type,
                            TableFieldValuesFilter::ValuesIntervalList& list)
{
  switch(type)
  {
  case T_BOOL:
    add_values_to_intervals<DBool>(list, from, to);
    break;

  case T_CHAR:
    add_values_to_intervals<DChar>(list, from, to);
    break;

  case T_DATE:
    add_values_to_intervals<DDate>(list, from, to);
    break;

  case T_DATETIME:
    add_values_to_intervals<DDateTime>(list, from, to);
    break;

  case T_HIRESTIME:
    add_values_to_intervals<DDateTime>(list, from, to);
    break;

  case T_INT8:
    add_values_to_intervals<DInt8>(list, from, to);
    break;

  case T_INT16:
    add_values_to_intervals<DInt16>(list, from, to);
    break;

  case T_INT32:
    add_values_to_intervals<DInt32>(list, from, to);
    break;

  case T_INT64:
    add_values_to_intervals<DInt64>(list, from, to);
    break;

  case T_UINT8:
    add_values_to_intervals<DUInt8>(list, from, to);
    break;

  case T_UINT16:
    add_values_to_intervals<DUInt16>(list, from, to);
    break;

  case T_UINT32:
    add_values_to_intervals<DUInt32>(list, from, to);
    break;

  case T_UINT64:
    add_values_to_intervals<DUInt64>(list, from, to);
    break;

  case T_REAL:
    add_values_to_intervals<DReal>(list, from, to);
    break;

  case T_RICHREAL:
    add_values_to_intervals<DRichReal>(list, from, to);
    break;

  default:
    return false;
  }

  return true;
}



void
TableFieldValuesFilter::AddRow(const ROW_INDEX from, const ROW_INDEX to, const bool exclude)
{
  insert_interval(exclude ? mExcludedRowsIntervals : mRowsIntervals, from, to);
}


void
TableFieldValuesFilter::AddValue(const string& fieldName,
                      const uint16_t type,
                      const string& from,
                      const string& to,
                      const bool exclude)
{
  auto& usedInterval = exclude ? mExcludedValuesintervals : mValuesIntervals;

  auto entry = usedInterval.begin();
  while (entry != usedInterval.end())
  {
    if (get<0>(*entry) == fieldName)
      break;

    ++entry;
  }

  if (entry != usedInterval.end())
  {
    add_values_to_interval_list(from, to, type, get<2>(*entry));
    return ;
  }

  ValuesIntervalList interval;
  add_values_to_interval_list(from, to, type, interval);
  usedInterval.push_back(make_tuple(fieldName, type, interval));
}

const vector<TableFieldValuesFilter::RowEntry>&
TableFieldValuesFilter::GetRowsIntervals(const bool exclude) const
{
  return exclude ? mExcludedRowsIntervals : mRowsIntervals;
}


set<string>
TableFieldValuesFilter::GetFields() const
{
  set<string> result;

  for (const auto& interval : mValuesIntervals)
    result.insert(get<0>(interval));

  for (const auto& interval : mExcludedValuesintervals)
    result.insert(get<0>(interval));

  return result;
}


bool
TableFieldValuesFilter::GetFieldValues(const std::string& field,
                                       uint16_t* type,
                                       ValuesIntervalList* values,
                                       ValuesIntervalList* excludedValues) const
{
  bool fieldFound = false;

  *values = *excludedValues = ValuesIntervalList{};

  for (auto interval : mValuesIntervals)
  {
    if (get<0>(interval) == field)
    {
      *values = get<2>(interval);
      *type = get<1>(interval);
      fieldFound = true;

      break;
    }
  }

  for (auto interval : mExcludedValuesintervals)
  {
    if (get<0>(interval) == field)
    {
      *excludedValues = get<2>(interval);
      *type = get<1>(interval);
      fieldFound = true;

      break;
    }
  }

  return fieldFound;
}



template<typename T>
class TableFilterRunnerFieldRule : public TableFilterRunnerRule
{
public:
  explicit TableFilterRunnerFieldRule(ITable& table, const string& field)
    : TableFilterRunnerRule(),
      mFieldName(field),
      mTable(table)
  {
    auto fieldIdx  = mTable.FieldsCount();
    while (fieldIdx-- > 0)
    {
      if (mTable.DescribeField(fieldIdx).name == mFieldName)
      {
        mField = fieldIdx;
        break;
      }
    }

    if (mField == INVALID_FIELD_INDEX)
    {
      throw ExtException(_EXTRA(ExtException::FILTER_FIELD_NOT_EXISTENT),
                         "Field '%s'",
                         field.c_str());
    }

    mIsSearchIndexed = mTable.IsIndexed(mField);
  }

  ~TableFilterRunnerFieldRule() = default;

  DArray MatchRows(const DArray& rowsSet) override
  {
    DArray result;

    BuildValuesIntervals();
    if ( ! IsSearchIndexed())
    {
      ROW_INDEX rowsCount = rowsSet.Count();
      for (ROW_INDEX i = 0; i < rowsCount; ++i)
      {
        DUInt32 row;
        if (RowIsMatching(mTable, row.mValue))
          result.Add(row);
      }
      result.Sort();
      return result;
    }

    for (auto entry = mValues.begin(); entry != mValues.end(); ++entry)
    {
      DArray entryMatches = mTable.MatchRows(get<0>(*entry),
                                             get<1>(*entry),
                                             0,
                                             mTable.AllocatedRows(),
                                             mField);
      result = array_unite(result, entryMatches);
    }

    return array_intersect(rowsSet, result);
  }

  bool RowIsMatching(const ITable& table, ROW_INDEX row) override
  {
    T val;

    BuildValuesIntervals();
    mTable.Get(row, mField, val);
    for (auto entry = mValues.cbegin(); entry != mValues.cend(); ++entry)
    {
      if ((get<0>(*entry) <= val) && (val <= get<1>(*entry)))
        return true;
    }
    return false;
  }

  bool IsSearchIndexed() const override
  {
    return mTable.IsIndexed(mField);
  }

  void AddValues (const string& from, const string& to)
  {
    T first, last;

    Utf8Translator::Read(_RC(const uint8_t*, from.c_str()), from.length(), &first);
    Utf8Translator::Read(_RC(const uint8_t*, to.c_str()), to.length(), &last);

    insert_interval(mIncluded, first, last);
    mAreValuesValid = false;
  }

  void AddExcludedValues (const string from, const string to)
  {
    T first, last;

    Utf8Translator::Read(_RC(const uint8_t*, from.c_str()), from.length(), &first);
    Utf8Translator::Read(_RC(const uint8_t*, to.c_str()), to.length(), &last);

    insert_interval(mExcluded, first, last);
    mAreValuesValid = false;
  }

private:
  void BuildValuesIntervals()
  {
    if (mAreValuesValid)
      return ;

    if (mIncluded.size () == 0)
      mValues.push_back(make_tuple(T{}, T::Max()));
    else
    {
      for (auto entry = mIncluded.begin(); entry != mIncluded.end(); ++entry)
        insert_interval(mValues, get<0>(*entry), get<1>(*entry));
    }

    for (auto entry = mExcluded.begin(); entry != mExcluded.end(); ++entry)
      exclude_interval(mValues, get<0>(*entry), get<1>(*entry));

    mAreValuesValid = true;
  }

  const string      mFieldName;
  ITable&           mTable;
  ROW_INDEX         mField = INVALID_FIELD_INDEX;
  bool              mAreValuesValid = false;
  bool              mIsSearchIndexed = false;

  vector<tuple<T, T>> mIncluded;
  vector<tuple<T, T>> mExcluded;
  vector<tuple<T, T>> mValues;
};



TableFilterRunner::TableFilterRunner(ITable& table)
   : mTable(table)
{
}


TableFilterRunner::~TableFilterRunner()
{
  for (auto field : mFilterRules)
    delete field;
}



bool
TableFilterRunner::AddFilterRules(TableFieldValuesFilter& filter)
{
  set<string> fields = filter.GetFields();
  for (const auto& f : fields)
  {
    uint16_t type;
    TableFieldValuesFilter::ValuesIntervalList values, excludedValues;

    if (! filter.GetFieldValues(f, &type, &values, &excludedValues))
      return false;

    AddFieldValues(f, values, excludedValues);
  }


  for (const auto& interval : filter.GetRowsIntervals(false))
    AddRowInterval(get<0>(interval), get<1>(interval), false);

  for (const auto& interval : filter.GetRowsIntervals(true))
    AddRowInterval(get<0>(interval), get<1>(interval), true);

  return true;
}


void
TableFilterRunner::AddRowInterval(ROW_INDEX from, ROW_INDEX to, const bool excluded)
{
  if (to == INVALID_ROW_INDEX)
    from = to;

  insert_interval(excluded ? mExcludedRowsIntervals : mRowsIntervals, from, to);
}


void
TableFilterRunner::AddFieldValues(const std::string& field,
                                  const TableFieldValuesFilter::ValuesIntervalList& values,
                                  const TableFieldValuesFilter::ValuesIntervalList& excludedValues)
{
  FIELD_INDEX fieldIdx = INVALID_FIELD_INDEX;
  uint16_t    type     = T_UNKNOWN;

  for (FIELD_INDEX f = 0; f < mTable.FieldsCount(); ++f)
  {
    const DBSFieldDescriptor fDesc = mTable.DescribeField(f);
    if (fDesc.name != field)
      continue ;

    if (fDesc.isArray || fDesc.type == T_TEXT)
    {
      throw ExtException(_EXTRA(ExtException::FILTER_FIELD_INVALID_TYPE),
                         "Field '%s' is either of type TEXT or ARRAY and"
                           " cannot be used for a filter rule.",
                         field.c_str());
    }
    fieldIdx = f;
    type     = fDesc.type;
    break;
  }

  if (fieldIdx == INVALID_FIELD_INDEX)
  {
      throw ExtException(_EXTRA(ExtException::FILTER_FIELD_NOT_EXISTENT),
                         "Field '%s'",
                         field.c_str());
  }

  switch(type)
  {
    case T_BOOL:
    {
      unique_ptr<TableFilterRunnerFieldRule<DBool>> fieldRule(
          new TableFilterRunnerFieldRule<DBool>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_CHAR:
    {
      unique_ptr<TableFilterRunnerFieldRule<DChar>> fieldRule(
          new TableFilterRunnerFieldRule<DChar>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_DATE:
    {
      unique_ptr<TableFilterRunnerFieldRule<DDate>> fieldRule(
          new TableFilterRunnerFieldRule<DDate>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_DATETIME:
    {
      unique_ptr<TableFilterRunnerFieldRule<DDateTime>> fieldRule(
          new TableFilterRunnerFieldRule<DDateTime>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_HIRESTIME:
    {
      unique_ptr<TableFilterRunnerFieldRule<DHiresTime>> fieldRule(
          new TableFilterRunnerFieldRule<DHiresTime>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_INT8:
    {
      unique_ptr<TableFilterRunnerFieldRule<DInt8>> fieldRule(
          new TableFilterRunnerFieldRule<DInt8>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_INT16:
    {
      unique_ptr<TableFilterRunnerFieldRule<DInt16>> fieldRule(
          new TableFilterRunnerFieldRule<DInt16>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_INT32:
    {
      unique_ptr<TableFilterRunnerFieldRule<DInt32>> fieldRule(
          new TableFilterRunnerFieldRule<DInt32>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_INT64:
    {
      unique_ptr<TableFilterRunnerFieldRule<DInt64>> fieldRule(
          new TableFilterRunnerFieldRule<DInt64>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_UINT8:
    {
      unique_ptr<TableFilterRunnerFieldRule<DUInt8>> fieldRule(
          new TableFilterRunnerFieldRule<DUInt8>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_UINT16:
    {
      unique_ptr<TableFilterRunnerFieldRule<DUInt16>> fieldRule(
          new TableFilterRunnerFieldRule<DUInt16>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_UINT32:
    {
      unique_ptr<TableFilterRunnerFieldRule<DUInt32>> fieldRule(
          new TableFilterRunnerFieldRule<DUInt32>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_UINT64:
    {
      unique_ptr<TableFilterRunnerFieldRule<DUInt64>> fieldRule(
          new TableFilterRunnerFieldRule<DUInt64>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_REAL:
    {
      unique_ptr<TableFilterRunnerFieldRule<DReal>> fieldRule(
          new TableFilterRunnerFieldRule<DReal>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    case T_RICHREAL:
    {
      unique_ptr<TableFilterRunnerFieldRule<DRichReal>> fieldRule(
          new TableFilterRunnerFieldRule<DRichReal>(mTable, field));

      for (const auto interval : values)
        fieldRule->AddValues(get<0>(interval), get<1>(interval));

      for (const auto interval : excludedValues)
        fieldRule->AddExcludedValues(get<0>(interval), get<1>(interval));

      mFilterRules.push_back(fieldRule.release());
    }
    break;

    default:
      assert(false);

      throw ExtException(_EXTRA(ExtException::GENERAL_CONTROL_ERROR),
                         "Field '%s' has an unexpected type '%d'. ",
                         field.c_str(),
                         type);
  }
}


void
TableFilterRunner::ResetRowsFilter()
{
  mRowsIntervals.clear();
  mExcludedRowsIntervals.clear();
}


void
TableFilterRunner::ResetFilterRules()
{
  for (auto filter : mFilterRules)
    delete filter;

  mFilterRules.clear();
}


DArray
TableFilterRunner::Run()
{
  if (mTable.AllocatedRows() == 0)
    return DArray();

  vector<tuple<ROW_INDEX, ROW_INDEX>> rowsIntervals;
  for (const auto& interval : mRowsIntervals)
    insert_interval(rowsIntervals, get<0>(interval), get<1>(interval));

  if (rowsIntervals.empty() )
    insert_interval<ROW_INDEX>(rowsIntervals, 0, mTable.AllocatedRows() - 1);

  for (const auto& interval : mExcludedRowsIntervals)
    exclude_interval(rowsIntervals, get<0>(interval), get<1>(interval));

  DArray result;
  for (const auto& interval : rowsIntervals)
  {
    for (ROW_INDEX row = get<0>(interval); row <= get<1>(interval); ++row)
      result.Add(DUInt32(row));
  }

  bool allIndexed = true;
  for (size_t rulesUsed = 0; rulesUsed < mFilterRules.size(); ++rulesUsed)
  {
    if (! mFilterRules[rulesUsed]->IsSearchIndexed())
    {
      allIndexed = false;
      continue ;
    }
    result = mFilterRules[rulesUsed]->MatchRows(result);
  }

  if (allIndexed)
    return result;

  DArray temp = result;
  result = DArray();
  for (uint64_t rowIdx = 0; rowIdx < temp.Count(); ++rowIdx)
  {
    bool matches = true;
    DUInt32 row;
    temp.Get(rowIdx, row);
    for (size_t rulesUsed = 0; (rulesUsed < mFilterRules.size()) && matches; ++rulesUsed)
    {
      if (mFilterRules[rulesUsed]->IsSearchIndexed())
        continue ;
      matches &= mFilterRules[rulesUsed]->RowIsMatching(mTable, row.mValue);
    }

    if (matches)
      result.Add(row);
  }

  return result;
}

