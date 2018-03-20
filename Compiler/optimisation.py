import re
import hashlib
import random

def generate_rand8():
    hash_obj = hashlib.md5()
    hash_obj.update(str(random.randint(0, 10**10)).encode())
    return hash_obj.hexdigest()[-8:]

def optimise(text: str) -> str:
    """
    Performs some basic optimisation and returns a (hopefully) more efficient version of the assembly.
    :param text:
    :return:
    """

    original_text = text

    # POST-CODEGEN OPTIMISATION 1 - MERGE CHANGES TO SAME REGISTER
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
        if i != len(lines) - 1 and this_rname == reg_chg_data[i+1][0][0]:
            # The next register is also the same as this one, so do nothing
            continue
        # Remove last one, replace this one with correct change
        lines[i-1] = ""
        total_change = this_amt + last_amt
        if total_change == 0:
            lines[i] = ""   # They cancel each other out
        elif total_change > 0:
            lines[i] = "ADD uint {} {}".format(this_rname, total_change)
        elif total_change < 0:
            lines[i] = "SUB uint {} {}".format(this_rname, 0-total_change)

    # Stitch back together, removing empty lines
    text = "\n".join(l for l in lines if l)




    # POST-CODEGEN OPTIMISATION 2 - MERGE BLOCK ENDINGS
    lines = text.split("\n")
    end_block_regex = re.compile("^(?P<label>[a-zA-Z_0-9]+)\s*MOV 4B eax eax", re.IGNORECASE)
    for i in range(1, len(lines)):
        # Loop through the lines, with each looking back at the last as well
        this_match = re.match(end_block_regex, lines[i])
        last_match = re.match(end_block_regex, lines[i-1])
        if this_match is None or last_match is None:
            # Either this line or the last was not a block ending, so do nothing
            continue

        # Both this line and the last were matches
        this_label = this_match.group("label")
        last_label = last_match.group("label")
        new_label = "end_" + generate_rand8()

        # Step 1: Replace the two lines with one labelled statement
        lines[i-1] = ""
        lines[i] = "{label} MOV 4B eax eax".format(label=new_label)

        # Step 2: Replace all occurences of both old labels with the new one
        lines = [(l+" ").replace(" " + last_label + " ", " " + new_label + " ").rstrip() for l in lines]
        lines = [(l+" ").replace(" " + this_label + " ", " " + new_label + " ").rstrip() for l in lines]

    # Stitch back together and remove empty lines, again
    text = "\n".join(l for l in lines if l)




    # POST-CODEGEN OPTIMISATION 2 - REMOVE END BLOCK INSTRUCTIONS AND MOVE LABELS
    lines = text.split("\n")
    op_name_regex = re.compile("(HLT|CMP|JMP|JE|JNE|JLT|JLE|JGT|JGE|MOV|LEA|" + \
                               "ADD|SUB|MUL|IDIV|MOD|EDIV|AND|OR|XOR|NOT|LSH|RSH)", re.IGNORECASE)
    replacements_to_make = []
    for i in range(len(lines) - 1):
        # Loop through the lines looking to see if this line then the next can be acted on
        endblock_match = re.match(end_block_regex, lines[i])
        if endblock_match is None:
            # Don't bother anyway, this isn't a statement end block
            continue
        # This is a statement end block
        # Split the next line by its operation, then we know its label
        next_op_parts = re.split(op_name_regex, lines[i+1])
        if len(next_op_parts) < 3 or len(next_op_parts) > 3:
            # Cannot quite sure what's happening, just skip it
            continue
        # It has 2 parts, a label then a data type/some operands
        label, _, rest = next_op_parts
        if label.strip():
            # It does have a label, so replace all instances of end block label with this one
            replacements_to_make.append((endblock_match.group("label"), label.strip()))
        else:
            # It does not have a label, so give it one
            lines[i+1] = endblock_match.group("label") + " " + lines[i+1]
        # Any changes to the useful line have been made, now get rid of end block line
        lines[i] = ""

    # Put it back into text
    text = "\n".join(l for l in lines if l)

    # Then on that text, perform the replacements
    for from_, to_ in replacements_to_make:
        text = re.sub(r"\b{}\b".format(from_), to_, text)

    if text == original_text:
        # Nothing changed, done all we can
        return text
    else:
        # Could use another go round
        return optimise(text)