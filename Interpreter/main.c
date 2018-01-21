// This program does the actual interpretation
// When the GUI wants to use it, the GUI program will call this with the -i/--instructions command line argument
// This will cause commands to be printed that can be interpreted by the GUI program
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <mem.h>

#include "instructions.h"

#define MAX_META_SECTION_LENGTH 100
#define MAX_CONFIG_KEY_LENGTH 10

// GLOBAL STRUCTURES

// Stores the config information
struct {
    int memorykb;
} config;


// Contains everything about the environment; registers, etc.
struct {
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
    //unsigned long mar;
    //unsigned long mdr;
    //unsigned long cir; // This won't actually be used for these

    // The memory
    unsigned char* memory;
} env;



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

void run_loop()
{
    // Executes the instructions
    while (1)
    {
        // Fetch
        char opcode = env.memory[env.pc];

        // If a HLT instruction, exit the loop
        if (opcode == 0) {
            break;
        }

        // Decode
        char dtype = 'u';
        int op1_type, op2_type;
        if (opcode == 0x01 |
                opcode == 0x20 | opcode == 0x28 |
                opcode == 0x30 | opcode == 0x38 |
                opcode == 0x40 | opcode == 0x48) {
            dtype = 'b';
        } else if (opcode == 0x02 |
                opcode == 0x21 | opcode == 0x29 |
                opcode == 0x31 | opcode == 0x39 |
                opcode == 0x41 | opcode == 0x49) {
            dtype = 'B';
        } else if (opcode == 0x03 |
                opcode == 0x22 | opcode == 0x2A |
                opcode == 0x32 | opcode == 0x3A |
                opcode == 0x42 | opcode == 0x4A) {
            dtype = 'h';
        } else if (opcode == 0x04 |
                opcode == 0x23 | opcode == 0x2B |
                opcode == 0x33 | opcode == 0x3B |
                opcode == 0x43 | opcode == 0x4B) {
            dtype = 'H';
        } else if (opcode == 0x05 |
                opcode == 0x24 | opcode == 0x2C |
                opcode == 0x34 | opcode == 0x3C) {
            dtype = 'i';
        } else if (opcode == 0x06 |
                opcode == 0x25 | opcode == 0x2D |
                opcode == 0x36 | opcode == 0x3D |
                opcode == 0x46 | opcode == 0x4D) {
            dtype = 'I';
        } else if (opcode == 0x07 |
                opcode == 0x26 | opcode == 0x2E |
                opcode == 0x36 | opcode == 0x3D |
                opcode == 0x46 | opcode == 0x4D) {
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
        } else if (opcode == 0x10) {
            dtype = '1';
        } else if (opcode == 0x11) {
            dtype = '2';
        } else if (opcode == 0x12) {
            dtype = '4';
        } else if ((opcode > 0x08) & (opcode < 0x0E)) {
            dtype = 'n';
        } else if (opcode == 0x14) {
            dtype = 'n';
        }

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

        // Print out info
        printf("opcode=0x%x, dtype=%c, op1_type=%i, op2_type=%i\n", opcode, dtype, op1_type, op2_type);
        printf("\tOperand 1: ");
        for (int i = 0; i < op1_len; i++) {
            printf("%x ", op1_str[i]);
        }
        printf("\n\tOperand 2: ");
        for (int i = 0; i < op2_len; i++) {
            printf("%x ", op2_str[i]);
        }
        printf("\n");
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