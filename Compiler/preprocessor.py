"""
The preprocessor - handles directives.
"""

import re
import io
import logging
from collections import deque
import json

logging.basicConfig(level=logging.DEBUG)

# DO NOT TOUCH THESE
DIRECTIVES = {
    "include": r"\s*#\s*include\s+[<\"](?P<fname>[\w\.\\/]*)[>\"]",
    "define": r"\s*#\s*define\s*(?P<name>\w*)\s*(?P<value>[^\n]*)",
    "undef": r"\s*#\s*undef\s*(?P<name>\w*)",
    "ifdef": r"\s*#\s*ifdef\s*(?P<name>\w*)",
    "ifndef": r"\s*#\s*ifndef\s*(?P<name>\w*)",
    "endif": r"\s*#\s*endif\s*"
}

def directive_include(text: str, lineno, filename):
    """
    Takes the overall text of the file, the line number on which an #include statement happens, and the filename
    it specified, then replaces the line with the content of that file
    :param text:
    :param lineno:
    :param filename:
    :return:
    """
    logging.debug("Running directive_include(text, lineno={0}, filename={1}".format(lineno, filename))
    lines = text.split("\n")
    include_statement = lines[lineno]

    with open(filename, "rt") as file:
        file_content = file.read()

    return text.replace(include_statement, file_content)

def lineof(text, substring):
    logging.debug("Running lineof(text, substring={0})".format(substring))
    lines = text.split("\n")
    for i, line in enumerate(lines):
        if substring in line:
            return i

    return -1

def get_is_defined(defined_lines, const, line):
    """
    Returns True if, based on defined_lines, `const` will be defined at line `line`.
    :param defined_lines:
    :param const:
    :return:
    """
    logging.debug("Running get_is_defined(defined_lines, const={0}, line={1})".format(const, line))
    for name, start, end in defined_lines:
        if name != const:
            # Not useful
            continue

        # This is const
        if start <= line <= end:
            return True

    # It isn't defined anywhere
    return False

DIRECTIVE_FUNCTIONS = {

}

def process(text, interactive_mode=False):
    logging.debug("Running process(text)")
    # -------- First, look for all of the #include statements
    while True:
        # Find the first instance of #include currently in the text
        m = re.search(DIRECTIVES["include"], text)
        if m is None:
            break   # No more includes

        filename = m.group("fname")
        lineno = lineof(text, m.string[m.start():m.end()].strip())

        text = directive_include(text, lineno, filename)

        if interactive_mode:
            print("prep_include", json.dumps([
                filename,
                text
            ]))
        

    # -------- Next, work through the #define statements
    defined_lines = []
    while True:
        # Get the first existing #define statement
        m_def = re.search(DIRECTIVES["define"], text)

        if m_def is None:
            # No matches of #define left, so this stage is done
            break

        # Get some info on the #define line
        start_def = lineof(text, m_def.string[m_def.start():m_def.end()].strip())
        name = m_def.group("name")
        value = m_def.group("value")

        # Get the
        # m_undef = re.search(DIRECTIVES["undef"], text)
        # if m_undef is None or m_undef.group("name") != name:
        #     end_def = len(text.split("\n"))    # As in go to the last line
        # else:
        #     end_def = lineof(text, m_undef.string[m_undef.start():m_undef.end()].strip())
        undef_statements = re.compile(DIRECTIVES["undef"]).finditer(text)
        end_def = len(text.split("\n"))
        for m_undef in undef_statements:
            if m_undef.group("name") == name:
                # Found one that the name matches so write down its line
                end_def = lineof(text, m_undef.string[m_undef.start():m_undef.end()].strip())
                break

        # Split up into lines to make individual replacements
        lines = text.split("\n")
        for i, line in enumerate(lines):
            if start_def <= i <= end_def and not line.lstrip().startswith("#"):
                lines[i] = line.replace(name, value)
        del lines[start_def]
        try:
            del lines[end_def-1]    # The -1 is to compensate for the removal of the #define line
        except IndexError:
            pass    # This is perfectly expected, it just means there is no undef statement

        text = "\n".join(lines)

        # Add the info about this to the defined_lines list
        defined_lines.append([name, start_def, end_def-1])

        # Update the existing things to cope with the extra removed lines
        for item in defined_lines:
            # For each of the start and end lines, check this:
            # If both the start and end here are below it, then reduce it by 2
            # If just the start is before, then reduce it by 1
            if item[1] > end_def:
                item[1] = item[1] - 2
            elif item[1] > start_def:
                item[1] = item[1] - 1

            if item[2] > end_def:
                item[2] = item[2] - 2
            elif item[2] > start_def:
                item[2] = item[2] - 1
        
        if interactive_mode:
            print("prep_define", json.dumps([
                name,
                value,
                "\n".join(lines)
            ]))

    # -------- Mark down where which if statements apply
    lines = text.split("\n")
    linedata = [[] for _ in lines]
    ifstack = deque()

    for i, line in enumerate(lines):
        m = re.match(DIRECTIVES["ifdef"], line.strip())
        if m is not None:
            # This line is an ifdef statement
            name = m.group("name")
            linedata[i].append(("def", name))
            ifstack.append(("def", name))

        m = re.match(DIRECTIVES["ifndef"], line.strip())
        if m is not None:
            # This line is an ifndef statement
            name = m.group("name")
            linedata[i].append(("ndef", name))
            ifstack.append(("ndef", name))

        # Add all of the constraints already on the stack
        for constraint in ifstack:
            linedata[i].append(constraint)

        # If this is an endif line, pop the stack
        m = re.match(DIRECTIVES["endif"], line.strip())
        if m is not None:
            ifstack.pop()
    
    if interactive_mode:
        print("prep_ifanalysis", json.dumps(
            "\n".join("{}".format(data) for data in linedata)
        ))

    # -------- Act on the constraints
    for i, line in enumerate(lines):
        constraints = linedata[i]
        erase = False
        for constraint in constraints:
            is_def = get_is_defined(defined_lines, constraint[1], i)
            if constraint[0] == "def" and not is_def:
                erase = True
            elif constraint[0] == "ndef" and is_def:
                erase = True

        if erase:
            lines[i] = ""
    text = "\n".join(lines)

    if interactive_mode:
        print("prep_if", json.dumps(text))

    # -------- Remove all remaining lines beginning with a #
    lines = text.split("\n")
    for i, line in enumerate(lines):
        if line.strip().startswith("#"):
            lines[i] = ""
    text = "\n".join(lines)

    # -------- Remove all duplicate newlines
    text = re.sub("\n+", "\n", text)

    if interactive_mode:
        print("prep_done", json.dumps(text))

    return text

# Make an interactive version available
if __name__ == "__main__":
    with open("testing/csamples/if1.c", "rt") as f:
        print(process(f.read()))

    import code
    code.interact(local=locals())