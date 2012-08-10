/*
 * dbs_shl.h
 *
 *  Created on: Aug 10, 2012
 *      Author: iupo
 */

#ifndef DBS_SHL_H_
#define DBS_SHL_H_

#ifdef DBS_EXPORTING
  #define DBS_SHL SHL_EXPORT_SYMBOL
#else
  #define DBS_SHL SHL_IMPORT_SYMBOL
#endif

#ifdef PASTRA_EXPORTING
  #define PASTRA_SHL SHL_EXPORT_SYMBOL
#else
  #define PASTRA_SHL SHL_IMPORT_SYMBOL
#endif

#endif /* DBS_SHL_H_ */
