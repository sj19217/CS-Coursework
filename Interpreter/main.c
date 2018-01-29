// This program does the actual interpretation
// When the GUI wants to use it, the GUI program will call this with the -i/--instructions command line argument
// This will cause commands to be printed that can be interpreted by the GUI program
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <mem.h>

#include "headers/main.h"
#include "headers/instructions.h"
#include "headers/commands.h"
#include "headers/util.h"
#include "headers/log.h"

#define MAX_META_SECTION_LENGTH 100
#define MAX_CONFIG_KEY_LENGTH 10

// This is a bit of code that checks the type and length of an operand are correct
// It is a macro, meaning that wherever shield_exec_jump appears elsewhere this will be pasted in
#define shield_exec_jump(C, T, L) \
if (op1_type == (T) && op1_len == (L)) { \
    C (*(unsigned int*) op1_str);\
} else { \
    printf("Unable to execute cmd statement with op1_type=%i, op1_len=%i", \
                                op1_type, op1_len);\
}



// FUNCTIONS

int processConfig(const unsigned char *bytecode, int length)
{
    // Keep running until 4 nulls are reached
    int num_nulls = 0;
    int i = 0;
    char config_str[MAX_META_SECTION_LENGTH];
    while (i < length)
    {
        if (bytecode[i] == 0) {
            num_nulls++;
            if (num_nulls == 4) {
                break;
            }
        }

        config_str[i] = bytecode[i];
        i++;
    }
    // Add a null character to the end
    config_str[i] = '\0';

    log_info("Config string: %s", config_str);

    // Split the string at a & sign
    char* cfg_str = strdup(config_str);
    char* pair = strtok(cfg_str, "&");

    while (pair != NULL)
    {
        char key[MAX_CONFIG_KEY_LENGTH];
        int key_len = 0;
        char val[10];
        int at_value = 0;

        // Loop through the characters in the pair
        for (i = 0; i < strlen(pair); i++) {
            // If still processing the value, act like it.
            if (at_value == 0) {
                // When an = sign is reached, switch to value mode
                if (pair[i] == '=') {
                    at_value = 1;
                    key[i] = '\0';
                    key_len = strlen(key);
                } else {
                    key[i] = pair[i];
                }
            } else {
                // (key_len+1) is the number of characters in pair before val starts
                val[i - key_len - 1] = pair[i];
            }
        }

        // Process it
        if (strcmp(key, "mem_amt") == 0) {
            // Convert to int and put into dict
            config.memorykb = strtol(val, NULL, 10);
        }

        pair = strtok(NULL, "&");
    }

    // Print out the config struct
    log_info("config.memorykb = %i", config.memorykb);

    return strlen(config_str);
}

int getOpLen(int type)
{
    log_trace("getOpLen(type=%i)", type);
    switch (type) {
        case 0: // None
            return 0;
        case 1: // Register
            return 1;
        case 2: // 8-bit immediate
            return 1;
        case 3: // 16-bit immediate
            return 2;
        case 4: // 32-bit immediate
            return 4;
        case 5: // Memory address
            return 4;
        case 6: // "a" form arithmetic
            return 1;
        case 7: // "a*b" form arithmetic
            return 2;
        case 8: // "a+b" form arithmetic
            return 2;
        case 9: // "a*b+c" form arithmetic
            return 3;
        case 10: // "a+b*c" form arithmetic
            return 3;
        default: // Invalid
            return -1;
    }
}

unsigned char getRegisterSize(unsigned char regnum)
{
    log_trace("getRegisterSize(regnum=0x%i)", regnum);
    if ((regnum >= 0xE1 && regnum <= 0xE4) || (regnum >> 4 >= 10 && regnum >> 4 <= 13)) {
        return 4;
    } else if ((regnum >> 4) >= 10 && (regnum >> 4) <= 13) {
        if ((regnum & 0x0F) == 1) {
            return 2;
        } else if ((regnum & 0x0F) == 2 || (regnum & 0x0F) == 3) {
            return 1;
        }
    } else if (regnum == 0xF0 || regnum == 0xF1) {
        return 4;
    } else {
        printf("Unknown register number in getRegisterSize: 0x%x", regnum);
        return 0;
    }
}

void setRegisterValue(unsigned char regnum, void* data)
{
    log_trace("setRegisterValue(regnum=0x%i, void* data)");
    switch (regnum) {
        case 0xA0: // eax
            env.eax.eax = *(unsigned int*) data;
            //return (void *) &env.eax;
        case 0xB0: // ebx
            env.ebx.ebx = *(unsigned int*) data;
        case 0xC0: // ecx
            env.ebx.ebx = *(unsigned int*) data;
        case 0xD0: // edx
            env.ebx.ebx = *(unsigned int*) data;
        case 0xE1: // esi
            env.esi = *(unsigned int*) data;
        case 0xE2: // edi
            env.edi = *(unsigned int*) data;
        case 0xE3: // ebp
            env.ebp = *(unsigned int*) data;
        case 0xE4: // esp
            env.esp = *(unsigned int*) data;
        case 0xA1: // ax
            env.eax.div.ax = *(uint16_t*) data;
            //return (void *) &env.eax.div.ax;
        case 0xB1: // bx
            env.ebx.div.bx = *(uint16_t*) data;
        case 0xC1: // cx
            env.ecx.div.cx = *(uint16_t*) data;
        case 0xD1: // dx
            env.edx.div.dx = *(uint16_t*) data;
        case 0xA2: // ah
            env.eax.div.a.ah = *(unsigned char*) data;
        case 0xB2: // bh
            env.ebx.div.b.bh = *(unsigned char*) data;
        case 0xC2: // ch
            env.ecx.div.c.ch = *(unsigned char*) data;
        case 0xD2: // dh
            env.edx.div.d.dh = *(unsigned char*) data;
        case 0xA3: // al
            env.eax.div.a.al = *(unsigned char*) data;
        case 0xB3: // bl
            env.ebx.div.b.bl = *(unsigned char*) data;
        case 0xC3: // cl
            env.ecx.div.c.cl = *(unsigned char*) data;
        case 0xD3: // dl
            env.edx.div.d.dl = *(unsigned char*) data;
        case 0xF1:
            log_error("Cannot set input value as a register");
        case 0xF0:
            printf("%c", *(unsigned char*) data);
            break;
        default:
            log_error("Unknown register number in setRegisterValue: 0x%x", regnum);
    }
}

void* getRegisterValue(unsigned char regnum)
{
    log_trace("getRegisterValue(regnum=0x%x)", regnum);
    unsigned long retval;
    switch (regnum) {
        case 0xA0: // eax
            return (void*) &env.eax;
        case 0xB0: // ebx
            return (void*) &env.ebx;
        case 0xC0: // ecx
            return (void*) &env.ecx;
        case 0xD0: // edx
            return (void*) &env.edx;
        case 0xE1: // esi
            return (void*) &env.esi;
        case 0xE2: // edi
            return (void*) &env.edi;
        case 0xE3: // ebp
            return (void*) &env.ebp;
        case 0xE4: // esp
            return (void*) &env.esp;
        case 0xA1: // ax
            return (void*) &env.eax.div.ax;
        case 0xB1: // bx
            return (void*) &env.ebx.div.bx;
        case 0xC1: // cx
            return (void*) &env.ecx.div.cx;
        case 0xD1: // dx
            return (void*) &env.edx.div.dx;
        case 0xA2: // ah
            return (void*) &env.eax.div.a.ah;
        case 0xB2: // bh
            return (void*) &env.ebx.div.b.bh;
        case 0xC2: // ch
            return (void*) &env.ecx.div.c.ch;
        case 0xD2: // dh
            return (void*) &env.edx.div.d.dh;
        case 0xA3: // al
            return (void*) &env.eax.div.a.al;
        case 0xB3: // bl
            return (void*) &env.ebx.div.b.bl;
        case 0xC3: // cl
            return (void*) &env.ecx.div.c.cl;
        case 0xD3: // dl
            return (void*) &env.edx.div.d.dl;
        case 0xF1:
            printf("> ");
            scanf("%lu", &retval);
            printf("\n");
            return (void*) retval;
        case 0xF0:
            printf("Cannot get value from output register");
            return (void*) &env.memory[0];
        default:
            printf("Unknown register number in getRegisterValue: 0x%x\n", regnum);
            return 0;
    }
}

unsigned long interpretArithmeticVariable(unsigned char val)
{
    log_trace("interpretArithmeticVariable(val=0x%x)", val);
    if (val == 2 || val == 4 || val == 8) {
        return val;
    } else {
        return *(unsigned long*) getRegisterValue(val);
    }
}

unsigned long getMAddrFromArithmetic(int type, unsigned char *str)
{
    log_trace("getMAddrFromArithmetic(type=%i, str[0]=0x%x)", type, str[0]);
    unsigned long a;
    unsigned long b;
    unsigned long c;
    switch (type) {
        case 6: // "a"-form arithmetic
            a = interpretArithmeticVariable(str[0]);
            return a;
        case 7: // "a*b"-form arithmetic
            a = interpretArithmeticVariable(str[0]);
            b = interpretArithmeticVariable(str[1]);
            return a*b;
        case 8: // "a+b"-form arithmetic
            a = interpretArithmeticVariable(str[0]);
            b = interpretArithmeticVariable(str[1]);
            return a+b;
        case 9: // "a*b+c"-style arithmetic
            a = interpretArithmeticVariable(str[0]);
            b = interpretArithmeticVariable(str[1]);
            c = interpretArithmeticVariable(str[2]);
            return (a*b)+c;
        case 10: // "a+b*c"-form arithmetic
            a = interpretArithmeticVariable(str[0]);
            b = interpretArithmeticVariable(str[1]);
            c = interpretArithmeticVariable(str[2]);
            return a+(b*c);
        default:
            log_error("Invalid arithmetic type: %i", type);
    }
}

void* getOperandValue(int type, unsigned char *str)
{
    // Gets the value represented by this, including (e.g.) dereferencing the memory address or getting a register's value
    log_trace("getOperandValue(type=%i, str[0]=0x%x)", type, str[0]);
    unsigned long maddr;
    switch (type) {
        case 0:
            //return NULL;
            return (void*) &env.memory[0];
        case 1: // A register
            // Take one byte
            return getRegisterValue(str[0]);
        case 2: // 1-byte immediate
            return (void*) str;
        case 3: // 2-byte immediate
            return (void*) str;
        case 4: //4-byte immediate
            return (void*) str;
        case 5: // Memory address
            maddr = 0;
            maddr += str[0] << 24;
            maddr += str[1] << 16;
            maddr += str[2] << 8;
            maddr += str[3];
            // Copy 4 bytes from memory into a separate variable
            return (void*) &env.memory[maddr];
        default:
            if (type >= 6 && type <= 10) {
                // Arithmetic type
                maddr = getMAddrFromArithmetic(type, str);
                return (void*) &env.memory[maddr];
            }
            printf("Unknown type in getOperandValue: 0x%x\n", type);
            break;
    }
}

void execute(unsigned char opcode, char dtype,
             int op1_type, int op1_len, unsigned char* op1_str,
             int op2_type, int op2_len, unsigned char* op2_str)
{
    log_trace("execute(opcode=0x%02x, op1_type=%i, op1_len=%i, op1_str[0]=0x%02x, " \
                "op2_type=%i, op2_len=%i, op2_str[0]=0x%02x", opcode, op1_type, op1_len, op1_str[0],
                op2_type, op2_len, op2_str[0]);
    switch (opcode) {
        case CMP_char:
            // CMP takes 2 values
            exec_CMP_char(*(char*) getOperandValue(op1_type, op1_str),
                          *(char*) getOperandValue(op2_type, op2_str));
            break;
        case CMP_uchar:
            exec_CMP_uchar(*(unsigned char*) getOperandValue(op1_type, op1_str),
                          *(unsigned char*) getOperandValue(op2_type, op2_str));
            break;
        case CMP_short:
            exec_CMP_short(*(int16_t*) getOperandValue(op1_type, op1_str),
                          *(int16_t*) getOperandValue(op2_type, op2_str));
            break;
        case CMP_ushort:
            exec_CMP_ushort(*(uint16_t*) getOperandValue(op1_type, op1_str),
                          *(uint16_t*) getOperandValue(op2_type, op2_str));
            break;
        case CMP_int:
            exec_CMP_int(*(int*) getOperandValue(op1_type, op1_str),
                          *(int*) getOperandValue(op2_type, op2_str));
            break;
        case CMP_uint:
            exec_CMP_uint(*(unsigned int*) getOperandValue(op1_type, op1_str),
                          *(unsigned int*) getOperandValue(op2_type, op2_str));
            break;
        case CMP_float:
            exec_CMP_float(*(float*) getOperandValue(op1_type, op1_str),
                          *(float*) getOperandValue(op2_type, op2_str));
            break;
        case JMP:
            shield_exec_jump(exec_JMP, 5, 4)
            break;
        case JE:
            shield_exec_jump(exec_JE, 5, 4);
        case JNE:
            shield_exec_jump(exec_JNE, 5, 4);
            break;
        case JLT:
            shield_exec_jump(exec_JLT, 5, 4);
            break;
        case JLE:
            shield_exec_jump(exec_JLE, 5, 4);
            break;
        case JGT:
            shield_exec_jump(exec_JGT, 5, 4);
            break;
        case JGE:
            shield_exec_jump(exec_JGE, 5, 4);
            break;
        case MOV_1B:
            if (op1_type == 1) {
                exec_MOV_reg(op1_str[0], 1, (unsigned char*) getOperandValue(op2_type, op2_str));
            } else if (op1_type == 5) {
                unsigned char* data = getOperandValue(op2_type, op2_str);
                exec_MOV_mem(op1_str[0], 1, data);
            } else if (op1_type >= 6 && op1_type <= 10) {
                exec_MOV_mem(getMAddrFromArithmetic(op1_type, op1_str),
                             1,
                             (unsigned char*) getOperandValue(op2_type, op2_str));
            } else {
                printf("Cannot move content to form %i\n", op1_type);
            }
            break;
        case MOV_2B:
            if (op1_type == 1) {
                exec_MOV_reg(op1_str[0], 2, (unsigned char*) getOperandValue(op2_type, op2_str));
            } else if (op1_type == 5) {
                exec_MOV_mem(op1_str[0], 2, (unsigned char*) getOperandValue(op2_type, op2_str));
            } else if (op1_type >= 6 && op1_type <= 10) {
                exec_MOV_mem(getMAddrFromArithmetic(op1_type, op1_str),
                             2,
                             (unsigned char*) getOperandValue(op2_type, op2_str));
            } else {
                printf("Cannot move content to form %i\n", op1_type);
            }
            break;
        case MOV_4B:
            if (op1_type == 1) {
                exec_MOV_reg(op1_str[0], 4, (unsigned char*) getOperandValue(op2_type, op2_str));
            } else if (op1_type == 5) {
                exec_MOV_mem(op1_str[0], 4, (unsigned char*) getOperandValue(op2_type, op2_str));
            } else if (op1_type >= 6 && op1_type <= 10) {
                exec_MOV_mem(getMAddrFromArithmetic(op1_type, op1_str),
                             4,
                             (unsigned char*) getOperandValue(op2_type, op2_str));
            } else {
                printf("Cannot move content to form %i\n", op1_type);
            }
            break;
        case LEA:
            if (op1_type == 1) {
                // Load effective address into register
                if (op2_type == 5) {
                    // Memory address -> Register
                    exec_LEA_reg(op1_str[0], convertTo_uint(op2_str));
                } else if (op2_type >= 6 && op2_type <= 10) {
                    // Arithmetic -> Register
                    exec_LEA_reg(op1_str[0], getMAddrFromArithmetic(op2_type, op2_str));
                }
            } else if (op1_type == 5) {
                // Load effective address into a memory location
                if (op2_type == 5) {
                    // Memory address -> Memory
                    exec_LEA_mem(convertTo_uint(op1_str), convertTo_uint(op1_str));
                } else if (op2_type >= 6 && op2_type <= 10) {
                    // Arithmetic -> Memory
                    exec_LEA_mem(convertTo_uint(op1_str), getMAddrFromArithmetic(op2_type, op2_str));
                }
            } else {
                printf("Invalid combination of operand types for LEA: 0x%x and 0x%x", op1_type, op2_type);
            }
        default:
            if (opcode >= ADD_char && opcode <= ADD_float) {
                exec_arithmetic("ADD", dtype, op1_str, op1_type, op2_str, op2_type);
            } else if (opcode >= SUB_char && opcode <= SUB_float) {
                exec_arithmetic("SUB", dtype, op1_str, op1_type, op2_str, op2_type);
            } else if (opcode >= MUL_char && opcode <= MUL_float) {
                exec_arithmetic("MUL", dtype, op1_str, op1_type, op2_str, op2_type);
            } else if (opcode >= IDIV_char && IDIV_float) {
                exec_arithmetic("IDIV", dtype, op1_str, op1_type, op2_str, op2_type);
            } else if (opcode >= MOD_char && opcode <= MOD_float) {
                exec_arithmetic("MOD", dtype, op1_str, op1_type, op2_str, op2_type);
            } else if (opcode >= EDIV_char && opcode <= EDIV_float) {
                exec_arithmetic("EDIV", dtype, op1_str, op1_type, op2_str, op2_type);
            } else {
                log_error("Unknown opode: %i", opcode);
                return;
            }
    }
}

void runLoop()
{
    log_trace("runLoop()");
    // Executes the instructions
    while (1)
    {
        // Fetch
        unsigned char opcode = env.memory[env.pc];

        // If a HLT instruction, exit the loop
        if (opcode == 0) {
            break;
        }

        // Decode
        char dtype = 'u';
        int op1_type, op2_type;
        if (opcode == CMP_char |
                opcode == ADD_char | opcode == SUB_char |
                opcode == MUL_char | opcode == IDIV_char |
                opcode == MOD_char | opcode == EDIV_char) {
            dtype = 'b';
        } else if (opcode == CMP_uchar |
                opcode == ADD_uchar | opcode == SUB_uchar |
                opcode == MUL_uchar | opcode == IDIV_uchar |
                opcode == MOD_uchar | opcode == EDIV_uchar) {
            dtype = 'B';
        } else if (opcode == CMP_short |
                opcode == ADD_short | opcode == SUB_short |
                opcode == MUL_short | opcode == IDIV_short |
                opcode == MOD_short | opcode == EDIV_short) {
            dtype = 'h';
        } else if (opcode == CMP_ushort |
                opcode == ADD_ushort | opcode == SUB_ushort |
                opcode == MUL_ushort | opcode == IDIV_ushort |
                opcode == MOD_ushort | opcode == EDIV_ushort) {
            dtype = 'H';
        } else if (opcode == CMP_int |
                opcode == ADD_int | opcode == SUB_int |
                opcode == MUL_int | opcode == IDIV_int |
                opcode == MOD_int | opcode == EDIV_int) {
            dtype = 'i';
        } else if (opcode == CMP_uint |
                opcode == ADD_uint | opcode == SUB_uint |
                opcode == MUL_uint | opcode == IDIV_uint |
                opcode == MOD_uint | opcode == EDIV_uint) {
            dtype = 'I';
        } else if (opcode == CMP_float |
                opcode == ADD_float | opcode == SUB_float |
                opcode == MUL_float | opcode == IDIV_float |
                opcode == MOD_float | opcode == EDIV_float) {
            dtype = 'f';
        } else if ((opcode & 0b11110000) == 80 |
                (opcode & 0b11110000) == 96) {
            // One of the size-based commands
            if ((opcode & 0b00001111) == 0) {
                dtype = '1';
            } else if ((opcode & 0b00001111) == 1) {
                dtype = '2';
            } else if ((opcode & 0b00001111) == 2) {
                dtype = '4';
            }
        } else if (opcode == MOV_1B) {
            dtype = '1';
        } else if (opcode == MOV_2B) {
            dtype = '2';
        } else if (opcode == MOV_4B) {
            dtype = '4';
        } else if ((opcode > JMP) & (opcode < JGE)) {
            // One of the jump commands
            dtype = 'n';
        } else if (opcode == LEA) {
            dtype = 'n';
        }

        // Interpret the operand type
        op1_type = (env.memory[env.pc+1] & 0b11110000) >> 4;
        op2_type = env.memory[env.pc+1] & 0b00001111;

        if (getOpLen(op1_type) == -1 | getOpLen(op2_type) == -1) {
            // One of them is invalid
            log_error("Invalid operand byte 0x%x at PC %lu", env.memory[env.pc+1], env.pc+1);
        }

        // Pull the first operand as a char[]
        int op1_len = getOpLen(op1_type);
        int op2_len = getOpLen(op2_type);
        unsigned char op1_str[op1_len];
        unsigned char op2_str[op2_len];

        // Move the PC along to the start of the first operand and move to op1_str
        env.pc += 2;
        for (int i = 0; i < op1_len; i++) {
            op1_str[i] = env.memory[env.pc + i];
        }

        // Move the PC along again and interpret
        env.pc += op1_len;
        for (int i = 0; i < op2_len; i++) {
            op2_str[i] = env.memory[env.pc + i];
        }

        // Move the PC past the second operand (should be to the next opcode byte)
        env.pc += op2_len;

        // Execute
        execute(opcode, dtype, op1_type, op1_len, op1_str, op2_type, op2_len, op2_str);
    }
}

void run(unsigned char* bytecode, int iflag, int length)
{
    log_trace("run(bytecode[0]=%02x, iflag=%i, length=%i)", bytecode[0], iflag, length);

    // Begin the process of running the bytecode
    // Start by loading all of the configuration data
    int config_length = processConfig(bytecode, length) + 4;

    // Initialise the environment
    const int memsize = config.memorykb * 1024;
    env.memory = (unsigned char*) malloc(memsize*sizeof(unsigned char));

    // Fill up the memory with the instructions
    for (int i = config_length; i < length; i++) {
        env.memory[i - config_length] = bytecode[i];
        //printf("0x%x: %x\n", i - config_length, env.memory[i - config_length]);
    }

    // Dump out the memory
    memdump(200, 1, 16);


    // Start running instructions
    runLoop();
}

int main(int argc, char** argv)
{
    log_trace("main(argc=%i, char** argv)", argc);

    // Process command line arguments
    int iflag = 0;
    char fname[100];
    int has_fname = 0;

    // -i means instructions, meaning that the GUI program is listening and wants display instructions
    // -f <filename>, meaning the filename

    int c;
    while ((c = getopt(argc, argv, "if:")) != -1)
    {
        switch (c){
            case 'i':
                iflag = 1;
                break;
            case 'f':
                strcpy(fname, optarg);
                has_fname = 1;
            default:
                break;
        }
    }

    // Check that a filename was given
    if (has_fname == 0) {
        // Ask for the filename now
        printf("File name: ");
        scanf("%s", fname);
    }

    // Read the file
    FILE* fileptr;
    unsigned char* content;
    long filelen;

    fileptr = fopen(fname, "rb");

    if (fileptr == NULL) {
        printf("Cannot find file %s\n", fname);
        return 1;
    }

    fseek(fileptr, 0, SEEK_END);
    filelen = ftell(fileptr);
    rewind(fileptr);


    content = (unsigned char*)malloc(filelen*sizeof(char));
    fread(content, (size_t) filelen, 1, fileptr);
    fclose(fileptr);

    printf("%i\n", __BYTE_ORDER__);
    printf("Little: %i\n", __ORDER_LITTLE_ENDIAN__);
    printf("Big: %i\n", __ORDER_BIG_ENDIAN__);

    run(content, iflag, filelen);


    return 0;
}

/*
 * Error codes:
 * 0 - Completed fine
 * 1 - Could not find file
 */