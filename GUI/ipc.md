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