#ifndef ASSEMBLER_H
#define ASSEMBLER_H

enum ERRORS
{
    NO_ERRORS = 0,
    NULL_POINTER = 1,
    NO_CODE_FILE = 2
};

struct line_poz
{
    char* start;
    int length;
};


#endif