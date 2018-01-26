#include "headers/main.h"
#include "headers/log.h"

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