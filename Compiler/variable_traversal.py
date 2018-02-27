"""
The stage that manages local variable management and type checking.
"""

from collections import namedtuple
import logging

from pycparser.c_ast import Compound, Decl, PtrDecl, TypeDecl, If, For, While, \
                            FuncCall, ID, Assignment, Constant, UnaryOp, BinaryOp, \
                            Cast, ArrayDecl, ArrayRef

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


def variable_search(statement):
    """
    A recursive generator - Quite an elegant but distinctly pythonic construct.
    This will check to see if this node is an ID. If so, yield the statement back up and return out. If not, run
    this function on the sub-parts of this node (like the args of a FuncCall or the ""lvalue" and "rvalue" of an
    Assignment.
    :param statement:
    :return:
    """

    if isinstance(statement, ID):
        # Yield back this thing already
        yield statement
        return

    if isinstance(statement, FuncCall):
        for arg in statement.args.exprs:
            yield from variable_search(arg)
    elif isinstance(statement, Assignment):
        yield from variable_search(statement.lvalue)
        yield from variable_search(statement.rvalue)
    elif isinstance(statement, Constant):
        # if it is a constant it is neither a variable or a thing with sub-parts, so just end.
        return
    elif isinstance(statement, UnaryOp):
        yield from variable_search(statement.expr)
    elif isinstance(statement, BinaryOp):
        yield from variable_search(statement.left)
        yield from variable_search(statement.right)
    elif isinstance(statement, Cast):
        yield from variable_search(statement.expr)
    elif isinstance(statement, ArrayDecl):
        # An array declaration could have a variable length given
        yield from variable_search(statement.dim)
    elif isinstance(statement, ArrayRef):
        # Check in the subscript of an array
        yield from variable_search(statement.subscript)
    else:
        # If here then assume this is a non-ID, childless thing.
        return


def climbing_variable_search(varname, block_list):
    """
    From back to front, look through the list of blocks for a declaration of the variable name. Return its entry.
    :param block_list:
    :return:
    """
    for block in reversed(block_list):
        for var_decl in block.locals:
            if var_decl[0] == varname:
                return var_decl


def parse_compound(block: Compound, parents: list, globals: list):
    """
    Parses through the block, registering any local variables it finds and running itself recursively on any sub-blocks.
    :param block:
    :param parents:
    :param globals:
    :return:
    """

    if len(parents) == 0:
        block.parent = None
    else:
        block.parent = parents[-1]

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

        # Recursively call the sub-blocks of compound types
        elif isinstance(statement, If):
            parse_compound(statement.iftrue, parents + [block], globals)
            parse_compound(statement.iffalse, parents + [block], globals)
        elif isinstance(statement, (For, While)):
            parse_compound(statement.stmt, parents + [block], globals)

        # Other types of statement to analyse for references to variables
        else:
            for var in variable_search(statement):
                decl = climbing_variable_search(var, parents + [block])
                if decl is None:
                    # Maybe try the globals?
                    for var_decl in globals:
                        if var_decl[0] == var:
                            decl = var_decl
                # Still None? it wasn't found.
                if decl is None:
                    logging.error("Variable not found: {}".format(var))

