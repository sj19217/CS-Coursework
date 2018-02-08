"""
The stage that manages local variable management and type checking.
"""

from collections import namedtuple
import logging

from pycparser.c_ast import Compound, Decl, PtrDecl, TypeDecl

LocalVariable = namedtuple("LocalVariable", "name type initial")


class UnknownTypeException(Exception):
    def __init__(self, type_list, *args):
        super().__init__(*args)
        self.type_list = type_list


def get_type_name(typedecl: TypeDecl):
    """
    Returns the name of the type, in the terms used by the assembler. For example, "uchar".
    :param typedecl:
    :return:
    """

    type_name_list = typedecl.type.names
    if "char" in type_name_list:
        type_name = "char"
    elif "short" in type_name_list:
        type_name = "short"
    elif "int" in type_name_list:
        type_name = "int"
    elif "float" in type_name_list:
        type_name = "float"
    elif "void" in type_name_list:
        type_name = "void"
    else:
        raise UnknownTypeException(type_name_list)

    if "unsigned" in type_name_list:
        type_name = "u" + type_name

    return type_name


def parse_compound(block: Compound):
    """
    Parses through the block, registering any local variables it finds and running itself recursively on any sub-blocks.
    :param block:
    :return:
    """

    for statement in block.block_items:
        if isinstance(statement, Decl):
            # A Decl statement, so a local variable needs to be added to the block.

            # Get the variable's name
            name = statement.name

            # The type declaration, either in statement.type or, if a pointer, statement.type.type.
            if isinstance(statement.type, PtrDecl):
                typedecl = statement.type.type
            else:
                typedecl = statement.type

            # Get the variable's type - more complicated
            try:
                type_name = get_type_name(typedecl)
            except UnknownTypeException as err:
                logging.error("Unknown type for variable {name}: {type}".format(name=name, type=err.type_list))
                continue

            if isinstance(statement.type, PtrDecl):
                type_name += "*"

            # The initial value
            initial = statement.init

            # Actually add it to the block
            block.locals.append(LocalVariable(name, type_name, initial))
        elif isinstance()