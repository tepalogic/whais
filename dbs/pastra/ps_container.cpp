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

#include <assert.h>
#include <string.h>
#include <memory>

#include "dbs_mgr.h"

#include "ps_container.h"

using namespace pastra;

static void
safe_memcpy (D_UINT8* pDest, D_UINT8* pSrc, D_UINT64 uCount)
{
  while (uCount-- > 0)
    *pDest++ = *pSrc++;
}

void
pastra::append_int_to_str (std::string& dest, D_UINT64 number)
{
  const D_UINT bitsCount    = sizeof (number) * 8;
  D_CHAR       buffer[bitsCount];
  D_CHAR*      conv         = &buffer[bitsCount - 1];

  *conv = 0;
  do
    {
      conv--;
      *conv  = _SC (D_CHAR, ('0' + (number % 10)));
      number /= 10;
    }
  while (number != 0);

  dest += conv;
}

FileContainer::FileContainer (const D_CHAR*  pFileNameBase,
                              const D_UINT64 maxFileSize,
                              const D_UINT64 unitsCount)
  : m_MaxFileUnitSize (maxFileSize),
    m_FilesHandles (),
    m_FileNameBase (pFileNameBase),
    m_IsMarked (false)
{
  D_UINT uOpenMode;

  uOpenMode = (unitsCount > 0) ? WHC_FILEOPEN_EXISTING : WHC_FILECREATE_NEW;
  uOpenMode |= WHC_FILERDWR;

  for (D_UINT unit = 0; unit < unitsCount; ++unit)
    {
      std::string fileName = m_FileNameBase;
      if (unit != 0)
        append_int_to_str (fileName, unit);

      WFile container (fileName.c_str (), uOpenMode);
      m_FilesHandles.push_back (container);
    }

  assert (m_FilesHandles.size () == unitsCount);

  // Check for structural consistency
  for (D_UINT unit = 0; unit < unitsCount; ++unit)
    {
      WFile& unitFile = m_FilesHandles[unit];

      if ((unitFile.GetSize () != maxFileSize) &&
          ((unit != (unitsCount - 1)) || (unitFile.GetSize () > maxFileSize)))
        {
          throw WFileContainerException ("Inconsistent container!",
                                         _EXTRA (WFileContainerException::CONTAINTER_INVALID));
        }
    }
}

FileContainer::~FileContainer ()
{
  if (m_IsMarked)
    Colapse (0, Size() );
}

void
FileContainer::Write (D_UINT64 to, D_UINT64 size, const D_UINT8* pSource)
{
  const D_UINT unitsCount   = m_FilesHandles.size ();
  D_UINT64     unitIndex    = to / m_MaxFileUnitSize;
  D_UINT64     unitPosition = to % m_MaxFileUnitSize;

  if (unitIndex > unitsCount)
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));
  else if (unitIndex == unitsCount)
    {
      if (unitPosition != 0)
        throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));
      else
        ExtendContainer ();
    }

  D_UINT64 actualSize = size;

  if ((actualSize + unitPosition) > m_MaxFileUnitSize)
    actualSize = m_MaxFileUnitSize - unitPosition;

  assert (actualSize <= size);

  WFile& unitFile = m_FilesHandles[unitIndex];

  if (unitFile.GetSize () < unitPosition)
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));

  unitFile.Seek (unitPosition, WHC_SEEK_BEGIN);
  unitFile.Write (pSource, actualSize);

  //Let write the rest
  if (actualSize < size)
    Write (to + actualSize, size - actualSize, pSource + actualSize);
}

void
FileContainer::Read (D_UINT64 from, D_UINT64 size, D_UINT8* pDest)
{
  const D_UINT unitsCount   = m_FilesHandles.size ();
  D_UINT64     unitIndex    = from / m_MaxFileUnitSize;
  D_UINT64     unitPosition = from % m_MaxFileUnitSize;

  if ((unitIndex > unitsCount) || (from + size > Size ()))
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));

  WFile unitFile = m_FilesHandles[unitIndex];

  D_UINT64 actualSize = size;

  if (actualSize + unitPosition > unitFile.GetSize ())
    actualSize = unitFile.GetSize () - unitPosition;

  unitFile.Seek (unitPosition, WHC_SEEK_BEGIN);
  unitFile.Read (pDest, actualSize);

  //Lets read the rest
  if (actualSize != size)
    Read (from + actualSize, size - actualSize, pDest + actualSize);

}

void
FileContainer::Colapse (D_UINT64 from, D_UINT64 to)
{
  const D_UINT   bufferSize    = 4096;        //4KB
  const D_UINT64 intervalSize  = to - from;
  const D_UINT64 containerSize = Size ();

  if ((to < from) || (to > containerSize))
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_PARAMETERS));
  else if (intervalSize == 0)
    return;

  std::auto_ptr< D_UINT8 > aBuffer (new D_UINT8[bufferSize]);

  while (to < containerSize)
    {
      D_UINT64 stepSize = bufferSize;

      if (stepSize + to > containerSize)
        stepSize = containerSize - to;

      Read (to, stepSize, aBuffer.get ());
      Write (from, stepSize, aBuffer.get ());

      to   += stepSize;
      from += stepSize;
    }

  //Let's delete the remaining junk.
  const D_UINT64 newSize      = containerSize - intervalSize;
  D_INT          lastUnit     = newSize / m_MaxFileUnitSize;
  const D_INT    lastUnitSize = newSize % m_MaxFileUnitSize;

  if (newSize == 0)
    --lastUnit;
  else
    m_FilesHandles[lastUnit].SetSize (lastUnitSize);

  for (D_INT unit = m_FilesHandles.size () - 1; unit > lastUnit; --unit)
    {
      m_FilesHandles[unit].Close ();

      std::string fileName = m_FileNameBase;
      if (unit != 0)
        append_int_to_str (fileName, unit);

      if (!whc_fremove (fileName.c_str ()))
        throw WFileContainerException (NULL, _EXTRA (WFileContainerException::FILE_OS_IO_ERROR));

      m_FilesHandles.pop_back ();
    }

}

D_UINT64
FileContainer::Size () const
{
  if (m_FilesHandles.size () == 0)
    return 0;

  const WFile& lastUnitFile = m_FilesHandles[m_FilesHandles.size () - 1];
  D_UINT64     result       = (m_FilesHandles.size () - 1) * m_MaxFileUnitSize;

  result += lastUnitFile.GetSize ();

  return result;
}

void
FileContainer::MarkForRemoval()
{
  m_IsMarked = true;
}

void
FileContainer::ExtendContainer ()
{
  D_UINT      count   = m_FilesHandles.size ();
  std::string fileName = m_FileNameBase;

  if (count != 0)
    append_int_to_str (fileName, count);

  WFile unitFile (fileName.c_str (), WHC_FILECREATE_NEW | WHC_FILERDWR);
  m_FilesHandles.push_back (unitFile);
}

////////////WTempFileContainer///////////////////////////////////////////////

FileTempContainer::FileTempContainer (const D_CHAR*  pFileNameBase,
                                      const D_UINT32 uMaxFileSize)
  : FileContainer (pFileNameBase, uMaxFileSize, 0)
{
  MarkForRemoval ();
}

FileTempContainer::~FileTempContainer ()
{
}

//////////////////WTemCotainer/////////////////////////////////////////////////

TempContainer::TempContainer (const D_CHAR* pTempDirectory, D_UINT uReservedMemory)
  : I_DataContainer (),
    m_FileContainer (NULL),
    m_Cache (new D_UINT8[uReservedMemory]),
    m_CacheStartPos (0),
    m_CacheEndPos (0),
    m_CacheSize (uReservedMemory),
    m_DirtyCache (false)
{
}

TempContainer::~TempContainer ()
{
}

void
TempContainer::Write (D_UINT64 to, D_UINT64 size, const D_UINT8* pSource)
{
  if (to > Size ())
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));

  while (size > 0)
    {
      assert ((m_CacheStartPos % m_CacheSize) == 0);
      if ((to >= m_CacheStartPos) && (to < (m_CacheStartPos + m_CacheSize)))
        {
          const D_UINT toWrite = MIN (size, m_CacheStartPos + m_CacheSize - to);
          memcpy (m_Cache.get () + (to - m_CacheStartPos), pSource, toWrite);

          if (to + toWrite > m_CacheEndPos)
            m_CacheEndPos = to + toWrite;

          m_DirtyCache = true;
          to    += toWrite;
          pSource += toWrite;
          size      -= toWrite;

        }
      else
        FillCache (to);
    }
}

void
TempContainer::Read (D_UINT64 from, D_UINT64 size, D_UINT8* pDest)
{
  if (from + size > Size ())
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));

  while (size > 0)
    {
      assert ((m_CacheStartPos % m_CacheSize) == 0);
      if ((from >= m_CacheStartPos) && (from < m_CacheEndPos))
        {
          const D_UINT toRead = MIN (size, m_CacheEndPos - from);
          memcpy (pDest, m_Cache.get () + (from - m_CacheStartPos), toRead);

          from         += toRead;
          pDest += toRead;
          size           -= toRead;

        }
      else
        FillCache (from);
    }
}

void
TempContainer::Colapse (D_UINT64 from, D_UINT64 to)
{
  if (from > to)
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_PARAMETERS));

  if (to > Size ())
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_PARAMETERS));

  if (m_FileContainer.get () != NULL)
    {
      //Flush the buffer first!
      if (m_DirtyCache)
        {
          m_FileContainer->Write (m_CacheStartPos, m_CacheEndPos - m_CacheStartPos, m_Cache.get ());
          m_DirtyCache = false;
        }

      m_FileContainer->Colapse (from, to);

      if (m_FileContainer->Size () < from)
        FillCache (0);
      else
        FillCache (from);
    }
  else
    {
      D_UINT8* const pMem        = m_Cache.get();
      const D_UINT   colapseSize = m_CacheEndPos - to;

      safe_memcpy ( pMem + from, pMem + to, colapseSize);
      m_CacheEndPos -= (to - from);
    }

  if ((m_FileContainer.get () != NULL) && (m_CacheSize > m_FileContainer->Size()))
    m_FileContainer.reset (NULL);

}

void
TempContainer::MarkForRemoval ()
{
  return ; //This is automatically deleted. Nothing to do here!
}

D_UINT64
TempContainer::Size () const
{
  assert ((m_CacheStartPos % m_CacheSize) == 0);
  assert (m_CacheStartPos <= m_CacheEndPos);

  if (m_FileContainer.get () != NULL)
    {
      const D_UINT64 result = m_FileContainer->Size ();

      assert (result >= m_CacheStartPos);

      return MAX (result, m_CacheEndPos);
    }

  assert (m_CacheStartPos == 0);
  assert (m_CacheEndPos <= m_CacheSize);

  return m_CacheEndPos;
}

void
TempContainer::FillCache (D_UINT64 position)
{
  position -= (position % m_CacheSize);

  assert ((m_CacheStartPos % m_CacheSize) == 0);
  assert ((position % m_CacheSize) == 0);

  if (m_CacheStartPos == position)
    return;

  if (m_FileContainer.get () == NULL)
    {
      D_UINT64 curentId;

      sm_Sync.Enter ();
      curentId = sm_TemporalsCount++;
      sm_Sync.Leave ();

      assert (m_CacheStartPos == 0);
      assert (m_CacheEndPos == m_CacheSize);
      assert (position == m_CacheSize);

      std::string baseFile (DBSGetTempDir ());
      baseFile += "wtemp";
      append_int_to_str (baseFile, curentId);
      baseFile += ".tmp";


      m_FileContainer.reset (new FileTempContainer (baseFile.c_str (), DBSGetMaxFileSize ()));
      m_FileContainer->Write (0, m_CacheEndPos, m_Cache.get ());

      m_CacheStartPos = m_CacheEndPos = position;
      m_DirtyCache    = false;

      return;
    }
  else
    {
      if (m_DirtyCache)
        {
          m_FileContainer->Write (m_CacheStartPos, m_CacheEndPos - m_CacheStartPos, m_Cache.get ());
          m_DirtyCache = false;
        }

      const D_UINT toRead = MIN (m_CacheSize, m_FileContainer->Size() - position);

      m_FileContainer->Read (position, toRead, m_Cache.get ());
      m_CacheStartPos = position;
      m_CacheEndPos   = m_CacheStartPos + toRead;
    }
}

D_UINT64      TempContainer::sm_TemporalsCount = 0;
WSynchronizer TempContainer::sm_Sync;
