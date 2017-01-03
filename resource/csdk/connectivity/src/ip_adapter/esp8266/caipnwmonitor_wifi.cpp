/* ****************************************************************
*
* Copyright 2016 Web of Objects project All Rights Reserved.
* Author: Khaled Q. AbdelFadeel <kqorany2@gmail.com> and Khaled F. Elsayed <kelsayed@gmail.com>
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
 * This file is to keep design in sync with other platforms.  Right now
 * there is no api for network monitoring in esp8266.
 */

#include "caipinterface.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <IPAddress.h>

#include "logger.h"
#include "cacommon.h"
#include "caipadapter.h"
#include "caipnwmonitor.h"
#include "caadapterutils.h"
#include "oic_malloc.h"
#include "oic_string.h"

#define TAG "IPNW"

// Since the CA abstraction expects a value for "family", AF_INET will be
// defined & used (as-is defined in the linux socket headers).
#define AF_INET (2)

CAResult_t CAIPStartNetworkMonitor(CAIPAdapterStateChangeCallback callback,
                                   CATransportAdapter_t adapter)
{
    return CA_STATUS_OK;
}
CAResult_t CAIPStopNetworkMonitor(CATransportAdapter_t adapter)
{
    return CA_STATUS_OK;
}

/// Retrieves the IP address assigned to esp8266 WiFi
void CAESP8266GetInterfaceAddress(char *address, size_t len)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (WiFi.status() != WL_CONNECTED)
    {
        OIC_LOG(DEBUG, TAG, "No WIFI");
        return;
    }

    VERIFY_NON_NULL_VOID(address, TAG, "Invalid address");

    IPAddress ip = WiFi.localIP();
    snprintf(address, len, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    OIC_LOG_V(DEBUG, TAG, "Wifi shield address is: %s", address);
    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}

u_arraylist_t *CAIPGetInterfaceInformation(int desiredIndex)
{
    bool result = true;

    u_arraylist_t *iflist = u_arraylist_create();
    if (!iflist)
    {
        OIC_LOG(ERROR, TAG, "Failed to create iflist");
        return NULL;
    }

    CAInterface_t *ifitem = (CAInterface_t *)OICCalloc(1, sizeof(CAInterface_t));
    if (!ifitem)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed");
        goto exit;
    }

    // Since ESP8266 currently only supports one interface, the next 4 lines are sufficient.
    OICStrcpy(ifitem->name, INTERFACE_NAME_MAX, "WIFI");
    ifitem->index = 1;
    ifitem->family = AF_INET;
    ifitem->flags = 0;
    CAESP8266GetInterfaceAddress(ifitem->addr, sizeof(ifitem->addr));

    result = u_arraylist_add(iflist, ifitem);
    if (!result)
    {
        OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
        goto exit;
    }

    OIC_LOG_V(DEBUG, TAG, "Added interface: %s (%d)", ifitem->name, ifitem->family);

    return iflist;

exit:
    u_arraylist_destroy(iflist);
    return NULL;
}

