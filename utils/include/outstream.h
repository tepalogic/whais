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

#define OUTSTREAM_INCREMENT_SIZE    512

struct OutputStream
{
  uint8_t* data;
  uint_t   dataSize;
  uint_t   increment;
  uint_t   reserved;
};

#ifdef __cplusplus
extern "C"
{
#endif

struct OutputStream*
init_outstream (const uint_t increment, struct OutputStream* pOutStream);

void
destroy_outstream (struct OutputStream* pStream);

struct OutputStream*
output_data (struct OutputStream* pStream, const uint8_t* pData, uint_t dataSize);

INLINE static struct OutputStream*
output_uint8 (struct OutputStream* pStream, uint8_t value)
{
  return output_data (pStream, &value, sizeof value);
}

INLINE static struct OutputStream*
output_uint16 (struct OutputStream* pStream, uint16_t value)
{
  return output_data (pStream, (uint8_t*)&value, sizeof value);
}

INLINE static struct OutputStream*
output_uint32 (struct OutputStream* pStream, uint32_t value)
{
  return output_data (pStream, (uint8_t*)&value, sizeof value);
}

INLINE static struct OutputStream*
output_uint64 (struct OutputStream* pStream, uint64_t value)
{
  return output_data (pStream, (uint8_t*)&value, sizeof value);
}

#define get_buffer_outstream(pStream) ((pStream)->data)
#define get_size_outstream(pStream) ((pStream)->dataSize)

#ifdef __cplusplus
}                                /* extern "C" */
#endif

#endif /* OUTSTREAM_H */
