"""
This turns the code from tree_exp.c and turns it into a textual output of the
pycparser tree form, so that I can analyse it.
"""

import pycparser as pcp

ast = pcp.parse_file("tree_exp.c")
print(ast, file=open("tree_out.txt", "w"))