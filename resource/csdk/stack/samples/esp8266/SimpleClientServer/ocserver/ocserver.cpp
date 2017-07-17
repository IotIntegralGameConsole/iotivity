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
#include <string.h>

#ifdef ESP8266WIFI
// ESP8266 WiFi
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#endif


const char *getResult(OCStackResult result);

#define TAG "ESP8266Server"

int gLightUnderObservation = 0;
void createLightResource();

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

// Structure to represent a Light resource
typedef struct LIGHTRESOURCE{
    OCResourceHandle handle;
    bool state;
    int power;
} LightResource;

static LightResource Light;

#ifdef ESP8266WIFI
// ESP8266 WiFi

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
        delay(1000);
    }
    OIC_LOG(DEBUG, TAG, ("Connected to wifi"));

    IPAddress ip = WiFi.localIP();
    OIC_LOG(INFO, TAG, "IP Address:");
    printf("%d.%d.%d.%d \n", ip[0], ip[1], ip[2], ip[3]);
    OIC_LOG_V(INFO, TAG, "IP Address:  %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return 0;
}
#endif //ESP8266WIFI

/*
// On Arduino Atmel boards with Harvard memory architecture, the stack grows
// downwards from the top and the heap grows upwards. This method will print
// the distance(in terms of bytes) between those two.
// See here for more details :
// http://www.atmel.com/webdoc/AVRLibcReferenceManual/malloc_1malloc_intro.html
void PrintArduinoMemoryStats()
{
    #ifdef ARDUINO_AVR_MEGA2560
    //This var is declared in avr-libc/stdlib/malloc.c
    //It keeps the largest address not allocated for heap
    extern char *__brkval;
    //address of tmp gives us the current stack boundry
    int tmp;
    OIC_LOG_V(INFO, TAG, "Stack: %u         Heap: %u", (unsigned int)&tmp, (unsigned int)__brkval);
    OIC_LOG_V(INFO, TAG, "Unallocated Memory between heap and stack: %u",
            ((unsigned int)&tmp - (unsigned int)__brkval));
    #endif
}
*/

// This is the entity handler for the registered resource.
// This is invoked by OCStack whenever it recevies a request for this resource.
OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest,
                                        void *callbackParam)
{
    OCEntityHandlerResult ehRet = OC_EH_OK;
    OCEntityHandlerResponse response = {0};
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, ("Failed to allocate Payload"));
        return OC_EH_ERROR;
    }

    if(entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        OIC_LOG(INFO, TAG, ("Flag includes OC_REQUEST_FLAG"));

        if(OC_REST_GET == entityHandlerRequest->method)
        {
            OCRepPayloadSetUri(payload, "/a/light");
            OCRepPayloadSetPropBool(payload, "state", true);
            OCRepPayloadSetPropInt(payload, "power", 10);
        }
        else if(OC_REST_PUT == entityHandlerRequest->method)
        {
            //Do something with the 'put' payload
            OCRepPayloadSetUri(payload, "/a/light");
            OCRepPayloadSetPropBool(payload, "state", false);
            OCRepPayloadSetPropInt(payload, "power", 0);
        }

        if (ehRet == OC_EH_OK)
        {
            // Format the response.  Note this requires some info about the request
            response.requestHandle = entityHandlerRequest->requestHandle;
            response.resourceHandle = entityHandlerRequest->resource;
            response.ehResult = ehRet;
            response.payload = (OCPayload*) payload;
            response.numSendVendorSpecificHeaderOptions = 0;
            memset(response.sendVendorSpecificHeaderOptions, 0,
                    sizeof response.sendVendorSpecificHeaderOptions);
            memset(response.resourceUri, 0, sizeof response.resourceUri);
            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            // Send the response
            if (OCDoResponse(&response) != OC_STACK_OK)
            {
                OIC_LOG(ERROR, TAG, "Error sending response");
                ehRet = OC_EH_ERROR;
            }
        }
    }
    if (entityHandlerRequest && (flag & OC_OBSERVE_FLAG))
    {
        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            OIC_LOG(INFO, TAG, ("Received OC_OBSERVE_REGISTER from client"));
            gLightUnderObservation = 1;
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
        {
            OIC_LOG(INFO, TAG, ("Received OC_OBSERVE_DEREGISTER from client"));
            gLightUnderObservation = 0;
        }
    }
    OCRepPayloadDestroy(payload);
    return ehRet;
}

// This method is used to display 'Observe' functionality of OC Stack.
static uint8_t modCounter = 0;
void *ChangeLightRepresentation (void *param)
{
    (void)param;
    OCStackResult result = OC_STACK_ERROR;
    modCounter += 1;
    // Matching the timing that the Linux Sample Server App uses for the same functionality.
    if(modCounter % 50 == 0)
    {
        Light.power += 5;
        OIC_LOG(INFO, TAG, "=====> Status of Light.power is changed to");
        printf("%d\n", Light.power);
        OIC_LOG_V(INFO, TAG, "=====> Status of Light.power is changed to  %d\n", Light.power);
        if (gLightUnderObservation)
        {
            OIC_LOG(INFO, TAG, " =====> Notifying stack of new power level");
            printf("%d\n", Light.power);
        	OIC_LOG_V(INFO, TAG, " =====> Notifying stack of new power level %d\n", Light.power);
            result = OCNotifyAllObservers (Light.handle, OC_NA_QOS);
            if (OC_STACK_NO_OBSERVERS == result)
            {
                gLightUnderObservation = 0;
            }
        }
    }
    return NULL;
}

//The setup function is called once at startup of the sketch
void setup()
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
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, ("OCStack init error"));
        return;
    }

    // Declare and create the example resource: Light
    createLightResource();
}

// The loop function is called in an endless loop
void loop()
{

    // This call displays the amount of free SRAM available on Arduino
    //PrintArduinoMemoryStats();
char buffer[5] = {0};
size_t len;
if (Serial.available() > 0)
{
    GetData(buffer, sizeof(buffer), &len);
    if (0 >= len)
    {
      OIC_LOG(ERROR, TAG, "i/p err");
      return;
    }
    switch (toupper(buffer[0]))
    {
      case 'H': // Hold
      case 'h':
          while(Serial.available()<=0){
          }
          GetData(buffer, sizeof(buffer), &len);
          if (0 >= len)
          {
            OIC_LOG(ERROR, TAG, "i/p err");
            return;
          }
          switch (toupper(buffer[0]))
          {
              case 'R': //Resume
              case 'r':
                  break;
          }
          break;
      default:
          OIC_LOG(DEBUG, TAG, "!! Bad input !!");
          break;
    }
}
// Give CPU cycles to OCStack to perform send/recv and other OCStack stuff
if (OCProcess() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, ("OCStack process error"));
        return;
    }
    ChangeLightRepresentation(NULL);
}

void createLightResource()
{
    Light.state = false;
    OCStackResult res = OCCreateResource(&Light.handle,
            "core.light",
            OC_RSRVD_INTERFACE_DEFAULT,
            "/a/light",
            OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE|OC_OBSERVABLE);
    OIC_LOG(INFO, TAG, "Created Light resource with result");
    printf("%s\n",getResult(res));
    OIC_LOG_V(INFO, TAG, "Created Light resource with result: %s", getResult(res));
}

const char *getResult(OCStackResult result) {
    switch (result) {
    case OC_STACK_OK:
        return "OC_STACK_OK";
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
    case OC_STACK_ERROR:
        return "OC_STACK_ERROR";
    default:
        return "UNKNOWN";
    }
}
