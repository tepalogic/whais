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
                              const D_UINT64 uMaxFileSize,
                              const D_UINT64 uUnitsCount)
  : m_uMaxFileUnitSize (uMaxFileSize),
    m_FilesHandles (),
    m_FileNameBase (pFileNameBase),
    m_IsMarked (false)
{
  D_UINT uOpenMode;

  uOpenMode = (uUnitsCount > 0) ? WHC_FILEOPEN_EXISTING : WHC_FILECREATE_NEW;
  uOpenMode |= WHC_FILERDWR;

  for (D_UINT uCounter = 0; uCounter < uUnitsCount; ++uCounter)
    {
      std::string fileName = m_FileNameBase;
      if (uCounter != 0)
        append_int_to_str (fileName, uCounter);

      WFile container (fileName.c_str (), uOpenMode);
      m_FilesHandles.push_back (container);
    }

  assert (m_FilesHandles.size () == uUnitsCount);

  // Check for structural consistency
  for (D_UINT uCounter = 0; uCounter < uUnitsCount; ++uCounter)
    {
      WFile& rContainerUnit = m_FilesHandles[uCounter];

      if ((rContainerUnit.GetSize () != uMaxFileSize) &&
          ((uCounter != (uUnitsCount - 1)) || (rContainerUnit.GetSize () > uMaxFileSize)))
        {
          throw WFileContainerException ("Inconsistent container!",
                                         _EXTRA
                                         (WFileContainerException::CONTAINTER_INVALID));
        }
    }

}

FileContainer::~FileContainer ()
{
  if (m_IsMarked)
    ColapseContent (0, GetContainerSize() );
}

void
FileContainer::StoreData (D_UINT64 uPosition, D_UINT64 uLength, const D_UINT8* puDataSource)
{
  D_UINT64     uContainerIndex  = uPosition / m_uMaxFileUnitSize;
  D_UINT64     uUnitPosition    = uPosition % m_uMaxFileUnitSize;
  const D_UINT uContainersCount = m_FilesHandles.size ();

  if (uContainerIndex > uContainersCount)
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));
  else if (uContainerIndex == uContainersCount)
    {
      if (uUnitPosition != 0)
        throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));
      else
        ExtendContainer ();
    }

  D_UINT uWriteLength = uLength;

  if ((uWriteLength + uUnitPosition) > m_uMaxFileUnitSize)
    uWriteLength = m_uMaxFileUnitSize - uUnitPosition;

  assert (uWriteLength <= uLength);

  WFile & rUnitContainer = m_FilesHandles[uContainerIndex];

  if (rUnitContainer.GetSize () < uUnitPosition)
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));

  rUnitContainer.Seek (uUnitPosition, WHC_SEEK_BEGIN);
  rUnitContainer.Write (puDataSource, uWriteLength);

  //Let write the rest
  if (uWriteLength < uLength)
    StoreData (uPosition + uWriteLength, uLength - uWriteLength, puDataSource + uWriteLength);
}

void
FileContainer::RetrieveData (D_UINT64 uPosition, D_UINT64 uLength, D_UINT8* puDataDestination)
{
  D_UINT64     uContainerIndex  = uPosition / m_uMaxFileUnitSize;
  D_UINT64     uUnitPosition    = uPosition % m_uMaxFileUnitSize;
  const D_UINT uContainersCount = m_FilesHandles.size ();

  if ((uContainerIndex > uContainersCount) || (uPosition + uLength > GetContainerSize ()))
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));

  WFile rUnitContainer = m_FilesHandles[uContainerIndex];

  D_UINT uReadLength = uLength;

  if (uReadLength + uUnitPosition > rUnitContainer.GetSize ())
    uReadLength = rUnitContainer.GetSize () - uUnitPosition;

  rUnitContainer.Seek (uUnitPosition, WHC_SEEK_BEGIN);
  rUnitContainer.Read (puDataDestination, uReadLength);

  //Lets read the rest
  if (uReadLength != uLength)
    RetrieveData (uPosition + uReadLength, uLength - uReadLength, puDataDestination + uReadLength);

}

void
FileContainer::ColapseContent (D_UINT64 uStartPosition, D_UINT64 uEndPosition)
{
  const D_UINT   cuBufferSize   = 4096;	//4KB
  const D_UINT64 uIntervalSize  = uEndPosition - uStartPosition;
  const D_UINT64 uContainerSize = GetContainerSize ();

  if ((uEndPosition < uStartPosition) || (uEndPosition > uContainerSize))
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_PARAMETERS));
  else if (uIntervalSize == 0)
    return;

  std::auto_ptr< D_UINT8 > aBuffer (new D_UINT8[cuBufferSize]);

  while (uEndPosition < uContainerSize)
    {
      D_UINT64 uStepSize = cuBufferSize;

      if (uStepSize + uEndPosition > uContainerSize)
        uStepSize = uContainerSize - uEndPosition;

      RetrieveData (uEndPosition, uStepSize, aBuffer.get ());
      StoreData (uStartPosition, uStepSize, aBuffer.get ());

      uEndPosition   += uStepSize;
      uStartPosition += uStepSize;
    }

  //Let's delete the remaining junk.
  const D_UINT64 uNewSize           = uContainerSize - uIntervalSize;
  D_INT          uLastContainer     = uNewSize / m_uMaxFileUnitSize;
  const D_INT    uLastContainerSize = uNewSize % m_uMaxFileUnitSize;

  if (uNewSize == 0)
    --uLastContainer;
  else
    m_FilesHandles[uLastContainer].SetSize (uLastContainerSize);

  for (D_INT uIndex = m_FilesHandles.size () - 1; uIndex > uLastContainer;
       --uIndex)
    {
      m_FilesHandles[uIndex].Close ();

      std::string fileName = m_FileNameBase;
      if (uIndex != 0)
        append_int_to_str (fileName, uIndex);

      if (!whc_fremove (fileName.c_str ()))
        throw WFileContainerException (NULL, _EXTRA (WFileContainerException::FILE_OS_IO_ERROR));

      m_FilesHandles.pop_back ();
    }

}

D_UINT64
FileContainer::GetContainerSize () const
{
  if (m_FilesHandles.size () == 0)
    return 0;

  const WFile& rLastUnit = m_FilesHandles[m_FilesHandles.size () - 1];
  D_UINT64     uResult   = (m_FilesHandles.size () - 1) * m_uMaxFileUnitSize;

  uResult += rLastUnit.GetSize ();

  return uResult;
}

void
FileContainer::MarkForRemoval()
{
  m_IsMarked = true;
}

void
FileContainer::ExtendContainer ()
{
  D_UINT      uCount   = m_FilesHandles.size ();
  std::string fileName = m_FileNameBase;

  if (uCount != 0)
    append_int_to_str (fileName, uCount);

  WFile container (fileName.c_str (), WHC_FILECREATE_NEW | WHC_FILERDWR);
  m_FilesHandles.push_back (container);
}

////////////WTempFileContainer///////////////////////////////////////////////

FileTempContainer::FileTempContainer (const D_CHAR*  pFileNameBase,
                                      const D_UINT32 uMaxFileSize):
    FileContainer (pFileNameBase, uMaxFileSize, 0)
{
  MarkForRemoval ();
}

FileTempContainer::~FileTempContainer ()
{
}

//////////////////WTemCotainer/////////////////////////////////////////////////

TempContainer::TempContainer (const D_CHAR* pTempDirectory, D_UINT uReservedMemory) :
  I_DataContainer (),
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
TempContainer::StoreData (D_UINT64 uPosition, D_UINT64 uLength, const D_UINT8* puDataSource)
{
  if (uPosition > GetContainerSize ())
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));

  while (uLength > 0)
    {
      assert ((m_CacheStartPos % m_CacheSize) == 0);
      if ((uPosition >= m_CacheStartPos) && (uPosition < (m_CacheStartPos + m_CacheSize)))
        {
          const D_UINT toWrite = MIN (uLength, m_CacheStartPos + m_CacheSize - uPosition);
          memcpy (m_Cache.get () + (uPosition - m_CacheStartPos), puDataSource, toWrite);

          if (uPosition + toWrite > m_CacheEndPos)
            m_CacheEndPos = uPosition + toWrite;

          m_DirtyCache = true;
          uPosition    += toWrite;
          puDataSource += toWrite;
          uLength      -= toWrite;

        }
      else
        FillCache (uPosition);
    }
}

void
TempContainer::RetrieveData (D_UINT64 uPosition, D_UINT64 uLength, D_UINT8* puDataDestination)
{
  if (uPosition + uLength > GetContainerSize ())
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION));

  while (uLength > 0)
    {
      assert ((m_CacheStartPos % m_CacheSize) == 0);
      if ((uPosition >= m_CacheStartPos) && (uPosition < m_CacheEndPos))
        {
          const D_UINT toRead = MIN (uLength, m_CacheEndPos - uPosition);
          memcpy (puDataDestination, m_Cache.get () + (uPosition - m_CacheStartPos), toRead);

          uPosition         += toRead;
          puDataDestination += toRead;
          uLength           -= toRead;

        }
      else
        FillCache (uPosition);
    }
}

void
TempContainer::ColapseContent (D_UINT64 uStartPosition, D_UINT64 uEndPosition)
{
  if (uStartPosition > uEndPosition)
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_PARAMETERS));

  if (uEndPosition > GetContainerSize ())
    throw WFileContainerException (NULL, _EXTRA (WFileContainerException::INVALID_PARAMETERS));

  if (m_FileContainer.get () != NULL)
    {
      //Flush the buffer first!
      if (m_DirtyCache)
        {
          m_FileContainer->StoreData (m_CacheStartPos, m_CacheEndPos - m_CacheStartPos, m_Cache.get ());
          m_DirtyCache = false;
        }

      m_FileContainer->ColapseContent (uStartPosition, uEndPosition);

      if (m_FileContainer->GetContainerSize () < uStartPosition)
        FillCache (0);
      else
        FillCache (uStartPosition);
    }
  else
    {
      D_UINT8* const pMem        = m_Cache.get();
      const D_UINT   colapseSize = m_CacheEndPos - uEndPosition;

      safe_memcpy ( pMem + uStartPosition, pMem + uEndPosition, colapseSize);
      m_CacheEndPos -= (uEndPosition - uStartPosition);
    }

  if ((m_FileContainer.get () != NULL) && (m_CacheSize > m_FileContainer->GetContainerSize()))
    m_FileContainer.reset (NULL);

}

void
TempContainer::MarkForRemoval ()
{
  return ; //This is automatically deleted. Nothing to do here!
}

D_UINT64
TempContainer::GetContainerSize () const
{
  assert ((m_CacheStartPos % m_CacheSize) == 0);
  assert (m_CacheStartPos <= m_CacheEndPos);

  if (m_FileContainer.get () != NULL)
    {
      const D_UINT64 result = m_FileContainer->GetContainerSize ();

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
      m_FileContainer->StoreData (0, m_CacheEndPos, m_Cache.get ());

      m_CacheStartPos = m_CacheEndPos = position;
      m_DirtyCache    = false;

      return;
    }
  else
    {
      if (m_DirtyCache)
        {
          m_FileContainer->StoreData (m_CacheStartPos, m_CacheEndPos - m_CacheStartPos, m_Cache.get ());
          m_DirtyCache = false;
        }

      const D_UINT toRead = MIN (m_CacheSize, m_FileContainer->GetContainerSize() - position);

      m_FileContainer->RetrieveData (position, toRead, m_Cache.get ());
      m_CacheStartPos = position;
      m_CacheEndPos   = m_CacheStartPos + toRead;
    }
}

D_UINT64      TempContainer::sm_TemporalsCount = 0;
WSynchronizer TempContainer::sm_Sync;
