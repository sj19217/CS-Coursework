// Contains the declarations of the gui.c functions and the structs/enums that are used to control it.

#ifndef INTERPRETER_GUI_H
#define INTERPRETER_GUI_H

// Sent to the locking function to say what is just about to occur.
enum stage { s_start, s_fetch, s_decode_operands, s_exec_switch, s_exec_func };
// s_start - The next thing to happen will be the initialisation (right at the start)
// s_fetch - Taking the opcode from the memory
// s_decode_operands - Decoding and reading the operands
// s_exec_switch - In the switch statement, about to decode the opcode's meaning
// s_exec_func - In the exec_* function for this action, about to actually run it.

void pauseUntilPermitted(enum stage next_stage);
void handleCommand(char* inp);

#endif //INTERPRETER_GUI_H