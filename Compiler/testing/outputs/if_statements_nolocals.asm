section.meta
mem_amt=4

section.data
a VAR int 5


section.text
MOV 4B esp 2048
MOV 4B ebp 2048
SUB uint esp 4
MOV 4B [esp] a
SUB uint esp 4
MOV 4B [esp] 5
MOV 4B edx [esp]
ADD uint esp 4
MOV 4B ecx [esp]
ADD uint esp 4
CMP uint ecx edx
JE jmptrue_3c5d585a
JMP jmpfalse_3c5d585a
jmptrue_3c5d585a SUB esp 4
MOV 4B [esp] 1
JMP jmpcmpend_3c5d585a
jmpfalse_3c5d585a SUB esp 4
MOV 4B [esp] 0
jmpcmpend_3c5d585a MOV 4B eax eax
CMP int [esp] 1
ADD uint esp 4
JNE else_95fb3b7f
SUB uint esp 4
MOV 4B [esp] a
SUB uint esp 4
MOV 4B [esp] 5
MOV 4B edx [esp]
ADD uint esp 4
MOV 4B ecx [esp]
ADD uint esp 4
ADD int ecx edx
SUB uint esp 4
MOV 4B [esp] ecx
MOV 4B out [esp]
ADD uint esp 4
JMP endif_95fb3b7f
else_95fb3b7f SUB uint esp 4
MOV 4B [esp] a
MOV 4B out [esp]
ADD uint esp 4
endif_95fb3b7f MOV 4B eax eax
exit HLT