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

#ifndef DBS_TABLE_H_
#define DBS_TABLE_H_

#include "dbs_types.h"
#include "dbs_values.h"

namespace whisper {

struct CreateIndexCallbackContext
{
  CreateIndexCallbackContext () {}
  virtual ~CreateIndexCallbackContext () {};

  uint64_t      mRowIndex;
  uint64_t      mRowsCount;
};

typedef void CREATE_INDEX_CALLBACK_FUNC (CreateIndexCallbackContext* cbContext);

class DBS_SHL ITable
{
public:
  ITable ();
  virtual ~ITable ();

  virtual bool IsTemporal () const = 0;

  virtual ITable& Spawn () const = 0;

  virtual FIELD_INDEX FieldsCount () = 0;

  virtual FIELD_INDEX RetrieveField (const char* fieldName) = 0;

  virtual DBSFieldDescriptor DescribeField (const FIELD_INDEX field) = 0;

  virtual ROW_INDEX AllocatedRows () = 0;

  virtual ROW_INDEX AddRow () = 0;

  virtual ROW_INDEX AddReusedRow () = 0;

  virtual void  MarkRowForReuse (const ROW_INDEX row) = 0;

  virtual void CreateIndex (const FIELD_INDEX                  field,
                            CREATE_INDEX_CALLBACK_FUNC* const  cbFunc,
                            CreateIndexCallbackContext* const  cbContext) = 0;

  virtual void RemoveIndex (const FIELD_INDEX field) = 0;

  virtual bool IsIndexed (const FIELD_INDEX field) const = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DBool&      value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DChar&      value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DDate&      value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DDateTime&  value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DHiresTime& value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DInt8&      value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DInt16&     value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DInt32&     value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DInt64&     value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DReal&      value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DRichReal&  value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DUInt8&     value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DUInt16&    value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DUInt32&    value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DUInt64&    value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DText&      value) = 0;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DArray&     value) = 0;


  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DBool&            outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DChar&            outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DDate&            outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DDateTime&        outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DHiresTime&       outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DInt8&            outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DInt16&           outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DInt32&           outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DInt64&           outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DReal&            outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DRichReal&        outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DUInt8&           outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DUInt16&          outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DUInt32&          outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DUInt64&          outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DText&            outValue) = 0;

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DArray&           outValue) = 0;

  virtual void ExchangeRows (const ROW_INDEX    row1,
                             const ROW_INDEX    row2) = 0;

  virtual void Sort (const FIELD_INDEX  field,
                     const ROW_INDEX    from,
                     const ROW_INDEX    to,
                     const bool         reverse) = 0;

  virtual DArray MatchRows (const DBool&      min,
                            const DBool&      max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DChar&      min,
                            const DChar&      max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DDate&      min,
                            const DDate&      max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DDateTime&   min,
                            const DDateTime&   max,
                            const ROW_INDEX    fromRow,
                            const ROW_INDEX    toRow,
                            const FIELD_INDEX  field) = 0;

  virtual DArray MatchRows (const DHiresTime&   min,
                            const DHiresTime&   max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const FIELD_INDEX   field) = 0;

  virtual DArray MatchRows (const DUInt8&     min,
                            const DUInt8&     max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DUInt16&    min,
                            const DUInt16&    max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DUInt32&    min,
                            const DUInt32&    max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DUInt64&    min,
                            const DUInt64&    max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DInt8&      min,
                            const DInt8&      max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DInt16&     min,
                            const DInt16&     max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DInt32&     min,
                            const DInt32&     max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DInt64&     min,
                            const DInt64&     max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DReal&      min,
                            const DReal&      max,
                            const ROW_INDEX   fromRow,
                            const ROW_INDEX   toRow,
                            const FIELD_INDEX field) = 0;

  virtual DArray MatchRows (const DRichReal&   min,
                            const DRichReal&   max,
                            const ROW_INDEX    fromRow,
                            const ROW_INDEX    toRow,
                            const FIELD_INDEX  field) = 0;
};

} //namespace whisper

#endif /* DBS_TABLE_H_ */
