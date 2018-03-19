"""
Contains the functions for writing the data and text sections. Lots of the work, however, is done by the classes in
code_block_gen.py.
"""

import logging
import io
import collections

from pycparser.c_ast import FuncDecl, FuncDef, Decl, Constant, TypeDecl
from global_parser import GlobalVariable

def produce_data_section(global_symbols, interactive_mode=False):
    data_section = ""
    var_list = {}

    if interactive_mode:
        print("gen_data")

    for symbol in global_symbols:
        if not isinstance(symbol, GlobalVariable):
            continue

        initial = "0"
        if symbol.initial is not None:
            if isinstance(symbol.initial, Constant):
                if isinstance(symbol.initial.value, int):
                    initial = symbol.initial.value
                else:
                    logging.error("Only integer initial global values are allowed currently")
            else:
                logging.error("Only constants are allowed for initial global values")

        var_list[symbol.name] = (initial, symbol.type)

    for name, (value, type_) in var_list.items():
        data_section += "{name} VAR {type} {initial}\n".format(name=name, type=type_, initial=value)
        if interactive_mode:
            print("gen_data_line", json.dumps([
                name, type_, value
            ]))

    if interactive_mode:
        print("fin_gen_data", json.dumps(data_section))
    return data_section

def produce_text_section(top_block, global_symbols, interactive_mode=False):
    assembly = io.StringIO()
    # A queue containing (block name, block object)
    queue = collections.deque()

    if interactive_mode:
        print("gen_text")

    # Set up the stack and base pointer
    # TODO Make the heap size change
    assembly.write("MOV 4B esp 2048\n")
    assembly.write("MOV 4B ebp 2048\n")

    # Write the code blocks
    queue.append(("block", top_block))
    while len(queue) > 0:
        name, block = queue.popleft()
        assembly.write(block.generate_code(name, global_symbols, queue, interactive_mode))

        # The child blocks are added in generate_code

    # Add a HLT instruction to the end
    assembly.write("exit HLT\n")

    assembly.seek(0)
    return assembly.read()
