/* ****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * @file
 * This file provides APIs ethernet client/server/network monitor modules.
 */

#ifndef CA_ETHERNET_ADAPTER_UTILS_
#define CA_ETHERNET_ADAPTER_UTILS_

#include <Arduino.h>
#include <Ethernet.h>
#include <socket.h>
#include <w5100.h>
#include <IPAddress.h>

#include "experimental/logger.h"
#include "cacommon.h"
#include "caadapterinterface.h"
#include "caipadapter.h"
#include "caadapterutils.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Get available TCP socket.
 * @param[out]   sockID         Available UDP socket ID.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAArduinoGetAvailableTCPSocket(int *sockID);

/**
 * Initialize Unicast TCP socket.
 * @param[in/out]   port        Port to start the unicast server.
 * @param[out]      socketID    Unicast socket ID.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAArduinoInitTCPSocket(uint16_t *port, int *socketID);

#ifdef __cplusplus
}
#endif

#endif /* CA_ETHERNET_ADAPTER_UTILS_ */

