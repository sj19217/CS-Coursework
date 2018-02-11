import sys
import logging

from pycparser import CParser
from pycparser.c_ast import Compound

from preprocessor import process as preprocess
from global_parser import global_parser
from variable_traversal import parse_compound
from code_block_gen import generate_code_block

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
    top_compound = Compound(tree.ext)
    parse_compound(top_compound, [], global_symbols)

    # STAGE 5 - HIERARCHICAL INSTRUCTION GENERATION
    main_block = generate_code_block(top_compound)

    


if __name__ == "__main__":
    if len(sys.argv) == 2:
        filename = sys.argv[2]
    else:
        filename = input("Filename: ")

    with open(filename, "rt") as file:
        text = file.read()

    main(text)