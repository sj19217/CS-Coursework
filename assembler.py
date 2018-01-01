"""
The assembler. This file will manage the actual assembly, with calls to the GUI module added to make the educational
part work.
"""

import re

def main(asmfile):
    with open(asmfile, "rt") as file:
        text = file.read()

    # Now the text is available

    # 1. PERFORM TEXT NORMALISATION

    # 1.1. Split text into lines

    lines = text.split("\n")

    for i, line in enumerate(lines):
        # 1.2. For each line, if there is a semicolon, remove everything after the first semicolon
        lines[i] = lines[i].split("#")[0]

        # 1.3. Strip all whitespace from the start and end of every line
        lines[i] = lines[i].strip()

    # 1.4. Remove empty lines
    lines = [line for line in lines if line != ""]

    # 1.5. Remove duplicate whitespace
    multiple_whitespace = re.compile(r"\s+")
    for i, line in enumerate(lines):
        lines[i] = multiple_whitespace.sub(" ", line)

    # 1.6. Put the lines back together
    normalised_text = "\n".join(lines)

    print(normalised_text)





if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        print("Assembly file is unspecified")