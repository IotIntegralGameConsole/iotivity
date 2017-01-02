/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

 #include "ocendpoint.h"
 #include "logger.h"
 #include "oic_malloc.h"
 #include "oic_string.h"
 #include <string.h>
 #include "cainterface.h"

 #define VERIFY_NON_NULL(arg) { if (!arg) {OIC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }
 #define VERIFY_GT_ZERO(arg) { if (arg < 1) {OIC_LOG(FATAL, TAG, #arg " < 1"); goto exit;} }
 #define TAG  "OIC_RI_ENDPOINT"

OCStackResult OCGetSupportedEndpointFlags(const OCTpsSchemeFlags givenFlags, OCTpsSchemeFlags* out)
{
    if (!out)
    {
        return OC_STACK_INVALID_PARAM;
    }

    CATransportAdapter_t SelectedNetwork = CAGetSelectedNetwork();
    if ((givenFlags & (OC_COAP | OC_COAPS)) && (SelectedNetwork & CA_ADAPTER_IP))
    {
        *out = (OCTpsSchemeFlags)(*out | OC_COAP);

        if (OC_SECURE)
        {
            *out = (OCTpsSchemeFlags)(*out | OC_COAPS);
        }
    }
#ifdef TCP_ADAPTER
    if ((givenFlags & (OC_COAP_TCP | OC_COAPS_TCP)) && (SelectedNetwork & CA_ADAPTER_TCP))
    {

        *out = (OCTpsSchemeFlags)(*out | OC_COAP_TCP);

        if (OC_SECURE)
        {
            *out = (OCTpsSchemeFlags)(*out | OC_COAPS_TCP);
        }
    }
#endif
#ifdef EDR_ADAPTER
    if ((givenFlags & OC_COAP_RFCOMM) && (SelectedNetwork & CA_ADAPTER_RFCOMM_BTEDR))
    {
        *out = (OCTpsSchemeFlags)(*out | OC_COAP_RFCOMM);
    }
#endif

    return OC_STACK_OK;
}

OCStackResult OCGetMatchedTpsFlags(const CATransportAdapter_t adapter,
                                   const CATransportFlags_t flags,
                                   OCTpsSchemeFlags* out)
{
    // return matched TPS Flags with given CA flags.
    if (!out)
    {
        OIC_LOG(ERROR, TAG, "out param is NULL!!!");
        return OC_STACK_INVALID_PARAM;
    }

    if ((adapter & OC_ADAPTER_IP) && (flags & (OC_IP_USE_V4 | OC_IP_USE_V6)))
    {
        if (flags & OC_FLAG_SECURE)
        {
            // OC_COAPS
            // typecasting to support C90(arduino)
            *out = (OCTpsSchemeFlags)(*out | OC_COAPS);
        }
        else
        {   // OC_COAP
            // typecasting to support C90(arduino)
            *out = (OCTpsSchemeFlags)(*out | OC_COAP);
        }
    }

#ifdef TCP_ADAPTER
    if ((adapter & OC_ADAPTER_TCP) && (flags & (OC_IP_USE_V4 | OC_IP_USE_V6)))
    {
        if (flags & OC_FLAG_SECURE)
        {
            // OC_COAPS_TCP
            // typecasting to support C90(arduino)
            *out = (OCTpsSchemeFlags)(*out | OC_COAPS_TCP);
        }
        else
        {
            // OC_COAP_TCP
            // typecasting to support C90(arduino)
            *out = (OCTpsSchemeFlags)(*out | OC_COAP_TCP);
        }
    }
#endif
#ifdef HTTP_ADAPTER
    // OC_HTTP
    // @todo iotivity not support http transport yet...

    // OC_HTTPS
    // @todo iotivity not support https transport yet...
#endif
#ifdef EDR_ADAPTER
    // OC_COAP_RFCOMM
    if ((adapter & OC_ADAPTER_RFCOMM_BTEDR) && (flags == OC_DEFAULT_FLAGS))
    {
        // typecasting to support C90(arduino)
        *out = (OCTpsSchemeFlags)(*out | OC_COAP_RFCOMM);
    }
#endif

    return OC_STACK_OK;
}


OCStackResult OCConvertTpsToString(const OCTpsSchemeFlags tps, char** out)
{
    // return given tps as string for payload
    // OC_COAP_IPV4 -> OC_COAP

    if (!out)
    {
        return OC_STACK_INVALID_PARAM;
    }

    switch (tps)
    {
        case OC_COAP:
            *out = OICStrdup(COAP_STR);
            break;

        case OC_COAPS:
            *out = OICStrdup(COAPS_STR);
            break;
#ifdef TCP_ADAPTER
        case OC_COAP_TCP:
            *out = OICStrdup(COAP_TCP_STR);
            break;

        case OC_COAPS_TCP:
            *out = OICStrdup(COAPS_TCP_STR);
            break;
#endif
#ifdef HTTP_ADAPTER
        case OC_HTTP:
            *out = OICStrdup(HTTP_STR);
            break;

        case OC_HTTPS:
            *out = OICStrdup(HTTPS_STR);
            break;
#endif
#ifdef EDR_ADAPTER
        case OC_COAP_RFCOMM:
            *out = OICStrdup(COAP_RFCOMM_STR);
            break;
#endif
        default:
            return OC_STACK_INVALID_PARAM;
    }
    VERIFY_NON_NULL(*out);
    return OC_STACK_OK;

exit:
    return OC_STACK_NO_MEMORY;
}

char* OCCreateEndpointString(const OCEndpointPayload* endpoint)
{
    if (!endpoint)
    {
        return NULL;
    }

    char* buf = (char*)OICCalloc(MAX_ADDR_STR_SIZE, sizeof(char));
    VERIFY_NON_NULL(buf);

    if ((strcmp(endpoint->tps, COAP_STR) == 0) || (strcmp(endpoint->tps, COAPS_STR) == 0)
#ifdef TCP_ADAPTER
        || (strcmp(endpoint->tps, COAP_TCP_STR) == 0) ||(strcmp(endpoint->tps, COAPS_TCP_STR) == 0)
#endif
#ifdef HTTP_ADAPTER
        || (strcmp(endpoint->tps, HTTP_STR) == 0) ||(strcmp(endpoint->tps, HTTPS_STR) == 0)
#endif
        )
    {
        // checking addr is ipv4 or not
        if (endpoint->family == OC_IP_USE_V4)
        {
            // ipv4
            sprintf(buf, "%s://%s:%d", endpoint->tps, endpoint->addr, endpoint->port);
        }
        else
        {
            // ipv6
            sprintf(buf, "%s://[%s]:%d", endpoint->tps, endpoint->addr, endpoint->port);
        }
    }
#ifdef EDR_ADAPTER
    else if ((strcmp(endpoint->tps, COAP_RFCOMM_STR) == 0))
    {
        // coap+rfcomm
        sprintf(buf, "%s://%s", endpoint->tps, endpoint->addr);
    }
#endif
    else
    {
        OIC_LOG_V(ERROR, TAG, "Payload has invalid TPS!!! %s", endpoint->tps);
        return NULL;
    }
    return buf;

exit:
    return NULL;
}

OCStackResult OCParseEndpointString(const char* endpointStr, OCEndpointPayload* out)
{
    if (!endpointStr || !out)
    {
        return OC_STACK_INVALID_PARAM;
    }

    char* tmp = NULL;
    char* ret = NULL;
    char* tps = NULL;
    char* addr = NULL;
    char* origin = NULL;
    char* tokPos = NULL;
    OCStackResult isEnabledAdapter = OC_STACK_ADAPTER_NOT_ENABLED;
    OCTransportAdapter parsedAdapter = OC_DEFAULT_ADAPTER;

    tps = (char*)OICCalloc(OC_MAX_TPS_STR_SIZE, sizeof(char));
    VERIFY_NON_NULL(tps);

    addr = (char*)OICCalloc(OC_MAX_ADDR_STR_SIZE, sizeof(char));
    VERIFY_NON_NULL(addr);

    origin = OICStrdup(endpointStr);
    VERIFY_NON_NULL(origin);

    // token start pos
    tokPos = strstr(origin, OC_ENDPOINT_TPS_TOKEN);
    VERIFY_NON_NULL(tokPos);

    // copy tps
    VERIFY_GT_ZERO(tokPos - origin);
    ret = strncpy(tps, origin, tokPos - origin);
    VERIFY_NON_NULL(ret);
    OIC_LOG_V(INFO, TAG, "parsed tps is:%s", tps);

    // check tps type
    if (strcmp(tps, COAP_STR) == 0)
    {
        isEnabledAdapter = OC_STACK_OK;
        parsedAdapter = OC_ADAPTER_IP;
    }
    else if (strcmp(tps, COAPS_STR) == 0)
    {
        isEnabledAdapter = OC_STACK_OK;
        parsedAdapter = OC_ADAPTER_IP;
    }
#ifdef TCP_ADAPTER
    else if (strcmp(tps, COAP_TCP_STR) == 0)
    {
        isEnabledAdapter = OC_STACK_OK;
        parsedAdapter = OC_ADAPTER_TCP;
    }
    else if (strcmp(tps, COAPS_TCP_STR) == 0)
    {
        isEnabledAdapter = OC_STACK_OK;
        parsedAdapter = OC_ADAPTER_TCP;
    }
#endif
#ifdef HTTP_ADAPTER
    // @todo iotivity not support http, https transport yet...
#endif
#ifdef EDR_ADAPTER
    else if (strcmp(tps, COAP_RFCOMM_STR) == 0)
    {
        isEnabledAdapter = OC_STACK_OK;
        parsedAdapter = OC_ADAPTER_RFCOMM_BTEDR;
    }
#endif
    // ignore unrecognized tps type
    if (isEnabledAdapter == OC_STACK_ADAPTER_NOT_ENABLED
        && parsedAdapter == OC_DEFAULT_ADAPTER)
    {
        OICFree(tps);
        OICFree(addr);
        OICFree(origin);
        return OC_STACK_ADAPTER_NOT_ENABLED;
    }

    if (parsedAdapter == OC_ADAPTER_RFCOMM_BTEDR)
    {
        // copy addr
        tokPos = tokPos + 3;
        ret = strcpy(addr, tokPos);
        VERIFY_NON_NULL(ret);
        out->tps = tps;
        out->addr = addr;
        out->family = OC_DEFAULT_FLAGS;
        out->port = 0;
    }
    else
    {
        // first check epl has square bracket
        tmp = strchr(origin, OC_ENDPOINT_BRACKET_START);
        if (tmp)
        {
            out->family = OC_IP_USE_V6;
            tokPos = tokPos + 4;
            tmp = strrchr(origin, OC_ENDPOINT_BRACKET_END);
            VERIFY_NON_NULL(tmp);
        }
        else
        {
            out->family = OC_IP_USE_V4;
            tokPos = tokPos + 3;
            tmp = strrchr(origin, OC_ENDPOINT_ADDR_TOKEN);
            VERIFY_NON_NULL(tmp);
        }

        // copy addr
        VERIFY_GT_ZERO(tmp - tokPos);
        ret = strncpy(addr, tokPos, tmp - tokPos);
        VERIFY_NON_NULL(ret);
        OIC_LOG_V(INFO, TAG, "parsed addr is:%s", addr);

        tmp = strrchr(origin, OC_ENDPOINT_ADDR_TOKEN);
        VERIFY_NON_NULL(tmp);

        // port start pos
        tokPos = tmp + 1;
        VERIFY_GT_ZERO(atoi(tokPos));
        OIC_LOG_V(INFO, TAG, "parsed port is:%s", tokPos);

        out->tps = tps;
        out->addr = addr;
        out->port = atoi(tokPos);
    }

    OICFree(origin);
    origin = NULL;
    tokPos = NULL;
    tmp = NULL;
    ret = NULL;

    return OC_STACK_OK;
exit:
    OICFree(origin);
    OICFree(tps);
    OICFree(addr);
    origin = NULL;
    tokPos = NULL;
    tmp = NULL;
    ret = NULL;
    tps = NULL;
    addr = NULL;

    return OC_STACK_ERROR;
}

OCTpsSchemeFlags OCGetSupportedTpsFlags()
{
    OCTpsSchemeFlags ret = (OCTpsSchemeFlags)0;
    CATransportAdapter_t SelectedNetwork = CAGetSelectedNetwork();

    if (SelectedNetwork & CA_ADAPTER_IP)
    {
        ret = (OCTpsSchemeFlags)(ret | OC_COAP);

        if (OC_SECURE)
        {
            ret = (OCTpsSchemeFlags)(ret | OC_COAPS);
        }
    }
#ifdef TCP_ADAPTER
    else if (SelectedNetwork & CA_ADAPTER_TCP)
    {
        ret = (OCTpsSchemeFlags)(ret | OC_COAP_TCP);

        if (OC_SECURE)
        {
            ret = (OCTpsSchemeFlags)(ret | OC_COAPS_TCP);
        }
    }
#endif
#ifdef EDR_ADAPTER
    else if (SelectedNetwork & CA_ADAPTER_RFCOMM_BTEDR)
    {
        ret = (OCTpsSchemeFlags)(ret | OC_COAP_RFCOMM);
    }
#endif

    return ret;
}