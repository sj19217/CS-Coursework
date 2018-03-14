# Algorithms - assembler

This is a pseudocode breakdown of the assembler.

## Main function

The pseudocode for the main function is this:

```pseudocode
Run normalise_text()
Run split_into_sections()
Run divide_and_contextualise()
Run record_labels_and_variables()
Run place_memory_addresses()
Create empty byte string
Add result of encode_metadata() to bytecode
Add result of encode_instruction_list() to bytecode
If output format is "hex":
    Print as hex
Else if output format is "binstr":
    Print as binary string
Else if output format is "return":
    Return binary result
Else if output format is "file":
    Ask for file to print to
    Write bytecode to file
```

The UML diagram for the overall program is:

```plantuml
@startuml
start
partition normalise_text() {
    :Remove comments;
    :Remove redundant whitespace;
    :Remove empty lines;
}
partition split_into_sections() {
    :Divide at instances of section.*;
    :Add parts to dict;
}
partition divide_and_contextualise() {
    :Create copy of default meta dictionary;
    :Split meta sections into lines and by = sign;
    :Add keys/values to dictionary;
    :Create a DataInstruction for each data line;
    :Create a TextInstruction for each text line;
    :Return the config dict and a combined list;
}
partition record_labels_and_variables() {
    :Add the line number of each labelled line to a list;
    :Add the line number of each DataInstruction to a list;
    :Turn these into a unified memory table;
}
partition place_memory_addresses() {
    repeat
        :Replace operand 1;
        :Replace operand 2;
    repeat while (more instructions?)
}
:Make empty byte string;
partition encode_metadata() {
    while (more config data?) is (yes)
        :Turn into bytes;
    end while (no)
}
:Add result to byte string;
partition encode_instruction_list() {
    while (more instructions?) is (yes)
        :Turn instruction to binary;
    end while (no)
}
:Add to bytecode;
:Output final bytecode;
@enduml
```

## normalise_text()

This function does the removal of comments and unnecessary whitespace.

```pseudocode
function normalise_text(text)
    lines = split(text)
    for i from 0 to lines.length:
        parts = split(lines[i], ";")
        lines[i] = parts[0]
    list non_blank
    for i from 0 to lines.length:
        if lines[i] != ""
            non_blank.append(lines[i])
    for i from 0 to non_blank.length:
        remove_whitespace(non_blank[i])
    string = non_blank[0]
    for i from 1 to non_blank.length
        string += "\n" + non_blank[i]
    return string
endfunction
```

```plantuml
@startuml
start
:Split into lines;

while (more lines?) is (yes)
    :Split by ";";
    :Strip whitespace;
    :Add first part back into list;
endwhile (no)

:Remove empty lines;

while (more lines?) is (yes)
    :Remove duplicate whitespace;
endwhile (no)

:Add together into string;
:Return string;
@enduml
```

## split_into_sections()

Takes normalised text and splits it into sections

```pseudocode
function split_into_sections(text)
    parts = split text on "section."
    parts_with_titles = []
    for i from 0 to parts.length:
        title_and_lines = split parts[i] on "\n", max=1
        parts_with_titles.append(title_and_lines)

    sections = new HashTable()
    for i = 0 to parts_with_titles.length:
        title = parts_with_titles[0]
        text = parts_with_titles[1]
        set sections["title"] to text

    return sections
endfunction
```

```plantuml
@startuml
start
:Split on appearances of "section.";
:Separate first line from others;
:Create hash table;
while (part of list remaining?) is (yes)
    :Add first value -> last value
    to the has table;
endwhile (no)

:Return hash table;
@enduml
```

## divide_and_contextualise()

```pseudocode
functions (sections)
    config_dict = {}
    meta = split sections("meta") on "\n"
    for i from 0 to meta.length:
        parts = split(meta[i], "=")
        key = parts[0]
        value = parts[1]
        config_gict.set(key, value)

    instruction_list = []
    data = split sections("data") on "\n"
    for i from 0 to data.length:
        parts = split data[i] on "VAR"
        right_parts = split parts on " "
        name = parts[0]
        type = right_parts[0]
        initial = right_parts[1]
        add DataInstruction(name, type, initial) to instruction_list

    commands = split sections("text") on "\n"
    for i from 0 to commands.length:
        
endfunction
```

```plantuml
@startuml
:Create meta dictionary;
:Divide meta section into lines;
while (More lines to go through?) is (yes)
    :Split line by = sign;
    :Add info to dict;
endwhile (no)
:Create list of instructions;
:Split data section into lines;
while (more lines remaining?) is (yes)
    :Divide by "VAR";
    :Split right hand up by space;
    :Create data instruction object and add to list;
endwhile (no)
:Split text section into lines;
while (more lines remaining?) is (yes)
    :Detect possible label;
    :Find mnemonic;
    :Detect type;
    :Find opcodes and turn into objects;
    :Create text instruction object and add to list;
endwhile (no)
:Return list and meta dict;
@enduml
```

## record_labels_and_variables()

```pseudocode
```

```plantuml
@startuml
:Get instruction list;
:Create label/variable table;
while (instructions remaining?) is (yes)
    if (is DataInstruction?) then (yes)
        :Add data to variable table;
    else if (is TextInstruction?) then (yes)
        :Add instruction number to label table if label is given;
    endif
endwhile (no)
:Add variables to memory table;
:Add labels to memory table;
@enduml
```

## place_memory_addresses()

```pseudocode
```

```plantuml
@startuml
:Get instruction list and memory table;
while (instructions remaining?) is (yes)
    if (operand1 is an address) then (yes)
        :Replace operand1 with memory address;
    endif
    if (operand2 is an address) then (yes)
        :Replace operand2 with memory address;
    endif
endwhile (no)
end
@enduml
```

## encode_metadata()

```pseudocode
```

```plantuml
@startuml
:Get config dictionary;
:Create byte string;
while (config item remaining?) is (yes)
    :Encode key;
    :Encode value;
    :Add both to byte string with = inbetween;
endwhile (no)
:Return byte string;
@enduml
```

## encode_instruction_list()

```pseudocode
```

```plantuml
@startuml
:Get instruction list and memory table;
:Create empty byte string;
while (instruction remaining) is (yes)
    :Get bytes of instruction;
    :Add bytes to byte string;
endwhile (no)
:Return byte string;
@enduml
```