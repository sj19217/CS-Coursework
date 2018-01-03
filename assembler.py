"""
The assembler. This file will manage the actual assembly, with calls to the GUI module added to make the educational
part work.
"""

import re
from pprint import pprint


def normalise_text(text):
    # 1.1. Split text into lines

    lines = text.split("\n")

    for i, line in enumerate(lines):
        # 1.2. For each line, if there is a semicolon, remove everything after the first semicolon
        lines[i] = lines[i].split(";")[0]

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

    return normalised_text


def split_into_sections(text):
    # Don't bother with regular expressions for this
    # Just split where "section." occurs
    parts = text.split("section.")
    parts_with_titles = [part.split("\n", maxsplit=1) for part in parts]

    sections_dict = {}
    for title, *other in parts_with_titles:
        if title.strip() == "" and len(other) == 0:
            continue

        sections_dict[title] = other[0] if len(other) > 0 else ""

    return sections_dict


def main(asmfile):
    with open(asmfile, "rt") as file:
        text = file.read()

    # Now the text is available

    # 1. PERFORM TEXT NORMALISATION

    normalised_text = normalise_text(text)
    print(normalised_text)

    # 2. SPLIT DOCUMENT INTO SECTIONS
    section_dict = split_into_sections(normalised_text)
    pprint(section_dict)

if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        if sys.argv[1].lower() == "<ask>":
            file = input("Input file: ")
        else:
            file = sys.argv[1]

        main(file)
    else:
        print("Assembly file is unspecified")