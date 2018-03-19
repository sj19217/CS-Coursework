import re

def optimise(text: str) -> str:
    """
    Performs some basic optimisation and returns a (hopefully) more efficient version of the assembly.
    :param text:
    :return:
    """

    # Record which lines are just the changing of a register by an immediate amount
    lines = text.split("\n")
    reg_chg_data = [[] for _ in lines]
    for i, line in enumerate(lines):
        # Optional label and space, then ADD uint, then a register name then a number
        m = re.match("^(?P<op>ADD|SUB) uint (?P<rname>eax|ebx|ecx|edx|esi|edi|esp|ebp) (?P<amt>\d*)", line)
        if m is None:
            reg_chg_data[i].append(("none", 0))
        else:
            # Dissect the match
            operation = m.group("op")
            rname = m.group("rname")
            amt = int(m.group("amt"))

            # If it was subtracted then the change is negative
            if operation.upper() == "SUB":
                amt = 0 - amt

            # Record the change
            reg_chg_data[i].append((rname, amt))

    for i in range(1, len(lines)):
        this_rname, this_amt = reg_chg_data[i][0]
        last_rname, last_amt = reg_chg_data[i-1][0]
        if this_rname != last_rname or this_rname == "none":
            continue    # Nothing to learn
        # This register and the last one are the same and are not "none"
        # Remove last one, replace this one with correct change
        lines[i-1] = ""
        total_change = this_amt + last_amt
        if total_change == 0:
            lines[i] = ""   # They cancel each other out
        elif total_change > 0:
            lines[i] = "ADD uint {} {}".format(this_rname, total_change)
        elif total_change < 0:
            lines[i] = "SUB uint {} {}".format(this_rname, 0-total_change)

    # Stitch back together and remove empty lines
    text = "\n".join(lines)
    text = re.sub("\n+", "\n", text)

    return text

optimise("SUB uint esp 4\nADD uint esp 4")