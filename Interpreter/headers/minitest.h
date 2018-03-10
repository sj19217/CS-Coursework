#define TEST(code, name, ...) \
startTest(name, __VA_ARGS__); \
code; \
endTest();

#define SUBTEST(code, name, ...) \
startSubTest(name, __VA_ARGS__); \
code; \
endSubTest();

void assert(_Bool test, char* failMessage, ...);

void startTest(char* name, ...);
void endTest();
void startSubTest(char* name, ...);
void endSubTest();

void finalReport();