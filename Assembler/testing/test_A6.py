import unittest

from assembler import *

class Test_Instruction(unittest.TestCase):
    def test_A601(self):
        instr = Instruction(0)

        with self.subTest(func="get_bytes_length"):
            with self.assertRaises(NotImplementedError):
                instr.get_bytes_length()

        with self.subTest(func="get_bytes"):
            with self.assertRaises(NotImplementedError):
                instr.get_bytes({})

    def test_A610(self):
        instr1 = DataInstruction(0, "i", "5", "int")
        instr2 = DataInstruction(0, "i", "5", "int")
        self.assertEquals(instr1, instr2)

    def test_A611(self):
        test_values = {
            "-1000000": 4,
            "-20000": 2,
            "-10": 1,
            "10": 1,
            "256": 2,
            "65536": 4,
            "3.67": 4
        }

        for value, size in test_values.items():
            with self.subTest(value=value):
                instr = DataInstruction(0, "i", value, "int" if "." not in value else "float")
                self.assertEquals(instr._calculate_valsize(), size)

    def test_A612(self):
        instr = DataInstruction(0, "i", "5", "int")
        self.assertEquals(instr.get_bytes_length(), len(instr.get_bytes({"i": 40})))

    def test_A620(self):
        instr1 = TextInstruction(0, "MOV", "int", RegisterOperand("eax"), ImmediateOperand("5"))
        instr2 = TextInstruction(0, "MOV", "int", RegisterOperand("eax"), ImmediateOperand("5"))
        self.assertEquals(instr1, instr2)

    def test_A621(self):
        instr = TextInstruction(0, "MOV", "int", RegisterOperand("eax"), ImmediateOperand("5"))
        self.assertEquals(instr.get_bytes_length(), len(instr.get_bytes({})))

    def test_A622(self):
        instructions = [
            [["MOV", "int", RegisterOperand("eax"), RegisterOperand("ebx")], 0x12],
            [["SUB", "int", AddressOperand("i"), ImmediateOperand("1"), "loop"], 0x28],
            [["HLT", "", "", ""], 0x00],
            [["MOV", "1B", AddressOperand("a"), AddressOperand("b")], 0x10],
            [["ADD", "int", RegisterOperand("eax"), ArithmeticOperand("ebx+1")], 0x24]
        ]
        for args, opcode in instructions:
            with self.subTest(args=str(args)):
                instr = TextInstruction(0, *args)
                self.assertEquals(instr.get_bytes({"i": 40, "a": 44, "b": 44})[0], opcode)

    def test_A623(self):
        instructions = [
            [["HLT", "", "", ""], 0x00],
            [["MOV", "int", RegisterOperand("eax"), RegisterOperand("ebx")], 0x11],
            [["MOV", "int", AddressOperand("a"), ArithmeticOperand("eax*ebx")], 0x57],
            [["MOV", "int", RegisterOperand("eax"), ImmediateOperand("5")], 0x12]
        ]
        for args, operand_byte in instructions:
            with self.subTest(args=args):
                instr = TextInstruction(0, *args)
                self.assertEquals(instr.get_bytes({"i": 40, "a": 44})[0], operand_byte)