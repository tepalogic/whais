/******************************************************************************
UTILS - Common routines used trough WHISPER project
Copyright (C) 2009  Iulian Popa

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

#ifndef OUTSTREAM_H
#define OUTSTREAM_H

#include "whisper.h"

#include "utils_shl.h"

#define OUTSTREAM_INCREMENT_SIZE    512

struct OutputStream
{
  D_UINT8* data;
  D_UINT   dataSize;
  D_UINT   increment;
  D_UINT   reserved;
};

#ifdef __cplusplus
extern "C"
{
#endif

UTILS_SHL struct OutputStream*
init_outstream (const D_UINT increment, struct OutputStream* pOutStream);

UTILS_SHL void
destroy_outstream (struct OutputStream* pStream);

UTILS_SHL struct OutputStream*
output_data (struct OutputStream* pStream, const D_UINT8* pData, D_UINT dataSize);

INLINE static struct OutputStream*
output_uint8 (struct OutputStream* pStream, D_UINT8 value)
{
  return output_data (pStream, &value, sizeof value);
}

INLINE static struct OutputStream*
output_uint16 (struct OutputStream* pStream, D_UINT16 value)
{
  return output_data (pStream, (D_UINT8*)&value, sizeof value);
}

INLINE static struct OutputStream*
output_uint32 (struct OutputStream* pStream, D_UINT32 value)
{
  return output_data (pStream, (D_UINT8*)&value, sizeof value);
}

INLINE static struct OutputStream*
output_uint64 (struct OutputStream* pStream, D_UINT64 value)
{
  return output_data (pStream, (D_UINT8*)&value, sizeof value);
}

#define get_buffer_outstream(pStream) ((pStream)->data)
#define get_size_outstream(pStream) ((pStream)->dataSize)

#ifdef __cplusplus
}                                /* extern "C" */
#endif

#endif /* OUTSTREAM_H */
