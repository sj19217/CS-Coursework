#ifndef INTERPRETER_UTIL_H
#define INTERPRETER_UTIL_H

#include <stdint.h>

union OperandValue {
    char b;
    unsigned char B;
    int16_t h;
    uint16_t H;
    int i;
    unsigned int I;
    float f;
};

char convertTo_char(const unsigned char* str);
unsigned char convertTo_uchar(const unsigned char* str);
int16_t convertTo_short(const unsigned char* str);
uint16_t convertTo_ushort(const unsigned char* str);
unsigned int convertTo_uint(const unsigned char *str);
int convertTo_int(const unsigned char *str);
float convertTo_float(const unsigned char* str);
unsigned char* convertTo_str(int num);
void setMemory(unsigned int maddr, int length, const unsigned char* value);
void memdump(int max, _Bool headings, int columns);
void linearMemdump(int max);
_Bool startsWith(const char* a, const char* b);
char* convertTypeLetterToName(char letter);
void printEnvData();
char* getRegisterName(int regnum);
char* bytesAsJSONArray(unsigned char* str, int len);
char* getOperandType(unsigned char num);
int getOperandValueLength(unsigned char type, unsigned char* str, int default_);
unsigned char* reverse(const unsigned char* forward, int length);
unsigned char getSizeOfType(char typeletter);

// Stuff from csv.c
void free_csv_line( char **parsed );
static int count_fields( const char *line );
char** parse_csv( const char *line );

#endif