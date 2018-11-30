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

#include "table_filter.h"

#include "dbs/dbs_valtranslator.h"


using namespace whais;
using namespace std;


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
insert_string_value(vector<tuple<T, T>>& dest, const std::string& from, const std::string& to)
{
  T f, t;

  Utf8Translator::Read(_RC(const uint8_t*, from.c_str()), from.length(), &f);
  Utf8Translator::Read(_RC(const uint8_t*, to.c_str()), to.length(), &t);

  insert_interval(dest, f, t);
}


template<typename T> TableFilter::ValuesIntervalList
convert_to_interval_list(const vector<tuple<T,T>>& intervals)
{
  TableFilter::ValuesIntervalList result;

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
convert_from_interval_list(const TableFilter::ValuesIntervalList& intervals)
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
add_values_to_intervals(TableFilter::ValuesIntervalList& list,
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
                            TableFilter::ValuesIntervalList& list)
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
TableFilter::AddRow(const ROW_INDEX from, const ROW_INDEX to)
{
  insert_interval(mRowsIntervals, from, to);
}


void
TableFilter::AddValue(const string& fieldName,
                      const uint16_t type,
                      const string& from,
                      const string& to,
                      const bool    excluded)
{
  auto& usedInterval = excluded ? mExcludedValuesintervals : mValuesIntervals;

  auto entry = usedInterval.begin();
  while (entry != usedInterval.end())
  {
    if (get<0>(*entry) == fieldName)
      break;
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

