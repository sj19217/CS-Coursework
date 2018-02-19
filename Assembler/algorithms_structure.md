# Algorithms - assembler
This is a pseudocode breakdown of the assembler.

## Main function
The pseudocode for the main function is this:

```
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