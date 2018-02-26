"""
Contains the functions for writing the data and text sections. Lots of the work, however, is done by the classes in
code_block_gen.py.
"""

import logging
import collections
import io

from pycparser.c_ast import FuncDecl, FuncDef, Decl, Constant, TypeDecl
from global_parser import GlobalVariable

def produce_data_section(global_symbols):
    data_section = ""
    var_list = {}

    for symbol in global_symbols:
        if not isinstance(symbol, GlobalVariable):
            continue

        initial = "0"
        if symbol.initial is not None:
            if isinstance(symbol.initial, Constant):
                if symbol.initial.type == "int":
                    initial = symbol.initial.value
                else:
                    logging.error("Only integer initial global values are allowed currently")
            else:
                logging.error("Only constants are allowed for initial global values")

        var_list[symbol.name] = (initial, symbol.type)

    for name, (value, type_) in var_list.items():
        data_section += "{name} VAR {type} {initial}\n".format(name=name, type=type_, initial=value)

    return data_section

def produce_text_section(top_block):
    code = io.StringIO()

    # A queue of (name, block)
    block_queue = collections.deque([("block", top_block)])
    while len(block_queue):
        name, block = block_queue.popleft()
        code.write(block.generate_code(name, block_queue))