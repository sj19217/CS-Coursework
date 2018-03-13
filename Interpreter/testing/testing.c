#include <stdio.h>
#include <mem.h>
#include "../headers/minitest.h"
#include "../headers/main.h"

void test_I101()
{
    char outputs[12] = {0, 1, 1, 2, 4, 4, 1, 2, 2, 3, 3, -1};

    for (int i = 0; i <= 11; i++) {
//        ASSERT(getOpLen(i) == outputs[i], "Successful run of I101 (test outputs of getOpLen)\n",
//               "I101 (test outputs of getOpLen) failed for i=%i\n", i)
        SUBTEST(
        assert(getOpLen(i) == outputs[i], "Expected %i, got %i", outputs[i], getOpLen(i));
        , "input=%i", i);
//        startSubTest("input=%i", i);
//        assert(getOpLen(i) == outputs[i], "Unexpected result %i returned", getOpLen(i));
//        endSubTest();
    }
}

void test_I102()
{
    unsigned char inputs[21] = {0xA0, 0xB0, 0xC0, 0xD0,
                                0xA1, 0xB1, 0xC1, 0xD1,
                                0xA2, 0xB2, 0xC2, 0xD2,
                                0xA3, 0xB3, 0xC3, 0xD3,
                                0xE1, 0xE2, 0xE3, 0xE4, 0xF1};
    unsigned char outputs[21] = {4, 4, 4, 4,
                                 2, 2, 2, 2,
                                 1, 1, 1, 1,
                                 1, 1, 1, 1,
                                 4, 4, 4, 4, 4};

    for (int i = 0; i < 21; i++) {
//        ASSERT(getRegisterSize(inputs[i]) == outputs[i],
//            "Successful run of I102 (test outputs of getRegisterSize)\n",
//            "I201 (test outputs of getRegisterSize) failed for input=%i", inputs[i])
        SUBTEST(
        assert(getRegisterSize(inputs[i]) == outputs[i], "Expected %i, got %i", outputs[i], getRegisterSize(inputs[i]));
        , "input=%i", inputs[i]);
    }
}

void test_I103()
{
    setRegisterValue(0xA0, "ABCD");
    SUBTEST(
        assert(strncmp(getRegisterValue(0xA0), "ABCD", 4) == 0, "Expected ABCD which was not returned");
    , "Putting ABCD into eax", 0)

    setRegisterValue(0xA1, "AB");
    SUBTEST(
        assert(strncmp(getRegisterValue(0xA1), "AB", 2) == 0, "Expected AB which was not returned");
    , "Putting AB into ax", 0)

    setRegisterValue(0xA3, "A");
    SUBTEST(
        assert(strncmp(getRegisterValue(0xA3), "A", 1) == 0, "Expected A which was not returned");
    , "Putting ABCD into eax", 0)

    setRegisterValue(0xB1, "ABCD");
    SUBTEST(
        assert(strncmp(getRegisterValue(0xB1), "CD", 4) == 0, "Expected CD which was not returned");
    , "Putting ABCD into bx (which should not fit)", 0)
}

// Calls them one by one. Called from the main function when the -t argument is given.
void runTests()
{
    TEST(test_I101(), "I101/test outputs of getOpLen", "");
    TEST(test_I102(), "I102/test outputs of getRegisterSize", "");
    TEST(test_I103(), "I103/test tha setRegisterValue and getRegisterValue work together", "");
}