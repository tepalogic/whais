/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2008  Iulian Popa

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

#ifndef WHAIS_FILEIO_H_
#define WHAIS_FILEIO_H_

#define WH_FILECREATE          0x00000001
#define WH_FILECREATE_NEW      0x00000002
#define WH_FILEOPEN_EXISTING   0x00000004
#define WH_FILETRUNC           0x00000008
#define WH_FILESYNC            0x00000010

#define WH_FILEREAD            0x00000100
#define WH_FILEWRITE           0x00000200
#define WH_FILERDWR            (WH_FILEREAD | WH_FILEWRITE)

#define WH_SEEK_BEGIN          0x00000001
#define WH_SEEK_CURR           0x00000002
#define WH_SEEK_END            0x00000004

#ifdef __cplusplus
extern "C"
{
#endif

WH_FILE
whf_open(const char* const file, uint_t mode);

bool_t
whf_read(WH_FILE hnd, uint8_t* dstBuffer, uint_t size);

bool_t
whf_write(WH_FILE hnd, const uint8_t* srcBuffer, uint_t size);

bool_t
whf_seek(WH_FILE hnd, int64_t where, int whence);

bool_t
whf_tell(WH_FILE hnd, uint64_t* const outPosition);

bool_t
whf_sync(WH_FILE hnd);

bool_t
whf_tell_size(WH_FILE hnd, uint64_t* const outSize);

bool_t
whf_set_size(WH_FILE, const uint64_t newSize);

bool_t
whf_close(WH_FILE hnd);

uint32_t
whf_last_error();

bool_t
whf_err_to_str(uint64_t errorCode, char* str, uint_t strSize);

bool_t
whf_remove(const char* const file);

void
whf_move_file(const char* existingFIle, const char* newFile );

const char
whf_dir_delim();

const char*
whf_current_dir();

bool_t
whf_is_absolute(const char* const path);

bool_t
whf_file_exists(const char* const file);

#ifdef __cplusplus
} /* extern 'C' */
#endif

#endif /* WHAIS_FILEIO_H_ */

