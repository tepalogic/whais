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

class I_DBSTable
{
public:
  I_DBSTable ()
  {
  }

  virtual ~ I_DBSTable ()
  {
  }

  virtual D_UINT GetFieldsCount () = 0;
  virtual DBSFieldDescriptor GetFieldDescriptor (D_UINT fieldIndex) = 0;
  virtual DBSFieldDescriptor GetFieldDescriptor (const D_CHAR * const pFieldName) = 0;
  virtual D_UINT64 GetAllocatedRows () = 0;

  virtual D_UINT64 AddRow () = 0;
  virtual D_UINT64 AddReusedRow () = 0;
  virtual void MarkRowForReuse (D_UINT64 rowindex) = 0;

  virtual void SetEntry (const DBSChar &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSBool &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSDate &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSDateTime &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSHiresTime &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSInt8 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSInt16 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSInt32 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSInt64 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSReal &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSRichReal &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSUInt8 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSUInt16 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSUInt32 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSUInt64 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSText &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void SetEntry (const DBSArray &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;

  virtual void GetEntry (DBSChar &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSBool &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSDate &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSDateTime &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSHiresTime &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSInt8 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSInt16 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSInt32 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSInt64 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSReal &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSRichReal &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSUInt8 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSUInt16 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSUInt32 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSUInt64 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;

  virtual void GetEntry (DBSText &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;
  virtual void GetEntry (DBSArray &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex) = 0;

};

#endif /* DBS_TABLE_H_ */
