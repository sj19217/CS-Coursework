#include <mem.h>
#include <malloc.h>
#include "headers/main.h"
#include "headers/log.h"

char convertTo_char(const unsigned char* str)
{
    log_trace("convertTo_char(str[0]=%0x2)", str[0]);
    return *(char*) str;
}

unsigned char convertTo_uchar(const unsigned char* str)
{
    log_trace("convertTo_uchar(str[0]=%0x2)", str[0]);
    return *str;
}

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
    log_trace("convertTo_ushort(str[0]=%0x2)", str[0]);
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

float convertTo_float(const unsigned char* str)
{
    return *(float*) str;
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
    log_trace("setMemory(maddr=%i, length=%i, value[0]=%02x)", maddr, length, value[0]);
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

// Prints out the prefix "mem" followed by the `max` first memory items in decimal, separated by spaces
void linearMemdump(int max)
{
    log_trace("linearMemdump(max=%i)", max);
    printf("mem");
    for (int i = 0; i < max; i++) {
        printf(" %03i", env.memory[i]);
    }
    printf("\n");
    fflush(stdout); // Flush it to get it out immediately
}

_Bool startsWith(const char* a, const char* b)
{
    if (strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

// Converts the given letter (e.g. "B") to a string name (e.g. "uchar")
char* convertTypeLetterToName(char letter)
{
    switch (letter) {
        case 'b':
            return "char";
        case 'B':
            return "uchar";
        case 'h':
            return "short";
        case 'H':
            return "ushort";
        case 'i':
            return "int";
        case 'I':
            return "uint";
        case 'f':
            return "float";
        default:
            return "unknown";
    }
}

// Prints data about the environment in the JSON format specified in GUI/ipc.md
void printEnvData()
{
    printf("data {");   // Intro to what this is and opening the JSON object

    // Print out the current PC
    printf("'pc': %i, ", (int) env.pc);

    // Print out the general registers
    printf("'genregs': {");
    printf("'eax': %i, ", (int) env.eax.eax);
    printf("'ebx': %i, ", (int) env.ebx.ebx);
    printf("'ecx': %i, ", (int) env.ecx.ecx);
    printf("'edx': %i, ", (int) env.edx.edx);
    printf("'esi': %i, ", (int) env.esi);
    printf("'edi': %i, ", (int) env.edi);
    printf("'ebp': %i, ", (int) env.ebp);
    printf("'esp': %i}, ", (int) env.esp);

    // Comparisons
    printf("'cmp': {");
    printf("'p': %i, ", env.cmp_p);
    printf("'n': %i, ", env.cmp_n);
    printf("'e': %i}, ", env.cmp_e);

    // Memory printout
    printf("'memory': [");
    printf("%i", env.memory[0]); // Use this arrangement to
    for (int i = 1; i < config.memorykb * 1024; i++) {
        printf(", %i", env.memory[i]);
    }
    printf("]");

    // End the object and command
    printf("}\n");
}