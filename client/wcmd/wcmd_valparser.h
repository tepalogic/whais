/******************************************************************************
  WCMD - An utility to manage whisper database files.
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
******************************************************************************/

#ifndef WCMD_VALPARSER_H_
#define WCMD_VALPARSER_H_

#include <assert.h>
#include <iostream>
#include <vector>

#include "whisper.h"

#include "utils/range.h"
#include "dbs/dbs_table.h"


#define DSIZE           24  //This should be bigger then any size of DBS type.

struct FieldValuesUpdate
{
  template<typename T>
  FieldValuesUpdate (const uint_t field, const uint_t type, const T& value)
    : mFieldId (field),
      mFieldType (type)
  {
    assert (sizeof (value) <= sizeof (mValue));
    _placement_new (mValue, value);
  }

  FieldValuesUpdate (const FieldValuesUpdate& src)
    : mFieldId (src.mFieldId),
      mFieldType (src.mFieldType)
  {
    memcpy (mValue, src.mValue, sizeof (mValue));

    _CC (uint_t&, src.mFieldType) = T_UNKNOWN;
  }

  FieldValuesUpdate& operator= (const FieldValuesUpdate& src)
  {
    if (this != &src)
    {
      this->~FieldValuesUpdate ();
      _placement_new (this, src);
    }

    return *this;
  }

  ~FieldValuesUpdate ();


  uint_t    mFieldId;
  uint_t    mFieldType;
  uint8_t   mValue[DSIZE];
};



struct FieldValuesSelection
{
  FieldValuesSelection ();
  ~FieldValuesSelection ();

  FieldValuesSelection (const FieldValuesSelection& src)
    : mFieldId (src.mFieldId),
      mFieldType (src.mFieldType),
      mSearchNull (src.mSearchNull),
      mRange (src.mRange)
    {
      _CC (void*&, src.mRange) = NULL;
    }

  FieldValuesSelection& operator= (const FieldValuesSelection& src)
  {
    if (this != &src)
      {
        this->~FieldValuesSelection ();
        _placement_new (this, src);
      }

    return *this;
  }

  uint_t        mFieldId;
  uint_t        mFieldType;
  bool          mSearchNull;
  void*         mRange;
};



struct RowsSelection
{
  whisper::Range<ROW_INDEX>          mRows;
  std::vector<FieldValuesSelection>  mSearchedValue;
};



bool
ParseRowsSelectionClause (std::ostream* const    os,
                          whisper::ITable&       table,
                          const char*            str,
                          RowsSelection&         outRowsSelection);



bool
ParseFieldUpdateValues (std::ostream* const              os,
                        whisper::ITable&                 table,
                        const char*                      str,
                        size_t*                          outSize,
                        std::vector<FieldValuesUpdate>&  outUpdates);

bool
UpdateTableRow (std::ostream const*                   os,
                whisper::ITable&                      table,
                const ROW_INDEX                       row,
                const std::vector<FieldValuesUpdate>& fieldVals);


void
MatchSelectedRows (whisper::ITable&    table,
                   RowsSelection&      select);


void
PrintFieldValue (std::ostream&        os,
                 whisper::ITable&     table,
                 const ROW_INDEX      row,
                 const FIELD_INDEX    field);

#endif //WCMD_VALPARSER_H_
