#ifndef COMMANDS_H
#define COMMANDS_H

enum assembler_constants
{
    HALF_BYTE = 4,
    BYTE = 8,
    MAX_OF_READ_ARGS = 4
};

struct commands_informations
{
    char* command_str;
    int opcode;
    int max_num_of_args;
};

struct registers_informations
{
    char* register_str;
    int register_code;
};

enum COMMANDS
{
    CMD_POP = 0,
    CMD_PUSH = 1,
    CMD_ADD = 2,
    CMD_SUB = 3,
    CMD_MUL = 4,
    CMD_DIV = 5,
    CMD_OUT = 6,
    CMD_HLT = 7,
    CMD_LBL = 8,
    CMD_JMP = 10
};

enum REGISTERS
{
    REG_RAX = 0,
    REG_RBX = 1,
    REG_RCX = 2,
    REG_RDX = 3,
    REG_RBP = 4,
    REG_RSI = 5,
    REG_RDI = 6,
    REG_RSP = 7,
    NUM_OF_REGISTERS = 8
};

const commands_informations COMMANDS_ATTRIBUTES[] = 
{
    {"pop", CMD_POP, 1},
    {"push", CMD_PUSH, 1},
    {"add", CMD_ADD, 2},
    {"sub", CMD_SUB, 2},
    {"mul", CMD_MUL, 2},
    {"div", CMD_DIV, 2},
    {"out", CMD_HLT, 2},
    {"", CMD_LBL, 0},
    {"jmp", CMD_JMP, 1}
};

const registers_informations REGISTERS_ATTRIBUTES[] = 
{
    {"rax", REG_RAX},
    {"rbx", REG_RBX},
    {"rcx", REG_RCX},
    {"rdx", REG_RDX},
    {"rbp", REG_RBP},
    {"rsi", REG_RSI},
    {"rdi", REG_RDI},
    {"rsp", REG_RSP}
};

#endif