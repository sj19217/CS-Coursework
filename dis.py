"""
A disassembler; takes bytecode and prints out a breakdown of it.
The format of the output is the metadata then a list of commands.
The metadata is in the format of key: value
Each code line is formatted as:
start_byte  opcode (dtype)  operand1    operand2
"""

from collections import namedtuple
import io
import struct
import sys

from assembler import OPCODES, REGISTERS

# Get the OPCODES and REGISTERS dicts from the assembler file and swap everything round
for mnemonic, opcode in OPCODES.items():
    OPCODES[opcode] = mnemonic
    del OPCODES[mnemonic]

for regname, regnum in REGISTERS.items():
    REGISTERS[regnum] = regname
    del REGISTERS[regname]

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
        start_byte = text.tell() + text_offset

        # Making an instruction. First up should be an opcode.
        opcode_byte = text.read(1)[0]   # The [0] turns it into an int.
        opcode_desc = OPCODES[opcode_byte]

        if "_" in opcode_desc:
            mnemonic, dtype = opcode_desc.split("_")

        # Read operand byte
        op_byte = text.read(1)[0]
        op1_desc = op_byte & 0b00001111
        op2_desc = (op_byte & 0x11110000) >> 4


def interpret_operand(text_stream, desc):
    if desc == 0:
        return ""
    elif desc == 1:
        # Register
        regnum = text_stream.read(1)[0]
        return REGISTERS[regnum]
    elif desc == 2:
        # 8-bit immediate operand
        op_bytes = text_stream.read(1)
        return struct.unpack(">B", op_bytes)
    elif desc == 3:
        # 16-bit immediate operand
        op_bytes = text_stream.read(2)
        return struct.unpack(">H", op_bytes)


if __name__ == "__main__":
    if len(sys.argv) > 0:
        fname = sys.argv[1]
    else:
        fname = input("Bytecode file: ")

    with open(fname, "rb") as file:
        dis(file.read())