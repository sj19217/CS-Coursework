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
  * Next depends on what type of operation this is
* get_mem <amt> - Instructs the interpreter to return a dump of its memory up to the given address

The types of data that can return are:
* done_step <name> <other_args> - Tells the GUI that a step has been performed. The name, and corresponding other
  arguments, are these:
  * get_opcode - The opcode has been read from memory. Gives the number as an argument.
  * get_opbyte - The operand byte has been read from memory. Gives the number as an argument.
  * dec_opbyte - The operand byte has been decoded. Gives two arguments, the numerical type of the two operands.
  * read_op - Read than operand byte from memory. The first argument is "1" or "2", saying which operand it is.  Number
    of other arguments depends on the size, with the bytes of it given as hex.
  * in_switch - Jumped to the position in the execute() switch statement. The argument is the instruction name.
* mem <val>* - Gives the contents of the memory in decimal separated by spaces