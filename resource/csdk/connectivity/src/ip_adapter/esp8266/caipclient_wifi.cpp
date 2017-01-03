/******************************************************************
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
#include "caipinterface.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <IPAddress.h>

#include "logger.h"
#include "cacommon.h"
#include "caadapterinterface.h"
#include "caipadapter.h"
#include "caadapterutils.h"
#include "oic_string.h"

/// This is the max buffer size of esp8266
#define ESP8266_IP_BUFFERSIZE (90)
#define TAG "IPC"
#define IPv4_MULTICAST     "224.0.1.187"
// Create two different WiFiUDP instances, one for listening and one for transmitting.
//To workaround with the bidirectional communications problem
static WiFiUDP Udp;


void CAIPSetUnicastSocket(int socketID)
{

}

void CAIPSetUnicastPort(uint16_t port)
{

}

void CAIPSendData(CAEndpoint_t *endpoint,
                  const void *data, uint32_t dataLength, bool isMulticast)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL_VOID(data, TAG, "data");
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint");

    uint8_t ip[4] = {0};
    uint16_t parsedPort = 0;

    OIC_LOG_V(DEBUG, TAG, "send from port: %d", caglobals.ip.u4.port);

    if (isMulticast) //multicast
    {
    	OICStrcpy(endpoint->addr, sizeof(endpoint->addr), IPv4_MULTICAST);
        CAResult_t res = CAParseIPv4AddressInternal(endpoint->addr, ip, sizeof(ip),
                                                    &parsedPort);
        if (res != CA_STATUS_OK)
        {
            OIC_LOG_V(ERROR, TAG, "Remote adrs parse fail %d", res);
            return;
        }
        parsedPort = CA_COAP;
        OIC_LOG_V(DEBUG, TAG, "send to remoteip: %s", endpoint->addr);
        OIC_LOG_V(DEBUG, TAG, "send to port: %d", parsedPort);
        IPAddress remoteIp(ip);
    	int ttl=1;
    	Udp.beginPacketMulticast(remoteIp,parsedPort,WiFi.localIP(),ttl);

    }
    else  //unicast
    {
        CAResult_t res = CAParseIPv4AddressInternal(endpoint->addr, ip, sizeof(ip),
                                                    &parsedPort);
        if (res != CA_STATUS_OK)
        {
            OIC_LOG_V(ERROR, TAG, "Remote adrs parse fail %d", res);
            return;
        }
    	IPAddress remoteIp(ip);
    	OIC_LOG_V(DEBUG, TAG, "remoteip: %s", endpoint->addr);
        OIC_LOG_V(DEBUG, TAG, "port: %d", endpoint->port);
        Udp.beginPacket(remoteIp, endpoint->port);
    }
    uint32_t bytesWritten = 0;
    while (bytesWritten < dataLength)
    {
        // get remaining bytes
        size_t writeCount = dataLength - bytesWritten;
        // write upto max ESP8266_WIFI_BUFFERSIZE bytes
        writeCount = Udp.write((uint8_t *)data + bytesWritten,
                                (writeCount > ESP8266_IP_BUFFERSIZE ?
                                 ESP8266_IP_BUFFERSIZE:writeCount));
        if(writeCount == 0)
        {
            // write failed
            OIC_LOG_V(ERROR, TAG, "Failed after %u", bytesWritten);
            break;
        }
        bytesWritten += writeCount;
    }

    if (Udp.endPacket() == 0)
    {
        OIC_LOG(ERROR, TAG, "Failed to send");
        return;
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}
