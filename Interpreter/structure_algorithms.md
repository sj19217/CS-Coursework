# Structure of the solution and algorithms - Interpreter

The pseudocode for the main function is this:

```pseudocode
interactive = False
filename = ""
has_filename = False

for arg in arguments
    if arg == 'i':
        interactive = True
    else if arg == 'f':
        has_filename = True
        filename = arg.value

if not has_filename:
    filename = input("Filename: ")

file = openRead(filename)
content = file.read()

if interactive_mode:
    config.interactive_mode = True

run(content, file.length)
```

The general UML activity diagram for this is:

```plantuml
@startuml
start

:Set variables:
:interactive=False;
:filename="";
:has_filename=False;

while (more arguments?)
    if (argument='i') then (yes)
        :Set interactive to True;
    elseif (argument='f') then (yes)
        :Set has_filename to True;
        :Set filename=argument.value;
    endif
endwhile

if (has_filename?)
    :Ask for file name;
endif

:Open file;
:Read content of file;

if (interactive)
    :Set global config to
    interactive mode;
endif

:Run run();
@enduml
```