//
// Created by Stephen on 22/01/2018.
//

#ifndef INTERPRETER_MAIN_H
#define INTERPRETER_MAIN_H

#include <stdint.h>

struct Environment{
    // The general registers. Smaller ones are contained within these.
    union {
        unsigned long eax;
        struct {
            uint16_t ax;
            union {
                unsigned char ah;
                unsigned char al;
            } a;
        } div;
    } eax;

    union {
        unsigned long ebx;
        struct {
            uint16_t bx;
            union {
                unsigned char bh;
                unsigned char bl;
            } b;
        } div;
    } ebx;

    union {
        unsigned long ecx;
        struct {
            uint16_t cx;
            union {
                unsigned char ch;
                unsigned char cl;
            } c;
        } div;
    } ecx;

    union {
        unsigned long edx;
        struct {
            uint16_t dx;
            union {
                unsigned char dh;
                unsigned char dl;
            } d;
        } div;
    } edx;

    unsigned long esi;
    unsigned long edi;
    unsigned long ebp;
    unsigned long esp;

    // The special registers
    unsigned long pc;
    unsigned long cmp_p;
    unsigned long cmp_n;
    unsigned long cmp_e;
    //unsigned long mar;
    //unsigned long mdr;
    //unsigned long cir; // This won't actually be used for these

    // The memory
    unsigned char* memory;
} env;

int process_config(const unsigned char* bytecode, int length);
int get_op_len(int type);
unsigned char get_register_size(unsigned char regnum);
void set_register_value(unsigned char regnum, void* data);
void* get_register_value(unsigned char regnum);
unsigned long interpret_arithmetic_item(unsigned char val);
unsigned long get_maddr_from_arithmetic(int type, unsigned char* str);
void* get_operand_value(int type, unsigned char* str);
void execute(unsigned char opcode,
             int op1_type, int op1_len, unsigned char* op1_str,
             int op2_type, int op2_len, unsigned char* op2_str);
void run_loop();
void run(unsigned char* bytecode, int iflag, int length);
int main(int argc, char** argv);

#endif //INTERPRETER_MAIN_H
