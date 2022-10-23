#ifndef COMMANDS_H
#define COMMANDS_H

enum assembler_constants
{
    HALF_BYTE = 4,
    BYTE = 8,
    MAX_OF_READ_ARGS = 4,
    MAX_SIZE_OF_COMMAND_STR = 5,
    SIZE_OF_COMMAND = 5,
    SIZE_OF_ARG = 19,
    NUM_OF_REGISTERS = 8,
    NUM_OF_COMMANDS = 20
};


struct commands_informations
{
    char* command_str;
    int op_code;
    int max_num_of_args;
    int min_num_of_args;
};

struct registers_informations
{
    char* register_str;
    int register_code;
};

enum COMMANDS
{
    CMD_POP = 9,
    CMD_PUSH = 1,
    CMD_ADD = 2,
    CMD_SUB = 3,
    CMD_MUL = 4,
    CMD_DIV = 5,
    CMD_OUT = 6,
    CMD_HLT = 7,
    CMD_LBL = 8,
    CMD_JMP = 10,
    CMD_JB = 11,
    CMD_JBE = 12,
    CMD_JA = 13,
    CMD_JAE = 14,
    CMD_JE = 15,
    CMD_JNE = 16,
    CMD_RET = 17,
    CMD_IN = 18,
    CMD_CALL = 19
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
    REG_RSP = 7
};

const commands_informations COMMANDS_ATTRIBUTES[] = 
{
    {"pop", CMD_POP, 1, 1},
    {"push", CMD_PUSH, 1, 1},
    {"add", CMD_ADD, 2, 0},
    {"sub", CMD_SUB, 2, 0},
    {"mul", CMD_MUL, 2, 0},
    {"div", CMD_DIV, 2, 0},
    {"out", CMD_OUT, 1, 1},
    {"hlt", CMD_HLT, 0, 0},
    {"", CMD_LBL, 0, 0},
    {"jmp", CMD_JMP, 1, 1},
    {"jb", CMD_JB, 1, 1},
    {"jbe", CMD_JBE, 1, 1},
    {"ja", CMD_JA, 1, 1},
    {"jae", CMD_JAE, 1, 1},
    {"je", CMD_JE, 1, 1},
    {"jne", CMD_JNE, 1, 1},
    {"ret", CMD_RET, 0, 0},
    {"in", CMD_IN, 1, 0},
    {"call", CMD_CALL, 1, 1}
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

const int SIZEOF_REGISTERS_ATTRIBUTES = sizeof(REGISTERS_ATTRIBUTES) / sizeof(registers_informations);
const int SIZEOF_COMMANDS_ATTRIBUTES = sizeof(COMMANDS_ATTRIBUTES) / sizeof(commands_informations);

#endif