* Made main function
* Worked out how to process command line arguments
* Did file reading (research: https://stackoverflow.com/questions/22059189/read-a-file-as-byte-array)
* Went back and changed from throwing an error if no file is given to asking for a file
* Made run function
* Find config string
* Moved config string stuff into processConfig
* Looked up strtok and wrote processing of config info (http://www.tutorialspoint.com/ansi_c/c_strtok.htm)
* Looked up how to interpret an int from the value string (http://en.cppreference.com/w/c/string/byte/strtol)
* Looked up how to compare strings (https://www.tutorialspoint.com/c_standard_library/c_function_strcmp.htm)
* Wrote code to add config info into struct
* Initialise the environment; had some trouble with how to create a char array correctly
* Wrote run_loop function (with a large if statement). Programmed a while 1 loop and get it to process opcode
  and operand byte, but not yet get the operands or move along (this was an oversight)
* Found that the memory was not correctly made. Old code was:

```
int passed_delim = 0;
    int num_nulls = 0;
    int length_pre_instr = 0;
    for (int i = 0; i < length; i++) {
        if (passed_delim == 0) {
            if (bytecode[i] == 0) {
                num_nulls++;
            }

            if (num_nulls == 4) {
                passed_delim = 1;
                length_pre_instr = i;
            }
        } else {
            env.memory[i - length_pre_instr] = bytecode[i];
        }
    }
```

* Fiddled around and used debugger, print hex version of bytecode
* Replaced code with:

```
for (int i = config_length; i < length - config_length; i++) {
        env.memory[i - config_length] = bytecode[i];
    }
```

* That seemed to work, but runLoop stayed on char 1 and didn't iterate
* Added ability to consume operands into a char[]; created getOpLen
* Wrote code and wrote text to output data about each opcode and operand (printing raw operand hex)
* Got a very strange error where everything printed fine until memory address 51, when suddenly it broke and contained
  bytes not present in the original bytecode
* After much messing around and using debugger, determined it was probably the memory not being transferred right
* Changed memory from being a new array to being based on malloc
* Added memory dump to the loop and saw that the memory stops at address 49, meaning not all of the bytecode is placed
  into the memory
* Changed i<length-config_length to i<length, because this was cutting it off short
* This now works
* Wrote instructions.h and used its definitions to make the giant wall of opcodes easier to read
* Remembered to add the comparison registers
* Created the execute function and commands.c
* Created header files for main.c and commands.c
* Did some research (https://stackoverflow.com/questions/16522341/pseudo-generics-in-c) and decided to use a macro to simplify commands.c
* Moved definition of Environment to main.h to allow it to be visible in commands.c
* Added CMP and JMP instructions to commands.c
* Started writing execute() and made get_operand_value()
* Realised that the wall of if statements finding the data type is probably unnecessary. Commented it out/deprecated.
* Made getRegisterValue
* Changed some of the registers to be unions
* Completed get_operand_value (with help froma function to interpret arithmetic)
* Research done on void pointers
* Wrote some testing statements in execute()
* As expected, it didn't work. Crashing issue turned out to be dereferencing NULL pointers ಠ_ರೃ
* Fixed this, but difficult to say at present if getOperandValue really works. It seems to.
* Got rid of the testing statements (causing the crashing). Made a switch statement.
* Wrote CMP instructions in switch statement
* Wrote jump instructions using preprocessor macro
* Began to write command for MOV and made set_register_name; needed to work out how to only transfer a number of bytes
  (i.e. to pad with zeroes if the size of the movement and size of the register do not match)
* Worked that out and implemented the switch statements in main.c. Realised I might have to add code to the assembler
  to check that the right type of operands are paired with commands (e.g. can't move data into an immediate value)
* In preparation for making LEA function, moved interpreting arithmetic expressions into a function to separate
  calculating the address from finding the value there.
* Changed things in the execute() switch statement to make MOV-ing stuff to an arithmetic operand work
* Implemented the LEA function in the switch statement and commands.c
* Renamed functions to use camelCase
* Wrote ADD function for char then turned it into a macro
* Abandoned macro idea because it produced a million errors
* Implemented logging library (https://github.com/rxi/log.c) and made memdump function
* Added logging statements around the program, including replacing existing print statements
* Wrote one function to replace all of the other arithmetic ones
* Stopped commenting out the data type code in runLoop() and now pass the result to execute()
* Implemented arithmetic in the execute() switch statement (though to avoid repetitiveness it is all placed in default)