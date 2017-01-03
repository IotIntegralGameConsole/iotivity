//******************************************************************
//
// Copyright 2016 Web of Objects project All Rights Reserved.
// Author: Khaled Q. AbdelFadeel <kqorany2@gmail.com> and Khaled F. Elsayed <kelsayed@gmail.com>
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Do not remove the include below
#include "Arduino.h"

#include "logger.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "octypes.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ESP8266WIFI
// ESP8266 WiFi
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#endif

#define TAG "ESP8266Client"

#define MAX_BUF_LEN 100 //1024
#define MAX_OPT_LEN 16
#define PORT_LENGTH 5

#define DEFAULT_CONTEXT_VALUE 0x99

static const char *DEVICE_DISCOVERY_QUERY = "%s/oic/d";
static const char *PLATFORM_DISCOVERY_QUERY = "%s/oic/p";
static const char *RESOURCE_DISCOVERY_QUERY = "%s/oic/res";
static const char *LIGHT_RESOURCE_DISCOVERY_QUERY = "/a/light";

//The following variable determines the interface protocol (IPv4, IPv6, etc)
//to be used for sending unicast messages. Default set to IP dual stack.
static OCConnectivityType ConnType = CT_ADAPTER_IP;
static OCDevAddr serverAddr;
static char discoveryAddr[100];

// Tracking user input
static int TestCase = 0;
// The handle for the observe registration
OCDoHandle gObserveDoHandle;
// After this crosses a threshold client deregisters for further notifications
int gNumObserveNotifies = 0;

ICACHE_FLASH_ATTR static void PrintMenu();
ICACHE_FLASH_ATTR bool ParseData(char *buf, char *url, char *port, char *resourceUri);
ICACHE_FLASH_ATTR OCStackResult InvokeOCDoResource(const char *query,
                                 OCDevAddr *remoteAddr,
                                 OCMethod method,
                                 OCQualityOfService qos,
                                 OCClientResponseHandler cb,
                                 OCHeaderOption * options,
                                 uint8_t numOptions);
ICACHE_FLASH_ATTR int InitObserveRequest(OCQualityOfService qos);
ICACHE_FLASH_ATTR int InitPlatformDiscovery(OCQualityOfService qos);
ICACHE_FLASH_ATTR int InitDeviceDiscovery(OCQualityOfService qos);
ICACHE_FLASH_ATTR int ResourceDiscovery(char type);
ICACHE_FLASH_ATTR void GetData(char *readInput, size_t bufferLength, size_t *dataLength)
{
    if (!readInput || bufferLength == 0 || !dataLength)
    {
        Serial.println("Invalid buffer");
        return;
    }

    while (!Serial.available())
    {
        delay(500);
    }
    int len = 0;
    while (Serial.available())
    {
        delay(100);
        char c = Serial.read();
        if ('\n' != c && '\r' != c && len < bufferLength - 1)
        {
            readInput[len++] = c;
        }
        else
        {
            break;
        }
    }

    readInput[len] = '\0';
    Serial.flush();
    Serial.print("PD: ");
    Serial.println(readInput);
    (*dataLength) = len;
}

#ifdef ESP8266WIFI
// ESP8266 WiFi
static const char ESP8266_WIFI_UDP_FW_VER[] = "1.0";

/// WiFi network info and credentials
char ssid[] = "Home";
char pass[] = "12345home12345";

ICACHE_FLASH_ATTR int ConnectToNetwork()
{
    WiFi.begin(ssid, pass);
    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED)
    {
    	OIC_LOG(INFO, TAG, "Attempting to connect to SSID");
    	printf("%s\n",ssid);
    	OIC_LOG_V(INFO, TAG, "Attempting to connect to SSID: %s", ssid);
        // wait 10 seconds for connection:
        delay(500);
    }
    OIC_LOG(DEBUG, TAG, ("Connected to wifi"));

    IPAddress ip = WiFi.localIP();
    OIC_LOG(INFO, TAG, "IP Address:");
    printf("%d.%d.%d.%d \n", ip[0], ip[1], ip[2], ip[3]);
    OIC_LOG_V(INFO, TAG, "IP Address:  %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return 0;
}
#endif //ESP8266WIFI

ICACHE_FLASH_ATTR const char *getResult(OCStackResult result)
{
    switch (result)
    {
        case OC_STACK_OK:
            return "OC_STACK_OK";
        case OC_STACK_RESOURCE_CREATED:
            return "OC_STACK_RESOURCE_CREATED";
        case OC_STACK_RESOURCE_DELETED:
            return "OC_STACK_RESOURCE_DELETED";
        case OC_STACK_INVALID_URI:
            return "OC_STACK_INVALID_URI";
        case OC_STACK_INVALID_QUERY:
            return "OC_STACK_INVALID_QUERY";
        case OC_STACK_INVALID_IP:
            return "OC_STACK_INVALID_IP";
        case OC_STACK_INVALID_PORT:
            return "OC_STACK_INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
            return "OC_STACK_INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
            return "OC_STACK_INVALID_METHOD";
        case OC_STACK_NO_MEMORY:
            return "OC_STACK_NO_MEMORY";
        case OC_STACK_COMM_ERROR:
            return "OC_STACK_COMM_ERROR";
        case OC_STACK_INVALID_PARAM:
            return "OC_STACK_INVALID_PARAM";
        case OC_STACK_NOTIMPL:
            return "OC_STACK_NOTIMPL";
        case OC_STACK_NO_RESOURCE:
            return "OC_STACK_NO_RESOURCE";
        case OC_STACK_RESOURCE_ERROR:
            return "OC_STACK_RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
            return "OC_STACK_SLOW_RESOURCE";
        case OC_STACK_NO_OBSERVERS:
            return "OC_STACK_NO_OBSERVERS";
        case OC_STACK_UNAUTHORIZED_REQ:
            return "OC_STACK_UNAUTHORIZED_REQ";
#ifdef WITH_PRESENCE
        case OC_STACK_PRESENCE_STOPPED:
            return "OC_STACK_PRESENCE_STOPPED";
        case OC_STACK_PRESENCE_TIMEOUT:
            return "OC_STACK_PRESENCE_TIMEOUT";
#endif
        case OC_STACK_ERROR:
            return "OC_STACK_ERROR";
        default:
            return "UNKNOWN";
    }
}
ICACHE_FLASH_ATTR char * getConnectivityType (OCConnectivityType connType)
{
    switch (connType & CT_MASK_ADAPTER)
    {
        case CT_ADAPTER_IP:
            return "IP";

        case CT_IP_USE_V4:
            return "IPv4";

        case CT_IP_USE_V6:
            return "IPv6";

        case CT_ADAPTER_GATT_BTLE:
            return "GATT";

        case CT_ADAPTER_RFCOMM_BTEDR:
            return "RFCOMM";

        default:
            return "Incorrect connectivity";
    }
}
ICACHE_FLASH_ATTR OCPayload* putPayload()
{
    OCRepPayload* payload = OCRepPayloadCreate();

    if (!payload)
    {
    	OIC_LOG(ERROR, TAG, ("Failed to create put payload object"));
        abort();
    }

    OCRepPayloadSetPropInt(payload, "power", 15);
    OCRepPayloadSetPropBool(payload, "state", true);

    return (OCPayload*) payload;
}
// This is a function called back when a device is discovered
ICACHE_FLASH_ATTR OCStackApplicationResult discoveryReqCB(void* ctx, OCDoHandle /*handle*/,
                                        OCClientResponse * clientResponse)
{
    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        OIC_LOG(INFO, TAG, "Callback Context for DISCOVER query recvd successfully");
    }

    if (clientResponse)
    {
        OIC_LOG(INFO, TAG, "StackResult: ");
        printf("%s\n", getResult(clientResponse->result));
    	OIC_LOG_V(INFO, TAG, "StackResult: %s", getResult(clientResponse->result));

        char* connectionType = getConnectivityType (clientResponse->connType);
        OIC_LOG(INFO, TAG, "Discovered on ");
        printf("%s\n", connectionType);
        OIC_LOG_V(INFO, TAG, "Discovered on %s", connectionType);
        OIC_LOG(INFO, TAG,"Device =============> Discovered @ ");
        printf("%s: %d\n",clientResponse->devAddr.addr,clientResponse->devAddr.port);
        OIC_LOG_V(INFO, TAG,
                "Device =============> Discovered @ %s:%d",
                clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        OIC_LOG(INFO, TAG, "Payload: ");
        printf("%s\n", clientResponse->payload);
        OIC_LOG_V(INFO, TAG, "Payload: %s ", clientResponse->payload);

        ConnType = clientResponse->connType;
        serverAddr = clientResponse->devAddr;
        //parseClientResponse(clientResponse);
        switch(TestCase)
                {
        			case 0:	 // nothing after discovery
        				break;
        			case 1:  // observe connection @ /a/light
        				InitObserveRequest(OC_LOW_QOS);
                        break;
                    default:
                    	OIC_LOG(ERROR, TAG, "Undefined after discovery");
                    	break;
                }
    }
    else
    {
        OIC_LOG(INFO, TAG, "discoveryReqCB received Null clientResponse");
    }
    return OC_STACK_KEEP_TRANSACTION;
}
ICACHE_FLASH_ATTR OCStackApplicationResult PlatformDiscoveryReqCB(void* ctx,
                                                OCDoHandle /*handle*/,
                                                OCClientResponse * clientResponse)
{
    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        OIC_LOG(INFO, TAG, "Callback Context for Platform DISCOVER query recvd successfully");
    }

    if (clientResponse)
    {
        OIC_LOG(INFO, TAG, ("Discovery Response:"));
        printf("%s\n",clientResponse->payload);
        OIC_LOG_V(INFO, TAG, "%s\n", clientResponse->payload);
    }
    else
    {
        OIC_LOG(INFO, TAG, "PlatformDiscoveryReqCB received Null clientResponse");
    }

    return OC_STACK_KEEP_TRANSACTION;
}
ICACHE_FLASH_ATTR OCStackApplicationResult DeviceDiscoveryReqCB(void* ctx, OCDoHandle /*handle*/,
                                              OCClientResponse * clientResponse)
{
    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        OIC_LOG(INFO, TAG, "Callback Context for Device DISCOVER query recvd successfully");
    }

    if (clientResponse)
    {
        OIC_LOG(INFO, TAG, ("Discovery Response:"));
        printf("%s\n",clientResponse->payload);
        OIC_LOG_V(INFO, TAG, "%s\n", clientResponse->payload);
    }
    else
    {
        OIC_LOG(INFO, TAG, "PlatformDiscoveryReqCB received Null clientResponse");
    }

    return OC_STACK_KEEP_TRANSACTION;
}
//The setup function is called once at startup of the sketch
ICACHE_FLASH_ATTR void setup()
{
    // Add your initialization code here
    // Note : This will initialize Serial port on Arduino at 115200 bauds
    OIC_LOG_INIT();
    OIC_LOG(DEBUG, TAG, ("OCServer is starting..."));

    // Connect to WiFi network
    if (ConnectToNetwork() != 0)
    {
        OIC_LOG(ERROR, TAG, ("Unable to connect to network"));
        return;
    }

    // Initialize the OC Stack in Server mode
    if (OCInit(NULL, 0, OC_CLIENT) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, ("OCStack init error"));
        return;
    }

}

// The loop function is called in an endless loop
ICACHE_FLASH_ATTR void loop()
{
    char buffer[5] = {0};
    size_t len_loop;
    if (Serial.available() > 0)
    {
        GetData(buffer, sizeof(buffer), &len_loop);
        if (0 >= len_loop)
        {
        	OIC_LOG(ERROR, TAG, "i/p err");
            return;
        }
        switch (toupper(buffer[0]))
        {
            case 'S': // show the menu
            case 's':
                PrintMenu();
                break;
            case 'U': // Unicast resource Discovery query /oic/res
            case 'u':
            	TestCase=0; // nothing after discovery
            	ResourceDiscovery('u');
            	break;
            case 'P': // Multicast platform discovery query /oic/p
            case 'p':
            	InitPlatformDiscovery(OC_LOW_QOS);
            	break;
            case 'D': // Multicast device discovery query /oic/d
            case 'd':
            	InitDeviceDiscovery(OC_LOW_QOS);
            	break;
            case 'L': // Multicast /a/light discovery query + observe connection
            case 'l':
            	TestCase=1;  // observe connection after discovery
            	ResourceDiscovery('l');
                break;
            case 'M': // Multicast resource discovery query  /oic/res
            case 'm':
            	TestCase=0; // nothing after discovery
            	ResourceDiscovery('m');
                break;
            default:
            	OIC_LOG(DEBUG, TAG, "!! Bad input !!");
                break;
        }
    }
    if (OCProcess() != OC_STACK_OK)
    {
         OIC_LOG(ERROR, TAG, "OCStack process error");
         return;
    }
}

ICACHE_FLASH_ATTR void PrintMenu()
{
    Serial.println("============");
    Serial.println("s: Show the menu");
    Serial.println("u: (unitcsat)  Resource Discovery query  /oic/res");
    Serial.println("m: (multicast) Resource Discovery query  /oic/res");
    Serial.println("p: (multicast) Platform Discovery query  /oic/p");
    Serial.println("d: (multicast) Device Discovery query    /oic/d");
    Serial.println("l: (multicsat) /a/light Discovery + observe connection");
    Serial.println("============");
}

ICACHE_FLASH_ATTR void StripNewLineChar(char* str)
{
    int i = 0;
    if (str)
    {
        while( str[i])
        {
            if (str[i] == '\n')
            {
                str[i] = '\0';
            }
            i++;
        }
    }
}

ICACHE_FLASH_ATTR OCStackApplicationResult obsReqCB(void* ctx, OCDoHandle /*handle*/,
                                  OCClientResponse * clientResponse)
{
    if (ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OIC_LOG(INFO, TAG, "Callback Context for OBS query recvd successfully");
    }

    if (clientResponse)
    {
        OIC_LOG(INFO, TAG, "StackResult: ");
        printf("%s\n",getResult(clientResponse->result));
    	OIC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
    	OIC_LOG(INFO, TAG, "SEQUENCE NUMBER: ");
    	printf("%d\n",clientResponse->sequenceNumber);
        OIC_LOG_V(INFO, TAG, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
        OIC_LOG(INFO, TAG, "Callback Context for OBSERVE notification recvd successfully");
        printf("%d\n",gNumObserveNotifies);
        OIC_LOG_V(INFO, TAG, "Callback Context for OBSERVE notification recvd successfully %d",gNumObserveNotifies);
        printf("%s\n",clientResponse->payload);
        OIC_LOG_V(INFO, TAG, "%s\n" clientResponse->payload);
        gNumObserveNotifies++;
        /*OIC_LOG_V(INFO, TAG, ("=============> Obs Response"));
        if (gNumObserveNotifies == 15) //large number to test observing in DELETE case.
        {
                if (OCCancel (gObserveDoHandle, OC_LOW_QOS, NULL, 0) != OC_STACK_OK)
                {
                    OIC_LOG(ERROR, TAG, "Observe cancel error");
                }
                return OC_STACK_DELETE_TRANSACTION;
        }*/
        if (clientResponse->sequenceNumber == OC_OBSERVE_REGISTER)
        {
            OIC_LOG(INFO, TAG, "This also serves as a registration confirmation");
        }
        else if (clientResponse->sequenceNumber == OC_OBSERVE_DEREGISTER)
        {
            OIC_LOG(INFO, TAG, "This also serves as a deregistration confirmation");
            return OC_STACK_DELETE_TRANSACTION;
        }
        else if (clientResponse->sequenceNumber == OC_OBSERVE_NO_OPTION)
        {
            OIC_LOG(INFO, TAG, "This also tells you that registration/deregistration failed");
            return OC_STACK_DELETE_TRANSACTION;
        }
    }
    else
    {
        OIC_LOG(INFO, TAG, "obsReqCB received Null clientResponse");
    }
    return OC_STACK_KEEP_TRANSACTION;
}

ICACHE_FLASH_ATTR OCStackResult InvokeOCDoResource(const char *query,
                                 OCDevAddr *remoteAddr,
                                 OCMethod method,
                                 OCQualityOfService qos,
                                 OCClientResponseHandler cb,
                                 OCHeaderOption * options,
                                 uint8_t numOptions)
{
    OCStackResult ret;
    OCCallbackData cbData;
    OCDoHandle handle;

    cbData.cb = cb;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(&handle, method, query, remoteAddr,
                       (method == OC_REST_PUT) ? putPayload() : NULL,
                       (ConnType), qos, &cbData, options, numOptions);

    if (ret != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCDoResource returns error %d with method ");
        printf("%d\n", ret, method);
    	OIC_LOG_V(ERROR, TAG, "OCDoResource returns error %d with method %d", ret, method);
    }
    else if (method == OC_REST_OBSERVE || method == OC_REST_OBSERVE_ALL)
    {
        gObserveDoHandle = handle;
    }
    return ret;
}

ICACHE_FLASH_ATTR int InitObserveRequest(OCQualityOfService qos)
{
    OIC_LOG(INFO, TAG, "\n\n Executing ");
    printf("%s\n",__func__);
	OIC_LOG_V(INFO, TAG, "\n\n Executing %s", __func__);
    char szQueryUri[100] = { 0 };
    return (InvokeOCDoResource(LIGHT_RESOURCE_DISCOVERY_QUERY, &serverAddr, OC_REST_OBSERVE,
              (qos == OC_HIGH_QOS)? OC_HIGH_QOS:OC_LOW_QOS, obsReqCB, NULL, 0));
}

ICACHE_FLASH_ATTR int InitPlatformDiscovery(OCQualityOfService qos)
{
	discoveryAddr[0] = '\0';
    OIC_LOG(INFO, TAG, "\n\n Executing ");
    printf("%s\n",__func__);
	OIC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);

    OCStackResult ret;
    OCCallbackData cbData;
    char szQueryUri[100] = { 0 };

    snprintf(szQueryUri, sizeof (szQueryUri) - 1, PLATFORM_DISCOVERY_QUERY, discoveryAddr);

    cbData.cb = PlatformDiscoveryReqCB;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(NULL, OC_REST_DISCOVER, szQueryUri, NULL, 0, CT_DEFAULT,
                       (qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS,
                       &cbData, NULL, 0);
    if (ret != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack device error");
    }

    return ret;
}

ICACHE_FLASH_ATTR int InitDeviceDiscovery(OCQualityOfService qos)
{
	discoveryAddr[0] = '\0';
    OIC_LOG(INFO, TAG, "\n\n Executing ");
    printf("%s\n",__func__);
	OIC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);

    OCStackResult ret;
    OCCallbackData cbData;
    char szQueryUri[100] = { 0 };

    snprintf_P(szQueryUri, sizeof (szQueryUri) - 1, DEVICE_DISCOVERY_QUERY, discoveryAddr);

    OIC_LOG(INFO, TAG, "=====> szQueryUri:");
    printf("%s\n",szQueryUri);
    OIC_LOG_V(INFO, TAG, "=====> szQueryUri: %s",szQueryUri);

    cbData.cb = DeviceDiscoveryReqCB;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(NULL, OC_REST_DISCOVER, szQueryUri, NULL, 0, CT_DEFAULT,
                       (qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS,
                       &cbData, NULL, 0);
    if (ret != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack device error");
    }

    return ret;
}

ICACHE_FLASH_ATTR int ResourceDiscovery(char type)
{
	discoveryAddr[0] = '\0';
	char szQueryUri[100] = { 0 };
	switch (type){
		case 'u':
		{
			OIC_LOG(INFO, TAG, "Unicast Resource Discovery");
			OIC_LOG(INFO, TAG, "Enter IP address of server with optional port");
			OIC_LOG(INFO, TAG, "IPv4: 192.168.0.15:5683\n");
			size_t len_rd = 0;
			GetData(discoveryAddr, sizeof(discoveryAddr), &len_rd);
			if (0 >= len_rd)
		    {
			    OIC_LOG(ERROR, TAG, "!! Bad input !!");
			    return 0;
			}
			else{
			    StripNewLineChar(discoveryAddr);
			}
			break;
		}
		case 'm':
		case 'l':
			OIC_LOG(INFO, TAG, "Multicsat Resource Discovery");
			break;
		default:
			OIC_LOG(ERROR, TAG, "Undefined communications type not unicast nor multicast");
			return 0;
	}
    /////
    OCStackResult ret;
    OCCallbackData cbData;

    snprintf_P(szQueryUri, sizeof (szQueryUri) - 1, RESOURCE_DISCOVERY_QUERY, discoveryAddr);

    OIC_LOG(INFO, TAG, "=====> szQueryUri:");
    printf("%s\n",szQueryUri);
    OIC_LOG_V(INFO, TAG, "=====> szQueryUri: %s",szQueryUri);

    cbData.cb = discoveryReqCB;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(NULL, OC_REST_DISCOVER, szQueryUri, NULL, 0, CT_DEFAULT,
                       (OC_LOW_QOS == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS,
                       &cbData, NULL, 0);
    if (ret != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }
    return ret;
}
