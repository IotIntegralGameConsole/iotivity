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



extern "C" {
    #include "ocrandom.h"
}

#include "gtest/gtest.h"
#include "math.h"

#define ARR_SIZE (20)

TEST(RandomGeneration,OCGetRandom) {
    uint32_t value = OCGetRandom();
    EXPECT_LE((uint8_t )0, value);
    EXPECT_GT(pow(2, 32), value);
}

TEST(RandomGeneration,OCGetRandomBytes) {
    uint8_t array[ARR_SIZE] = {};
    EXPECT_TRUE(OCGetRandomBytes(array + 1, ARR_SIZE - 2));

    for (int i = 1; i <= ARR_SIZE - 2; i++) {
        uint8_t value = array[i];
        EXPECT_LE((uint8_t )0, value);
        EXPECT_GT(pow(2, 8), value);
    }
    EXPECT_EQ((uint8_t )0, array[0]);
    EXPECT_EQ((uint8_t )0, array[ARR_SIZE - 1]);
}

TEST(RandomGeneration, OCGenerateUuid)
{
    EXPECT_FALSE(OCGenerateUuid(NULL));

    uint8_t uuid[UUID_SIZE] = {};

    EXPECT_TRUE(OCGenerateUuid(uuid));

    EXPECT_FALSE(uuid[0] == '0' && uuid[1] == '0' &&
                 uuid[2] == '0' && uuid[3] == '0' &&
                 uuid[4] == '0' && uuid[5] == '0' &&
                 uuid[6] == '0' && uuid[7] == '0' &&
                 uuid[8] == '0' && uuid[9] == '0' &&
                 uuid[10] == '0' && uuid[11] == '0' &&
                 uuid[12] == '0' && uuid[13] == '0' &&
                 uuid[14] == '0' && uuid[15] == '0');
}

TEST(RandomGeneration, OCGenerateUuidString)
{
    char uuidString[UUID_STRING_SIZE] = {};
    uint8_t uuid[UUID_SIZE] = {};

    EXPECT_TRUE(OCGenerateUuid(uuid));
    EXPECT_TRUE(OCConvertUuidToString(uuid, uuidString));
    EXPECT_EQ('\0', uuidString[36]);
    EXPECT_EQ('-', uuidString[8]);
    EXPECT_EQ('-', uuidString[13]);
    EXPECT_EQ('-', uuidString[18]);
    EXPECT_EQ('-', uuidString[23]);

    for(int i = 0; i < 36; ++i)
    {
        EXPECT_TRUE(
                i == 8 || i == 13 || i == 18 || i == 23 ||
                (uuidString[i] >= 'a' && uuidString[i] <= 'f') ||
                (uuidString[i] >= '0' && uuidString[i] <= '9'))
                << "UUID Character out of range: "<< uuidString[i];
    }
}
