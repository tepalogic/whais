/*
 * wlimits.h
 *
 *  Created on: 28.05.2013
 *      Author: Iulian
 */

#ifndef WLIMITS_H_
#define WLIMITS_H_

#include <assert.h>
#include <limits.h>

#include "whisper.h"

namespace whisper {


template<typename T> T
w_max_value ()
{
  assert (false);
}

template<typename T> T
w_min_value ()
{
  assert (false);
}



template<> int8_t
w_max_value ()
{
  return 127ll;
}

template<> int8_t
w_min_value ()
{
  return -128ll;
}



template<> int16_t
w_max_value ()
{
  return 32767ll;
}

template<> int16_t
w_min_value ()
{
  return -32768ll;
}



template<> int32_t
w_max_value ()
{
  return 2147483647ll;
}

template<> int32_t
w_min_value ()
{
  return -2147483648ll;
}



template<> int64_t
w_max_value ()
{
  return 9223372036854775807ll;
}

template<> int64_t
w_min_value ()
{
  /* Avoid some fake warnings! */
  uint64_t val = 1;

  val <<= 63;

  return val;
}




template<> uint8_t
w_max_value ()
{
  return 255ull;
}

template<> uint8_t
w_min_value ()
{
  return 0;
}



template<> uint16_t
w_max_value ()
{
  return 6553ull;
}

template<> uint16_t
w_min_value ()
{
  return 0;
}



template<> uint32_t
w_max_value ()
{
  return 4294967295ull;
}

template<> uint32_t
w_min_value ()
{
  return 0;
}



template<> uint64_t
w_max_value ()
{
  return 18446744073709551615ull;
}

template<> uint64_t
w_min_value ()
{
  return 0;
}




} //namespace prima


#endif /* WLIMITS_H_ */
