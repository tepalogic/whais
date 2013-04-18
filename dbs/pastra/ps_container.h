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

#include "utils/wfile.h"
#include "utils/wthread.h"

namespace whisper {
namespace pastra {

void
append_int_to_str (std::string& dest, uint64_t number);

class DataContainerException : public Exception
{
public:
  DataContainerException (const char* message,
                          const char* file,
                          uint32_t      line,
                          uint32_t      extra)
    : Exception (message, file, line, extra)
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

  virtual void     Write (uint64_t to, uint64_t size, const uint8_t* pSource) = 0;
  virtual void     Read (uint64_t from, uint64_t size, uint8_t* pDest) = 0;
  virtual void     Colapse (uint64_t from, uint64_t to) = 0;
  virtual uint64_t Size () const = 0;
  virtual void     MarkForRemoval () = 0;
};


class FileContainer : public I_DataContainer
{
public:
  FileContainer (const char*  pFileNameBase,
                 const uint64_t maxFileSize,
                 const uint64_t unitsCount);

  virtual ~FileContainer ();

  // WIDataContainer virtual functions
  virtual void     Write (uint64_t to, uint64_t size, const uint8_t* pSource);
  virtual void     Read (uint64_t from, uint64_t size, uint8_t* pDest);
  virtual void     Colapse (uint64_t from, uint64_t to);
  virtual uint64_t Size () const;
  virtual void     MarkForRemoval ();

private:
  void ExtendContainer ();

  const uint64_t       m_MaxFileUnitSize;
  std::vector< File > m_FilesHandles;
  std::string          m_FileNameBase;
  bool                 m_IsMarked;
};

class FileTempContainer : public FileContainer
{
public:
  FileTempContainer (const char*  pFileNameBase,
                     const uint32_t maxFileSize);
  virtual ~FileTempContainer ();
};

class TempContainer : public I_DataContainer
{
public:
  TempContainer (const char* pTempDirectory, const uint_t uReservedMemory);
  virtual ~TempContainer ();

  // WIDataContainer virtual functions
  virtual void     Write (uint64_t to, uint64_t size, const uint8_t* pSource);
  virtual void     Read (uint64_t from, uint64_t size, uint8_t* pDest);
  virtual void     Colapse (uint64_t from, uint64_t to);
  virtual uint64_t Size () const;
  virtual void     MarkForRemoval ();

private:
  void  FillCache (uint64_t position);

  std::auto_ptr<FileTempContainer> m_FileContainer;
  std::auto_ptr<uint8_t>           m_Cache;
  uint64_t                         m_CacheStartPos;
  uint64_t                         m_CacheEndPos;
  const uint_t                     m_CacheSize;
  bool                             m_DirtyCache;

  static uint64_t      smTemporalsCount;
  static Lock smSync;
};

class WFileContainerException : public DataContainerException
{
public:
  explicit
  WFileContainerException (const char* message,
                           const char* file,
                           uint32_t      line,
                           uint32_t      extra)
    : DataContainerException (message, file, line, extra)
  {
  }

  virtual ~WFileContainerException ()
  {
  }

  virtual Exception*     Clone () const
  {
    return new WFileContainerException (*this);
  }
  virtual EXPCEPTION_TYPE Type () const { return FILE_CONTAINER_EXCEPTION; }
  virtual const char*   Description () const
  {
    switch (Extra ())
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

} //namespace pastra
} //namespace whisper

#endif /* PS_CONTAINER_H_ */
