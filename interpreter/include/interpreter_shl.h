/*
 * interpreter_shl.h
 *
 *  Created on: Aug 10, 2012
 *      Author: iupo
 */

#ifndef INTERPRETER_SHL_H_
#define INTERPRETER_SHL_H_

#ifdef INTERP_EXPORTING
  #define INTERP_SHL SHL_EXPORT_SYMBOL
#else
  #define INTERP_SHL SHL_IMPORT_SYMBOL
#endif

#ifdef PRIMA_EXPORTING
  #define PRIMA_SHL SHL_EXPORT_SYMBOL
#else
  #define PRIMA_SHL SHL_IMPORT_SYMBOL
#endif

#endif /* INTERPRETER_SHL_H_ */
