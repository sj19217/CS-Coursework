import unittest

from assembler import *

class Test_normalise_text(unittest.TestCase):
    def test_A101(self):
        for inp in (" ", "\n", "\t", "; Comment"):
            with self.subTest(input=inp):
                self.assertEqual(normalise_text(inp), "")

    def test_A102(self):
        text = "MOV eax ebx"
        self.assertEqual(text, normalise_text(text))

    def test_A103(self):
        self.assertEqual(normalise_text("MOV   eax   ebx"), "MOV eax ebx")
        self.assertEqual(normalise_text("   ADD \t ecx  5"), "ADD ecx 5")

    def test_A104(self):
        text = """MOV eax [ebx]

ADD eax 5"""

        normalised = """MOV eax [ebx]
ADD eax 5"""

        self.assertEqual(normalise_text(text), normalised)

    def test_A105(self):
        text = """CMP eax ebx ; On line
        ; On own"""

        self.assertEqual(normalise_text(text), "CMP eax ebx")

    def test_A106(self):
        text = """  section.data ; A section
i VAR ubyte \t 0
x VAR int\t-100

; Another section
section.text
MOV eax     i

   MOV ebx  x"""

        normalised = """section.data
i VAR ubyte 0
x VAR int -100
section.text
MOV eax i
MOV ebx x"""

        self.assertEqual(normalise_text(text), normalised)