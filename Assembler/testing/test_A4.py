import unittest

from ..assembler import *

class Test_record_labels_and_variables(unittest.TestCase):
    def test_A401(self):
        self.assertEqual(record_labels_and_variables([]), {})

    def test_A402(self):
        inp = [DataInstruction(0, "i", "5", "int")]
        self.assertEqual(record_labels_and_variables(inp), {"i": 7})

    def test_A403(self):
        inp = [TextInstruction(0, "MOV", "2B", RegisterOperand("eax"), RegisterOperand("ebx"), "start")]
        self.assertEqual(record_labels_and_variables(inp), {"start": 0})


class Test_calculate_var_table_size(unittest.TestCase):
    def test_A410(self):
        self.assertEqual(calculate_var_table_size({}), 0)

    def test_A411(self):
        tests = {
            "char": 1,
            "uchar": 1,
            "short": 2,
            "ushort": 2,
            "int": 4,
            "uint": 4,
            "float": 4
        }
        for dtype, size in tests.items():
            with self.subTest(dtype=dtype):
                self.assertEqual(calculate_var_table_size({"x": (0, dtype)}), size)

    def test_A412(self):
        inp = {
            "x": (0, "char"),
            "y": (1, "float"),
            "z": (5, "ushort"),
            "temp": (7, "uchar")
        }
        self.assertEqual(calculate_var_table_size(inp), 8)