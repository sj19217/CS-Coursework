import unittest

from assembler import *

class Test_divide_and_contextualise(unittest.TestCase):
    def test_A301(self):
        inp = {"meta": "", "data": "", "text": ""}
        self.assertEqual(divide_and_contextualise(inp), (META_CONFIG_DEFAULT, []))

    def test_A302(self):
        inp = {"meta": "a=b\nc=d", "data": "", "text": ""}
        out = META_CONFIG_DEFAULT.copy()
        out.update({
            "a": "b",
            "c": "d"
        })

        self.assertEqual(divide_and_contextualise(inp), (out, []))

    def test_A303(self):
        inp = {"meta": "", "data": "a VAR int 5", "text": ""}
        out = (META_CONFIG_DEFAULT, [DataInstruction(0, "a", "5", "int")])
        self.assertEqual(divide_and_contextualise(inp), out)

    def test_A304(self):
        inp = {"meta": "", "data": "", "text": "MOV eax ebx\nSUB eax 5"}
        out = (META_CONFIG_DEFAULT, [
            TextInstruction(0, "MOV", "char", RegisterOperand("eax"), RegisterOperand("ebx")),
            TextInstruction(1, "SUB", "char", RegisterOperand("eax"), ImmediateOperand(5))
        ])
        self.assertEqual(divide_and_contextualise(inp), out)

    def test_A305(self):
        inp = {"meta": "a=b\nc=d",
               "data": "a VAR int 5",
               "text": "MOV eax ebx"}
        out_config = META_CONFIG_DEFAULT.copy()
        out_config.update({"a": "b", "c": "d"})
        out = (out_config, [
            DataInstruction(0, "a", "5", "int"),
            TextInstruction(1, "MOV", "char", RegisterOperand("eax"), RegisterOperand("ebx"))
        ])
        self.assertEqual(divide_and_contextualise(inp), out)


class Test_interpret_operand(unittest.TestCase):
    def test_A310(self):
        for x in ("", "\t"):
            with self.subTest(x=x):
                with self.assertRaises(ValueError):
                    interpret_operand(x)

    def test_A311(self):
        self.assertEqual(interpret_operand("eax"),
                         RegisterOperand("eax"))

    def test_A312(self):
        self.assertEqual(interpret_operand("EAX"),
                         RegisterOperand("eax"))

    def test_A313(self):
        self.assertEqual(interpret_operand("5"),
                         ImmediateOperand(5))

    def test_A314(self):
        self.assertEqual(interpret_operand("-100"),
                         ImmediateOperand(-100)),

    def test_A315(self):
        self.assertEqual(interpret_operand("7.5"),
                         ImmediateOperand(7.5))

    def test_A316(self):
        self.assertEqual(interpret_operand("varname"),
                         AddressOperand("varname"))

    def test_A317(self):
        with self.assertRaises(ValueError):
            interpret_operand("[]")

    def test_A318(self):
        self.assertEqual(interpret_operand("[eax]"),
                         ArithmeticOperand("eax"))

    def test_A319(self):
        self.assertEqual(interpret_operand("[eax*ebx]"),
                         ArithmeticOperand("eax*ebx"))

    def test_A320(self):
        self.assertEqual(interpret_operand("[eax+ebx]"),
                         ArithmeticOperand("eax+ebx"))

    def test_A321(self):
        self.assertEqual(interpret_operand("[eax+ebx*ecx]"),
                         ArithmeticOperand("eax+ebx*ecx"))

    def test_A322(self):
        self.assertEqual(interpret_operand("[eax*ebx+ecx]"),
                         ArithmeticOperand("eax*ebx+ecx"))

    def test_A323(self):
        self.assertEqual(interpret_operand("[eax*4]"),
                         ArithmeticOperand("eax*4"))