import io
import logging
import random
import hashlib
import json

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
        self.name = ""
        self.coord = ""

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

    def get_child_index(self, block):
        """
        Look through the child blocks and compare the coord, returning the correct index, or -1 if it is not present.
        :param block:
        :return:
        """
        for i, child in enumerate(self.child_blocks):
            if child.coord == block.coord:
                return i
        return -1

    def generate_code(self, block_name, global_symbols, queue, interactive_mode=False):
        # The assembly is written to here. More efficient than remaking strings.
        assembly = io.StringIO()
        # If the block has variables, this stores info about how far from
        # the base pointer the variables are (as in, this index and the next
        # few bytes are the number) and their type

        if interactive_mode:
            print("gen_block", block_name)

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
            assembly.write(instr.generate_code(self, global_symbols, queue, interactive_mode))

        if len(self.locals) > 0:
            assembly.write(self.generate_return())

        assembly.seek(0)
        return assembly.read()

    def generate_init(self, block_name) -> (dict, str):
        """
        Generates the initialisation code that will handle the stack
        :return:
        """
        self.name = block_name
        # 1. Push old base pointer to stack
        # 2. Change base pointer to one below the current stack pointer
        # 3. Write the variables to the stack and record where they are

        #variable_data = {}

        init_code = io.StringIO()
        init_code.write("; Initialising frame on stack\n")
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

            if type_ in ("char", "uchar", "short", "ushort", "int", "uint", "float"):
                init_code.write("; Adding variable {} to stack\n".format(name))

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
        return """; Removing stack frame and returning
MOV 4B esi ebp
SUB uint esi 4
MOV 4B ebp [esi]
JMP {return_label}
""".format(return_label=self.return_label)


class Instruction:
    def __init__(self):
        pass

    def generate_code(self, block: CodeBlock, global_symbols, queue, interactive_mode):
        raise NotImplementedError()


class InstrFuncCall(Instruction):
    def __init__(self, name, arg_types):
        super().__init__()
        self.func_name = name
        self._arg_types = arg_types

    def generate_code(self, block, global_symbols, queue, interactive_mode):
        if isinstance(self.func_name, ID) and self.func_name.name == "printf":
            # Currently only supports the ridiculously simple task of printing an integer type
            size = util.get_size_of_type(self._arg_types[0])
            # First line is doing it, second line is popping the item (no need to overwrite)
            asm = "; Calling printf\nMOV {size}B out [esp]\nADD uint esp {size}\n".format(size=size)
            if interactive_mode:
                print("gen_stmt", json.dumps([
                    "Made call to function {}".format(self.func_name),
                    asm
                ]))
            return asm
        else:
            logging.error("Unsupported function: {}".format(self.func_name))


class InstrVariableAssignment(Instruction):
    def __init__(self, lvalue: ID, stack_top_type: str):
        super().__init__()
        self.lvalue = lvalue
        self.var_name = lvalue.name
        self._stack_top_type = stack_top_type

    def generate_code(self, block: CodeBlock, global_symbols, queue, interactive_mode):
        code = "; Assigning top of stack to variable {}\n".format(self.var_name)
        var, rel = block.get_local_var_data(self.var_name)

        # Depending on whether it is global or local, set edi to point to it
        if var is None:
            # Could be a global
            for global_ in [x for x in ID.globals if isinstance(x, GlobalVariable)]:
                if global_.name == self.var_name:
                    var = global_
                    code += "LEA edi {}      ; Pointer to a global\n".format(self.var_name)
                    break
            else:
                # A for-else is an odd construct
                # But this will only run if no global was found
                logging.error("No variable found for assignment: {}".format(self.var_name))
                return
        else:
            # It is a local
            code += "MOV 4B edi ebp  ; Getting pointer to a local\n"

            if rel > 0:
                code += "SUB uint edi {}\n".format(rel)
            elif rel < 0:
                code += "ADD uint edi {}\n".format(0-rel)

        # edi has the address of the variable now. Get the size.
        size_var = util.get_size_of_type(var.type)
        size_stack = util.get_size_of_type(self._stack_top_type)

        # Move the thing from the stack to a register
        code += "; Move from stack to variable\n"
        code += "MOV {size}B ecx [esp]\n".format(size=size_stack)
        code += "ADD uint esp {size}\n".format(size=size_stack)

        # Move it from the register to the variable location
        code += "MOV {size}B [edi] ecx\n".format(size=size_var)

        if interactive_mode:
            print("gen_stmt", json.dumps([
                "Assigning an expression to variable {}".format(self.var_name),
                code
            ]))

        return code


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

    def generate_code(self, block: CodeBlock, global_symbols, queue, interactive_mode):
        code = io.StringIO()
        hash_obj = hashlib.md5()
        hash_obj.update(bytes(id(self)))
        block_rand = hash_obj.hexdigest()[-8:]


        # Start with a label to signify the beginning of the block, and a check for the condition
        code.write("; Beginning while loop\n")
        code.write("while_{rand} ")
        condition_instrs, top_type = expression_instructions(self._stmt.cond, block)
        for instr in condition_instrs:
            code.write(instr.generate_code(block, global_symbols, queue, interactive_mode))
        code.write("CMP {type} [esp] 1  ; See if true and jump accordingly\n".format(type=top_type))
        code.write("ADD uint esp {size}\n".format(size=util.get_size_of_type(top_type)))
        code.write("JNE endwhile_{rand}\n")

        # Next up is the actual code block
        child_block = generate_code_block(self._stmt.stmt, global_symbols, parent=block)
        child_block.return_label = "endwhile_{rand}".format(rand=block_rand)
        child_block.name = block.name + "_" + str(block.get_child_index(child_block))

        if len(child_block.locals) > 0:
            code.write("; Jumping to child block\n")
            code.write("JMP " + child_block.name + "\n")
            queue.append(child_block)
        else:
            code.write("; Running child block\n")
            code.write(child_block.generate_code(child_block.name, global_symbols, queue))
            code.write("JMP while_{rand}\n")

        code.write("endwhile_{rand} MOV 4B eax eax\n")

        if interactive_mode:
            code.seek(0)
            print("gen_stmt", json.dumps([
                "Generated code for while loop",
                code.read()
            ]))

        code.seek(0)
        return code.read().format(rand=block_rand)


class InstrIfStmt(Instruction):
    def __init__(self, stmt: If):
        super().__init__()
        self._stmt = stmt

    def generate_code(self, block: CodeBlock, global_symbols, queue, interactive_mode):
        # First, evaluate the truth expression
        code = io.StringIO()
        instrs, type_ = expression_instructions(self._stmt.cond, block)
        code.write("; Evaluating condition")
        for instr in instrs:
            code.write(instr.generate_code(block, global_symbols, queue))

        hash_obj = hashlib.md5()
        hash_obj.update(bytes(id(self)))
        block_rand = hash_obj.hexdigest()[-8:]

        # At this point, on the top of the stack should be the result (i.e. 0 being false, 1 being true)
        code.write("CMP {type} [esp] 1  ; See if true\n".format(type=type_))
        code.write("ADD uint esp {}\n".format(util.get_size_of_type(type_)))

        # Jump if not equal (i.e., if the expression evaluated to false)
        code.write("JNE else_{rand} ; Not true so jump to else\n".format(rand=block_rand))

        # This bit will be jumped over if the expression was false
        # Check if it has locals. If it does, jump to its block. If not, put the code here.
        true_block = generate_code_block(self._stmt.iftrue, global_symbols)
        if len(self._stmt.iftrue.locals) == 0:
            code.write("Running true section\n")
            code.write(true_block.generate_code(block.name + "_" + str(block.get_child_index(true_block)),
                                                global_symbols,
                                                queue))
            code.write("JMP endif_{rand}\n".format(rand=block_rand))
        else:
            code.write("; Jumping to true section\n")
            code.write("JMP " + block.name + "_" + str(block.get_child_index(true_block)) + "\n")
            true_block.return_label = "endif_" + block_rand
            queue.append((block.name + "_" + str(block.get_child_index(true_block)), true_block))

        # Got the true part done, now for the else part
        if self._stmt.iffalse is not None:
            false_block = generate_code_block(self._stmt.iffalse, global_symbols)
            if len(self._stmt.iffalse.locals) == 0:
                code.write("; Running else section\n")
                code.write("else_{rand} ".format(rand=block_rand))
                code.write(false_block.generate_code(block.name + "_" + str(block.get_child_index(false_block)),
                                                     global_symbols,
                                                     queue))
            else:
                code.write("; Jumping to else section\n")
                code.write("else_{rand} JMP ".format(rand=block_rand) + block.name + \
                           "_" + str(block.get_child_index(false_block)))
                false_block.return_label = "endif_" + block_rand
                queue.append((block.name + "_" + str(block.get_child_index(false_block)), false_block))

        code.write("endif_{rand} MOV 4B eax eax\n".format(rand=block_rand))

        if interactive_mode:
            code.seek(0)
            print("gen_stmt", json.dumps(
                "Generating if statement",
                code.read()
            ))

        code.seek(0)
        return code.read()

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

    def generate_code(self, block: CodeBlock, global_symbols, queue, interactive_mode):
        # Move the stack pointer down by the right amount then write the data
        if isinstance(self._value, Constant) and self._value.type == "int":
            return "; Pushing constant to stack\n" + \
                   "SUB uint esp 4\n" + \
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
                        return "; Pushing global variable to stack\n" +\
                               "SUB uint esp {size}\n".format(size=size) + \
                               "MOV {size}B [esp] {name}\n".format(size=size, name=global_var.name)
            else:
                # It is a local variable
                _, type_, _ = var
                size = util.get_size_of_type(type_)
                code = """; Pushing local variable to stack
SUB uint esp {size}
MOV 4B esi ebp
""".format(size=size)
                if rel_to_base < 0:
                    code += "ADD uint esi {rel}\n".format(rel=0-rel_to_base)
                elif rel_to_base > 0:
                    code += "SUB uint esi {rel}\n".format(rel=rel_to_base)
                code += "MOV {size}B [esp] [esi]\n".format(size=size)

                if interactive_mode:
                    print("gen_stmt", json.dumps([
                        "Created code to push a value to the stack",
                        code
                    ]))

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

    @property
    def lsize(self):
        return util.get_size_of_type(self._ltype)

    @property
    def rsize(self):
        return util.get_size_of_type(self._rtype)

    def generate_code(self, block: CodeBlock, global_symbols, queue, interactive_mode):
        # Pop the right hand value into edx
        code = "; Evaluating binary expression: Popping values to registers.\n"
        code += "MOV {size}B edx [esp]\n".format(size=self.rsize)
        code += "ADD uint esp {size}\n".format(size=self.rsize)
        # Pop the left hand value into ecx
        code += "MOV {size}B ecx [esp]\n".format(size=self.lsize)
        code += "ADD uint esp {size}\n".format(size=self.lsize)

        if self._op == "+":
            mnemonic = "ADD"
        elif self._op == "-":
            mnemonic = "SUB"
        elif self._op == "*":
            mnemonic = "MUL"
        elif self._op == "/":
            # TODO Change this in future to correctly choose IDIV vs EDIV
            mnemonic = "IDIV"
        elif self._op == "%":
            mnemonic = "MOD"
        else:
            # Not one of the standard arithmetic types
            return self._generate_code_comparison(code, block, global_symbols, interactive_mode)

        # It is one of the normal arithmetic types
        # Find the biggest type of these two
        maxtype = max(self._ltype, self._rtype, key=util.get_size_of_type)

        # Perform the operation
        code += "; Performing {} and pushing to stack\n".format(self._op)
        code += "{op} {maxtype} ecx edx\n".format(op=mnemonic, maxtype=maxtype)

        # Push it to the stack
        s = util.get_size_of_type
        code += "SUB uint esp {size}\n".format(size=max(s(self._ltype), s(self._rtype)))
        code += "MOV {size}B [esp] ecx\n".format(size=max(s(self._ltype), s(self._rtype)))

        if interactive_mode:
            print("gen_stmt", json.dumps([
                "Generating code to evaluate a binary expression ({op})".format(op=self._op),
                code
            ]))

        return code

    def _generate_code_comparison(self, pop_code, block: CodeBlock, global_symbols, interactive_mode=False):
        code = pop_code
        # ecx and edx contain the things to be compared
        code += "; Making comparison ({})\n".format(self._op)
        code += "CMP uint ecx edx\n"
        # The comparison registers are now set correctly

        hash_obj = hashlib.md5()
        hash_obj.update(bytes(random.randint(0, 100000000)))
        block_rand = hash_obj.hexdigest()[-8:]

        if self._op == "==":
            code += "JE jmptrue_{rand}\n"
        elif self._op == "!=":
            code += "JNE jmptrue_{rand}\n"
        elif self._op == "<":
            code += "JLT jmptrue_{rand}\n"
        elif self._op == ">":
            code += "JGT jmptrue_{rand}\n"
        elif self._op == "<=":
            code += "JLE jmptrue_{rand}\n"
        elif self._op == ">=":
            code += "JGE jmptrue_{rand}\n"

        code += "JMP jmpfalse_{rand}\n"
        code += "jmptrue_{rand} SUB esp 4\n"
        code += "MOV 4B [esp] 1\n"
        code += "JMP jmpcmpend_{rand}\n"
        code += "jmpfalse_{rand} SUB esp 4\n"
        code += "MOV 4B [esp] 0\n"
        code += "jmpcmpend_{rand} MOV 4B eax eax    ; Determined truth and added to stack\n"

        code = code.format(rand=block_rand)

        if interactive_mode:
            print("gen_stmt", json.dumps([
                "Generating code to evaluate a binary comparison ({op})".format(op=self._op),
                code
            ]))

        return code


### FUNCTIONS

def generate_code_block(compound: Compound, global_symbols, parent=None) -> CodeBlock:
    # Create the code block everything will be added to
    code_block = CodeBlock()

    # Assign locals and parent blocks
    code_block.parent = parent
    code_block.locals = compound.locals
    code_block.coord = compound.coord

    # Loop through the statements
    for stmt in compound.block_items:
        # Get the instructions from the block
        instrs = get_stmt_instructions(stmt, code_block, global_symbols)
        code_block.instructions.extend(instrs)

        # See if it contains sub-blocks
        if isinstance(stmt, If):
            code_block.child_blocks.append(generate_code_block(stmt.iftrue, global_symbols, code_block))
            code_block.child_blocks.append(generate_code_block(stmt.iffalse, global_symbols, code_block))
        elif isinstance(stmt, (For, While)):
            code_block.child_blocks.append(generate_code_block(stmt.stmt, global_symbols, code_block))

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
        top_type = max(ltype, rtype, key=util.get_size_of_type)
        return instructions, top_type