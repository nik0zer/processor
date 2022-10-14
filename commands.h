#ifndef COMMANDS_H
#define COMMANDS_H

struct command_asm_code
{
    char* command_str;
    int command_code;
};

enum COMMANDS
{
    CMD_PUSH = 1,
    CMD_ADD = 2,
    CMD_SUB = 3,
    CMD_MUL = 4,
    CMD_DIV = 5,
    CMD_OUT = 6,
    CMD_HLT = 7,
    CMD_JMP = 10
};

command_asm_code commands_code[] = 
{
    {"PUSH", CMD_PUSH},
    {"ADD", CMD_ADD},
    {"SUB", CMD_SUB},
    {"MUL", CMD_MUL},
    {"DIV", CMD_DIV},
    {"OUT", CMD_HLT},
    {"JMP", CMD_JMP}
};

#endif