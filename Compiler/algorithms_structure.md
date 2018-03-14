# Algorithms and structure - Compiler

The general overall structure of the compiler is this:

```plantuml
@startuml
start
:Understand command line arguments;
:Perform preprocessing;
:Turn into syntax tree;
:Find list of global symbols;
:Find local variables and record them;
:Convert tree to code block objects;
:Generate assembly code;
stop
@enduml
```

```pseudocode
```

## Preprocessor

```plantuml
@startuml
:Take text;
while (find #include) is (found)
    :Read filename from statement;
    :Paste content into text;
endwhile (none left)
while (find #define) is (found)
    :Find line;
    :Find line of #undef;
    :Perform replacement between those lines;
    :Record which lines these were;
    :Delete #define and #undef statements;
endwhile (none left)
while (next line) is (found)
    :If line contains #ifdef, record it;
    :If line contains #ifndef, record it;
endwhile (none left)
:Cut out all parts that do not meet the info just gathered;
:Remove all lines starting with #;
@enduml
```

## Lexical and syntax analysis

This is all done by the pycparser library

## Global symbol parsing

```plantuml
@startuml
:Create table as empty list;
while (select node from tree) is (found)
    if (statement is a declaration?)
        :Record name, type and initial;
        :Add variable to table;
    endif
endwhile (none left)
@enduml
```

## parse_compound()

```plantuml
@startuml
:Get block, list of parents and globals;
:Add parent to block if known;
while (read next statement) is (found)
    if (statement is a declaration) is (true)
        :Get name and type;
        :Create local variable object;
        :Add variable to locals;
    endif
    if (is an if statement) is (true)
        :Run this function on true and false sub-statements;
    endif
    if (is a while statement) is (true)
        :Run this function on statement sub-block;
    endif
endwhile (none left)
stop
@enduml
```

## generate_code_block()

```plantuml
@startuml
:Take compound object, parent and global symbol list;
:Create empty code block;
:Add locals from compound to block;
:Add parent to block;
while (read next statement in compound) is (found)
    :Get instructions from statement;
    :Add to code block instructions;

    if (is an if statement) is (true)
        :Add true and false statements to code block;
    endif
    if (is a loop statement) is (true)
        :Add sub-statement to block;
    endif
endwhile (none left)
:Return code block;
@enduml
```

## produce_data_section()

```plantuml
@startuml
:Accept list of global symbols;
:Create variable dictionary and string for data section;
while (next global variable) is (found)
    :Add initial value and type from variable to dictionary;
endwhile (none left)
while (next item in dictionary) is (found)
    :Turn variable into text;
    :Add text to data section text;
endwhile (none left)
:Return text;
@enduml
```

## produce_text_section()

```plantuml
:Accept top level block and global variable list;
:Create assembly text and code block queue;
:Write assembly code to move stack and base pointer;
:Add top block to queue;
while (accept next block in queue) is (found)
    :Get name and block from queue;
    :Run code generation function on block;
    :Write code to text section;
endwhile (none left)
:Write assembly to exit;
:Return assembly code;
```