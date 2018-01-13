"""
A disassembler; takes bytecode and prints out a breakdown of it.
The format of the output is the metadata then a list of commands.
The metadata is in the format of key: value
Each code line is formatted as:
start_byte  opcode (dtype)  operand1    operand2

The operands take the following formats:
 * An immediate value is just printed as a base-10 int
 * A register operand is printed as the name of the register
 * An arithmetic operand is printed like the assembly input format
 * A memory address is formatted with the letter M followed by the base-16 address
"""

from collections import namedtuple
import io
import struct
import sys

from assembler import OPCODES, REGISTERS

# Get the OPCODES and REGISTERS dicts from the assembler file and swap everything round
for mnemonic, opcode in OPCODES.copy().items():
    OPCODES[opcode] = mnemonic

for regname, regnum in REGISTERS.copy().items():
    REGISTERS[regnum] = regname

# A dict to map from the name of a data type to its struct designation
DTYPE_STRUCT_FMT_STRINGS = {
    "byte": ">b",
    "ubyte": ">B",
    "short": ">h",
    "ushort": ">H",
    "int": ">i",
    "uint": ">I",
    "float": ">f",
    "1B": ">B",
    "2B": ">H",
    "4B": ">I",
    "": ""
}

Instruction = namedtuple("Instruction", "start_byte opcode dtype op1 op2")

def dis(bytecode):
    config, text = bytecode.split(b"\x00\x00\x00\x00", maxsplit=1)

    # Calculate the number of bytes into the file the text section is offset
    text_offset = len(config) + 4

    # Config section should be able to be interpreted as text
    config = config.decode()

    # Split on & sign
    kvpairs = [x for x in config.split("&") if x]

    # Split each by = sign and make config dict
    config_dict = {}
    for pair in kvpairs:
        key, value = pair.split("=")
        config_dict[key] = int(value)


    # Turn the text section into a byte stream so it can be consumed
    text = io.BytesIO(text)

    instruction_list = []
    # Go on a loop. Consume the bytes as the interpreter would, generating a list of instructions.
    while True:
        # Calculate the starting byte of this instruction
        start_byte = text.tell()

        # Making an instruction. First up should be an opcode.
        opcode_byte = text.read(1)[0]   # The [0] turns it into an int.
        opcode_desc = OPCODES[opcode_byte]

        if "_" in opcode_desc:
            mnemonic, dtype = opcode_desc.split("_")
        else:
            mnemonic, dtype = opcode_desc, ""

        # Read operand byte
        op_byte = text.read(1)[0]
        op1_desc = (op_byte & 0b11110000) >> 4
        op2_desc = op_byte & 0b00001111

        operand1 = interpret_operand(text, op1_desc, dtype)
        operand2 = interpret_operand(text, op2_desc, dtype)

        instruction_list.append(Instruction(start_byte, mnemonic, dtype, operand1, operand2))


        #### THIS SHOULD BE TEMPORARY
        dtype_str = "(" + dtype + ")" if dtype is not None else ""
        print("\t{start_byte}\t{mnemonic} {dtype_str}\t{op1}\t{op2}".format(start_byte=start_byte,
                                                                            mnemonic=mnemonic,
                                                                            dtype_str=dtype_str,
                                                                            op1=operand1,
                                                                            op2=operand2))

        if not len(text.getbuffer()):
            break

    # With the config dict and instruction list ready, do the printing
    print("Disassembling {} bytes\n".format(len(bytecode)))
    print("Config dictionary (took {} bytes)".format(len(config)))
    for key, value in config_dict.items():
        print("    {key}\t\t{value}".format(key=key, value=value))

    # Print all the instructions
    print("\nInstructions (took {} bytes)".format(len(bytecode) - len(config) - 4))
    for start_byte, mnemonic, dtype, op1, op2 in instruction_list:
        dtype_str = "(" + dtype + ")" if dtype is not None else ""
        print("\t{start_byte}\t{mnemonic} {dtype_str}\t{op1}\t{op2}".format(start_byte=start_byte,
                                                                            mnemonic=mnemonic,
                                                                            dtype_str=dtype_str,
                                                                            op1=op1,
                                                                            op2=op2))



def interpret_operand(text_stream, desc, dtype):
    if desc == 0:
        return ""
    elif desc == 1:
        # Register
        regnum = text_stream.read(1)[0]
        return REGISTERS[regnum]
    elif desc == 2:
        # 8-bit immediate operand
        op_bytes = text_stream.read(1)
        return struct.unpack(DTYPE_STRUCT_FMT_STRINGS[dtype], op_bytes)[0]
    elif desc == 3:
        # 16-bit immediate operand
        op_bytes = text_stream.read(2)
        return struct.unpack(DTYPE_STRUCT_FMT_STRINGS[dtype], op_bytes)[0]
    elif desc == 4:
        # 32-bit immediate operand
        op_bytes = text_stream.read(4)
        return struct.unpack(DTYPE_STRUCT_FMT_STRINGS[dtype], op_bytes)[0]
    elif desc == 5:
        # Memory address
        op_bytes = text_stream.read(4)
        return "M" + hex(struct.unpack(">I", op_bytes)[0])[2:]
    elif desc == 6:
        # Arithmetic operand in the form "a"
        a = read_arithmetic_part(text_stream)
        return "[" + a + "]"
    elif desc == 7:
        # Arithmetic operand in the form "a*b"
        a = read_arithmetic_part(text_stream)
        b = read_arithmetic_part(text_stream)
        return "[" + a + "*" + b + "]"
    elif desc == 8:
        # Arithmetic operand in the form "a+b"
        a = read_arithmetic_part(text_stream)
        b = read_arithmetic_part(text_stream)
        return "[" + a + "*" + b + "]"
    elif desc == 9:
        # Arithmetic operand in the form "a*b+c"
        a = read_arithmetic_part(text_stream)
        b = read_arithmetic_part(text_stream)
        c = read_arithmetic_part(text_stream)
        return "[" + a + "*" + b + "+" + c + "]"
    elif desc == 10:
        # Arithmetic operand in the form "a+b*c"
        a = read_arithmetic_part(text_stream)
        b = read_arithmetic_part(text_stream)
        c = read_arithmetic_part(text_stream)
        return "[" + a + "+" + b + "*" + c + "]"


def read_arithmetic_part(text_stream):
    num = text_stream.read(1)[0]
    if num in REGISTERS.keys():
        return REGISTERS[num]
    else:
        return str(num)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        fname = sys.argv[1]
    else:
        fname = input("Bytecode file: ")

    with open(fname, "rb") as file:
        dis(file.read())