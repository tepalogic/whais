/*
 * whisper_shl.h
 *
 *  Created on: Aug 13, 2012
 *      Author: iupo
 */

#ifndef WHISPER_SHL_H_
#define WHISPER_SHL_H_

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


#endif /* WHISPER_SHL_H_ */
