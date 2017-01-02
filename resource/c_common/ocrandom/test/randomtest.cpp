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

#define ARR_SIZE (20)

TEST(RandomGeneration,OCSeedRandom) {
    EXPECT_EQ(0, OCSeedRandom());
}

TEST(RandomGeneration,OCGetRandomByte) {
    EXPECT_NO_THROW(OCGetRandomByte());
}

TEST(RandomGeneration,OCGetRandom) {
    EXPECT_NO_THROW(OCGetRandom());
}

TEST(RandomGeneration,OCFillRandomMem_BoundsCheck) {
    uint8_t array[ARR_SIZE] = {};

    // Ignore the first and last bytes of the array
    OCFillRandomMem(array + 1, ARR_SIZE - 2);

    EXPECT_EQ((uint8_t )0, array[0]);
    EXPECT_EQ((uint8_t )0, array[ARR_SIZE - 1]);
}

// This test attempts to prevent developers from plugging in a memset as a
// random number generator.
TEST(RandomGeneration,OCFillRandomMem_GeneratedDataIsDifferent) {
    uint8_t array[ARR_SIZE] = {};
    uint8_t matchingByte = OCGetRandomByte();
    bool foundNonMatchingByte = false;

    OCFillRandomMem(array, ARR_SIZE);

    // Note: this test can flag a false-failure, but this is
    // statistically very unlikely to fail.  In a uniformly distributed
    // random function, we can expect that:
    //
    //   P(array is all matchingByte) = 1 in 2^(ARR_SIZE_IN_BITS)
    //
    // Let us assume that this test will only be run by CI or
    // a developer once per second. In our case, our quantities are:
    //
    //   Array size in bits = 160 bits
    //   Number of combinations = 2 ^ 160 ~= 1.46 * 10^48
    //   Test frequency        = 1 minute
    //   Seconds in 1000 years = 3.154 * 10 ^ 10
    //
    // If we ran this every second, our expected number of failures
    // is very insignificant.
    //
    //   Number of combinations >>> Seconds in 1000 years
    //
    // After crunching the numbers, after 1000 years we can expect to see:
    //
    //   (Seconds in 1000 years) / (Number of combinations) = False-fail count
    //
    //   Expect count of false-failures = 2.158 * 10 ^ -38 ~= 0
    //
    for (int i = 0; i < ARR_SIZE; i++)
    {
        if (matchingByte != array[i])
        {
            foundNonMatchingByte = true;
            break;
        }
    }

    EXPECT_TRUE(foundNonMatchingByte);
}

TEST(RandomGeneration, OCGenerateUuid)
{
    EXPECT_EQ(RAND_UUID_INVALID_PARAM, OCGenerateUuid(NULL));

    uint8_t uuid[16] = {};

    EXPECT_EQ(RAND_UUID_OK, OCGenerateUuid(uuid));

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
    EXPECT_EQ(RAND_UUID_INVALID_PARAM, OCGenerateUuidString(NULL));

    char uuidString[37] = {};

    EXPECT_EQ(RAND_UUID_OK, OCGenerateUuidString(uuidString));
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
