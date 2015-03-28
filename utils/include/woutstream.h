/******************************************************************************
UTILS - Common routines used trough WHAIS project
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

#ifndef WOUTSTREAM_H
#define WOUTSTREAM_H

#include "whais.h"

#include "utils/endianness.h"
/* A convenient way to build a buffer with user content. */

/* The size of memory to allocate when the buffer needs to be extended. */
#define OUTSTREAM_INCREMENT_SIZE    512



struct WOutputStream
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


/* Initialise the buffer builder. */
struct WOutputStream*
wh_ostream_init (const uint_t                increment,
                 struct WOutputStream* const outStream);


/* Clean the resources associated with this buffer. */
void
wh_ostream_clean (struct WOutputStream* const stream);


/* Send a chink of data to the stream. */
struct WOutputStream*
wh_ostream_write (struct WOutputStream* const stream,
                  const uint8_t*              data,
                  uint_t                      dataSize);


INLINE static struct WOutputStream*
wh_ostream_wint8 (struct WOutputStream* const stream, const uint8_t value)
{
  return wh_ostream_write (stream, &value, sizeof value);
}


INLINE static struct WOutputStream*
wh_ostream_wint16 (struct WOutputStream* const stream, const uint16_t value)
{
  uint8_t temp[2];

  store_le_int16 (value, temp);
  return wh_ostream_write (stream, temp, sizeof temp);
}


INLINE static struct WOutputStream*
wh_ostream_wint32 (struct WOutputStream* const stream, const uint32_t value)
{
  uint8_t temp[4];

  store_le_int32 (value, temp);
  return wh_ostream_write (stream, temp, sizeof temp);
}


INLINE static struct WOutputStream*
wh_ostream_wint64 (struct WOutputStream* stream, const uint64_t value)
{
  uint8_t temp[8];

  store_le_int64 (value, temp);
  return wh_ostream_write (stream, temp, sizeof temp);
}


#define wh_ostream_data(stream) ((stream)->data)
#define wh_ostream_size(stream) ((stream)->dataSize)

#ifdef __cplusplus
}                                /* extern "C" */
#endif

#endif /* WOUTSTREAM_H */

