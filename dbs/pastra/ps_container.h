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

#ifndef PS_CONTAINER_H_
#define PS_CONTAINER_H_

#include <vector>
#include <string>


#include "dbs_values.h"

#include "utils/include/wfile.h"
#include "utils/include/wthread.h"

namespace pastra
{

void
append_int_to_str (std::string& dest, D_UINT64 number);

class DataContainerException : public WException
{
public:
  DataContainerException (const D_CHAR* message,
                          const D_CHAR* file,
                          D_UINT32      line,
                          D_UINT32      extra)
    : WException (message, file, line, extra)
  {
  }
  virtual ~DataContainerException ()
  {
  }
};

class I_DataContainer
{
public:
  I_DataContainer ()
  {}
  virtual ~ I_DataContainer ()
  {}

  virtual void     Write (D_UINT64 to, D_UINT64 size, const D_UINT8* pSource) = 0;
  virtual void     Read (D_UINT64 from, D_UINT64 size, D_UINT8* pDest) = 0;
  virtual void     Colapse (D_UINT64 from, D_UINT64 to) = 0;
  virtual D_UINT64 Size () const = 0;
  virtual void     MarkForRemoval () = 0;
};


class FileContainer : public I_DataContainer
{
public:
  FileContainer (const D_CHAR*  pFileNameBase,
                 const D_UINT64 maxFileSize,
                 const D_UINT64 unitsCount);

  virtual ~FileContainer ();

  // WIDataContainer virtual functions
  virtual void     Write (D_UINT64 to, D_UINT64 size, const D_UINT8* pSource);
  virtual void     Read (D_UINT64 from, D_UINT64 size, D_UINT8* pDest);
  virtual void     Colapse (D_UINT64 from, D_UINT64 to);
  virtual D_UINT64 Size () const;
  virtual void     MarkForRemoval ();

private:
  void ExtendContainer ();

  const D_UINT64       m_MaxFileUnitSize;
  std::vector< WFile > m_FilesHandles;
  std::string          m_FileNameBase;
  bool                 m_IsMarked;
};

class FileTempContainer : public FileContainer
{
public:
  FileTempContainer (const D_CHAR*  pFileNameBase,
                     const D_UINT32 maxFileSize);
  virtual ~FileTempContainer ();
};

class TempContainer : public I_DataContainer
{
public:
  TempContainer (const D_CHAR* pTempDirectory, const D_UINT uReservedMemory);
  virtual ~TempContainer ();

  // WIDataContainer virtual functions
  virtual void     Write (D_UINT64 to, D_UINT64 size, const D_UINT8* pSource);
  virtual void     Read (D_UINT64 from, D_UINT64 size, D_UINT8* pDest);
  virtual void     Colapse (D_UINT64 from, D_UINT64 to);
  virtual D_UINT64 Size () const;
  virtual void     MarkForRemoval ();

private:
  void  FillCache (D_UINT64 position);

  std::auto_ptr<FileTempContainer> m_FileContainer;
  std::auto_ptr<D_UINT8>           m_Cache;
  D_UINT64                         m_CacheStartPos;
  D_UINT64                         m_CacheEndPos;
  const D_UINT                     m_CacheSize;
  bool                             m_DirtyCache;

  static D_UINT64      sm_TemporalsCount;
  static WSynchronizer sm_Sync;
};

class WFileContainerException : public DataContainerException
{
public:
  explicit
  WFileContainerException (const D_CHAR* message,
                           const D_CHAR* file,
                           D_UINT32      line,
                           D_UINT32      extra)
    : DataContainerException (message, file, line, extra)
  {
  }

  virtual ~WFileContainerException ()
  {
  }

  virtual WException*     Clone () const
  {
    return new WFileContainerException (*this);
  }
  virtual EXPCEPTION_TYPE Type () const { return FILE_CONTAINER_EXCEPTION; }
  virtual const D_CHAR*   Description () const
  {
    switch (GetExtra ())
      {
      case INVALID_PARAMETERS:
        return "Invalid parameters.";
      case CONTAINTER_INVALID:
        return "Container inconsistency detected.";
      case INVALID_ACCESS_POSITION:
          return "Container accessed outside bounds.";
      case FILE_OS_IO_ERROR:
          return "Container internal file IO error.";
      default:
          assert (false);
          return "Unknown container exception";
      }

    return NULL;
  }

  enum
  {
    INVALID_PARAMETERS = 1,
    CONTAINTER_INVALID,
    INVALID_ACCESS_POSITION,
    FILE_OS_IO_ERROR
  };
};
}

#endif /* PS_CONTAINER_H_ */
