#include <stdio.h>
#include <stdlib.h>
#include "processor.h"

enum
{
    SIZE_OF_COMMAND = 5
};

int main(int argc, char** argv)
{
    int num_of_commands = 0; 
    if(argc < 2)
    {
        return NOT_ENOUGH_ARGV;
    }
    FILE* commands_file = fopen(argv[1], "rb");
    fread(&num_of_commands, sizeof(int), 1, commands_file);

    int* command_arr = (int*)calloc(num_of_commands * SIZE_OF_COMMAND, sizeof(int));
    fread(command_arr, sizeof(int), num_of_commands * SIZE_OF_COMMAND, commands_file);

    for(int i = 0; i < num_of_commands; i++)
    {
        printf("%X ", command_arr[i * SIZE_OF_COMMAND]);
        for(int j = 1; j < 5; j++)
        {
            printf("%d ", command_arr[i * 5 + j]);
        }
        printf("\n");
    }

    free(command_arr);
    return NO_ERRORS;
}