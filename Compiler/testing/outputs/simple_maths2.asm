section.meta
mem_amt=4

section.data
a VAR int 5


section.text
MOV 4B esp 2048
MOV 4B ebp 2048
block SUB uint esp 4
MOV 4B [esp] ebp
ADD uint esp 4
MOV 4B ebp esp
SUB uint esp 4
SUB uint esp 4
MOV 4B [esp] 0
SUB uint esp 4
MOV 4B [esp] a
SUB uint esp 4
MOV 4B [esp] 1
MOV 4B ecx [esp]
ADD uint esp 4
MOV 4B edx [esp]
ADD uint esp 4
ADD int ecx edx
SUB uint esp 4
MOV 4B [esp] ecx
MOV 4B edi esp
SUB uint edi 8
MOV 4B ecx [esp]
ADD uint esp 4
MOV 4B [edi] ecx
SUB uint esp 4
MOV 4B esi ebp
SUB uint esi 8
MOV 4B [esp] [esi]
MOV 4B out [esp]
ADD uint esp 4
MOV 4B esi ebp
SUB uint esi 4
MOV 4B ebp [esi]
JMP exit
exit HLT


