// This program does the actual interpretation
// When the GUI wants to use it, the GUI program will call this with the -i/--instructions command line argument
// This will cause commands to be printed that can be interpreted by the GUI program
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <mem.h>

#define MAX_META_SECTION_LENGTH 100

// GLOBAL STRUCTURES

struct Config {
    int memorykb;
} config;



// FUNCTIONS

void process_config(const char* bytecode, int length)
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


}

void run(const char* bytecode, int iflag, int length)
{
    printf("Executing run()\n");

    // Begin the process of running the bytecode
    // Start by loading all of the configuration data
    process_config(bytecode, length);

    // Initialise the environment
}

int main(int argc, char** argv)
{
    // Process command line arguments
    int iflag = 0;
    char fname[100];
    int has_fname = 0;

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
    char* content;
    long filelen;

    fileptr = fopen(fname, "rb");

    if (fileptr == NULL) {
        printf("Cannot find file %s\n", fname);
        return 1;
    }

    fseek(fileptr, 0, SEEK_END);
    filelen = ftell(fileptr);
    rewind(fileptr);


    content = (char*)malloc(filelen*sizeof(char));
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