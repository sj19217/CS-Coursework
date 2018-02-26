import sys
import logging
from argparse import ArgumentParser

from pycparser import CParser
from pycparser.c_ast import Compound, FuncDef

from preprocessor import process as preprocess
from global_parser import global_parser
from variable_traversal import parse_compound
from code_block_gen import generate_code_block
from assembly_writing import produce_data_section, produce_text_section

def lexing_error(msg, line, column):
    print("Error on line {}, column {}: {}", line, column, msg)

def main(text):
    logging.debug("Running main function")

    # STAGE 1 - PREPROCESSOR
    text = preprocess(text)
    logging.info("After preprocessing, text is: " + text)

    # STAGE 2 - LEXICAL AND SYNTAX ANALYSIS
    parser = CParser()
    tree = parser.parse(text)
    logging.info(tree)

    # STAGE 3 - GLOBAL VARIABLE TABLE
    global_symbols = global_parser(tree.ext)

    # STAGE 4 - Store local variables and perform type checking
    top_compound = None
    for top_level in tree.ext:
        if isinstance(top_level, FuncDef) and top_level.decl.name == "main":
            top_compound = top_level.body
            break
    if top_compound is None:
        logging.fatal("No main function found")
        return
    parse_compound(top_compound, [], global_symbols)

    # STAGE 5 - HIERARCHICAL INSTRUCTION GENERATION
    main_block = generate_code_block(top_compound)

    # STAGE 6 - ASSEMBLY GENERATION
    assembly = """section.meta
    mem_amt={mem_amt}
    
    section.data
    {data_section}
    
    section.text
    {text_section}
    """.format(mem_amt=4,
              data_section=produce_data_section(global_symbols),
               text_section=produce_text_section(main_block))

if __name__ == "__main__":
    argparser = ArgumentParser(description="Compile a C file into assembly")
    argparser.add_argument("file", help="Specify the C file to compile", default="")
    args = argparser.parse_args()

    if args.file != "":
        filename = args.file
    else:
        filename = input("Filename: ")

    with open(filename, "rt") as file:
        text = file.read()

    main(text)