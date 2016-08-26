/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.service.pm.test.helper;

public class PMConstants {
    public static final String TAG = "PMTest";
    public static final String EMPTY_STRING = "";
    public static final String WRONG_ERROR_CODE = "Expected Error Code Mismatch";
    public static String mErrorMessage = "";
    public static final int NUMBER_ZERO = 0;
    public static final int BUFFER_SIZE = 1024;

    public static final int DELAY_SHORT = 1000;                                  // millisecond
    public static final int DISCOVERY_TIMEOUT_10 = 10;
    public static final int DISCOVERY_TIMEOUT_ZERO = 0;
    public static final int DISCOVERY_TIMEOUT_ONE = 1;
    public static final int DISCOVERY_TIMEOUT_TWO = 2;
    public static final int DISCOVERY_TIMEOUT_NEGATIVE = -1;

    // Callback Related Resources
    public static final int SUCCES_CODE = 0;
    public static final int ERROR_CODE = 1;
    public static final int CALLBACK_TIMEOUT = 60;                                    // 60
    // seconds
    public static final boolean CALLBACK_INVOKED = true;
    public static final boolean CALLBACK_NOT_INVOKED = false;
    public static final int CALLBACK_INVOKED_BUT_FAILED = 0;
    public static final String CallBackNotInvoked = "CallBack Not Invoked";
    public static final String InputPinCBCallbackInvoked = "InputPinCBCallbackInvoked";

    // PM Initialization Resources
    public static final String DIR_NAME_FILE = "files";
    public static final String DIR_NAME_DATABASES = "databases";
    public static final String OIC_CLIENT_CBOR_DB_FILE = "oic_svr_db_client.dat";
    public static final String OIC_JWSERVER_CBOR_DB_FILE_1 = "oic_svr_db_server_justworks.dat";
    public static final String OIC_JWSERVER_CBOR_DB_FILE_2 = "oic_svr_db_server.dat";
    public static final String OIC_DP_CLIENT_CBOR_DB_FILE = "oic_svr_db_client_directpairing.dat";
    public static final String OIC_SQL_DB_FILE = "Pdm.db";

    // ACL Related Resource
    public static final String DEFAULT_ROWNER_ID = "61646d69-6e44-6576-6963-655555494430";
    public static final String DEFAULT_RESOURCES = "*";
    public static final String HREF_RESOURCES_1A = "/a/device1a";
    public static final String HREF_RESOURCES_1B = "/a/device1b";
    public static final String HREF_RESOURCES_2A = "/a/device2a";
    public static final String HREF_RESOURCES_2B = "/a/device2b";
    public static final String RESOURCE_TYPE_1A = "oic.wk.dev1a";
    public static final String RESOURCE_TYPE_1B = "oic.wk.dev1b";
    public static final String RESOURCE_TYPE_2A = "oic.wk.dev2a";
    public static final String RESOURCE_TYPE_2B = "oic.wk.dev2b";

    public static final String DEFAULT_SUBJECT_ID1 = "44446d69-6e44-6576-6963-655555494430";
    public static final String DEFAULT_SUBJECT_ID2 = "55556d69-6e44-6576-6963-655555494430";

    public static final String RESOURCE_INTERFACE_1 = "oic.if.baseline";
    public static final String RESOURCE_INTERFACE_2 = "oic.if.r";

    public static final String DEFAULT_RECURRENCES_1 = "Daily";
    public static final String DEFAULT_RECURRENCES_2 = "Weekly";
    public static final String DEFAULT_PERIOD = "12-12-16";
    public static final int DEFAULT_PERMISSION = 31;

    // Older Version... will be removed
    public static final String SUBJECT_O1 = "44446d69-6e44-6576-6963-655555400001";
    public static final String SUBJECT_O2 = "55556d69-6e44-6576-6963-655555400002";
    public static final String DEFAULT_RECURRENCES = "Daily";

    // Credential Resource
    public static final int CREDENTIAL_TYPE = 1;
    public static final int OWNER_PSK_LENGTH_128 = 128 / 8;

    // Direct Pairing Resources
    public static final String DP_PRE_CONFIGURED_PIN = "00000000";

    // Error Code
    public static final String ERROR_INVALID_PARAM = "INVALID_PARAM";
    public static final String ERROR_INVALID_CALLBACK = "INVALID_CALLBACK";
    public static final String ERROR_ERROR = "ERROR";

    // Others
    public static final int DEVICE_INDEX_ZERO = 0;
    public static final int DEVICE_INDEX_ONE = 1;
}