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

#include "pm_generic_table.h"
#include "interpreter.h"


namespace whais {
namespace prima {


bool
GenericTable::IsTemporal() const
{
  return true;
}

ITable&
GenericTable::Spawn() const
{
  return Instance();
}

FIELD_INDEX
GenericTable::FieldsCount()
{
  return 0;
}

FIELD_INDEX
GenericTable::RetrieveField(const char*)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DBSFieldDescriptor
GenericTable::DescribeField(const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


ROW_INDEX
GenericTable::AllocatedRows()
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


ROW_INDEX
GenericTable::AddRow(const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


ROW_INDEX
GenericTable::GetReusableRow(const bool forceAdd)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


ROW_INDEX
GenericTable::ReusableRowsCount()
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::MarkRowForReuse(const ROW_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::CreateIndex(const FIELD_INDEX,
                           CREATE_INDEX_CALLBACK_FUNC* const,
                           CreateIndexCallbackContext* const)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::RemoveIndex(const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


bool
GenericTable::IsIndexed(const FIELD_INDEX) const
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DChar&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DBool&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DDate&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DDateTime&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DHiresTime&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DInt8&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DInt16&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DInt32&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DInt64&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DReal&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DRichReal&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DUInt8&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DUInt16&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DUInt32&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DUInt64&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DText&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Set(const ROW_INDEX, const FIELD_INDEX, const DArray&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DChar&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DBool&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DDate&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DDateTime&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DHiresTime&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DInt8&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DInt16&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DInt32&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DInt64&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DReal&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DRichReal&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DUInt8&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DUInt16&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DUInt32&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DUInt64&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DText&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Get(const ROW_INDEX, const FIELD_INDEX, DArray&, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::ExchangeRows(const ROW_INDEX, const ROW_INDEX, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::Sort(const FIELD_INDEX, const ROW_INDEX, const ROW_INDEX, const bool)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DBool&,
                        const DBool&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DChar&,
                        const DChar&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DDate&,
                        const DDate&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DDateTime&,
                        const DDateTime&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DHiresTime&,
                        const DHiresTime&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DUInt8&,
                        const DUInt8&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DUInt16&,
                        const DUInt16&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DUInt32&,
                        const DUInt32&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DUInt64&,
                        const DUInt64&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DInt8&,
                        const DInt8&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DInt16&,
                        const DInt16&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DInt32&,
                        const DInt32&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DInt64&,
                        const DInt64&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DReal&,
                        const DReal&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


DArray
GenericTable::MatchRows(const DRichReal&,
                        const DRichReal&,
                        const ROW_INDEX,
                        const ROW_INDEX,
                        const FIELD_INDEX)
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}

void
GenericTable::Flush()
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}

void
GenericTable::LockTable()
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}

void
GenericTable::UnlockTable()
{
  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


void
GenericTable::ReleaseFromDbs()
{
  //Do nothing!
}

GenericTable&
GenericTable::Instance()
{
  static GenericTable singleton;

  return singleton;
}


} //namespace prima
} //namespace whais
