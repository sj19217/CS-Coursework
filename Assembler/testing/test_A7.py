from collections import namedtuple
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


class Test_AddressOperand(unittest.TestCase):
    def test_A720(self):
        for x in (64, "variable"):
            with self.subTest(x=x):
                _ = AddressOperand(x)

    def test_A721(self):
        op = AddressOperand(100)
        self.assertEqual(op.get_bit_designation(), 5)

    def test_A722(self):
        op = AddressOperand(100)
        self.assertEqual(op.get_required_length(), 4)

    def test_A723(self):
        with self.subTest(input=99999):
            self.assertEqual(AddressOperand(99999).get_bytes(), b"\x00\x01\x86\x9f")

        with self.subTest(input="variable"):
            with self.assertRaises(Exception):
                AddressOperand("variable").get_bytes()

class Test_ArithmeticOperand(unittest.TestCase):
    def test_A730(self):
        ExpectedOutput = namedtuple("ExpectedOutput", "a b c")
        outputs = {
            "eax": ExpectedOutput("eax", None, None),
            "eax*4": ExpectedOutput("eax", "4", None),
            "eax+ebx": ExpectedOutput("eax", "ebx", None),
            "eax*ebx+4": ExpectedOutput("eax", "ebx", "4"),
            "eax+ebx*4": ExpectedOutput("eax", "ebx", "4")
        }
        for inp, out in outputs.items():
            with self.subTest(inp=inp):
                op = ArithmeticOperand(inp)
                self.assertEqual(op.a, out.a)
                self.assertEqual(op.b, out.b)
                self.assertEqual(op.c, out.c)

    def test_A731(self):
        outputs = {
            "eax": (6, 1),
            "eax*4": (7, 2),
            "eax+ebx": (8, 2),
            "eax*ebx+4": (9, 3),
            "eax+ebx*4": (10, 3)
        }
        for inp, out in outputs.items():
            with self.subTest(inp=inp):
                op = ArithmeticOperand(inp)
                self.assertEqual(op.get_bit_designation(), out[0])
                self.assertEqual(op.get_required_length(), out[1])

    def test_A732(self):
        for x in ("a+b+c+d", "eax-ebx", "*hello*"):
            with self.subTest(x=x):
                with self.assertRaises(Exception):
                    _ = ArithmeticOperand(x)

    def test_A733(self):
        outputs = {
            "eax": b"\xa0",
            "ax+4": b"\xa1\x04",
            "eax*ebx": b"\xa0\xb0",
            "eax*4+dl": b"\xa0\x04\xd3",
            "edx+esp*8": b"\xd0\xe4\x08"
        }
        for inp, out in outputs.items():
            with self.subTest(inp=inp):
                op = ArithmeticOperand(inp)
                self.assertEqual(op.get_bytes(), out)