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
    WRONG_REG_ARG = 6
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


#endif