import io
import logging

from pycparser.c_ast import *

### CLASSES

class CodeBlock:
    def __init__(self):
        self.instructions = []
        self.locals = []
        self.child_blocks = []

    def generate_code(self, block_name):
        # The assembly is written to here. More efficient than remaking strings.
        assembly = io.StringIO()
        # If the block has variables, this stores info about how far from
        # the base pointer the variables are (as in, this index and the next
        # few bytes are the number) and their type
        stack_block_info = None

        if len(self.locals) > 0:
            stack_block_info, init_code = self.generate_init()
            assembly.write(init_code)

        for instr in self.instructions:
            assembly.write(instr.generate_code(stack_block_info))

        if len(self.locals) > 0:
            assembly.write(self.generate_return())

        assembly.seek(0)
        return assembly.read()

    def generate_init(self) -> (dict, str):
        """
        Generates the initialisation code that will handle the stack
        :return:
        """
        # 1. Push old base pointer to stack
        # 2. Change base pointer to one below the current stack pointer
        # 3. Write the variables to the stack and record where they are

        variable_data = {}

        init_code = io.StringIO()
        init_code.write("SUB uint esp 4\n")
        init_code.write("MOV 4B [esp] ebp\n")
        init_code.write("ADD uint esp 3\n") # esp is now 1 below where it was
        init_code.write("MOV 4B ebp esp\n") # Set base pointer to where it should be

        # This handles how far to the left of the base pointer the stack pointer is
        virtual_esp = 0

        # Handle the variables
        for name, type_, initial in self.locals:
            if isinstance(initial, Constant):
                if initial.type in ("int", "float"):
                    formatted_initial = initial.value
                else:
                    logging.error("As-yet unsupported constant type {init.type} for variable {var}".format(init=initial,
                                                                                                           var=name))
                    continue
            else:
                # If it is not a constant then it will be handled later
                # TODO Write the code to initialise the variables which don't just have a simple initial constant
                formatted_initial = 0

            if type_ in ("char", "uchar"):   # 1B
                virtual_esp += 1
                init_code.write("SUB uint esp 1\n")
                init_code.write("MOV 1B [esp] {initial}\n".format(initial=formatted_initial))
            elif type_ in ("short", "ushort"): # 2B
                virtual_esp += 2
                init_code.write("SUB uint esp 2\n")
                init_code.write("MOV 2B [esp] {initial}\n".format(initial=formatted_initial))
            elif type_ in ("int", "uint", "float"):
                virtual_esp += 4
                init_code.write("SUB uint esp 4\n")
                init_code.write("MOV 4B [esp] {initial}\n".format(initial=formatted_initial))
            else:
                logging.error("Unknown type {type} for variable {name}".format(type=type_, name=name))

            variable_data[name] = (virtual_esp, type_)

        init_code.seek(0)
        return variable_data, init_code.read()


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

class InstrPushValue(Instruction):
    def __init__(self, value):
        super().__init__()
        self._value = value

class InstrEvaluateUnary(Instruction):
    def __init__(self, operation):
        super().__init__()
        self._op = operation

class InstrEvaluateBinary(Instruction):
    def __init__(self, operation):
        super().__init__()
        self._op = operation



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
    # For post-order traversal, first run down the left hand side, then the right, then the root
    instructions = []

    if isinstance(expr, (Constant, ID)):
        instructions.append(InstrPushValue(expr))
    elif isinstance(expr, UnaryOp):
        instructions.extend(expression_instructions(expr.expr))
        instructions.append(InstrEvaluateUnary(expr.op))
    elif isinstance(expr, BinaryOp):
        instructions.extend(expression_instructions(expr.left))
        instructions.extend(expression_instructions(expr.right))
        instructions.append(InstrEvaluateBinary(expr.op))

    return instructions