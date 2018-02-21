; Prints the fibonacci sequence up to 50 places
section.meta
mem_amt=2

section.data
temp VAR ushort 0
i VAR char 50

section.text
MOV eax 1
MOV ebx 1
loop MOV 2B temp eax
ADD ushort eax ebx
;ADD short ebx temp
MOV 2B ebx temp

SUB char i 1
MOV out eax
CMP char i 0
JNE loop