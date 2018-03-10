#define TEST(code, name) \
startTest(name); \
code; \
endTest();

#define SUBTEST(code, name) \
startSubTest(name); \
code; \
endTest();

void assert(_Bool test);

void startTest(char* name);
void endTets();
void startSubTest(char* name);
void endSubTest();

void finalReport();