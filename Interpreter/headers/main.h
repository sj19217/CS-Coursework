//
// Created by Stephen on 22/01/2018.
//

#ifndef INTERPRETER_MAIN_H
#define INTERPRETER_MAIN_H

struct Environment{
    // The general registers. Smaller ones are contained within these.
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
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
void execute(unsigned char opcode, char dtype,
             int op1_type, int op1_len, unsigned char* op1_str,
             int op2_type, int op2_len, unsigned char* op2_str);
void run_loop();
void run(unsigned char* bytecode, int iflag, int length);
int main(int argc, char** argv);

#endif //INTERPRETER_MAIN_H
