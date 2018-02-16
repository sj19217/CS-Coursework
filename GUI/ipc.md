# Inter-process communication
This document describes the way that the GUI process will communicate with the individual processes. When run in GUI
form (since the individual programs can be run from the command line), the GUI program will spawn each sub-program as
necessary as a child process. It will then send instructions and requests for information through the child process's
stdin channel and listen to responses coming from its stdout. Anything coming from stderr will be printed into the GUI's
log file. 

## Basic format
The basic format of the communication is that each part is separated by a space. The first part will be the general
instruction, and the next will either be the arguments of the instruction. Content being returned may not be quite in
this form - the only solid rule is that the title of the communication is what happens before the first space. It could
be followed by JSON or some other format.

## Communication with the compiler
The following instructions can be given to the compiler via stdin:
* 

These are the types of info that can be given back from the compiler to the GUI.
* 

## Communication with the assembler

## Communication with the interpreter
The interpreter will be started giving it the binary file through the -f command line argument, and the -i argument will
also be given, which tells the interpreter to act interactively, responding to instructions and giving output.

The commands given to the interpreter are:
* step - Moves on one step. The steps are:
  * Get opcode
  * Get operand byte
  * Decode operand byte
  * Read operand 1 data
  * Read operand 2 data
  * Switch statement entry
  * Execution function

The types of data that can return are:
* start - The initial setup procedures, as far as just about to get the first opcode
* fetch <pc> <opcode> <opbyte> <operand1> <operand2> - Got the opcode and operands from memory
* decode <chosen> - The switch statement has decoded what to interpret the opcode as
* exec_func <funcname> <args> - The relevant exec function has been run. It can be any of these:
  * cmp <val1> <val2> <p|n|e> - Ran a comparison function. For commentary, gives the values (decoded) and the register
    that was set to 1 as a result.
  * jmp <always|e|ne|lt|le|gt|ge> <addr> <val1> <val2> <true|false> - Any of the jump functions. Gives which jump
    instruction, along with the memory address to jump to, the decoded values and whether the jump was made.
  * mov_reg <regname> <size> <srctype> <srcdata> <srccontent> - Moved data to a register. `srctype` is "maddr",  "reg" or
    "immediate". If it is "maddr" then `srcdata` is the memory address, if it is `reg` then it is a register name or if
    it is "immediate" then it is the immediate value. `srccontent` is the value extracted, as decimal bytes. 
  * mov_mem <destaddr> <size> <srctype> <srcdata> <srccontent> - Moved data to a memory address. The other arguments are
    the same as above.
  * lea_reg <regname> <addr> - Moved the given data (`addr`) to the register of the specified name. 
  * lea_mem <destaddr> <addr> - Moved the given data to the given memory location.
  * arithmetic <opname> <type> <desttype> <dest> <op1> <op2> <res> - Performed a piece of arithmetic. `opname` is the
    name of the operation (e.g. "ADD" or "IDIV"). `type` is the type of the data, like `char`. `desttype` is either
    "maddr" or "reg", and then `dest` is either the register name or memory address depending on which `desttype` is
    given. Then `op1` and `op2` are the decoded values of the operands and `res` is the result.
* config <config_json> - Gives the content of the config dict in a JSON form
* data <content> - Given frequently by the interpreter to give status information. `content` is a JSON string,
  with these attributes:
  * pc - The current value of the program counter
  * genregs - A nested JSON object with all of the registers ("eax", "ebx", etc)
  * cmp - A JSON object containing the values "e", "n" and "p", corresponding to those registers
  * memory - A JSON array of the full memory
  Naturally, this will be quite large. 