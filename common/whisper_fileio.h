/******************************************************************************
WHISPER - An advanced database system
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

#ifndef WHISPER_FILEIO_H_
#define WHISPER_FILEIO_H_

#define WHC_FILECREATE          0x00000001
#define WHC_FILECREATE_NEW      0x00000002
#define WHC_FILEOPEN_EXISTING   0x00000004
#define WHC_FILEDIRECT          0x00000008
#define WHC_FILESYNC            0x00000010

#define WHC_FILEREAD            0x00000100
#define WHC_FILEWRITE           0x00000200
#define WHC_FILERDWR            (WHC_FILEREAD | WHC_FILEWRITE)

#define WHC_SEEK_BEGIN          0x00000001
#define WHC_SEEK_CURR           0x00000002
#define WHC_SEEK_END            0x00000004

#ifdef __cplusplus
extern "C"
{
#endif

CUSTOM_SHL WH_FILE_HND
whc_fopen (const D_CHAR* pFileName, D_UINT mode);

CUSTOM_SHL WH_FILE_HND
whc_fdup (WH_FILE_HND f_hnd);

CUSTOM_SHL D_BOOL
whc_fread (WH_FILE_HND f_hnd, D_UINT8* pBuffer, D_UINT size);

CUSTOM_SHL D_BOOL
whc_fwrite (WH_FILE_HND f_hnd, const D_UINT8* pBuffer, D_UINT size);

CUSTOM_SHL D_BOOL
whc_fseek (WH_FILE_HND f_hnd, D_INT64 where, D_INT whence);

CUSTOM_SHL D_BOOL
whc_ftell (WH_FILE_HND f_hnd, D_UINT64* pOutPosition);

CUSTOM_SHL D_BOOL
whc_fsync (WH_FILE_HND f_hnd);

CUSTOM_SHL D_BOOL
whc_ftellsize (WH_FILE_HND f_hnd, D_UINT64* pOutSize);

CUSTOM_SHL D_BOOL
whc_fsetsize (WH_FILE_HND, D_UINT64 newSize);

CUSTOM_SHL D_BOOL
whc_fclose (WH_FILE_HND f_hnd);

CUSTOM_SHL D_UINT32
whc_fgetlasterror ();

CUSTOM_SHL D_BOOL
whc_ferrtostrs (D_UINT64 errorCode, D_CHAR* str, D_UINT strSize);

CUSTOM_SHL D_BOOL
whc_fremove (const D_CHAR* pFileName);

CUSTOM_SHL const D_CHAR*
whc_get_directory_delimiter ();

CUSTOM_SHL const D_CHAR*
whc_get_current_directory ();

CUSTOM_SHL D_BOOL
whc_is_path_absolute (const D_CHAR* pPath);

#ifdef __cplusplus
} /* extern C++ */
#endif

#endif                                /* WHISPER_FILEIO_H_ */
