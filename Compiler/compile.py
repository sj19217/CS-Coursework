import sys
import logging

from pycparser import CParser
from pycparser.c_lexer import CLexer

from preprocessor import process as preprocess

def lexing_error(msg, line, column):
    print("Error on line {}, column {}: {}", line, column, msg)

def main(text):
    logging.debug("Running main function")

    # STAGE 1 - PREPROCESSOR
    text = preprocess(text)
    logging.info("After preprocessing, text is: " + text)

    # STAGE 2 - LEXICAL AND SYNTAX ANALYSIS
    lexer = CLexer(lexing_error, lambda: None, lambda: None, lambda _: True)
    tokens = []
    while True:
        tok = lexer.token()
        if tok is None:
            break
        tokens.append(tok)


if __name__ == "__main__":
    if len(sys.argv) == 2:
        filename = sys.argv[2]
    else:
        filename = input("Filename: ")

    with open(filename, "rt") as file:
        text = file.read()

    main(text)