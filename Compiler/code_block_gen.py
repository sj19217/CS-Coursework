from pycparser.c_ast import *

### CLASSES

class CodeBlock:
    def __init__(self):
        self.instructions = []
        self.locals = []
        self.child_blocks = []


class Instruction:
    def __init__(self):
        pass


class InstrFuncCall(Instruction):
    def __init__(self, name):
        super().__init__()
        self.func_name = name


class InstrVariableAssignment(Instruction):
    def __init__(self, lvalue: ID):
        super().__init__()
        self.lvalue = lvalue
        self.var_name = lvalue.name


class InstrArrayAssignment(Instruction):
    def __init__(self, lvalue: ArrayRef):
        super().__init__()
        self.lvalue = lvalue
        self.var_name = lvalue.name


class InstrForLoop(Instruction):
    def __init__(self, stmt: For):
        super().__init__()
        self._stmt = stmt
        self.decls = stmt.init.decls
        # TODO Finish this and the next two classes


class InstrWhileLoop(Instruction):
    def __init__(self, stmt):
        super().__init__()
        self._stmt = stmt


class InstrIfStmt(Instruction):
    def __init__(self, stmt):
        super().__init__()
        self._stmt = stmt



### FUNCTIONS

def generate_code_block(compound: Compound) -> CodeBlock:
    # Create the code block everything will be added to
    code_block = CodeBlock()

    # Assign locals
    code_block.locals = compound.locals

    # Loop through the statements
    for stmt in compound.block_items:
        # Get the instructions from the block
        instrs = get_stmt_instructions(stmt)
        code_block.instructions.extend(instrs)

        # See if it contains sub-blocks
        if isinstance(stmt, If):
            code_block.child_blocks.append(generate_code_block(stmt.iftrue))
            code_block.child_blocks.append(generate_code_block(stmt.iffalse))
        elif isinstance(stmt, (For, While)):
            code_block.child_blocks.append(generate_code_block(stmt.stmt))

    return code_block


def get_stmt_instructions(stmt) -> list:
    """
    Takes a statement that is a node in the tree and turns it into a list of instructions.
    :param stmt:
    :return:
    """
    instr_list = []

    if isinstance(stmt, FuncCall):
        # Sort out the evaluation of the arguments
        for arg in stmt.args:
            instr_list.append(expression_instructions(arg))
        # Now do the actual FuncCall
        instr_list.append(InstrFuncCall(stmt.name))
    elif isinstance(stmt, Assignment):
        instr_list.append(expression_instructions(stmt.rvalue))
        if isinstance(stmt.lvalue, ID):
            instr_list.append(InstrVariableAssignment(stmt.lvalue))
        elif isinstance(stmt.lvalue, ArrayRef):
            instr_list.append(InstrArrayAssignment(stmt.lvalue))
    elif isinstance(stmt, For):
        instr_list.append(InstrForLoop(stmt))
    elif isinstance(stmt, While):
        instr_list.append(InstrWhileLoop(stmt))
    elif isinstance(stmt, If):
        instr_list.append(InstrIfStmt(stmt))

    return instr_list


def expression_instructions(expr):
    """
    Performs post-order traversal and returns a list of expression evaluation objects.
    Each expression evaluation object has the job of taking (a) value(s) from the stack and processing it,
    then pushing the result back on.

    The object given to this function will be one of these:
    * A UnaryOp - Means one value needs popping
    * A BinaryOp - Means two values need popping
    * A Constant - Nothing needs popping, but a value needs popping on
    * An ID - Nothing needs popping, but a variable needs getting and pushing on
    :param expr:
    :return:
    """
