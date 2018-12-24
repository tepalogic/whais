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

#ifndef TABLE_FILTER_H
#define TABLE_FILTER_H

#include <vector>
#include <string>
#include <tuple>

#include "dbs/dbs_types.h"
#include "dbs/dbs_table.h"


namespace whais {

class TableFilterRunner;

class TableFilter
{
public:
  using RowEntry = std::tuple<ROW_INDEX, ROW_INDEX>;
  using ValuesIntervalList = std::vector<std::tuple<std::string, std::string>>;
  using ValuesList = std::tuple<std::string, uint16_t, ValuesIntervalList>;

  void AddRow (const ROW_INDEX from, const ROW_INDEX to, const bool exclude);
  void AddValue (const std::string& fieldName,
                 const uint16_t type,
                 const std::string& from,
                 const std::string& to,
                 const bool exclude);
protected:
  std::vector<RowEntry>    mRowsIntervals;
  std::vector<RowEntry>    mExcludedRowsIntervals;
  std::vector<ValuesList>  mValuesIntervals;
  std::vector<ValuesList>  mExcludedValuesintervals;
};


class TableFilterRunnerRule
{
public:
  virtual ~TableFilterRunnerRule() = default;

  virtual DArray MatchRows(const DArray& rowsSet) = 0;
  virtual bool   RowIsMatching(const ITable& table, ROW_INDEX row) = 0;
  virtual bool   IsSearchIndexed() const = 0;

  virtual bool operator<(const TableFilterRunnerRule& filter) const = 0;
};

class TableFilterRunner
{
public:
  explicit TableFilterRunner(ITable& table);
  virtual ~TableFilterRunner();

  DArray Run();

protected:
  void AddRowInterval(ROW_INDEX from, ROW_INDEX to = INVALID_ROW_INDEX, const bool excluded = false);
  void AddFieldValues(const std::string& field,
                      const TableFilter::ValuesIntervalList& values,
                      const TableFilter::ValuesIntervalList& excludedValues);

  void ResetRowsFilter();
  void ResetFilterRules();

  ITable&                             mTable;
  std::vector<TableFilterRunnerRule*> mFilterRules;
  std::vector<TableFilter::RowEntry>  mRowsIntervals;
  std::vector<TableFilter::RowEntry>  mExcludedRowsIntervals;

  friend class TableFilter;
};


} //namespce whais

#endif /* TABLE_FILTER_H */
