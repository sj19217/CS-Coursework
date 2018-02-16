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
* done_step <name> <other_args> - Tells the GUI that a step has been performed. The name, and corresponding other
  arguments, are these:
  * start - The initial setup procedures, as far as just about to get the first opcode
  * fetch <pc> <opcode> - Got the opcode from memory
  * fetch_opbyte <opbyte> - Got the given operand byte from memory
  * fetch_op1 <bytes> - Got the first operand from memory (with `bytes` as decimals separated by spaces)
  * fetch_op2 <bytes> - Got the second operand from memory
  * decode <chosen> - The switch statement has decoded what to interpret the opcode as
  * exec_func - The relevant exec function has been run
 
* data <content> - Given frequently by the interpreter to give status information. `content` is a JSON string,
  with these attricutes:
  * pc - The current value of the program counter
  * genregs - A nested JSON object with all of the registers ("eax", "ebx", etc)
  * cmp - A JSON object containing the values "e", "n" and "p", corresponding to those registers
  * memory - A JSON array of the full memory
  Naturally, this will be quite large. 