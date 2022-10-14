#ifndef COMMANDS_H
#define COMMANDS_H

struct command_asm_code
{
    char* command_str;
    int command_code;
    int max_num_of_args;
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

command_asm_code commands_attributes[] = 
{
    {"PUSH", CMD_PUSH, 1},
    {"ADD", CMD_ADD, 0},
    {"SUB", CMD_SUB, 0},
    {"MUL", CMD_MUL, 0},
    {"DIV", CMD_DIV, 0},
    {"OUT", CMD_HLT, 0},
    {"JMP", CMD_JMP, 1}
};

#endif