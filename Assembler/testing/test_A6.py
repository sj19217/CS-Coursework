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
        self.assertEqual(instr1, instr2)

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
                self.assertEqual(instr._calculate_valsize(), size)

    def test_A612(self):
        instr = DataInstruction(0, "i", "5", "int")
        self.assertEqual(instr.get_bytes_length(), len(instr.get_bytes({"i": 40})))

    def test_A620(self):
        instr1 = TextInstruction(0, "MOV", "int", RegisterOperand("eax"), ImmediateOperand("5"))
        instr2 = TextInstruction(0, "MOV", "int", RegisterOperand("eax"), ImmediateOperand("5"))
        self.assertEqual(instr1, instr2)

    def test_A621(self):
        instr = TextInstruction(0, "MOV", "int", RegisterOperand("eax"), ImmediateOperand("5"))
        self.assertEqual(instr.get_bytes_length(), len(instr.get_bytes({})))

    def test_A622(self):
        instructions = [
            [TextInstruction(0, "MOV", "int", RegisterOperand("eax"), RegisterOperand("ebx")), 0x12],
            [TextInstruction(1, "SUB", "int", AddressOperand("i"), ImmediateOperand("1"), "loop"), 0x2C],
            [TextInstruction(2, "HLT", "", None, None), 0x00],
            [TextInstruction(3, "MOV", "1B", AddressOperand("a"), AddressOperand("b")), 0x10],
            [TextInstruction(4, "ADD", "int", RegisterOperand("eax"), ArithmeticOperand("ebx+1")), 0x24]
        ]
        place_memory_addresses({"i": 40, "a": 44, "b": 44}, [pair[0] for pair in instructions])
        for instr, opcode in instructions:
            with self.subTest(args=str(instr)):
                self.assertEqual(instr.get_bytes({"i": 40, "a": 44, "b": 44})[0], opcode)

    def test_A623(self):
        instructions = [
            [TextInstruction(0, "HLT", "", None, None), 0x00],
            [TextInstruction(1, "MOV", "int", RegisterOperand("eax"), RegisterOperand("ebx")), 0x11],
            [TextInstruction(2, "MOV", "int", AddressOperand("a"), ArithmeticOperand("eax*ebx")), 0x57],
            [TextInstruction(3, "MOV", "int", RegisterOperand("eax"), ImmediateOperand("5")), 0x12]
        ]
        place_memory_addresses({"i": 40, "a": 44}, [pair[0] for pair in instructions])
        for instr, operand_byte in instructions:
            with self.subTest(args=repr(instr)):
                self.assertEqual(instr.get_bytes({"i": 40, "a": 44})[1], operand_byte)