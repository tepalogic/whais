/*
 * test_client_common.h
 *
 *  Created on: Jan 9, 2013
 *      Author: ipopa
 */

#ifndef TEST_CLIENT_COMMON_H_
#define TEST_CLIENT_COMMON_H_

#include "whisper.h"

#include "whisper_connector.h"

bool
tc_settup_connection (int              argc,
                      char const **    argv,
                      W_CONNECTOR_HND* pHnd);

#endif /* TEST_CLIENT_COMMON_H_ */
