section.meta
mem_amt=4

section.data
count VAR int 0
pow VAR int 1


section.text
MOV 4B esp 2048
MOV 4B ebp 2048
; Beginning while loop
while_5a227c2e ; Pushing global variable to stack
SUB uint esp 4
MOV 4B [esp] count
; Pushing constant to stack
SUB uint esp 4
MOV 4B [esp] 5
; Evaluating binary expression: Popping values to registers.
MOV 4B edx [esp]
ADD uint esp 4
MOV 4B ecx [esp]
ADD uint esp 4
; Making comparison (<)
CMP uint ecx edx
JLT jmptrue_a9b95a09
JMP jmpfalse_a9b95a09
jmptrue_a9b95a09 SUB esp 4
MOV 4B [esp] 1
JMP jmpcmpend_a9b95a09
jmpfalse_a9b95a09 SUB esp 4
MOV 4B [esp] 0
jmpcmpend_a9b95a09 MOV 4B eax eax    ; Determined truth and added to stack
CMP int [esp] 1  ; See if true and jump accordingly
JNE endwhile_5a227c2e
; Running child block
; Pushing global variable to stack
SUB uint esp 4
MOV 4B [esp] count
; Pushing constant to stack
SUB uint esp 4
MOV 4B [esp] 1
; Evaluating binary expression: Popping values to registers.
MOV 4B edx [esp]
ADD uint esp 4
MOV 4B ecx [esp]
ADD uint esp 4
; Performing + and pushing to stack
ADD int ecx edx
SUB uint esp 4
MOV 4B [esp] ecx
; Assigning top of stack to variable count
LEA edi count      ; Pointer to a global
; Move from stack to variable
MOV 4B ecx [esp]
ADD uint esp 4
MOV 4B [edi] ecx
; Pushing global variable to stack
SUB uint esp 4
MOV 4B [esp] pow
; Pushing constant to stack
SUB uint esp 4
MOV 4B [esp] 2
; Evaluating binary expression: Popping values to registers.
MOV 4B edx [esp]
ADD uint esp 4
MOV 4B ecx [esp]
ADD uint esp 4
; Performing * and pushing to stack
MUL int ecx edx
SUB uint esp 4
MOV 4B [esp] ecx
; Assigning top of stack to variable pow
LEA edi pow      ; Pointer to a global
; Move from stack to variable
MOV 4B ecx [esp]
ADD uint esp 4
MOV 4B [edi] ecx
; Pushing global variable to stack
SUB uint esp 4
MOV 4B [esp] pow
; Calling printf
MOV 4B out [esp]
ADD uint esp 4
endwhile_5a227c2e MOV 4B eax eax
exit HLT