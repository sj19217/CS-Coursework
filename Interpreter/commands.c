//
// Created by Stephen on 22/01/2018.
//

#include <stdint.h>
#include <stdio.h>
#include <mem.h>
#include "headers/main.h"
#include "headers/util.h"
#include "headers/log.h"
#include "headers/commands.h"

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
    log_trace("exec_JMP(addr=0x%x)", addr);
    env.pc = addr;
}

void exec_JE(unsigned int addr)
{
    log_trace("exec_JE(addr=0x%x)", addr);
    if (env.cmp_e) {
        env.pc = addr;
    }
}

void exec_JNE(unsigned int addr)
{
    log_trace("exec_JNE(addr=0x%x)", addr);
    if (!env.cmp_e) {
        env.pc = addr;
    }
}

void exec_JLT(unsigned int addr)
{
    log_trace("exec_JLT(addr=0x%x)", addr);
    if (env.cmp_n) {
        env.pc = addr;
    }
}

void exec_JLE(unsigned int addr)
{
    log_trace("exec_JLE(addr=0x%x)", addr);
    if (env.cmp_n || env.cmp_e) {
        env.pc = addr;
    }
}

void exec_JGT(unsigned int addr)
{
    log_trace("exec_JGT(addr=0x%x)", addr);
    if (env.cmp_p) {
        env.pc = addr;
    }
}

void exec_JGE(unsigned int addr)
{
    log_trace("exec_JGE(addr=0x%x)", addr);
    if (env.cmp_p || env.cmp_e) {
        env.pc = addr;
    }
}


// The MOV commands. 1B, 2B and 4B all have the same function and just get the length passed in as an argument.
// However, there are different functions depending on whether the destination is a register or a memory address.
void exec_MOV_reg(unsigned char regnum, int length, const unsigned char* str)
{
    log_trace("exec_MOV_reg(regnum=0x%02x, length=%i, str[0]=0x%02x", regnum, length, str[0]);
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
    unsigned char bytes[4];
    bytes[3] = (unsigned char) (0x000000FF & full);
    bytes[2] = (unsigned char) (0x0000FF00 & full) >> 8;
    bytes[1] = (unsigned char) (0x00FF0000 & full) >> 16;
    bytes[0] = (unsigned char) (0xFF000000 & full) >> 24;
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
    log_trace("exec_MOV_mem(maddr=0x%x, length=%i, str[0]=0x%02x", maddr, length, str[0]);
    for (int i = 0; i < length; i++) {
        env.memory[maddr + i] = str[i];
    }
}


// The LEA commands take any settable first operand and either a memory address or arithmetic second operand
void exec_LEA_reg(unsigned char regnum, unsigned long maddr)
{
    log_trace("exec_LEA_reg(regnum=0x%02x, maddr=0x%x)", regnum, maddr);
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
    log_trace("exec_LEA_mem(maddr_to=0x%x, pointer=0x%x", maddr_to, pointer);
    unsigned char* bytes = (unsigned char*) &pointer;
    env.memory[maddr_to] = bytes[0];
    env.memory[maddr_to+1] = bytes[1];
    env.memory[maddr_to+2] = bytes[2];
    env.memory[maddr_to+3] = bytes[3];
}

// Arithmetic commands begin here. Rather than having two versions, they are included in the same function.

void exec_ADD_char(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type)
{
    // The first can be a register, a memory address or an arithmetic operand
    char val1 = *(char*) getOperandValue(op1_type, op1);
    char val2 = *(char*) getOperandValue(op2_type, op2);
    char total = val1 + val2;
    int expanded_total = total;
    if (op1_type == 1) {
        // Destination is a register
        setRegisterValue(op1[0], (void *) &expanded_total);
    } else if (op1_type == 5) {
        // Destination is a memory address
        unsigned int maddr = convertTo_uint(op1);
        setMemory(maddr, 1, &((unsigned char*) &expanded_total)[3]);
    } else if (op1_type >= 6 && op1_type <= 10) {
        // Destination is an arithmetic expression
        unsigned int maddr = getMAddrFromArithmetic(op1_type, op1);
        setMemory(maddr, 1, &((unsigned char*) &expanded_total)[3]);
    }
}

void exec_ADD_uchar(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type)
{
    // The first can be a register, a memory address or an arithmetic operand
    unsigned char val1 = *(unsigned char*) getOperandValue(op1_type, op1);
    unsigned char val2 = *(unsigned char*) getOperandValue(op2_type, op2);
    unsigned char total = val1 + val2;
    unsigned int expanded_total = total;
    if (op1_type == 1) {
        // Destination is a register
        setRegisterValue(op1[0], (void *) &expanded_total);
    } else if (op1_type == 5) {
        // Destination is a memory address
        unsigned int maddr = convertTo_uint(op1);
        setMemory(maddr, 1, &((unsigned char*) &expanded_total)[3]);
    } else if (op1_type >= 6 && op1_type <= 10) {
        // Destination is an arithmetic expression
        unsigned int maddr = getMAddrFromArithmetic(op1_type, op1);
        setMemory(maddr, 1, &((unsigned char*) &expanded_total)[3]);
    }
}

void exec_ADD_short(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type)
{
    // The first can be a register, a memory address or an arithmetic operand
    int16_t val1 = *(int16_t*) getOperandValue(op1_type, op1);
    int16_t val2 = *(int16_t*) getOperandValue(op2_type, op2);
    int16_t total = val1 + val2;
    int expanded_total = total;
    if (op1_type == 1) {
        // Destination is a register
        setRegisterValue(op1[0], (void *) &expanded_total);
    } else if (op1_type == 5) {
        // Destination is a memory address
        unsigned int maddr = convertTo_uint(op1);
        setMemory(maddr, 2, &((unsigned char*) &expanded_total)[2]);
    } else if (op1_type >= 6 && op1_type <= 10) {
        // Destination is an arithmetic expression
        unsigned int maddr = getMAddrFromArithmetic(op1_type, op1);
        setMemory(maddr, 2, &((unsigned char*) &expanded_total)[2]);
    }
}

void exec_ADD_ushort(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type)
{
    // The first can be a register, a memory address or an arithmetic operand
    uint16_t val1 = *(uint16_t*) getOperandValue(op1_type, op1);
    uint16_t val2 = *(uint16_t*) getOperandValue(op2_type, op2);
    uint16_t total = val1 + val2;
    unsigned int expanded_total = total;
    if (op1_type == 1) {
        // Destination is a register
        setRegisterValue(op1[0], (void *) &expanded_total);
    } else if (op1_type == 5) {
        // Destination is a memory address
        unsigned int maddr = convertTo_uint(op1);
        setMemory(maddr, 2, &((unsigned char*) &expanded_total)[2]);
    } else if (op1_type >= 6 && op1_type <= 10) {
        // Destination is an arithmetic expression
        unsigned int maddr = getMAddrFromArithmetic(op1_type, op1);
        setMemory(maddr, 2, &((unsigned char*) &expanded_total)[2]);
    }
}

void exec_ADD_int(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type)
{
    // The first can be a register, a memory address or an arithmetic operand
    int val1 = *(int*) getOperandValue(op1_type, op1);
    int val2 = *(int*) getOperandValue(op2_type, op2);
    int total = val1 + val2;
    int expanded_total = total;
    if (op1_type == 1) {
        // Destination is a register
        setRegisterValue(op1[0], (void *) &expanded_total);
    } else if (op1_type == 5) {
        // Destination is a memory address
        unsigned int maddr = convertTo_uint(op1);
        setMemory(maddr, 4, (unsigned char*) &expanded_total);
    } else if (op1_type >= 6 && op1_type <= 10) {
        // Destination is an arithmetic expression
        unsigned int maddr = getMAddrFromArithmetic(op1_type, op1);
        setMemory(maddr, 4, (unsigned char*) &expanded_total);
    }
}

void exec_ADD_uint(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type)
{
    // The first can be a register, a memory address or an arithmetic operand
    unsigned int val1 = *(unsigned int*) getOperandValue(op1_type, op1);
    unsigned int val2 = *(unsigned int*) getOperandValue(op2_type, op2);
    unsigned int total = val1 + val2;
    unsigned int expanded_total = total;
    if (op1_type == 1) {
        // Destination is a register
        setRegisterValue(op1[0], (void *) &expanded_total);
    } else if (op1_type == 5) {
        // Destination is a memory address
        unsigned int maddr = convertTo_uint(op1);
        setMemory(maddr, 4, (unsigned char*) &expanded_total);
    } else if (op1_type >= 6 && op1_type <= 10) {
        // Destination is an arithmetic expression
        unsigned int maddr = getMAddrFromArithmetic(op1_type, op1);
        setMemory(maddr, 4, (unsigned char*) &expanded_total);
    }
}

void exec_ADD_float(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type)
{
    // The first can be a register, a memory address or an arithmetic operand
    float val1 = *(float*) getOperandValue(op1_type, op1);
    float val2 = *(float*) getOperandValue(op2_type, op2);
    float total = val1 + val2;
    if (op1_type == 1) {
        // Destination is a register
        setRegisterValue(op1[0], (void *) &total);
    } else if (op1_type == 5) {
        // Destination is a memory address
        unsigned int maddr = convertTo_uint(op1);
        setMemory(maddr, 4, (unsigned char*) &total);
    } else if (op1_type >= 6 && op1_type <= 10) {
        // Destination is an arithmetic expression
        unsigned int maddr = getMAddrFromArithmetic(op1_type, op1);
        setMemory(maddr, 4, (unsigned char*) &total);
    }
}

// The complicated, ugly code needed to get the value of an operand and place it in the right version of the union
#define SET_OPERANDS(D, T) operand1.b = *(T*) getOperandValue(op1_type, (void*) op1); \
operand2.D = *(T*) getOperandValue(op2_type, (void*) op2);

// The still complicated, though slightly less ugly, code for performing the given operation on all possible versions.
#define CALCULATE_TOTAL(O) switch (dtype) { \
case 'b':\
    total.b = operand1.b O operand2.b;\
    break; \
case 'B':\
    total.B = operand1.B O operand2.B; \
    break; \
case 'h': \
    total.h = operand1.h O operand2.h;\
    break; \
case 'H': \
    total.H = operand1.H O operand2.H; \
    break; \
case 'i': \
    total.i = operand1.i O operand2.i; \
    break; \
case 'I': \
    total.i = operand1.I O operand2.I; \
    break; \
case 'f': \
    total.f = operand1.f O operand2.f; \
    break; \
default: \
    log_error("Unknown type: %c", dtype);\
    break; \
}

// The prototype function that will replace almost anything
void exec_arithmetic(char* function, char dtype, unsigned char* op1, int op1_type, unsigned char* op2, int op2_type)
{
    union OperandValue operand1;
    union OperandValue operand2;

    // Set the operands to have the correct value, written in their correct data type
    switch (dtype)
    {
        case 'b':
            SET_OPERANDS(b, char); break;
        case 'B':
            SET_OPERANDS(B, unsigned char); break;
        case 'h':
            SET_OPERANDS(h, int16_t); break;
        case 'H':
            SET_OPERANDS(H, uint16_t); break;
        case 'i':
            SET_OPERANDS(i, int); break;
        case 'I':
            SET_OPERANDS(I, unsigned int); break;
        case 'f':
            SET_OPERANDS(f, float); break;
        default:
            log_error("Unknown data type: %c", dtype);
    }

    union {
        signed char b;
        unsigned char B;
        int16_t h;
        uint16_t H;
        int i;
        unsigned int I;
        float f;
    } total;

    if (strcmp(function, "ADD") == 0) {
        CALCULATE_TOTAL(+)
    } else if (strcmp(function, "SUB") == 0) {
        CALCULATE_TOTAL(-)
    } else if (strcmp(function, "MUL") == 0) {
        CALCULATE_TOTAL(*)
    } else if (strcmp(function, "IDIV") == 0) {
        CALCULATE_TOTAL(/)
    } else if (strcmp(function, "MOD") == 0) {
        switch (dtype) {
            case 'b':
                total.i = operand1.b % operand2.b;
            case 'B':
                total.i = operand1.B % operand2.B;
            case 'h':
                total.i = operand1.h % operand2.h;
            case 'H':
                total.i = operand1.H % operand2.H;
            case 'i':
                total.i = operand1.i % operand2.i;
            case 'I':
                total.i = operand1.I % operand2.I;
            default:
                log_error("Unknown type: %c", dtype);
        }
    } else if (strcmp(function, "EDIV") == 0) {
        switch (dtype) {
            case 'b':
                total.f = (float) operand1.b / (float) operand2.b;
            case 'B':
                total.f = (float) operand1.B / (float) operand2.B;
            case 'h':
                total.f = (float) operand1.h / (float) operand2.h;
            case 'H':
                total.f = (float) operand1.H / (float) operand2.H;
            case 'i':
                total.f = (float) operand1.i / (float) operand2.i;
            case 'I':
                total.f = (float) operand1.I / (float) operand2.I;
            case 'f':
                total.f = operand1.f / operand2.f;
            default:
                log_error("Unknown type: %c", dtype);
                return;
        }
    }

    // Convert the int to 4 chars
    char* bytes = (char*) &total;
    if (op1_type == 1) {
        // Register
        setRegisterValue(op1[0], (void*) bytes);
    } else if (op1_type == 5) {
        // Memory location
        setMemory(convertTo_uint(op1), 1, (unsigned char*) bytes+3);
    } else if (op1_type >= 6 && op1_type <= 10) {
        setMemory(getMAddrFromArithmetic(op1_type, op1), 1, (unsigned char*) bytes+3);
    } else {    // Add arithmetic expressions too
        log_error("Cannot move result of arithmetic to location of type %i", op1_type);
    }
}