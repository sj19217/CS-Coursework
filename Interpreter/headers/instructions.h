//
// A number of definitions to record the numerical forms of opcodes. Simplifies code.
//

#ifndef INTERPRETER_INSTRUCTIONS_H
#define INTERPRETER_INSTRUCTIONS_H

#endif //INTERPRETER_INSTRUCTIONS_H


#define HLT 0x00

#define CMP_char 0x01
#define CMP_uchar 0x02
#define CMP_short 0x03
#define CMP_ushort 0x04
#define CMP_int 0x05
#define CMP_uint 0x06
#define CMP_float 0x07

#define JMP 0x08
#define JE 0x09
#define JNE 0x0A
#define JLT 0x0B
#define JLE 0x0C
#define JGT 0x0D
#define JGE 0x0E

#define MOV_1B 0x10
#define MOV_2B 0x11
#define MOV_4B 0x12

#define LEA 0x14

#define ADD_char 0x20
#define ADD_uchar 0x21
#define ADD_short 0x22
#define ADD_ushort 0x23
#define ADD_int 0x24
#define ADD_uint 0x25
#define ADD_float 0x26

#define SUB_char 0x28
#define SUB_uchar 0x29
#define SUB_short 0x2A
#define SUB_ushort 0x2B
#define SUB_int 0x2C
#define SUB_uint 0x2D
#define SUB_float 0x2E

#define MUL_char 0x30
#define MUL_uchar 0x31
#define MUL_short 0x32
#define MUL_ushort 0x33
#define MUL_int 0x34
#define MUL_uint 0x35
#define MUL_float 0x36

#define IDIV_char 0x38
#define IDIV_uchar 0x39
#define IDIV_short 0x3A
#define IDIV_ushort 0x3B
#define IDIV_int 0x3C
#define IDIV_uint 0x3D
#define IDIV_float 0x3E

#define MOD_char 0x40
#define MOD_uchar 0x41
#define MOD_short 0x42
#define MOD_ushort 0x43
#define MOD_int 0x44
#define MOD_uint 0x45
#define MOD_float 0x46

#define EDIV_char 0x48
#define EDIV_uchar 0x49
#define EDIV_short 0x4A
#define EDIV_ushort 0x4B
#define EDIV_int 0x4C
#define EDIV_uint 0x4D
#define EDIV_float 0x4E

#define AND_1B 0x50
#define AND_2B 0x51
#define AND_4B 0x52

#define OR_1B 0x54
#define OR_2B 0x55
#define OR_4B 0x56

#define XOR_1B 0x58
#define XOR_2B 0x59
#define XOR_4B 0x5A

#define NOT_1B 0x5C
#define NOT_2B 0x5D
#define NOT_4B 0x5E

#define LSH_1B 0x60
#define LSH_2B 0x61
#define LSH_4B 0x62

#define RSH_1B 0x64
#define RSH_2B 0x65
#define RSH_4B 0x66