"""
The assembler. This file will manage the actual assembly, with calls to the GUI module added to make the educational
part work.
"""


def main(asmfile):
    with open(asmfile, "rt") as file:
        text = file.read()

    # Now the text is available


if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        print("Assembly file is unspecified")