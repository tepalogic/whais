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

#include "interpreter.h"

#include "pm_general_table.h"


namespace whisper {
namespace prima {



GeneralTable::GeneralTable ()
  : ITable ()
{
}


bool
GeneralTable::IsTemporal () const
{
  return true;
}


ITable&
GeneralTable::Spawn () const
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


FIELD_INDEX
GeneralTable::FieldsCount ()
{
  return 0;
}


FIELD_INDEX
GeneralTable::RetrieveField (const char* field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DBSFieldDescriptor
GeneralTable::DescribeField (const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


ROW_INDEX
GeneralTable::AllocatedRows ()
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


ROW_INDEX
GeneralTable::AddRow ()
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


ROW_INDEX
GeneralTable::AddReusedRow ()
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::MarkRowForReuse (const ROW_INDEX row)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::CreateIndex (const FIELD_INDEX                 field,
                           CREATE_INDEX_CALLBACK_FUNC* const cbFunc,
                           CreateIndexCallbackContext* const cbCotext)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::RemoveIndex (const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


bool
GeneralTable::IsIndexed (const FIELD_INDEX field) const
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DChar&         value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DBool&         value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DDate&         value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DDateTime&     value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DHiresTime&    value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DInt8&         value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DInt16&        value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DInt32&        value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DInt64&        value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DReal&         value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DRichReal&     value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DUInt8&        value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DUInt16&       value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DUInt32&       value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DUInt64&       value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DText&         value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Set (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   const DArray&        value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   DChar&               outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   DBool&               outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   DDate&               outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   DDateTime&           outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   DHiresTime&          outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   DInt8&               outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   DInt16&              outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX      row,
                   const FIELD_INDEX    field,
                   DInt32&              outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX       row,
                   const FIELD_INDEX     field,
                   DInt64&               outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX        row,
                   const FIELD_INDEX      field,
                   DReal&                 outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX        row,
                   const FIELD_INDEX      field,
                   DRichReal&             outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX        row,
                   const FIELD_INDEX      field,
                   DUInt8&                outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX        row,
                   const FIELD_INDEX      field,
                   DUInt16&               outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX        row,
                   const FIELD_INDEX      field,
                   DUInt32&               outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX        row,
                   const FIELD_INDEX      field,
                   DUInt64&               outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX        row,
                   const FIELD_INDEX      field,
                   DText&                 outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::Get (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DArray&         outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


void
GeneralTable::ExchangeRows (const ROW_INDEX    row1,
                            const ROW_INDEX    row2)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DBool&      min,
                         const DBool&      max,
                         const ROW_INDEX   fromRow,
                         const ROW_INDEX   toRow,
                         const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DChar&       min,
                         const DChar&       max,
                         const ROW_INDEX    fromRow,
                         const ROW_INDEX    toRow,
                         const FIELD_INDEX  field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DDate&      min,
                         const DDate&      max,
                         const ROW_INDEX   fromRow,
                         const ROW_INDEX   toRow,
                         const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DDateTime&   min,
                         const DDateTime&   max,
                         const ROW_INDEX    fromRow,
                         const ROW_INDEX    toRow,
                         const FIELD_INDEX  field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DHiresTime&   min,
                         const DHiresTime&   max,
                         const ROW_INDEX     fromRow,
                         const ROW_INDEX     toRow,
                         const FIELD_INDEX   field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DUInt8&       min,
                         const DUInt8&       max,
                         const ROW_INDEX     fromRow,
                         const ROW_INDEX     toRow,
                         const FIELD_INDEX   field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DUInt16&      min,
                         const DUInt16&      max,
                         const ROW_INDEX     fromRow,
                         const ROW_INDEX     toRow,
                         const FIELD_INDEX   field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DUInt32&     min,
                         const DUInt32&     max,
                         const ROW_INDEX    fromRow,
                         const ROW_INDEX    toRow,
                         const FIELD_INDEX  field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DUInt64&     min,
                         const DUInt64&     max,
                         const ROW_INDEX    fromRow,
                         const ROW_INDEX    toRow,
                         const FIELD_INDEX  field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DInt8&      min,
                         const DInt8&      max,
                         const ROW_INDEX   fromRow,
                         const ROW_INDEX   toRow,
                         const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DInt16&      min,
                         const DInt16&      max,
                         const ROW_INDEX    fromRow,
                         const ROW_INDEX    toRow,
                         const FIELD_INDEX  field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DInt32&      min,
                         const DInt32&      max,
                         const ROW_INDEX    fromRow,
                         const ROW_INDEX    toRow,
                         const FIELD_INDEX  field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DInt64&      min,
                         const DInt64&      max,
                         const ROW_INDEX    fromRow,
                         const ROW_INDEX    toRow,
                         const FIELD_INDEX  field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DReal&       min,
                         const DReal&       max,
                         const ROW_INDEX    fromRow,
                         const ROW_INDEX    toRow,
                         const FIELD_INDEX  field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DArray
GeneralTable::MatchRows (const DRichReal&    min,
                         const DRichReal&    max,
                         const ROW_INDEX     fromRow,
                         const ROW_INDEX     toRow,
                         const FIELD_INDEX   field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


GeneralTable&
GeneralTable::Instance ()
{
  static GeneralTable singleton;

  return singleton;
}


} //namespace prima
} //namespace whisper

