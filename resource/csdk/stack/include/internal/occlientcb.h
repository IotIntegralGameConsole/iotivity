/* ****************************************************************
 *
 * Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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
 *
 * This file contains the definition and types for client's callback mode and functions.
 */


#ifndef OC_CLIENT_CB_H_
#define OC_CLIENT_CB_H_

#include "ocstack.h"
#include "ocresource.h"
#include "cacommon.h"


#ifdef __cplusplus
extern "C"
{
#endif


#ifdef WITH_PRESENCE
/**
 * Data structure For presence Discovery.
 * This is the TTL associated with presence.
 */
typedef struct OCPresence
{
    /** Time to Live. */
    uint32_t TTL;

    /** Time out. */
    uint32_t * timeOut;

    /** TTL Level. */
    uint32_t TTLlevel;
} OCPresence;
#endif // WITH_PRESENCE

/**
 * Forward declaration of resource type.
 */
typedef struct resourcetype_t OCResourceType;

/**
 * Data structure for holding client's callback context, methods and Time to Live,
 * connectivity Types, presence and resource type, request uri etc.
 */
typedef struct ClientCB {
    /** callback method defined in application address space. */
    OCClientResponseHandler callBack;

    /** callback context data. */
    void * context;

    /** callback method to delete context data. */
    OCClientContextDeleter deleteCallback;

    /** Qos for the request */
    CAMessageType_t type;

    /**  when a response is recvd with this token, above callback will be invoked. */
    CAToken_t token;

    /** a response is recvd with this token length.*/
    uint8_t tokenLength;

    CAHeaderOption_t *options;

    uint8_t numOptions;

    CAPayload_t payload;

    size_t payloadSize;

    CAPayloadFormat_t payloadFormat;

    /** Invocation handle tied to original call to OCDoResource().*/
    OCDoHandle handle;

    /** This is used to determine if all responses should be consumed or not.
     * (For now, only pertains to OC_REST_OBSERVE_ALL vs. OC_REST_OBSERVE functionality).*/
    OCMethod method;

    /** This is the sequence identifier the server applies to the invocation tied to 'handle'.*/
    uint32_t sequenceNumber;

    /** The canonical form of the request uri associated with the call back.*/
    char * requestUri;

    /** Remote address complete.*/
    OCDevAddr * devAddr;

#ifdef WITH_PRESENCE
    /** Struct to hold TTL info for presence.*/
    OCPresence * presence;

    /** Struct to hold a resource type name for filtering a presence interesting.*/
    OCResourceType * interestingPresenceResourceType;
#endif

    /** The connectivity type on which the request was sent on.*/
    OCConnectivityType conType;

    /** The TTL for this callback. Holds the time till when this callback can
     * still be used. TTL is set to 0 when the callback is for presence and observe.
     * Presence has ttl mechanism in the "presence" member of this struct and observes
     * can be explicitly cancelled.*/
    uint32_t TTL;

    /** next node in this list.*/
    struct ClientCB    *next;
} ClientCB;

//TODO: Now ocstack is directly accessing the clientCB list to process presence.
//      It should be avoided after we make a presence feature separately.
/**
 * Linked list of ClientCB node.
 */
extern struct ClientCB *g_cbList;

/**
 * This method is used to add a client callback method in cbList.
 *
 * @param[out] clientCB            The resulting node from making this call. Null if out of memory.
 * @param[in]  cbData              Address to client callback function.
 * @param[in]  type                Qos type.
 * @param[in]  token               Identifier for OTA CoAP comms.
 * @param[in]  tokenLength         Length for OTA CoAP comms.
 * @param[in]  options             The address of an array containing the vendor specific header
 *                                 options to be sent with the request.
 * @param[in]  numOptions          Number of header options to be included.
 * @param[in]  payload             Request payload.
 * @param[in]  payloadSize         Size of payload.
 * @param[in]  payloadFormat       Format of payload.
 * @param[in]  handle              masked in the public API as an 'invocation handle'
 *                                 Used for callback management.
 * @param[in]  method              A method via which this client callback is expected to operate
 * @param[in]  devAddr             The Device address.
 * @param[in]  requestUri          The resource uri of the request.
 * @param[in]  resourceTypeName    The resource type associated with a presence request.
 * @param[in]  ttl                 time to live in coap_ticks for the callback.
 *
 * @note If the handle you're looking for does not exist, the stack will reply with a RST message.
 *
 * @return OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult AddClientCB(ClientCB** clientCB,
                          OCCallbackData* cbData,
                          CAMessageType_t type,
                          CAToken_t token,
                          uint8_t tokenLength,
                          CAHeaderOption_t *options,
                          uint8_t numOptions,
                          CAPayload_t payload,
                          size_t payloadSize,
                          CAPayloadFormat_t payloadFormat,
                          OCDoHandle *handle,
                          OCMethod method,
                          OCDevAddr *devAddr,
                          char *requestUri,
                          char *resourceTypeName,
                          uint32_t ttl);

/**
 * This method is used to remove a callback node from cbList.
 *
 * @param[in]  cbNode               Address to client callback node.
 */
void DeleteClientCB(ClientCB *cbNode);

/**
 * This method is used to clear the cbList.
 */
void DeleteClientCBList(void);

/**
 * This method is used to search and retrieve a cb node in cbList using token.
 *
 * @param[in]  token                Token to search for.
 * @param[in]  tokenLength          The Length of the token.
 *
 * @return address of the node if found, otherwise NULL
 */
ClientCB* GetClientCBUsingToken(const CAToken_t token,
                                const uint8_t tokenLength);

/**
 * This method is used to search and retrieve a cb node in cbList using a handle.
 *
 * @param[in]  handle               Handle to search for.
 *
 * @return address of the node if found, otherwise NULL
 */
ClientCB* GetClientCBUsingHandle(const OCDoHandle handle);

#ifdef WITH_PRESENCE
/**
 * This method is used to search and retrieve a cb node in cbList using a uri.
 *
 * @param[in]  requestUri           Uri to search for.
 *
 * @return address of the node if found, otherwise NULL
 */
ClientCB* GetClientCBUsingUri(const char *requestUri);
#endif // WITH_PRESENCE


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // OC_CLIENT_CB_H_

