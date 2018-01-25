//
// Created by Stephen on 22/01/2018.
//

#include <stdint.h>
#include <stdio.h>
#include "headers/main.h"
#include "headers/util.h"

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
void exec_MOV_reg(unsigned char regnum, int length, const unsigned char* str)
{
    unsigned char reg_size = getRegisterSize(regnum);

    if (reg_size < length) {
        printf("Warning: moving %i bytes to register number %i results in chopping off of bytes\n", length, reg_size);
    }

    // Store a full 4-byte integer and just add to it whatever the value is. This manages the zeroes.
    unsigned long full = 0;
    if (length == 1) { // MOV 1B
        full += str[0];
    } else if (length == 2) {
        full += *(uint16_t*) str;
    } else if (length == 4) {
        full += *(unsigned int*) str;
    } else {
        printf("Unknown length parameter %i\n", length);
    }

    // The 4-byte `full` now contains the value. Now place it into the register, trimming as necessary.
    // Turn it into an array of chars so that the right amount can be extracted
    char* bytes = (char*) &full;
    if (reg_size == 1) {
        setRegisterValue(regnum, (void *) &bytes[3]);
    } else if (reg_size == 2) {
        setRegisterValue(regnum, (void *) &bytes[2]);
    } else if (reg_size == 4) {
        setRegisterValue(regnum, (void *) bytes);
    }
}

void exec_MOV_mem(unsigned long maddr, int length, const unsigned char* str)
{
    for (int i = 0; i < length; i++) {
        env.memory[maddr + i] = str[i];
    }
}


// The LEA commands take any settable first operand and either a memory address or arithmetic second operand
void exec_LEA_reg(unsigned char regnum, unsigned long maddr)
{
    // This one is given the memory address as an int and stores that number in the destination register
    unsigned char reg_size = getRegisterSize(regnum);
    char* bytes = (char*) &maddr;
    if (reg_size == 1) {
        setRegisterValue(regnum, (void *) &bytes[3]);
    } else if (reg_size == 2) {
        setRegisterValue(regnum, (void *) &bytes[2]);
    } else if (reg_size == 4) {
        setRegisterValue(regnum, (void *) bytes);
    }
}

void exec_LEA_mem(unsigned long maddr_to, unsigned long pointer)
{
    unsigned char* bytes = (unsigned char*) &pointer;
    env.memory[maddr_to] = bytes[0];
    env.memory[maddr_to+1] = bytes[1];
    env.memory[maddr_to+2] = bytes[2];
    env.memory[maddr_to+3] = bytes[3];
}

// Arithmetic commands begin here. Rather than having two versions, they are included in the same function to make
// the macro technique already used for CMP

void exec_ADD_char(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type)
{
    // The first can be a register, a memory address or an arithmetic operand
    unsigned char val1 = *(unsigned char*) getOperandValue(op1_type, op1);
    unsigned char val2 = *(unsigned char*) getOperandValue(op2_type, op2);
    unsigned char total = val1 + val2;
    unsigned int expanded_total = total;
    if (op1_type == 1) {
        // Destination is a register
        unsigned char regnum = op1[0];
        setRegisterValue(op1[0], (void *) &expanded_total);
    } else if (op1_type == 5) {
        // Destination is a memory address
        unsigned int maddr = convertTo_uint(op1);
        setMemory(maddr, 1, &((unsigned char*) &expanded_total)[4-1]);
    } else if (op1_type >= 6 && op1_type <= 10) {
        // Destination is an arithmetic expression
        unsigned int maddr = getMAddrFromArithmetic(op1_type, op1);
        setMemory(maddr, 1, &((unsigned char*) &expanded_total)[4-1]);
    }
}