#include <mem.h>
#include <malloc.h>
#include "headers/main.h"
#include "headers/log.h"

int16_t convertTo_short(const unsigned char* str)
{
    log_trace("convertTo_short(str[0]=%0x2)", str[0]);
    int16_t result = 0;
    result += str[0] << 8;
    result += str[1];

    return result;
}

int16_t convertTo_ushort(const unsigned char* str)
{
    log_trace("convertTo_short(str[0]=%0x2)", str[0]);
    uint16_t result = 0;
    result += str[0] << 8;
    result += str[1];

    return result;
}

// Takes a char* and reads 4 bytes from it, turning them into an integer
unsigned int convertTo_uint(const unsigned char *str)
{
    log_trace("convertTo_uint(str[0]=%0x2)", str[0]);
    unsigned int result = 0;
    result += str[0] << 24;
    result += str[1] << 16;
    result += str[2] << 8;
    result += str[3];

    return result;
}

int convertTo_int(const unsigned char* str)
{
    log_trace("convertTo_int(str[0]=%02x)", str[0]);
    int result = 0;
    result += str[0] << 24;
    result += str[1] << 16;
    result += str[2] << 8;
    result += str[3];

    return result;
}

// Takes an int and turns it into a sequence of characters
unsigned char* convertTo_str(int num)
{
    unsigned char* str = (unsigned char*) malloc(sizeof(unsigned char)*4);
    str[0] = (unsigned char) (0x000000FF & num);
    str[1] = (unsigned char) (0x0000FF00 & num) >> 8;
    str[2] = (unsigned char) (0x00FF0000 & num) >> 16;
    str[3] = (unsigned char) (0xFF000000 & num) >> 24;
    return str;
}

void setMemory(unsigned int maddr, int length, const unsigned char* value)
{
    log_trace("setMemory(maddr=%i, length=%i, value[0]=%02x)", maddr, length, value);
    for (int i = 0; i < length; i++)
    {
        env.memory[maddr+i] = value[i];
    }
}

// Dumps the contents of memory onto the screen
void memdump(int len, _Bool headings, int columns)
{
    log_trace("memdump(len=%i, headings=%i, columns=%i)", len, headings, columns);
    if (len > config.memorykb * 1024) {
        len = config.memorykb * 1024;
        log_warn("Size of data to print is larger than memory (%i > %i)", len, config.memorykb*1024);
    }

    if (headings) {
        printf("       ");  // Get the alignment right
        for (int i = 0; i < columns; i++) {
            printf("%02x ", i);
        }
    }

    for (int i = 0; i < len; i++)
    {
        if (i % columns == 0) {
            // Print out a newline and heading (if headings are requested)
            printf("\n");
            if (headings) {
                printf("%06x", i / columns);
            }
        }

        printf(" %02x", env.memory[i]);
    }
}

_Bool startsWith(const char* a, const char* b)
{
    if (strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}