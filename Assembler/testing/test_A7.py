import unittest

from assembler import *

class Test_RegisterOperand(unittest.TestCase):
    def test_A701(self):
        for x in ("eax", "ax", "BH"):
            with self.subTest(x=x):
                RegisterOperand(x)

    def test_A702(self):
        for x in ("eex", "5", "outt"):
            with self.subTest(x=x):
                with self.assertRaises(Exception):
                    RegisterOperand(x)

    def test_A703(self):
        op = RegisterOperand("eax")
        self.assertEqual(op.get_bit_designation(), 1)
        self.assertEqual(op.get_required_length(), 1)

    def test_A704(self):
        bytes_ = {
            "esi": b"\xe1",
            "ah": b"\xa2",
            "in": b"\xf1"
        }
        for inp, expected_bytes in bytes_.items():
            op = RegisterOperand(inp)
            self.assertEqual(op.get_bytes(), expected_bytes)


class Test_ImmediateOperand(unittest.TestCase):
    def test_A710(self):
        for x in ("5", "-2", "4.3"):
            with self.subTest(x=x):
                op = ImmediateOperand(x)

    def test_A711(self):
        for x in ("--5", "3.4.3", "hello"):
            with self.subTest(x=x):
                with self.assertRaises(Exception):
                    op = ImmediateOperand(x)

    def test_A712(self):
        designations = {
            "3": 2,
            "287": 3,
            "1000000": 4,
            "-128": 2,
            "-32768": 3,
            "-32769": 4,
            "2.88888": 4
        }
        for inp, designation in designations.items():
            with self.subTest(inp=inp):
                op = ImmediateOperand(inp)
                self.assertEqual(op.get_bit_designation(), designation)

    def test_A713(self):
        lengths = {
            "3": 1,
            "287": 2,
            "1000000": 4,
            "-128": 1,
            "-32768": 2,
            "-32769": 4,
            "2.88888": 4
        }
        for inp, length in lengths.items():
            with self.subTest(inp=inp):
                op = ImmediateOperand(inp)
                self.assertEqual(op.get_required_length(), length)

    def test_A714(self):
        bytes_ = {
            "-7": b"\xf9",
            "127": b"\x7f",
            "-129": b"\xff\x7f",
            "65535": b"\xff\xff",
            "-70000": b"\xff\xfe\xee\x90",
            "65536": b"\x00\x01\x00\x00",
            "3.1415": b"\x40\x49\x0e\x56"
        }
        for inp, out in bytes_.items():
            with self.subTest(inp=inp):
                op = ImmediateOperand(inp)
                self.assertEqual(op.get_bytes(), out)