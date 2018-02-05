import re
import io

# DO NOT TOUCH THESE
DIRECTIVES = {
    "include": r"\s*#\s*include\s+[<\"](?P<fname>[\w\.\\/]*)[>\"]\s*",
    "define": r"\s*#\s*define\s*(?P<name>\w*)\s*(?P<value>[^\n]*)\s*",
    "undef": r"\s*#\s*undef\s*(?P<name>\w*)\s*"
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
    lines = text.split("\n")
    include_statement = lines[lineno]

    with open(filename, "rt") as file:
        file_content = file.read()

    return text.replace(include_statement, file_content)

def lineof(text, substring):
    lines = text.split("\n")
    for i, line in enumerate(lines):
        if substring in line:
            return i

    return -1

DIRECTIVE_FUNCTIONS = {

}

def process(text):
    # First, look for all of the #include statements
    while True:
        # Find the first instance of #include currently in the text
        m = re.search(DIRECTIVES["include"], text)
        if m is None:
            break   # No more includes

        filename = m.group("fname")
        lineno = lineof(text, m.string[m.start():m.end()])

        text = directive_include(text, lineno, filename)

    # Next, work through the #define statements
    while True:
        m_def = re.search(DIRECTIVES["define"], text)

        if m_def is None:
            # No matches of #define left
            break

        start_def = lineof(text, m_def.string[m_def.start():m_def.end()])
        name = m_def.group("name")
        value = m_def.group("value")

        m_undef = re.search(DIRECTIVES["undef"], text)
        if m_undef is None or m_undef.group("name") != name:
            end_def = len(text.split("\n"))    # As in go to the last line
        else:
            end_def = lineof(text, m_undef.string[m_undef.start():m_undef.end()])

        lines = text.split("\n")
        for i, line in enumerate(lines):
            if start_def <= i <= end_def:
                lines[i] = line.replace(name, value)

        text = "\n".join(lines)

    return text

# Make an interactive version available
if __name__ == "__main__":
    with open("testing/csamples/define.c", "rt") as f:
        print(process(f.read()))

    import code
    code.interact(local=locals())