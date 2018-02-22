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
#include "headers/gui.h"

#define MAX_META_SECTION_LENGTH 100
#define MAX_CONFIG_KEY_LENGTH 10

// This is a bit of code that checks the type and length of an operand are correct
// It is a macro, meaning that wherever shield_exec_jump appears elsewhere this will be pasted in
#define shield_exec_jump(C, T, L) \
if (op1_type == (T) && op1_len == (L)) { \
    C (convertTo_uint(op1_str));\
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
    log_trace("getRegisterSize(regnum=0x%x)", regnum);
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
    log_trace("setRegisterValue(regnum=0x%x, void* data)");
    switch (regnum) {
        case 0xA0: // eax
            env.eax.eax = *(unsigned int*) data;
            break;
            //return (void *) &env.eax;
        case 0xB0: // ebx
            env.ebx.ebx = *(unsigned int*) data;
            break;
        case 0xC0: // ecx
            env.ecx.ecx = *(unsigned int*) data;
            break;
        case 0xD0: // edx
            env.edx.edx = *(unsigned int*) data;
            break;
        case 0xE1: // esi
            env.esi = *(unsigned int*) data;
            break;
        case 0xE2: // edi
            env.edi = *(unsigned int*) data;
            break;
        case 0xE3: // ebp
            env.ebp = *(unsigned int*) data;
            break;
        case 0xE4: // esp
            env.esp = *(unsigned int*) data;
            break;
        case 0xA1: // ax
            env.eax.div.ax = *(uint16_t*) data;
            break;
            //return (void *) &env.eax.div.ax;
        case 0xB1: // bx
            env.ebx.div.bx = *(uint16_t*) data;
            break;
        case 0xC1: // cx
            env.ecx.div.cx = *(uint16_t*) data;
            break;
        case 0xD1: // dx
            env.edx.div.dx = *(uint16_t*) data;
            break;
        case 0xA2: // ah
            env.eax.div.a.ah = *(unsigned char*) data;
            break;
        case 0xB2: // bh
            env.ebx.div.b.bh = *(unsigned char*) data;
            break;
        case 0xC2: // ch
            env.ecx.div.c.ch = *(unsigned char*) data;
            break;
        case 0xD2: // dh
            env.edx.div.d.dh = *(unsigned char*) data;
            break;
        case 0xA3: // al
            env.eax.div.a.al = *(unsigned char*) data;
            break;
        case 0xB3: // bl
            env.ebx.div.b.bl = *(unsigned char*) data;
            break;
        case 0xC3: // cl
            env.ecx.div.c.cl = *(unsigned char*) data;
            break;
        case 0xD3: // dl
            env.edx.div.d.dl = *(unsigned char*) data;
            break;
        case 0xF1:
            log_error("Cannot set input value as a register");
            break;
        case 0xF0:
            printf("Output: %i\n", convertTo_int(reverse(data, 4)));
            break;
        default:
            log_error("Unknown register number in setRegisterValue: 0x%x", regnum);
    }
}

void* getRegisterValue(unsigned char regnum)
{
    log_trace("getRegisterValue(regnum=0x%x)", regnum);
    unsigned long* retval = (unsigned long*) malloc(sizeof(long));
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
            scanf("%lu", retval);
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

void* getOperandValue(unsigned char type, unsigned char *str)
{
    // Gets the value represented by this, including (e.g.) dereferencing the memory address or getting a register's value
    log_trace("getOperandValue(type=%i, str[0]=0x%x)", type, str[0]);

    int val_length = getOperandValueLength(type, str, 4);
    unsigned char* bytes = calloc(1, (size_t) val_length);

    unsigned long maddr;
    switch (type) {
        case 0:
            break;
        case 1: // A register
            // Take one byte
            bytes = getRegisterValue(str[0]);
            break;
        case 2: // 1-byte immediate
            bytes = str;
            break;
        case 3: // 2-byte immediate
            bytes = str;
            break;
        case 4: //4-byte immediate
            bytes = str;
            break;
        case 5: // Memory address
            maddr = 0;
            maddr += str[0] << 24;
            maddr += str[1] << 16;
            maddr += str[2] << 8;
            maddr += str[3];
            // Copy 4 bytes from memory into a separate variable
            bytes = &env.memory[maddr];
            break;
        default:
            if (type >= 6 && type <= 10) {
                // Arithmetic type
                maddr = getMAddrFromArithmetic(type, str);
                bytes = &env.memory[maddr];
            } else {
                printf("Unknown type in getOperandValue: 0x%x\n", type);
                break;
            }
    }

    return bytes;
}

void* getOperandValueR(unsigned char type, unsigned char *str, _Bool do_reverse, unsigned int length)
{
    unsigned char* bytes = getOperandValue(type, str);

    if (do_reverse) {
        return reverse(bytes, length);
    } else {
        return bytes;
    }
}

// Written these to simplify the annoyingly complex task of printing status after
// a MOV or LEA
#define REPORT_MOV_REG(size) \
if (config.interactive_mode) printf("exec_func mov_reg %s %i %s %s %s\n", getRegisterName(op1_str[0]), size, \
        getOperandType(op2_type), bytesAsJSONArray(op2_str, op2_len), \
        bytesAsJSONArray((unsigned char*) getOperandValue(op2_type, op2_str), \
        getOperandValueLength(op2_type, op2_str, size)));

#define REPORT_MOV_MEM(size) \
if (config.interactive_mode) printf("exec_func move_mem %i %i %s %s %s\n", convertTo_uint(op1_str), size, \
        getOperandType(op2_type), bytesAsJSONArray(op2_str, op2_len), \
        bytesAsJSONArray((unsigned char*) getOperandValue(op2_type, op2_str), \
        getOperandValueLength(op2_type, op2_str, size)));

#define REPORT_MOV_ARITH(size) \
if (config.interactive_mode) printf("exec_func move_mem %i %i %s %s %s\n", getMAddrFromArithmetic(op1_type, op1_str), \
        size, getOperandType(op2_type), bytesAsJSONArray(op2_str, op2_len), \
        bytesAsJSONArray((unsigned char*) getOperandValue(op2_type, op2_str), \
        getOperandValueLength(op2_type, op2_str, size)));


#define FIT_DECODED_OPERAND(size) \
unsigned char* bytes = calloc(1, size); \
int len = getOperandValueLength(op2_type, op2_str, size); \
unsigned char* val_str = (unsigned char*) getOperandValueR(op2_type, op2_str, op2_type==1, getOperandValueLength(op2_type, op2_str, size)); \
for (int i = 0; i < len; i++) { \
    bytes[i + ((size) - len)] = val_str[i]; \
}

void execute(unsigned char opcode, char dtype,
             unsigned char op1_type, int op1_len, unsigned char* op1_str,
             unsigned char op2_type, int op2_len, unsigned char* op2_str)
{
    log_trace("execute(opcode=0x%02x, op1_type=%i, op1_len=%i, op1_str[0]=0x%02x, " \
                "op2_type=%i, op2_len=%i, op2_str[0]=0x%02x)", opcode, op1_type, op1_len, op1_str[0],
                op2_type, op2_len, op2_str[0]);
    pauseUntilPermitted(s_decode);
    switch (opcode) {
        case CMP_char:
            // CMP takes 2 values
            if (config.interactive_mode) printf("decode CMP_char\n");
            exec_CMP_char(*(char*) getOperandValue(op1_type, op1_str),
                          *(char*) getOperandValue(op2_type, op2_str));
            break;
        case CMP_uchar:
            if (config.interactive_mode) printf("decode CMP_uchar\n");
            exec_CMP_uchar(*(unsigned char*) getOperandValue(op1_type, op1_str),
                          *(unsigned char*) getOperandValue(op2_type, op2_str));
            break;
        case CMP_short:
            if (config.interactive_mode) printf("decode CMP_short\n");
            exec_CMP_short(convertTo_short((unsigned char*) getOperandValue(op1_type, op1_str)),
                           convertTo_short((unsigned char*) getOperandValue(op1_type, op1_str)));
            break;
        case CMP_ushort:
            if (config.interactive_mode) printf("decode CMP_ushort\n");
            exec_CMP_ushort(convertTo_ushort((unsigned char*) getOperandValue(op1_type, op1_str)),
                           convertTo_ushort((unsigned char*) getOperandValue(op1_type, op1_str)));
            break;
        case CMP_int:
            if (config.interactive_mode) printf("decode CMP_int\n");
            exec_CMP_int(convertTo_int((unsigned char*) getOperandValue(op1_type, op1_str)),
                         convertTo_int((unsigned char*) getOperandValue(op2_type, op2_str)));
            break;
        case CMP_uint:
            if (config.interactive_mode) printf("decode CMP_uint\n");
            exec_CMP_uint(convertTo_uint((unsigned char*) getOperandValue(op1_type, op1_str)),
                          convertTo_uint((unsigned char*) getOperandValue(op2_type, op2_str)));
            break;
        case CMP_float:
            if (config.interactive_mode) printf("decode CMP_float\n");
            exec_CMP_float(*(float*) getOperandValue(op1_type, op1_str),
                          *(float*) getOperandValue(op2_type, op2_str));
            break;
        case JMP:
            if (config.interactive_mode) printf("decode JMP\n");
            shield_exec_jump(exec_JMP, 5, 4)
            break;
        case JE:
            if (config.interactive_mode) printf("decode JE\n");
            shield_exec_jump(exec_JE, 5, 4);
        case JNE:
            if (config.interactive_mode) printf("decode JNE\n");
            shield_exec_jump(exec_JNE, 5, 4);
            break;
        case JLT:
            if (config.interactive_mode) printf("decode JLT\n");
            shield_exec_jump(exec_JLT, 5, 4);
            break;
        case JLE:
            if (config.interactive_mode) printf("decode JLE\n");
            shield_exec_jump(exec_JLE, 5, 4);
            break;
        case JGT:
            if (config.interactive_mode) printf("decode JGT\n");
            shield_exec_jump(exec_JGT, 5, 4);
            break;
        case JGE:
            if (config.interactive_mode) printf("decode JGE\n");
            shield_exec_jump(exec_JGE, 5, 4);
            break;
        case MOV_1B:
            if (config.interactive_mode) printf("decode MOV_1B\n");
            if (op1_type == 1) {
                exec_MOV_reg(op1_str[0], 1, (unsigned char*) getOperandValue(op2_type, op2_str));
                REPORT_MOV_REG(1)
            } else if (op1_type == 5) {
                unsigned char* data = getOperandValue(op2_type, op2_str);
                exec_MOV_mem(convertTo_uint(op1_str), 1, data);
                REPORT_MOV_MEM(1)
            } else if (op1_type >= 6 && op1_type <= 10) {
                exec_MOV_mem(getMAddrFromArithmetic(op1_type, op1_str),
                             1,
                             (unsigned char*) getOperandValue(op2_type, op2_str));
                REPORT_MOV_ARITH(1)

            } else {
                printf("Cannot move content to form %i\n", op1_type);
            }
            break;
        case MOV_2B:
            if (config.interactive_mode) printf("decode MOV_2B\n");
            if (op1_type == 1) {
                exec_MOV_reg(op1_str[0], 2, (unsigned char*) getOperandValue(op2_type, op2_str));
                REPORT_MOV_REG(2)
            } else if (op1_type == 5) {
                //unsigned char* bytes = (unsigned char*) getOperandValue(op2_type, op2_str);
                FIT_DECODED_OPERAND(2)
                exec_MOV_mem(convertTo_uint(op1_str), 2, bytes);
                REPORT_MOV_MEM(2)
            } else if (op1_type >= 6 && op1_type <= 10) {
                FIT_DECODED_OPERAND(2)
                exec_MOV_mem(getMAddrFromArithmetic(op1_type, op1_str), 2, bytes);
                REPORT_MOV_ARITH(2)
            } else {
                printf("Cannot move content to form %i\n", op1_type);
            }
            break;
        case MOV_4B:
            if (config.interactive_mode) printf("decode MOV_4B\n");
            if (op1_type == 1) {
                exec_MOV_reg(op1_str[0], 4, (unsigned char*) getOperandValue(op2_type, op2_str));
                REPORT_MOV_REG(4)
            } else if (op1_type == 5) {
                FIT_DECODED_OPERAND(4)
                exec_MOV_mem(convertTo_uint(op1_str), 4, (unsigned char*) getOperandValue(op2_type, op2_str));
                REPORT_MOV_MEM(4)
            } else if (op1_type >= 6 && op1_type <= 10) {
                FIT_DECODED_OPERAND(4)
                exec_MOV_mem(getMAddrFromArithmetic(op1_type, op1_str), 4, bytes);
                REPORT_MOV_ARITH(4)
            } else {
                printf("Cannot move content to form %i\n", op1_type);
            }
            break;
        case LEA:
            if (config.interactive_mode) printf("decode LEA\n");
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
                if (config.interactive_mode) printf("decode ADD_%s\n", convertTypeLetterToName(dtype));
                exec_arithmetic("ADD", dtype, op1_str, op1_type, op2_str, op2_type);
            } else if (opcode >= SUB_char && opcode <= SUB_float) {
                if (config.interactive_mode) printf("decode SUB_%s\n", convertTypeLetterToName(dtype));
                exec_arithmetic("SUB", dtype, op1_str, op1_type, op2_str, op2_type);
            } else if (opcode >= MUL_char && opcode <= MUL_float) {
                if (config.interactive_mode) printf("decode MUL_%s\n", convertTypeLetterToName(dtype));
                exec_arithmetic("MUL", dtype, op1_str, op1_type, op2_str, op2_type);
            } else if (opcode >= IDIV_char && IDIV_float) {
                if (config.interactive_mode) printf("decode IDIV_%s\n", convertTypeLetterToName(dtype));
                exec_arithmetic("IDIV", dtype, op1_str, op1_type, op2_str, op2_type);
            } else if (opcode >= MOD_char && opcode <= MOD_float) {
                if (config.interactive_mode) printf("decode MOD_%s\n", convertTypeLetterToName(dtype));
                exec_arithmetic("MOD", dtype, op1_str, op1_type, op2_str, op2_type);
            } else if (opcode >= EDIV_char && opcode <= EDIV_float) {
                if (config.interactive_mode) printf("decode EDIV_%s\n", convertTypeLetterToName(dtype));
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
        pauseUntilPermitted(s_fetch);

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

        if (config.interactive_mode) {
            // Report the finishing of the fetch stage
            printf("fetch %i %i %i", ((int)env.pc-2-op1_len-op2_len),
                                      opcode,
                                      env.memory[env.pc-1-op1_len-op2_len]);
            printf(" [%i", op1_str[0]);
            for (int i = 1; i < op1_len; i++) printf(",%i", op1_str[i]);
            printf("] [%i", op2_str[0]);
            for (int i = 1; i < op2_len; i++) printf(",%i", op2_str[i]);
            printf("]");
            printf("\n");
        }

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

    if (config.interactive_mode) {
        printf("config {\"memorykb\": %i}\n", config.memorykb);
    }

    pauseUntilPermitted(s_start);

    // Initialise the environment
    const int memsize = config.memorykb * 1024;
    env.memory = (unsigned char*) calloc(memsize, sizeof(unsigned char));

    // Fill up the memory with the instructions
    for (int i = config_length; i < length; i++) {
        env.memory[i - config_length] = bytecode[i];
        //printf("0x%x: %x\n", i - config_length, env.memory[i - config_length]);
    }

    // Dump out the memory
    if (!config.interactive_mode) memdump(200, 1, 16);


    // Start running instructions
    if (config.interactive_mode) {
        printf("done_step start\n");
    }
    runLoop();
}

int main(int argc, char** argv)
{
    //log_trace("main(argc=%i, char** argv)", argc);

    // Process command line arguments
    int iflag = 0;
    char fname[100];
    int has_fname = 0;
    long logLevel = 0;
    char* end;

    // -i means instructions, meaning that the GUI program is listening and wants display instructions
    // -f <filename>, meaning the filename

    int c;
    while ((c = getopt(argc, argv, "id:f:")) != -1)
    {
        switch (c){
            case 'i':
                iflag = 1;
                break;
            case 'f':
                strcpy(fname, optarg);
                has_fname = 1;
                break;
            case 'd':
                logLevel = strtol(optarg, &end, 10);
            default:
                break;
        }
    }

    log_set_level(logLevel);

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

    if (iflag) {
        config.interactive_mode = TRUE;
    }



#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    log_info("Compiled as little endian\n");
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    log_info("Compiled as big endian\n");
#endif

    run(content, iflag, filelen);


    return 0;
}

/*
 * Error codes:
 * 0 - Completed fine
 * 1 - Could not find file
 */