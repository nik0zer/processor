#ifndef ASSEMBLER_H
#define ASSEMBLER_H

enum ERRORS
{
    NO_ERRORS = 0,
    NULL_POINTER = 1,
    NOT_ENOUGH_ARGV = 2,
    WRONG_NUM_OF_ARGS = 3,
    WRONG_LABEL_NAME = 4,
    WRONG_NUM_ARG = 5,
    WRONG_REG_ARG = 6,
    WRITE_TO_FILE_ERROR = 7,
    NO_MAIN_LABEL = 8
};

struct line_poz
{
    char* start;
    int length;
};

struct label
{
    char* label_name;
    int address;
};

struct arg_val
{
    int num_arg = 0;
    int reg_arg = 0;
    int num_arg_flag = 0;
    int reg_arg_flag = 0;
    int ram_arg_flag = 0;
};



#endif