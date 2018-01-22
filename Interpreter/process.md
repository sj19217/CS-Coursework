* Made main function
* Worked out how to process command line arguments
* Did file reading (research: https://stackoverflow.com/questions/22059189/read-a-file-as-byte-array)
* Went back and changed from throwing an error if no file is given to asking for a file
* Made run function
* Find config string
* Moved config string stuff into process_config
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

* That seemed to work, but run_loop stayed on char 1 and didn't iterate
* Added ability to consume operands into a char[]; created get_op_len
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