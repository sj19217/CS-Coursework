import pycparser as pcp

ast = pcp.parse_file("tree_exp.c")
print(ast, file=open("tree_out.txt", "w"))