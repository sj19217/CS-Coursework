// General unit tests. I could have used CUnit, but that would have been a bit unwieldy.

#include <stdio.h>
#include <windef.h>
#include <stdarg.h>
#include <malloc.h>

char* testName;
char* subtestName;

int numSubTestsInTest = 0;          // The number of sub tests in the current test
int numSubTestFailures = 0;         // The number of sub-tests in this major test that have failed
_Bool subTestHasFailed = FALSE;     // Set to true when an assertion fails during a sub-test
_Bool majorTestHasFailed = FALSE;   // Set to true when an assertion fails not during a sub-test
char* majorFailMessage = "";        // The failure message given in the failing assertion
char* subFailMessage = "";          // The failure message given in the failing assertion
_Bool inSubTest = FALSE;            // A bool that is true if a sub-test is currently active

int numMajorTests = 0;            // The number of major tests that have been done
int failedTests = 0;              // The overall number of tests that have failed

void startTest(char* name, ...)
{
    va_list va;
    va_start(va, name);
    char* formattedName = (char*) malloc((size_t) snprintf(NULL, 0, name, va) + 1);
    vsprintf(formattedName, name, va);
    va_end(va);

    // Reset everything, increment the number of names and set the test name.
    testName = formattedName;
    subtestName = "";
    numSubTestsInTest = 0;
    subTestHasFailed = FALSE;
    majorTestHasFailed = FALSE;
    majorFailMessage = "";
    subFailMessage = "";
    inSubTest = FALSE;

    printf("Starting test %i: %s\n", numMajorTests, name);
}

void endTest()
{
    if (!majorTestHasFailed) {
        printf("Test %i (%s) has succeeded. ", numMajorTests, testName);
    } else {
        printf("Test %i (%s) has failed: %s. ", numMajorTests, testName, majorFailMessage);
    }

    if (numSubTestsInTest == 0) {
        printf("No sub-tests\n");
    } else {
        if (numSubTestFailures == 0) {
            printf("All sub-tests successful\n");
        } else {
            printf("%i/%i sub-tests failed.\n", numSubTestFailures, numSubTestsInTest);
        }
    }
}

void startSubTest(char* name, ...)
{
    va_list va;
    va_start(va, name);
    char* formattedName = (char*) malloc((size_t) snprintf(NULL, 0, name, va) + 1);
    vsprintf(formattedName, name, va);
    va_end(va);

    numSubTestsInTest++;
    subtestName = formattedName;
    subTestHasFailed = FALSE;
    subFailMessage = "";
    inSubTest = TRUE;
}

void endSubTest()
{
    inSubTest = FALSE;
    if (subTestHasFailed) {
        printf("\tSub-test %i (%s) has failed: %s\n", numSubTestsInTest, subtestName, subFailMessage);
    } else {
        printf("\tSub-test %i (%s) has succeeded.\n", numSubTestsInTest, subtestName);
    }
}

void finalReport()
{
    printf("\n%i major tests performed\n", numMajorTests);
    printf(" - %i test procedures failed (including sub-tests)\n", failedTests);
}

void assert(_Bool test, char* failMessage, ...)
{
    va_list va;
    va_start(va, failMessage);
    char* message = (char*) malloc((size_t) snprintf(NULL, 0, failMessage, va) + 1);
    vsprintf(message, failMessage, va);
    va_end(va);

    if (!test) {
        // The assertion failed
        if (inSubTest) {
            numSubTestFailures++;
            subTestHasFailed = TRUE;
            subFailMessage = message;
        } else {
            majorTestHasFailed = TRUE;
            majorFailMessage = message;
        }
    }
}