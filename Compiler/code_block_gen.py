import collections
import io
import logging

from pycparser.c_ast import *

import util
from global_parser import GlobalVariable

### CLASSES

class CodeBlock:
    def __init__(self):
        self.instructions = []
        self.locals = []
        self.child_blocks = []
        self.parent = None
        self.return_label = "exit"

    def get_stack_block_size(self):
        """Returns the number of bytes this block will take on the stack"""
        size = 4    # BP reference
        for local in self.locals:
            size += util.get_size_of_type(local.type)
        return size

    def get_local_var_data(self, name):
        """
        Returns a tuple of a LocalVariable object and an integer showing where its start is relative to the base pointer
        (positive being below, in this block, and negative being above, in a calling block).
        :param name:
        :return:
        """
        # Starts 4 below the base pointer because of the record of the old base pointer
        relative = 4
        for local in self.locals:
            relative += util.get_size_of_type(local.type)
            if local.name != name:
                continue
            # This is the variable
            return local, relative

        # If here then it wasn't a local variable
        # Now set to be the next base pointer down and set based on that
        if self.parent is None:
            # It wasn't a local anywhere up the spectrum
            return None, None
        relative = -self.parent.get_stack_block_size()
        var, rel_to_parent = self.parent.get_local_var_data(name)
        if rel_to_parent is None:
            # The parent has found it to not exist, so pass this down
            return None, None
        # It does exist
        relative += rel_to_parent

        return var, rel_to_parent

    def generate_code(self, block_name, global_symbols):
        # The assembly is written to here. More efficient than remaking strings.
        assembly = io.StringIO()
        # If the block has variables, this stores info about how far from
        # the base pointer the variables are (as in, this index and the next
        # few bytes are the number) and their type

        if len(self.locals) > 0:
            init_code = self.generate_init(block_name)
            assembly.write(init_code)

        # If ano locals need initialising to something complicated, create some Assignment expressions here
        for local in self.locals:
            if not isinstance(local.initial, (ID, Constant)):
                asg = Assignment("=", ID(name=local.name), local.initial)
                instructions = get_stmt_instructions(asg, self, global_symbols)
                for i, instr in enumerate(instructions):
                    self.instructions.insert(i, instr)

        for instr in self.instructions:
            assembly.write(instr.generate_code(self, global_symbols))

        if len(self.locals) > 0:
            assembly.write(self.generate_return())

        assembly.seek(0)
        return assembly.read()

    def generate_init(self, block_name) -> (dict, str):
        """
        Generates the initialisation code that will handle the stack
        :return:
        """
        # 1. Push old base pointer to stack
        # 2. Change base pointer to one below the current stack pointer
        # 3. Write the variables to the stack and record where they are

        #variable_data = {}

        init_code = io.StringIO()
        init_code.write("{block_name} SUB uint esp 4\n".format(block_name=block_name))
        init_code.write("MOV 4B [esp] ebp\n")
        init_code.write("ADD uint esp 4\n") # esp is now 1 below where it was
        init_code.write("MOV 4B ebp esp\n") # Set base pointer to where it should be
        init_code.write("SUB uint esp 4\n") # Set stack pointer to beginning of old base pointer record

        # This handles how far to the left of the base pointer the stack pointer is
        #virtual_esp = 4

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
                #virtual_esp += 1
                init_code.write("SUB uint esp 1\n")
                init_code.write("MOV 1B [esp] {initial}\n".format(initial=formatted_initial))
            elif type_ in ("short", "ushort"): # 2B
                #virtual_esp += 2
                init_code.write("SUB uint esp 2\n")
                init_code.write("MOV 2B [esp] {initial}\n".format(initial=formatted_initial))
            elif type_ in ("int", "uint", "float"):
                #virtual_esp += 4
                init_code.write("SUB uint esp 4\n")
                init_code.write("MOV 4B [esp] {initial}\n".format(initial=formatted_initial))
            else:
                logging.error("Unknown type {type} for variable {name}".format(type=type_, name=name))

            #variable_data[name] = (virtual_esp, type_)

        init_code.seek(0)
        return init_code.read()

    def generate_return(self):
        """
        The assembly this returns will move the base pointer back to its old place and return control to the parent.
        :return:
        """
        return """MOV 4B esi ebp
SUB uint esi 4
MOV 4B ebp [esi]
JMP {return_label}
""".format(return_label=self.return_label)


class Instruction:
    def __init__(self):
        pass

    def generate_code(self, block: CodeBlock, global_symbols):
        raise NotImplementedError()


class InstrFuncCall(Instruction):
    def __init__(self, name, arg_types):
        super().__init__()
        self.func_name = name
        self._arg_types = arg_types

    def generate_code(self, block, global_symbols):
        if isinstance(self.func_name, ID) and self.func_name.name == "printf":
            # Currently only supports the ridiculously simple task of printing an integer type
            size = util.get_size_of_type(self._arg_types[0])
            # First line is doing it, second line is popping the item (no need to overwrite)
            return "MOV {size}B out [esp]\nADD uint esp {size}\n".format(size=size)
        else:
            logging.error("Unsupported function: {}".format(self.func_name))


class InstrVariableAssignment(Instruction):
    def __init__(self, lvalue: ID, stack_top_type: str):
        super().__init__()
        self.lvalue = lvalue
        self.var_name = lvalue.name
        self._stack_top_type = stack_top_type


class InstrArrayAssignment(Instruction):
    def __init__(self, lvalue: ArrayRef, stack_top_type: str):
        super().__init__()
        self.lvalue = lvalue
        self.var_name = lvalue.name
        self._stack_top_type = stack_top_type


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
        """
        Represents the pushing of a value to the stack.
        :param value:
        :return:
        """
        super().__init__()
        self._value = value
        # The value can be either a Constant or an ID

    def generate_code(self, block: CodeBlock, global_symbols):
        # Move the stack pointer down by the right amount then write the data
#         if isinstance(self._value, Constant) and self._value.type == "int":
#             value = self._value.value
#             size = 4
#         elif isinstance(self._value, ID):
#             value, _ = block.get_local_var_data(self._value.name)
#             value = value.initial
#             size = self._value.memory_size(block)
#         else:
#             logging.error("Can only push a Constant or ID to the stack")
#             return
#         return """SUB uint esp {size}
# MOV {size}B [esp] {value}""".format(value=value, size=size)
        if isinstance(self._value, Constant) and self._value.type == "int":
            return "SUB uint esp 4\n" + \
                   "MOV 4B [esp] {value}\n".format(value=self._value.value)
        elif isinstance(self._value, ID):
            # A variable, so its location has to be found. We need to know its size.
            # This unpacks the (LocalVariable, int) tuple.
            var, rel_to_base = block.get_local_var_data(self._value.name)
            if var is None:
                # It could be a global variable
                for global_var in [x for x in global_symbols if isinstance(x, GlobalVariable)]:
                    if global_var.name == self._value.name:
                        size = util.get_size_of_type(global_var.type)
                        return "SUB uint exp {size}\n".format(size=size) + \
                               "MOV {size}B [esp] {name}\n".format(size=size, name=global_var.name)
            else:
                # It is a local variable
                _, type_, _ = var
                size = util.get_size_of_type(type_)
                code = """SUB uint esp {size}
MOV 4B esi ebp
""".format(size=size)
                if rel_to_base < 0:
                    code += "ADD uint esi {rel}\n".format(rel=0-rel_to_base)
                elif rel_to_base > 0:
                    code += "SUB uint esi {rel}\n".format(rel=rel_to_base)
                code += "MOV {size}B [esp] [esi]\n".format(size=size)
                return code
        else:
            logging.error("Can only push a Constant or ID, not {}".format(self._value))

class InstrEvaluateUnary(Instruction):
    def __init__(self, operation, type_):
        super().__init__()
        self._op = operation
        self._type = type_

class InstrEvaluateBinary(Instruction):
    def __init__(self, operation, ltype, rtype):
        super().__init__()
        self._op = operation
        self._ltype = ltype
        self._rtype = rtype



### FUNCTIONS

def generate_code_block(compound: Compound, global_symbols) -> CodeBlock:
    # Create the code block everything will be added to
    code_block = CodeBlock()

    # Assign locals and parent blocks
    code_block.parent = compound.parent
    code_block.locals = compound.locals

    # Loop through the statements
    for stmt in compound.block_items:
        # Get the instructions from the block
        instrs = get_stmt_instructions(stmt, code_block, global_symbols)
        code_block.instructions.extend(instrs)

        # See if it contains sub-blocks
        if isinstance(stmt, If):
            code_block.child_blocks.append(generate_code_block(stmt.iftrue, global_symbols))
            code_block.child_blocks.append(generate_code_block(stmt.iffalse, global_symbols))
        elif isinstance(stmt, (For, While)):
            code_block.child_blocks.append(generate_code_block(stmt.stmt, global_symbols))

    return code_block


def get_stmt_instructions(stmt, code_block, global_symbols) -> list:
    """
    Takes a statement that is a node in the tree and turns it into a list of instructions.
    :param stmt:
    :param code_block:
    :return:
    """
    instr_list = []

    if isinstance(stmt, FuncCall):
        # Sort out the evaluation of the arguments
        typelist = []
        for arg in stmt.args:
            instructions, type_ = expression_instructions(arg, code_block)
            instr_list.extend(instructions)
            typelist.append(type_)
        # Now do the actual FuncCall
        instr_list.append(InstrFuncCall(stmt.name, typelist))
    elif isinstance(stmt, Assignment):
        expr_instructions, type_ = expression_instructions(stmt.rvalue, code_block)
        instr_list.extend(expr_instructions)
        if isinstance(stmt.lvalue, ID):
            instr_list.append(InstrVariableAssignment(stmt.lvalue, type_))
        elif isinstance(stmt.lvalue, ArrayRef):
            instr_list.append(InstrArrayAssignment(stmt.lvalue, type_))
    elif isinstance(stmt, For):
        instr_list.append(InstrForLoop(stmt))
    elif isinstance(stmt, While):
        instr_list.append(InstrWhileLoop(stmt))
    elif isinstance(stmt, If):
        instr_list.append(InstrIfStmt(stmt))

    return instr_list


def expression_instructions(expr, code_block) -> (list, str):
    """
    Performs post-order traversal and returns a list of expression evaluation objects.
    Each expression evaluation object has the job of taking (a) value(s) from the stack and processing it,
    then pushing the result back on. This function returns (instruction list, type on top of stack)

    The object given to this function will be one of these:
    * A UnaryOp - Means one value needs popping
    * A BinaryOp - Means two values need popping
    * A Constant - Nothing needs popping, but a value needs pushing on
    * An ID - Nothing needs popping, but a variable needs getting and pushing on
    :param expr:
    :param code_block:
    :return:
    """
    # For post-order traversal, first run down the left hand side, then the right, then the root
    instructions = []

    if isinstance(expr, (Constant, ID)):
        if isinstance(expr, Constant):
            return [InstrPushValue(expr)], expr.type
        elif isinstance(expr, ID):
            return [InstrPushValue(expr)], expr.get_type(code_block)
    elif isinstance(expr, UnaryOp):
        instrs, type_on_top = expression_instructions(expr.expr, code_block)
        instructions.extend(instrs)
        instructions.append(InstrEvaluateUnary(expr.op, type_on_top))
        return instructions, type_on_top
    elif isinstance(expr, BinaryOp):
        lexpr, ltype = expression_instructions(expr.left, code_block)
        rexpr, rtype = expression_instructions(expr.right, code_block)
        instructions.extend(lexpr)
        instructions.extend(rexpr)
        instructions.append(InstrEvaluateBinary(expr.op, ltype, rtype))
        return instructions, (ltype, rtype)