section.meta
mem_amt=4

section.data
count VAR int 0
pow VAR int 1


section.text
MOV 4B esp 2048
MOV 4B ebp 2048
while_3c030e41 SUB uint esp 4
MOV 4B [esp] count
SUB uint esp 4
MOV 4B [esp] 5
MOV 4B edx [esp]
ADD uint esp 4
MOV 4B ecx [esp]
ADD uint esp 4
CMP uint ecx edx
JLT jmptrue_0d21f69d
JMP jmpfalse_0d21f69d
jmptrue_0d21f69d SUB esp 4
MOV 4B [esp] 1
JMP jmpcmpend_0d21f69d
jmpfalse_0d21f69d SUB esp 4
MOV 4B [esp] 0
jmpcmpend_0d21f69d MOV 4B eax eax
CMP int [esp] 1
JNE endwhile_3c030e41
SUB uint esp 4
MOV 4B [esp] count
SUB uint esp 4
MOV 4B [esp] 1
MOV 4B edx [esp]
ADD uint esp 4
MOV 4B ecx [esp]
ADD uint esp 4
ADD int ecx edx
SUB uint esp 4
MOV 4B [esp] ecx
LEA edi count
MOV 4B ecx [esp]
ADD uint esp 4
MOV 4B [edi] ecx
SUB uint esp 4
MOV 4B [esp] pow
SUB uint esp 4
MOV 4B [esp] 2
MOV 4B edx [esp]
ADD uint esp 4
MOV 4B ecx [esp]
ADD uint esp 4
MUL int ecx edx
SUB uint esp 4
MOV 4B [esp] ecx
LEA edi pow
MOV 4B ecx [esp]
ADD uint esp 4
MOV 4B [edi] ecx
SUB uint esp 4
MOV 4B [esp] pow
MOV 4B out [esp]
ADD uint esp 4
endwhile_3c030e41 MOV 4B eax eax
exit HLT