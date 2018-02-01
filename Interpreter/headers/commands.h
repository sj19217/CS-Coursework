//
// Created by Stephen on 22/01/2018.
//

#include <stdint.h>
#include "main.h"

#ifndef INTERPRETER_COMMANDS_H
#define INTERPRETER_COMMANDS_H

void exec_CMP_char(char op1_val, char op2_val);
void exec_CMP_uchar(unsigned char op1_val, unsigned char op2_val);
void exec_CMP_short(int16_t op1_val, int16_t op2_val);
void exec_CMP_ushort(uint16_t op1_val, uint16_t op2_val);
void exec_CMP_int(int op1_val, int op2_val);
void exec_CMP_uint(unsigned int op1_val, unsigned int op2_val);
void exec_CMP_float(float op1_val, float op2_val);

void exec_JMP(unsigned int addr);
void exec_JE(unsigned int addr);
void exec_JNE(unsigned int addr);
void exec_JLT(unsigned int addr);
void exec_JLE(unsigned int addr);
void exec_JGT(unsigned int addr);
void exec_JGE(unsigned int addr);

void exec_MOV_reg(unsigned char regnum, int length, const unsigned char* str);
void exec_MOV_mem(unsigned long maddr, int length, const unsigned char* str);

void exec_LEA_reg(unsigned char regnum, unsigned int maddr);
void exec_LEA_mem(unsigned int maddr_to, unsigned int maddr_from);

void exec_ADD_char(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type);
void exec_ADD_uchar(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type);
void exec_ADD_short(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type);
void exec_ADD_ushort(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type);
void exec_ADD_int(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type);
void exec_ADD_uint(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type);
void exec_ADD_float(unsigned char* op1, int op1_type, unsigned char* op2, int op2_type);

void exec_arithmetic(char* function, char dtype, unsigned char* op1, int op1_type, unsigned char* op2, int op2_type);

#endif //INTERPRETER_COMMANDS_H