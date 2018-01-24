//
// Created by Stephen on 22/01/2018.
//

#include <stdint.h>
#include "headers/main.h"

// HLT has no actions, it just breaks the loop

// CMP

#define def_CMP(T, N) \
void exec_CMP_##N(T op1_val, T op2_val) \
{ \
    T diff = op1_val - op2_val; \
    if (diff < 0) { \
        env.cmp_n = 1; \
    } else if (diff == 0) { \
        env.cmp_e = 1; \
    } else if (diff > 0) { \
        env.cmp_p = 1; \
    } \
}

def_CMP(char, char);
def_CMP(unsigned char, uchar);
def_CMP(int16_t, short);
def_CMP(uint16_t, ushort);
def_CMP(int, int);
def_CMP(unsigned int, uint);
def_CMP(float, float);



// Jump instructions

void exec_JMP(unsigned int addr)
{
    env.pc = addr;
}

void exec_JE(unsigned int addr)
{
    if (env.cmp_e) {
        env.pc = addr;
    }
}

void exec_JNE(unsigned int addr)
{
    if (!env.cmp_e) {
        env.pc = addr;
    }
}

void exec_JLT(unsigned int addr)
{
    if (env.cmp_n) {
        env.pc = addr;
    }
}

void exec_JLE(unsigned int addr)
{
    if (env.cmp_n || env.cmp_e) {
        env.pc = addr;
    }
}

void exec_JGT(unsigned int addr)
{
    if (env.cmp_p) {
        env.pc = addr;
    }
}

void exec_JGE(unsigned int addr)
{
    if (env.cmp_p || env.cmp_e) {
        env.pc = addr;
    }
}


// The MOV commands. 1B, 2B and 4B all have the same function and just get the length passed in as an argument.
// However, there are different functions depending on whether the destination is a register or a memory address.
void exec_MOV_reg(unsigned char regnum, int length, unsigned char* str)
{
    if (length == 1) { // MOV 1B

    }
}