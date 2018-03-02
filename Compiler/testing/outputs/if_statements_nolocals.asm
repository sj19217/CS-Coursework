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
JE jmptrue_49587a2e
JMP jmpfalse_49587a2e
jmptrue SUB esp 4
MOV 4B [esp] 1
JMP jmpcmpend_49587a2e
jmpfalse_49587a2e SUB esp 4
MOV 4B [esp] 0
jmpcmpend_49587a2e MOV 4B eax eax
CMP int [esp] 1
ADD uint esp 4
JNE else_0e59e4da
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
SUB uint esp 4
MOV 4B [esp] a
MOV 4B out [esp]
ADD uint esp 4
endif_0e59e4da MOV 4B eax eax
exit HLT