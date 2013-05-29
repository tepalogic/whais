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
  return 127;
}

template<> int8_t
w_min_value ()
{
  return -_SC(int64_t, 128);
}



template<> int16_t
w_max_value ()
{
  return 32767;
}

template<> int16_t
w_min_value ()
{
  return -_SC(int64_t, 32768);
}



template<> int32_t
w_max_value ()
{
  return 2147483647;
}

template<> int32_t
w_min_value ()
{
  return -_SC(int64_t, 2147483648);
}



template<> int64_t
w_max_value ()
{
  return 9223372036854775807;
}

template<> int64_t
w_min_value ()
{
  return -_SC (int64_t, 9223372036854775808);
}




template<> uint8_t
w_max_value ()
{
  return 255;
}

template<> uint8_t
w_min_value ()
{
  return 0;
}



template<> uint16_t
w_max_value ()
{
  return 65535;
}

template<> uint16_t
w_min_value ()
{
  return 0;
}



template<> uint32_t
w_max_value ()
{
  return 4294967295;
}

template<> uint32_t
w_min_value ()
{
  return 0;
}



template<> uint64_t
w_max_value ()
{
  return 18446744073709551615;
}

template<> uint64_t
w_min_value ()
{
  return 0;
}




} //namespace prima


#endif /* WLIMITS_H_ */
