unsigned int convertTo_uint(const unsigned char *str);
void setMemory(unsigned int maddr, int length, const unsigned char* value);
void memdump(int max, _Bool headings, int columns);
_Bool startsWith(const char* a, const char* b);