// This program does the actual interpretation
// When the GUI wants to use it, the GUI program will call this with the -i/--instructions command line argument
// This will cause commands to be printed that can be interpreted by the GUI program
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>


int main(int argc, char** argv) {
    // Process command line arguments
    int iflag = 0;
    char* fname = NULL;

    int c;
    while ((c = getopt(argc, argv, "if:")) != -1)
    {
        switch (c){
            case 'i':
                iflag = 1;
                break;
            case 'f':
                fname = optarg;
            default:
                break;
        }
    }

    // Check that a filename was given
    if (fname == NULL) {
        printf("Must provide a filename");
    }

    // Read the file
    FILE* fileptr;
    char* buffer;
    long filelen;

    fileptr = fopen(fname, "rb");
    fseek(fileptr, 0, SEEK_END);
    filelen = ftell(fileptr);
    rewind(fileptr);

    buffer = (char*)malloc(filelen*sizeof(char));
    fread(buffer, (size_t) filelen, 1, fileptr);
    fclose(fileptr);



    return 0;
}