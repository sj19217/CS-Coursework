; Prints the fibonacci sequence up to 50 places
section.meta
mem_amt=2

section.data
temp VAR uint 0
i VAR char 40

section.text
MOV 4B eax 1
MOV 4B ebx 1
loop MOV 4B temp eax
ADD uint eax ebx
;ADD short ebx temp
MOV 4B ebx temp

SUB char i 1
MOV 4B out eax
CMP char i 0
JNE loop
HLT