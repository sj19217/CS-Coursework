import unittest

from assembler import *

class Test_split_into_sections(unittest.TestCase):
    def test_A201(self):
        text = """section.meta
section.data
section.text"""

        output = {"meta": "", "data": "", "text": ""}

        self.assertEqual(split_into_sections(text), output)

    def test_A202(self):
        text = """section.meta
a=b
c=d
section.data
x VAR int 50
y VAR int 5
section.text
MOV eax x
ADD eax y
MOV out eax"""

        output = {"meta": "a=b\nc=d",
                  "data": "x VAR int 50\ny VAR int 5",
                  "text": "MOV eax x\nADD eax y\nMOV out eax"}

        self.assertEqual(split_into_sections(text), output)

    def test_A203(self):
        text = "section.meta\nsection.data"

        with self.assertRaises(AssemblyError):
            split_into_sections(text)