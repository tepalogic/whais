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

#ifndef WHAIS_SHL_H_
#define WHAIS_SHL_H_

#ifdef USE_COMPILER_SHL
  #ifdef COMPILER_EXPORTING
    #define COMPILER_SHL SHL_EXPORT_SYMBOL
  #else
    #define COMPILER_SHL SHL_IMPORT_SYMBOL
  #endif
  #define USE_SHL
#else
  #define COMPILER_SHL
#endif

#ifdef USE_DBS_SHL
  #ifdef DBS_EXPORTING
    #define DBS_SHL SHL_EXPORT_SYMBOL
  #else
    #define DBS_SHL SHL_IMPORT_SYMBOL
  #endif
  #define USE_SHL
#else
  #define DBS_SHL
#endif

#ifdef USE_INTERP_SHL
  #ifdef INTERP_EXPORTING
    #define INTERP_SHL SHL_EXPORT_SYMBOL
  #else
    #define INTERP_SHL SHL_IMPORT_SYMBOL
  #endif
  #define USE_SHL
#else
  #define INTERP_SHL
#endif

#ifdef USE_SHL
  #ifdef EXPORT_EXCEP_SHL
    #define EXCEP_SHL SHL_EXPORT_SYMBOL
  #else
    #define EXCEP_SHL SHL_IMPORT_SYMBOL
  #endif
#else
  #define EXCEP_SHL
#endif


#if ! (defined(YYTOKENTYPE) || defined(YYBISON))
#ifdef __cplusplus
extern "C"
{
#endif


WH_SHLIB
wh_shl_load(const char* const library);

void
wh_shl_release(WH_SHLIB shl);

void*
wh_shl_symbol(WH_SHLIB shl, const char* const symbol);


#ifdef __cplusplus
}
#endif
#endif /* ! (defined(YYTOKENTYPE) || defined(YYBISON)) */

#endif /* WHAIS_SHL_H_ */

