import re
import io

# DO NOT TOUCH THESE
DIRECTIVES = {
    "include": r"^\s*#\s*include\s+[<\"](?P<fname>[\w\.\\/]*)[>\"]\s*"
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

    return text

# Make an interactive version available
if __name__ == "__main__":
    with open("testing/csamples/include.c", "rt") as f:
        print(process(f.read()))

    import code
    code.interact(local=locals())