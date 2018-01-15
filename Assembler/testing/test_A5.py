import unittest

from ..assembler import *

class Test_place_memory_address(unittest.TestCase):
    def test_A501(self):
        instr_list = [TextInstruction(
            0,
            "MOV",
            "1B",
            AddressOperand("i"),
            RegisterOperand("eax")
        )]
        out = [TextInstruction(
            0,
            "MOV",
            "1B",
            AddressOperand(50),
            RegisterOperand("eax")
        )]
        place_memory_addresses({"i": 50}, instr_list)
        self.assertEqual(instr_list, out)

    def test_A502(self):
        instr_list = [DataInstruction(
            0,
            "a",
            "20",
            "int"
        )]
        out = instr_list.copy()
        place_memory_addresses({"x": 100}, instr_list)
        self.assertEqual(instr_list, out)


class Test_encode_metadata(unittest.TestCase):
    def test_A510(self):
        self.assertEqual(encode_metadata({}), b"\x00" * 4)

    def test_A511(self):
        self.assertEqual(encode_metadata({"key": "value"}), b"key=value&" + b"\x00" * 4)

    def test_A512(self):
        out = (b"a=b&c=d&" + b"\x00" * 4,
               b"c=d&a=b&" + b"\x00" * 4)
        self.assertIn(encode_metadata({"a": "b", "c": "d"}), out)

    def test_A513(self):
        out = (b"a=b&i=5&" + b"\x00" * 4,
               b"i=5&a=b&" + b"\x00" * 4)
        self.assertIn(encode_metadata({"a": "b", "i": 5}), out)


class Test_encode_instruction_list(unittest.TestCase):
    def test_A520(self):
        inp = [TextInstruction(
            0,
            "MOV",
            "1B",
            RegisterOperand("eax"),
            ImmediateOperand("5")
        )]
        self.assertEqual(encode_instruction_list(inp, {}), b"\x10\x12\xA0\x05")