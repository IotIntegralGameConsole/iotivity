//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "iotivity_config.h"
#include "ocstack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

/// This example is using experimental API, so there is no garrantee of support for future release,
/// nor any there any guarantee that breaking changes will not occur across releases.
#include "experimental/logger.h"

#define TAG ("occlient")

int gQuitFlag = 0;

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}

// This is a function called back when a device is discovered
OCStackApplicationResult applicationDiscoverCB(
        OCClientResponse * clientResponse) {
    (void)clientResponse;
    OIC_LOG(INFO, TAG, "Entering applicationDiscoverCB (Application Layer CB)");
    OIC_LOG_V(INFO, TAG, "Device =============> Discovered %s @ %s:%d",
                                    clientResponse->resourceUri,
                                    clientResponse->devAddr.addr,
                                    clientResponse->devAddr.port);
    //return OC_STACK_DELETE_TRANSACTION;
    return OC_STACK_KEEP_TRANSACTION;
}

 
static FILE* override_fopen(const char* path, const char* mode)
{
    static const char* CRED_FILE_NAME = "oic_svr_db_client_devowner.dat";
    char const * const filename
        = (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
        ? CRED_FILE_NAME : path;
    FILE* file = fopen(filename, mode);
    return file;
}

// This is a function called back when a device is discovered
OCStackApplicationResult onDiscover(void *ctx,
                                    OCDoHandle handle,
                                    OCClientResponse *clientResponse)
{
    (void) ctx;
    (void) handle;
    (void) clientResponse;
    OIC_LOG_V(INFO, TAG, "Discovered");
    OIC_LOG(INFO, TAG, __PRETTY_FUNCTION__);

    OCStackResult result = OC_STACK_OK;
    return result;
}


int main() {
    OIC_LOG_V(INFO, TAG, "Starting occlient");

    /* Initialize OCStack*/
    static OCPersistentStorage ps = {override_fopen, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);
    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    OCCallbackData cbData = {NULL, NULL, NULL};
    cbData.cb = onDiscover;
    /* Start a discovery query*/
    char szQueryUri[MAX_QUERY_LENGTH] = { 0 };
    strcpy(szQueryUri, OC_RSRVD_WELL_KNOWN_URI);
    if (OCDoResource(NULL,
                     OC_REST_DISCOVER, 
                     szQueryUri, 
                     0, 
                     0, 
                     CT_DEFAULT,
                     OC_LOW_QOS, 
                     &cbData, //
                     0,
                     0) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
        return 0;
    }

    // Break from loop with Ctrl+C
    OIC_LOG(INFO, TAG, "Entering occlient main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag) {

        if (OCProcess() != OC_STACK_OK) {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }

        sleep(1);
    }

    OIC_LOG(INFO, TAG, "Exiting occlient main loop...");

    if (OCStop() != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
    }

    return 0;
}

