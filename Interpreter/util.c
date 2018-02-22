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
    str[1] = (unsigned char) ((0x0000FF00 & num) >> 8);
    str[2] = (unsigned char) ((0x00FF0000 & num) >> 16);
    str[3] = (unsigned char) ((0xFF000000 & num) >> 24);
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

char* getRegisterName(int regnum)
{
    log_trace("getRegisterName(regnum=%i)", regnum);
    switch (regnum) {
        case 0xA0: // eax
            return "eax";
            //return (void *) &env.eax;
        case 0xB0: // ebx
            return "ebx";
        case 0xC0: // ecx
            return "ecx";
        case 0xD0: // edx
            return "edx";
        case 0xE1: // esi
            return "esi";
        case 0xE2: // edi
            return "edi";
        case 0xE3: // ebp
            return "ebp";
        case 0xE4: // esp
            return "esp";
        case 0xA1: // ax
            return "ax";
        case 0xB1: // bx
            return "bx";
        case 0xC1: // cx
            return "cx";
        case 0xD1: // dx
            return "dx";
        case 0xA2: // ah
            return "ah";
        case 0xB2: // bh
            return "bh";
        case 0xC2: // ch
            return "ch";
        case 0xD2: // dh
            return "dh";
        case 0xA3: // al
            return "al";
        case 0xB3: // bl
            return "bl";
        case 0xC3: // cl
            return "cl";
        case 0xD3: // dl
            return "dl";
        case 0xF1:
            return "in";
        case 0xF0:
            return "out";
        default:
            log_error("Unknown register number in getRegisterName: 0x%x", regnum);
    }
}

char* bytesAsJSONArray(unsigned char* str, int len)
{
    char* json = (char*) calloc((size_t) len, 5);
    json[0] = '[';
    for (int i = 0; i < len; i++) {
        sprintf(json+strlen(json), "%i", str[i]);
        if (i < len-1) {
            sprintf(json+strlen(json), ", ");
        }
    }
    sprintf(json+strlen(json), "]");

    return json;
}

char* getOperandType(unsigned char num)
{
    if (num == 0) {
        return "none";
    } else if (num == 1) {
        return "reg";
    } else if (num >= 2 && num <= 4) {
        return "immediate";
    } else if (num >= 5 && num <= 10) {
        return "maddr";
    }
}

// Gets the size that the byte string returned by getOperandValue would be.
// type - The operand type
// str - The raw (non-decoded) content of the operand
// default_ - In the event that this is a memory address, then a default is used,
//            since the caller will probably know how many bytes from that address should be used
int getOperandValueLength(unsigned char type, unsigned char* str, int default_)
{
    switch (type) {
        case 0:
            return 0;
        case 1:
            // Register
            return getRegisterSize(str[0]);
        case 2: // 1-byte immediate
            return 1;
        case 3: // 2-byte immediate
            return 2;
        case 4: // 4-byte-immediate
            return 4;
        case 5:
            return default_;
        case 6:
            return default_;
        case 7:
            return default_;
        case 8:
            return default_;
        case 9:
            return default_;
        case 10:
            return default_;
        default:
            log_error("Unknown type in getOperandValueLength(): %i", type);
            return 0;
    }
}

// Reverses the given string
unsigned char* reverse(const unsigned char* forward, int length)
{
    unsigned char* backward = calloc(1, (size_t) length);
    for (int i = 0; i < length; i++) {
        backward[i] = forward[length - 1 - i];
    }
    return backward;
}

unsigned char getSizeOfType(char typeletter)
{
    switch (typeletter) {
        case 'b':
            return 1;
        case 'B':
            return 1;
        case 'h':
            return 2;
        case 'H':
            return 2;
        case 'i':
            return 4;
        case 'I':
            return 4;
        case 'f':
            return 4;
        default:
            log_error("Invalid type letter in getSizeOfType: %c", typeletter);
    }
}