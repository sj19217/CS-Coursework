"""
The assembler. This file will manage the actual assembly, with calls to the GUI module added to make the educational
part work.
"""

import re
from pprint import pprint
from enum import Enum
import struct

META_CONFIG_DEFAULT = {
    "mem_amt": 4
}

# A dict of opcodes.
OPCODES = {
    "HLT": 0x00,

    "CMP_char": 0x01,
    "CMP_uchar": 0x02,
    "CMP_short": 0x03,
    "CMP_ushort": 0x04,
    "CMP_int": 0x05,
    "CMP_uint": 0x06,
    "CMP_float": 0x07,

    "JMP": 0x08,
    "JE": 0x09,
    "JNE": 0x0A,
    "JLT": 0x0B,
    "JLE": 0x0C,
    "JGT": 0x0D,
    "JGE": 0x0E,

    "MOV_1B": 0x10,
    "MOV_2B": 0x11,
    "MOV_3B": 0x12,

    "LEA": 0x14,

    "ADD_char": 0x20,
    "ADD_uchar": 0x21,
    "ADD_short": 0x22,
    "ADD_ushort": 0x23,
    "ADD_int": 0x24,
    "ADD_uint": 0x25,
    "ADD_float": 0x26,

    "SUB_char": 0x28,
    "SUB_uchar": 0x29,
    "SUB_short": 0x2A,
    "SUB_ushort": 0x2B,
    "SUB_int": 0x2C,
    "SUB_uint": 0x2D,
    "SUB_float": 0x2E,

    "MUL_char": 0x30,
    "MUL_uchar": 0x31,
    "MUL_short": 0x32,
    "MUL_ushort": 0x33,
    "MUL_int": 0x34,
    "MUL_uint": 0x35,
    "MUL_float": 0x36,

    "IDIV_char": 0x38,
    "IDIV_uchar": 0x39,
    "IDIV_short": 0x3A,
    "IDIV_ushort": 0x3B,
    "IDIV_int": 0x3C,
    "IDIV_uint": 0x3D,
    "IDIV_float": 0x3E,

    "MOD_char": 0x40,
    "MOD_uchar": 0x41,
    "MOD_short": 0x42,
    "MOD_ushort": 0x43,
    "MOD_int": 0x44,
    "MOD_uint": 0x45,
    "MOD_float": 0x46,

    "EDIV_char": 0x48,
    "EDIV_uchar": 0x49,
    "EDIV_short": 0x4A,
    "EDIV_ushort": 0x4B,
    "EDIV_int": 0x4C,
    "EDIV_uint": 0x4D,
    "EDIV_float": 0x4E,

    "AND_1B": 0x50,
    "AND_2B": 0x51,
    "AND_4B": 0x52,

    "OR_1B": 0x54,
    "OR_2B": 0x55,
    "OR_4B": 0x56,

    "XOR_1B": 0x58,
    "XOR_2B": 0x59,
    "XOR_4B": 0x5A,

    "NOT_1B": 0x5C,
    "NOT_2B": 0x5D,
    "NOT_4B": 0x5E,

    "LSH_1B": 0x60,
    "LSH_2B": 0x61,
    "LSH_4B": 0x62,

    "RSH_1B": 0x64,
    "RSH_2B": 0x65,
    "RSH_4B": 0x66
}

OPCODE_NAMES = {
    "HLT",
    "CMP",
    "JMP",
    "JE",
    "JNE",
    "JLT",
    "JLE",
    "JGT",
    "JGE",
    "MOV",
    "LEA",
    "ADD",
    "SUB",
    "MUL",
    "IDIV",
    "MOD",
    "EDIV",
    "AND",
    "OR",
    "XOR",
    "NOT",
    "LSH",
    "RSH"
}

REGISTERS = {
    "eax": 0xA0,
    "ebx": 0xB0,
    "ecx": 0xC0,
    "edx": 0xD0,
    "esi": 0xE1,
    "edi": 0xE2,
    "ebp": 0xE3,
    "esp": 0xE4,

    "ax": 0xA1,
    "bx": 0xB1,
    "cx": 0xC1,
    "dx": 0xD1,

    "ah": 0xA2,
    "bh": 0xB2,
    "ch": 0xC2,
    "dh": 0xD2,

    "al": 0xA3,
    "bl": 0xB3,
    "cl": 0xC3,
    "dl": 0xD3
}

# ---------- CLASSES

class DataType(Enum):
    char=1
    uchar=2
    short=3
    ushort=4
    int=5
    uint=6
    float=7



class Instruction:
    def __init__(self, instr_num):
        self.instruction_num = instr_num

    def get_bytes_length(self):
        raise NotImplementedError("Must only use a subclass of Instruction")

    def get_bytes(self, var_table, label_table):
        raise NotImplementedError("Must only use a subclass of Instruction")

    def get_start_address(self):
        raise NotImplementedError("Must only use a subclass of Instruction")


class DataInstruction(Instruction):
    """
    A type of instruction from the data section. It sets a variable. In its implementation, it is a MOV command,
    moving an immediate value to a certain memory address
    """

    def __init__(self, instr_num, name, value, dtype):
        super().__init__(instr_num)
        self.name = name
        self.value = value
        self.data_type = dtype

    def _calculate_valsize(self):
        """Calculate the number of bytes in the value"""
        if self.data_type == DataType.float:
            valsize = 4
        else:
            # An integer type; calculate its size
            if abs(int(self.value)) < 256:
                valsize = 1
            elif abs(int(self.value)) < 65536:
                valsize = 2
            else:
                valsize = 4

        return valsize

    def get_bytes_length(self):
        # (Command (MOV) + Opcode data + Memory address = 6) + size of value
        return 6 + self._calculate_valsize()

    def get_bytes(self, var_table, label_table):
        # The instruction byte
        instr = struct.pack(">B", OPCODES["MOV_{}B".format(self._calculate_valsize())])

        valsize = self._calculate_valsize()
        # The byte to describe the operands and the format string for how to turn the immediate value into binary
        if valsize == 1:
            operand_num = b"R" # 0x52
            val_fmt_str = ">B"
        elif valsize == 2:
            operand_num = b"S" # 0x53
            val_fmt_str = ">H"
        elif valsize == 4:
            operand_num = b"T" # 0x54
            val_fmt_str = ">I"
        else:
            raise ValueError("Illegal value size: {}".format(valsize))

        # The memory address
        mem_addr = struct.pack(">I", var_table[self.name])

        # The initial value
        value_bytes = struct.pack(val_fmt_str, self.value)

        # Put them all together and return
        return instr + operand_num + mem_addr + value_bytes



class TextInstruction(Instruction):
    pass




class Operand:
    def __init__(self):
        self._bit_designation = -1
        self._required_length = -1

    def get_bit_designation(self):
        raise NotImplementedError("Must use a subclass of Operand")

    def get_required_length(self):
        raise NotImplementedError("Must use a subclass of Operand")

    def get_bytes(self):
        raise NotImplementedError("Must use a subclass of Operand")


class RegisterOperand(Operand):
    def __init__(self, regname):
        super().__init__()
        self.name = regname.lower()
        self.numerical = REGISTERS[regname.lower()]

        self._bit_designation = 1
        self._required_length = 1

    def get_bytes(self):
        # Basically turn the numerical register number into a byte
        return struct.pack(">B", self.numerical)


class ImmediateOperand(Operand):
    def __init__(self, value):
        super().__init__()
        self.value = int(value)

        if self.value < -32768 or self.value > 65536:
            self.size = 4
            self._bit_designation = 4
        elif self.value < -128 or self.value > 255:
            self.size = 2
            self._bit_designation = 3
        else:
            self.size = 1
            self._bit_designation = 2

        self._required_length = self.size

    def _get_value_format_string(self):
        # Find the formatting string to use to turn it into bytes
        if self.value < -32768:
            return ">i"
        elif self.value < -128:
            return ">h"
        elif self.value < 0:
            return ">b"
        elif self.value < 256:
            return ">B"
        elif self.value < 65536:
            return ">H"
        else:
            return ">I"

    def get_bytes(self):
        return struct.pack(self._get_value_format_string(), self.value)

class AddressOperand(Operand):
    def __init__(self, address):
        super().__init__()
        self.addr = address
        self._bit_designation = 5
        self._required_length = 4

    def get_bytes(self):
        return struct.pack(">I", self.addr)


class ArithmeticOperand(Operand):
    type_6 = r"(?P<a>[a-zA-Z0-9]+)"    # a
    type_7 = r"(?P<a>[a-zA-Z0-9]+)\*(?P<b>[a-zA-Z0-9]+)"    # a*b
    type_8 = r"(?P<a>[a-zA-Z0-9]+)\+(?P<b>[a-zA-Z0-9]+)"    # a+b
    type_9 = r"(?P<a>[a-zA-Z0-9]+)\*(?P<b>[a-zA-Z0-9]+)\+(?P<c>[a-zA-Z0-9]+)"    # a*b+c
    type_10 = r"(?P<a>[a-zA-Z0-9]+)\+(?P<b>[a-zA-Z0-9]+)\*(?P<c>[a-zA-Z0-9]+)"    # a+b*c

    def __init__(self, asm_str):
        super().__init__()
        self.asm_str = asm_str

        # See which type of operand this is and set everything accordingly
        if re.search(self.type_6, asm_str) is not None:
            m = re.match(self.type_6, asm_str)
            self._bit_designation = 6
            self._required_length = 1
            self.a = m.group("a")
            self.b = None; self.c = None
        elif re.search(self.type_7, asm_str) is not None:
            m = re.match(self.type_7, asm_str)
            self._bit_designation = 7
            self._required_length = 2
            self.a = m.group("a")
            self.b = m.group("b")
            self.c = None
        elif re.search(self.type_8, asm_str) is not None:
            m = re.match(self.type_8, asm_str)
            self._bit_designation = 8
            self._required_length = 2
            self.a = m.group("a")
            self.b = m.group("b")
            self.c = None
        elif re.search(self.type_9, asm_str) is not None:
            m = re.match(self.type_9, asm_str)
            self._bit_designation = 9
            self._required_length = 3
            self.a = m.group("a")
            self.b = m.group("b")
            self.c = m.group("c")
        elif re.search(self.type_10, asm_str) is not None:
            m = re.match(self.type_10, asm_str)
            self._bit_designation = 10
            self._required_length = 3
            self.a = m.group("a")
            self.b = m.group("b")
            self.c = m.group("c")
        else:
            raise ValueError("Incorrect format for arithmetic operand: {}".format(asm_str))

    def _interpret_value(self, value) -> int:
        # The values in arithmetic expressions can be immediate or a register name. Either way it has to be encoded.
        if isinstance(value, str):
            # A register
            try:
                return REGISTERS[value]
            except KeyError as err:
                raise ValueError("Invalid register name: {}".format(value)) from err

        # It is just an immediate value
        numval = int(value)
        if numval in (2, 4, 8):
            return numval
        else:
            raise ValueError("Only 2, 4 and 8 are permitted for multiplication in arithmetic operands")

    def get_bytes(self):
        bytes_ = b""
        for val in (self.a, self.b, self.c):
            if val is not None:
                bytes_ += struct.pack(">B", self._interpret_value(val))

        assert len(bytes_) == self._required_length, "Required length and calculated byte length do not match"
        return bytes_


# ---------- FUNCTIONS




def normalise_text(text):
    """
    Removes comments, unnecessary whitespace and empty lines
    :param text:
    :return:
    """
    # 1.1. Split text into lines

    lines = text.split("\n")

    for i, line in enumerate(lines):
        # 1.2. For each line, if there is a semicolon, remove everything after the first semicolon
        lines[i] = lines[i].split(";")[0]

        # 1.3. Strip all whitespace from the start and end of every line
        lines[i] = lines[i].strip()

    # 1.4. Remove empty lines
    lines = [line for line in lines if line != ""]

    # 1.5. Remove duplicate whitespace
    multiple_whitespace = re.compile(r"\s+")
    for i, line in enumerate(lines):
        lines[i] = multiple_whitespace.sub(" ", line)

    # 1.6. Put the lines back together
    normalised_text = "\n".join(lines)

    return normalised_text


def split_into_sections(text):
    """
    Takes the normalised text and splits it into its individual sections
    :param text:
    :return:
    """
    # Don't bother with regular expressions for this
    # Just split where "section." occurs
    parts = text.split("section.")
    parts_with_titles = [part.split("\n", maxsplit=1) for part in parts]

    sections_dict = {}
    for title, *other in parts_with_titles:
        if title.strip() == "" and len(other) == 0:
            continue

        sections_dict[title] = other[0] if len(other) > 0 else ""

    return sections_dict


def divide_and_contextualise(section_dict: dict):
    """
    Divides the sections up into lines. Interprets the meta section to create a config dict, and then creates a list
    of Instruction objects based on the data and text sections. Returns both as a tuple
    :param section_dict:
    :return:
    """

    # 4.1. Split the meta section into lines and interpret them
    # Create the dict based on META_CONFIG_DEFAULT
    config_dict = META_CONFIG_DEFAULT.copy()

    # Split meta section into lines
    meta_lines = [x.strip() for x in section_dict["meta"].split("\n") if x.strip()]

    # Go through the lines and split on an = sign, then act on that
    for line in meta_lines:
        item, value = line.split("=")
        config_dict.update(**{item: value})

    # Config dict done, moving on to the big part: instructions
    instruction_list = []

    # Start with the data section, adding each one as a DataInstruction instance
    data_lines = [x.strip() for x in section_dict["data"].split("\n") if x.strip()]
    for line in data_lines:
        # Takes the form name VAR type initial
        name, type_and_initial = [x.strip() for x in line.split("VAR") if x.strip()]
        dtype, initial = type_and_initial.split()

        data_type = getattr(DataType, dtype)

        instruction_list.append(DataInstruction(len(instruction_list), name, initial, data_type))

    # Next move onto the text section
    text_lines = [x.strip() for x in section_dict["text"].split("\n") if x.strip()]
    for line in text_lines:
        # Split into basic tokens
        parts = line.split()

        # See if the first part is in the list of opcodes. If not then it is likely a label
        label = ""
        if not parts[0].upper() in OPCODE_NAMES:
            # The first must be a label; check that the second is the opcode
            assert parts[1].upper() in OPCODE_NAMES, "Either the first or second part must be an opcode mnemonic"
            label = parts[0]
            del parts[0]    # Remove the label from the list

        # Now we can assume parts[0] is the opcode
        # Next is the type. parts[1] could be a data type or it could not be
        dtype = 0
        if parts[1].lower() in ("char", "uchar", "short", "ushort", "int", "uint", "float"):
            dtype = getattr(DataType, parts[1].lower())
            del parts[1]

        # If not then assume the data type is unspecified
        mnemonic = parts[0]
        del parts[0]

        # The lisy of parts should now just consist of the operands, separated by a space
        if len(parts) == 0:
            # No operands
            operand1 = None
            operand2 = None
        elif len(parts) == 1:
            operand1 = interpret_operand(parts[0])
            operand2 = None
        elif len(parts) == 2:
            operand1 = interpret_operand(parts[0])
            operand2 = interpret_operand(parts[1])

        # We now have both operands; that's everything



def interpret_operand(string: str) -> Operand:
    # The operand can be one of four things:
    # * register (the name of a register)
    # * immediate (a number)
    # * address (a string other than a register name)
    # * arithmetic (an expression inside square brackets)

    string = string.strip() # Just to make sure

    # Is it a register name?
    if string.lower() in REGISTERS.keys():
        # Yes it is
        return RegisterOperand(string)

    # Is it an immediate value?
    try:
        val = int(string)
        return ImmediateOperand(val)
    except ValueError:
        pass   # it isn't an immediate value

    # Is it an address label/variable?
    if string.isalnum():
        return AddressOperand(string)

    # Is it an arithmetic expression?
    if string[0] == "[" and string[-1] == "]":
        return ArithmeticOperand(string[1: -2])

    # If it was none of those then it is invalid
    raise ValueError("Invalid operand: {}".format(string))





def main(asmfile):
    with open(asmfile, "rt") as file:
        text = file.read()

    # Now the text is available

    # 1. PERFORM TEXT NORMALISATION

    normalised_text = normalise_text(text)
    print(normalised_text)

    # 2. SPLIT DOCUMENT INTO SECTIONS
    section_dict = split_into_sections(normalised_text)
    pprint(section_dict)

    # 3. DIVIDE LINES AND CONTEXTUALISE
    config_dict, instruction_list = divide_and_contextualise(section_dict)

if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        if sys.argv[1].lower() == "<ask>":
            file = input("Input file: ")
        else:
            file = sys.argv[1]

        main(file)
    else:
        print("Assembly file is unspecified")