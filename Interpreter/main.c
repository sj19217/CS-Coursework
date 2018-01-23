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

#define MAX_META_SECTION_LENGTH 100
#define MAX_CONFIG_KEY_LENGTH 10

// GLOBAL STRUCTURES

// Stores the config information
struct {
    int memorykb;
} config;

// Used to store the current opcode string being worked on
struct {
    unsigned char* str;
} current;



// FUNCTIONS

int process_config(const unsigned char* bytecode, int length)
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

    printf("Config string: %s\n", config_str);

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
    printf("config.memorykb = %i\n", config.memorykb);

    return strlen(config_str);
}

int get_op_len(int type)
{
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

void* get_register_value(unsigned char regnum)
{
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
            printf("Unknown register number in get_register_value: 0x%x\n", regnum);
            return 0;
    }
}

unsigned long interpret_arithmetic_item(unsigned char val)
{
    if (val == 2 || val == 4 || val == 8) {
        return val;
    } else {
        return *(unsigned long*) get_register_value(val);
    }
}

void* get_operand_value(int type, unsigned char* str)
{
    // Gets the value represented by this, including (e.g.) dereferencing the memory address or getting a register's value
    unsigned long maddr;
    unsigned long a;
    unsigned long b;
    unsigned long c;
    switch (type) {
        case 0:
            //return NULL;
            return (void*) &env.memory[0];
        case 1: // A register
            // Take one byte
            return get_register_value(str[0]);
        case 2: // 1-byte immediate
            return (void*) current.str;
        case 3: // 2-byte immediate
            return (void*) current.str;
        case 4: //4-byte immediate
            return (void*) current.str;
        case 5: // Memory address
            maddr = 0;
            maddr += current.str[0] << 24;
            maddr += current.str[1] << 16;
            maddr += current.str[2] << 8;
            maddr += current.str[3];
            // Copy 4 bytes from memory into a separate variable
            return (void*) &env.memory[maddr];
        case 6: // "a"-form arithmetic
            a = interpret_arithmetic_item(str[0]);
            return (void*) &env.memory[a];
        case 7: // "a*b"-form arithmetic
            a = interpret_arithmetic_item(str[0]);
            b = interpret_arithmetic_item(str[1]);
            return (void*) &env.memory[a*b];
        case 8: // "a+b"-form arithmetic
            a = interpret_arithmetic_item(str[0]);
            b = interpret_arithmetic_item(str[1]);
            return (void*) &env.memory[a+b];
        case 9: // "a*b+c"-style arithmetic
            a = interpret_arithmetic_item(str[0]);
            b = interpret_arithmetic_item(str[1]);
            c = interpret_arithmetic_item(str[2]);
            return (void*) &env.memory[(a*b)+c];
        case 10: // "a+b*c"-form arithmetic
            a = interpret_arithmetic_item(str[0]);
            b = interpret_arithmetic_item(str[1]);
            c = interpret_arithmetic_item(str[2]);
            return (void*) &env.memory[a+(b*c)];
        default:
            printf("Unknown type in get_operand_value: 0x%x", type);
            break;
    }
}

void execute(unsigned char opcode,
             int op1_type, int op1_len, unsigned char* op1_str,
             int op2_type, int op2_len, unsigned char* op2_str)
{
    // Actually executes the command
//    switch (opcode) {
//        case CMP_char:
//            break;
//        default:
//            printf("Unknown opode: %i", opcode);
//            return;
//    }

    printf("Opcode: 0x%x, op1_type=%i, op2_type=%i\n", opcode, op1_type, op2_type);
    current.str = op1_str;
    printf("\tOperand 1: 1B=0x%x, 2B=0x%x, 4B=0x%x\n", *(unsigned char*) get_operand_value(op1_type, op1_str),
           *(uint16_t*) get_operand_value(op1_type, op1_str),
           *(unsigned int*) get_operand_value(op1_type, op1_str));
    current.str = op2_str;
    printf("\tOperand 2: 1B=0x%x, 2B=0x%x, 4B=0x%x\n", *(unsigned char*) get_operand_value(op2_type, op2_str),
           *(uint16_t*) get_operand_value(op2_type, op2_str),
           *(unsigned int*) get_operand_value(op2_type, op2_str));
}

void run_loop()
{
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
//        char dtype = 'u';
        int op1_type, op2_type;
//        if (opcode == CMP_char |
//                opcode == ADD_char | opcode == SUB_char |
//                opcode == MUL_char | opcode == IDIV_char |
//                opcode == MOD_char | opcode == EDIV_char) {
//            dtype = 'b';
//        } else if (opcode == CMP_uchar |
//                opcode == ADD_uchar | opcode == SUB_uchar |
//                opcode == MUL_uchar | opcode == IDIV_uchar |
//                opcode == MOD_uchar | opcode == EDIV_uchar) {
//            dtype = 'B';
//        } else if (opcode == CMP_short |
//                opcode == ADD_short | opcode == SUB_short |
//                opcode == MUL_short | opcode == IDIV_short |
//                opcode == MOD_short | opcode == EDIV_short) {
//            dtype = 'h';
//        } else if (opcode == CMP_ushort |
//                opcode == ADD_ushort | opcode == SUB_ushort |
//                opcode == MUL_ushort | opcode == IDIV_ushort |
//                opcode == MOD_ushort | opcode == EDIV_ushort) {
//            dtype = 'H';
//        } else if (opcode == CMP_int |
//                opcode == ADD_int | opcode == SUB_int |
//                opcode == MUL_int | opcode == IDIV_int |
//                opcode == MOD_int | opcode == EDIV_int) {
//            dtype = 'i';
//        } else if (opcode == CMP_uint |
//                opcode == ADD_uint | opcode == SUB_uint |
//                opcode == MUL_uint | opcode == IDIV_uint |
//                opcode == MOD_uint | opcode == EDIV_uint) {
//            dtype = 'I';
//        } else if (opcode == CMP_float |
//                opcode == ADD_float | opcode == SUB_float |
//                opcode == MUL_float | opcode == IDIV_float |
//                opcode == MOD_float | opcode == EDIV_float) {
//            dtype = 'f';
//        } else if ((opcode & 0b11110000) == 80 |
//                (opcode & 0b11110000) == 96) {
//            // One of the size-based commands
//            if ((opcode & 0b00001111) == 0) {
//                dtype = '1';
//            } else if ((opcode & 0b00001111) == 1) {
//                dtype = '2';
//            } else if ((opcode & 0b00001111) == 2) {
//                dtype = '4';
//            }
//        } else if (opcode == MOV_1B) {
//            dtype = '1';
//        } else if (opcode == MOV_2B) {
//            dtype = '2';
//        } else if (opcode == MOV_4B) {
//            dtype = '4';
//        } else if ((opcode > JMP) & (opcode < JGE)) {
//            // One of the jump commands
//            dtype = 'n';
//        } else if (opcode == LEA) {
//            dtype = 'n';
//        }

        // Interpret the operand type
        op1_type = (env.memory[env.pc+1] & 0b11110000) >> 4;
        op2_type = env.memory[env.pc+1] & 0b00001111;

        if (get_op_len(op1_type) == -1 | get_op_len(op2_type) == -1) {
            // One of them is invalid
            printf("Invalid operand byte 0x%x at PC %lu", env.memory[env.pc+1], env.pc+1);
        }

        // Pull the first operand as a char[]
        int op1_len = get_op_len(op1_type);
        int op2_len = get_op_len(op2_type);
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
        execute(opcode, op1_type, op1_len, op1_str, op2_type, op2_len, op2_str);
    }
}

void run(unsigned char* bytecode, int iflag, int length)
{
    printf("Executing run()\n");

    int bi;
    for (bi = 0; bi < length; bi++) {
        printf("%x ", bytecode[bi]);
    }

    // Begin the process of running the bytecode
    // Start by loading all of the configuration data
    int config_length = process_config(bytecode, length) + 4;

    // Initialise the environment
    const int memsize = config.memorykb * 1024;
    env.memory = (unsigned char*) malloc(memsize*sizeof(unsigned char));

    // Fill up the memory with the instructions
    for (int i = config_length; i < length; i++) {
        env.memory[i - config_length] = bytecode[i];
        printf("0x%x: %x\n", i - config_length, env.memory[i - config_length]);
    }


    // Start running instructions
    run_loop();
}

int main(int argc, char** argv)
{
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

    run(content, iflag, filelen);


    return 0;
}

/*
 * Error codes:
 * 0 - Completed fine
 * 1 - Could not find file
 */