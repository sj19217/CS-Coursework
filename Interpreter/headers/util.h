union OperandValue {
    char b;
    unsigned char B;
    int16_t h;
    uint16_t H;
    int i;
    unsigned int I;
    float f;
};

unsigned int convertTo_uint(const unsigned char *str);
int convertTo_int(const unsigned char *str);
unsigned char* convertTo_str(int num);
void setMemory(unsigned int maddr, int length, const unsigned char* value);
void memdump(int max, _Bool headings, int columns);
_Bool startsWith(const char* a, const char* b);