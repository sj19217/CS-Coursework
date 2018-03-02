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
JE jmptrue_3aea7788
JMP jmpfalse_3aea7788
jmptrue_3aea7788 SUB esp 4
MOV 4B [esp] 1
JMP jmpcmpend_3aea7788
jmpfalse_3aea7788 SUB esp 4
MOV 4B [esp] 0
jmpcmpend_3aea7788 MOV 4B eax eax
CMP int [esp] 1
ADD uint esp 4
JNE else_caae8682
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
else_caae8682 SUB uint esp 4
MOV 4B [esp] a
MOV 4B out [esp]
ADD uint esp 4
endif_caae8682 MOV 4B eax eax
exit HLT