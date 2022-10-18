#ifndef PROCESSOR_H
#define PROCESSOR_H

enum ERRORS
{
    FILE_OPEN_ERROR = 1,
    NOT_ENOUGH_ARGV = 2,
    WRONG_ARGS = 3
};

struct args_flags
{
    int reg_flag_1 = 0;
    int num_flag_1 = 0;
    int ram_flag_1 = 0;
    int reg_flag_2 = 0;
    int num_flag_2 = 0;
    int ram_flag_2 = 0;
};

enum proc_constants
{
    SIZE_OF_RAM = 1024
};

#endif