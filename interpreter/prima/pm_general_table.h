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

#ifndef PM_GENERAL_TABLE_H_
#define PM_GENERAL_TABLE_H_

#include "dbs/dbs_table.h"

namespace whisper {
namespace prima {

class GeneralTable : public ITable
{
public:
  GeneralTable ();

  virtual bool IsTemporal () const;

  virtual ITable& Spawn () const;

  virtual FIELD_INDEX FieldsCount ();

  virtual FIELD_INDEX RetrieveField (const char* field);

  virtual DBSFieldDescriptor DescribeField (const FIELD_INDEX field);

  virtual ROW_INDEX AllocatedRows ();

  virtual ROW_INDEX AddRow ();

  virtual ROW_INDEX AddReusedRow ();

  virtual void MarkRowForReuse (const ROW_INDEX row);

  virtual void CreateIndex (const FIELD_INDEX                   field,
                            CREATE_INDEX_CALLBACK_FUNC* const   cbFunc,
                            CreateIndexCallbackContext* const   cbCotext);

  virtual void RemoveIndex (const FIELD_INDEX field);

  virtual bool IsIndexed (const FIELD_INDEX field) const;

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DBool&          value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DChar&          value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DDate&          value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DDateTime&      value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DHiresTime&     value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DInt8&          value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DInt16&         value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DInt32&         value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DInt64&         value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DReal&          value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DRichReal&      value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DUInt8&         value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DUInt16&        value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DUInt32&        value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DUInt64&        value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DText&          value);

  virtual void Set (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DArray&         value);


  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DBool&                outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DChar&                outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DDate&                outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DDateTime&            outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DHiresTime&           outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DInt8&                outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DInt16&               outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DInt32&               outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DInt64&               outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DReal&                outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DRichReal&            outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DUInt8&               outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DUInt16&              outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DUInt32&              outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DUInt64&              outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DText&                outValue);

  virtual void Get (const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    DArray&               outValue);


  virtual DArray MatchRows (const DBool&        min,
                            const DBool&        max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DChar&        min,
                            const DChar&        max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DDate&        min,
                            const DDate&        max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DDateTime&    min,
                            const DDateTime&    max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DHiresTime&   min,
                            const DHiresTime&   max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DUInt8&       min,
                            const DUInt8&       max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DUInt16&      min,
                            const DUInt16&      max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DUInt32&      min,
                            const DUInt32&      max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DUInt64&      min,
                            const DUInt64&      max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DInt8&        min,
                            const DInt8&        max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DInt16&       min,
                            const DInt16&       max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DInt32&       min,
                            const DInt32&       max,
                            const ROW_INDEX     fromRow,
                            const ROW_INDEX     toRow,
                            const ROW_INDEX     ignoreFirst,
                            const ROW_INDEX     maxCount,
                            const FIELD_INDEX   field);

  virtual DArray MatchRows (const DInt64&         min,
                            const DInt64&         max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DReal&          min,
                            const DReal&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DRichReal&      min,
                            const DRichReal&      max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  static GeneralTable& Instance ();
};

} //namespace prima
} //namespace whisper

#endif /* PM_GENERAL_TABLE_H_ */

