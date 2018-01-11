; Prints the fibonacci sequence up to 50 places
section.meta
mem_amt=2

section.data
temp VAR ushort 0
i VAR char 50

section.text
MOV eax 1
MOV ebx 1
loop MOV temp eax
ADD eax ebx
ADD ebx temp

SUB i 1
MOV out eax
CMP i 0
JNE loop