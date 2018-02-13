/*
  Used to handle everything to do with communicating with the GUI. The most important function here is.
  pauseUntilPermitted(), which will be passed an enum instance declaring where in the program it is. The function
  will then either continue, if permitted, or wait until input that allows it (i.e. a command to skip).
*/

#include <stdio.h>

#include "headers/util.h"
#include "headers/gui.h"
#include "headers/log.h"

enum stage next_pause = s_start;

void pauseUntilPermitted(enum stage next_stage)
{
    //log_trace("pauseUntilPermitted(next_stage=%s)", next_stage);
    if (next_pause != next_stage) {
        return;
    }

    char inp[50];

    for (;;) {
        // Keep listening until there is an input
        fgets(inp, 50, stdin);

        handleCommand(inp);

        if (next_pause != next_stage) {
            break;
        }
    }
}

void handleCommand(char* inp)
{
    log_trace("handleCommand(inp=%s)", inp);
    if (startsWith(inp, "step"))
    {
        switch (next_pause) {
            case s_start:
                next_pause = s_fetch;
                break;
            case s_fetch:
                next_pause = s_decode_operands;
                break;
            case s_decode_operands:
                next_pause = s_exec_switch;
                break;
            case s_exec_switch:
                next_pause = s_exec_func;
                break;
            case s_exec_func:
                next_pause = s_fetch;
                break;
        }
    } else if (startsWith(inp, "get_mem")) {
        // Scan the amount of memory to print
        int max_addr;
        sscanf(inp, "get_mem %d", &max_addr);

        linearMemdump(max_addr);
    }
}