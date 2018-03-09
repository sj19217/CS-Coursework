#include <stdio.h>
#include "../headers/minitest.h"
#include "../headers/main.h"

void test_I101()
{
    char outputs[11] = {0, 1, 1, 2, 4, 4, 1, 2, 2, 3, 3};

    for (int i = 0; i <= 11; i++) {
        ASSERT(getOpLen(i) == outputs[i], "Successful run of I101 (test outputs of getOpLen)\n",
               "I101 (test outputs of getOpLen) failed for i=%i\n", i)
    }
}

void test_I102()
{
    unsigned char inputs[21] = {0xA0, 0xB0, 0xC0, 0xD0,
                                0xA1, 0xB1, 0xC1, 0xD1,
                                0xA2, 0xB2, 0xC2, 0xD2,
                                0xA3, 0xB3, 0xC3, 0xD3,
                                0xE0, 0xE1, 0xE2, 0xE3, 0xF1};
    unsigned char outputs[21] = {4, 4, 4, 4,
                                 2, 2, 2, 2,
                                 1, 1, 1, 1,
                                 1, 1, 1, 1, 4};

    for (int i = 0; i <= 21; i++) {
        ASSERT(getRegisterSize(inputs[i]) == outputs[i],
            "Successful run of I102 (test outputs of getRegisterSize)\n",
            "I201 (test outputs of getRegisterSize) failed for input=%i", inputs[i])
    }
}

// Calls them one by one. Called from the main function when the -t argument is given.
void runTests()
{
    test_I101();
    test_I102();
}