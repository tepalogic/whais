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

using namespace prima;

GeneralTable::GeneralTable ()
  : I_DBSTable ()
{
}

bool
GeneralTable::IsTemporal () const
{
  return true;
}

I_DBSTable&
GeneralTable::Spawn () const
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

FIELD_INDEX
GeneralTable::GetFieldsCount ()
{
  return 0;
}

FIELD_INDEX
GeneralTable::GetFieldIndex (const D_CHAR* pFieldName)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DBSFieldDescriptor
GeneralTable::GetFieldDescriptor (const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

ROW_INDEX
GeneralTable::GetAllocatedRows ()
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
GeneralTable::CreateFieldIndex (const FIELD_INDEX                 field,
                                CREATE_INDEX_CALLBACK_FUNC* const cbFunc,
                                CallBackIndexData* const          pCbData)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::RemoveFieldIndex (const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

bool
GeneralTable::IsFieldIndexed (const FIELD_INDEX field) const
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSChar&    value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSBool&    value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSDate&    value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX    row,
                        const FIELD_INDEX  field,
                        const DBSDateTime& value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX     row,
                        const FIELD_INDEX   field,
                        const DBSHiresTime& value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSInt8&    value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSInt16&   value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSInt32&   value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSInt64&   value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSReal&    value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX    row,
                        const FIELD_INDEX  field,
                        const DBSRichReal& value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSUInt8&   value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSUInt16&  value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSUInt32&  value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSUInt64&  value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSText&    value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::SetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        const DBSArray&   value)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSChar&          outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSBool&          outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSDate&          outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSDateTime&      outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSHiresTime&     outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSInt8&          outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSInt16&         outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSInt32&         outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSInt64&         outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSReal&          outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSRichReal&      outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSUInt8&         outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSUInt16&        outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSUInt32&        outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSUInt64&        outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSText&          outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

void
GeneralTable::GetEntry (const ROW_INDEX   row,
                        const FIELD_INDEX field,
                        DBSArray&         outValue)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


DBSArray
GeneralTable::GetMatchingRows (const DBSBool&    min,
                               const DBSBool&    max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSChar&    min,
                               const DBSChar&    max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSDate&    min,
                               const DBSDate&    max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSDateTime& min,
                               const DBSDateTime& max,
                               const ROW_INDEX    fromRow,
                               const ROW_INDEX    toRow,
                               const ROW_INDEX    ignoreFirst,
                               const ROW_INDEX    maxCount,
                               const FIELD_INDEX  field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSHiresTime& min,
                               const DBSHiresTime& max,
                               const ROW_INDEX     fromRow,
                               const ROW_INDEX     toRow,
                               const ROW_INDEX     ignoreFirst,
                               const ROW_INDEX     maxCount,
                               const FIELD_INDEX   field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSUInt8&   min,
                               const DBSUInt8&   max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSUInt16&  min,
                               const DBSUInt16&  max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSUInt32&  min,
                               const DBSUInt32&  max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSUInt64&  min,
                               const DBSUInt64&  max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSInt8&    min,
                               const DBSInt8&    max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSInt16&   min,
                               const DBSInt16&   max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSInt32&   min,
                               const DBSInt32&   max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSInt64&   min,
                               const DBSInt64&   max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSReal&    min,
                               const DBSReal&    max,
                               const ROW_INDEX   fromRow,
                               const ROW_INDEX   toRow,
                               const ROW_INDEX   ignoreFirst,
                               const ROW_INDEX   maxCount,
                               const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

DBSArray
GeneralTable::GetMatchingRows (const DBSRichReal& min,
                               const DBSRichReal& max,
                               const ROW_INDEX    fromRow,
                               const ROW_INDEX    toRow,
                               const ROW_INDEX    ignoreFirst,
                               const ROW_INDEX    maxCount,
                               const FIELD_INDEX  field)
{
  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}


GeneralTable&
GeneralTable::Instance ()
{
  static GeneralTable singleton;

  return singleton;
}
