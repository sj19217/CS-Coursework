#include "headers/main.h"

// Takes a char* and reads 4 bytes from it, turning them into an integer
unsigned int convertTo_uint(const unsigned char *str)
{
    unsigned int result = 0;
    result += str[0] << 24;
    result += str[1] << 16;
    result += str[2] << 8;
    result += str[3];

    return result;
}

void setMemory(unsigned int maddr, int length, const unsigned char* value)
{
    for (int i = 0; i < length; i++) {
        env.memory[maddr+i] = value[i];
    }
}