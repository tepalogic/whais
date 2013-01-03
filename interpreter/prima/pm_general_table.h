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

#include "dbs/include/dbs_table.h"

namespace prima
{

class GeneralTable : public I_DBSTable
{
public:
  GeneralTable ();

  virtual bool               IsTemporal () const;
  virtual I_DBSTable&        Spawn () const;
  virtual FIELD_INDEX        GetFieldsCount ();
  virtual FIELD_INDEX        GetFieldIndex (const D_CHAR* pFieldName);
  virtual DBSFieldDescriptor GetFieldDescriptor (const FIELD_INDEX field);
  virtual ROW_INDEX          GetAllocatedRows ();

  virtual ROW_INDEX AddRow ();
  virtual ROW_INDEX AddReusedRow ();
  virtual void      MarkRowForReuse (const ROW_INDEX row);

  virtual void CreateFieldIndex (const FIELD_INDEX                 field,
                                 CREATE_INDEX_CALLBACK_FUNC* const cbFunc,
                                 CallBackIndexData* const          pCbData);
  virtual void RemoveFieldIndex (const FIELD_INDEX field);
  virtual bool IsFieldIndexed (const FIELD_INDEX field) const;

  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSBool& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSChar& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSDate& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSDateTime& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSHiresTime& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSInt8& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSInt16& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSInt32& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSInt64& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSReal& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSRichReal& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSUInt8& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSUInt16& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSUInt32& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSUInt64& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSText& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSArray& value);

  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSBool& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSChar& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSDate& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSDateTime& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSHiresTime& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSInt8& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSInt16& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSInt32& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSInt64& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSReal& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSRichReal& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSUInt8& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSUInt16& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSUInt32& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSUInt64& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSText& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSArray& outValue);

  virtual DBSArray GetMatchingRows (const DBSBool&    min,
                                    const DBSBool&    max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSChar&    min,
                                    const DBSChar&    max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSDate&    min,
                                    const DBSDate&    max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSDateTime& min,
                                    const DBSDateTime& max,
                                    const ROW_INDEX    fromRow,
                                    const ROW_INDEX    toRow,
                                    const ROW_INDEX    ignoreFirst,
                                    const ROW_INDEX    maxCount,
                                    const FIELD_INDEX  field);

  virtual DBSArray GetMatchingRows (const DBSHiresTime& min,
                                    const DBSHiresTime& max,
                                    const ROW_INDEX     fromRow,
                                    const ROW_INDEX     toRow,
                                    const ROW_INDEX     ignoreFirst,
                                    const ROW_INDEX     maxCount,
                                    const FIELD_INDEX   field);

  virtual DBSArray GetMatchingRows (const DBSUInt8&   min,
                                    const DBSUInt8&   max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSUInt16&  min,
                                    const DBSUInt16&  max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSUInt32&  min,
                                    const DBSUInt32&  max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSUInt64&  min,
                                    const DBSUInt64&  max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSInt8&    min,
                                    const DBSInt8&    max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSInt16&   min,
                                    const DBSInt16&   max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSInt32&   min,
                                    const DBSInt32&   max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSInt64&   min,
                                    const DBSInt64&   max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSReal&    min,
                                    const DBSReal&    max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSRichReal& min,
                                    const DBSRichReal& max,
                                    const ROW_INDEX    fromRow,
                                    const ROW_INDEX    toRow,
                                    const ROW_INDEX    ignoreFirst,
                                    const ROW_INDEX    maxCount,
                                    const FIELD_INDEX  field);

  static GeneralTable& Instance ();
};

}

#endif /* PM_GENERAL_TABLE_H_ */
