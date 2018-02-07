import logging
from collections import namedtuple

from pycparser.c_ast import Decl

GlobalVariable = namedtuple("GlobalVariable", "name type")

def global_parser(tree):
    table = []

    for top_node in tree:
        if isinstance(top_node, Decl):
            name = top_node.name
            type_list = top_node.type.type.names

            if "char" in type_list:
                str_type = "char"
            elif "short" in type_list:
                str_type = "short"
            elif "int" in type_list:
                str_type = "int"
            else:
                logging.error("Type is unknown ({0}) for variable {1}".format(type_list, name))
                return

            if "unsigned" in type_list:
                str_type = "u" + str_type

            logging.info("Found global variable {name} of type {type}".format(name=name, type=str_type))

            table.append(GlobalVariable(name, str_type))