#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "disassembler.h"

#define GET_REG_FLAG(ARG_MASK)  ARG_MASK & 0x01
#define GET_NUM_FLAG(ARG_MASK) (ARG_MASK & 0x02) >> 1
#define GET_RAM_FLAG(ARG_MASK) (ARG_MASK & 0x04) >> 2

#define ARG_FLAGS_1_MASK 0xF

char* get_command_str(int op_code)
{
    for(int i = 0; i < SIZEOF_COMMANDS_ATTRIBUTES; i++)
    {
        if(COMMANDS_ATTRIBUTES[i].op_code == op_code)
        {
            return COMMANDS_ATTRIBUTES[i].command_str;
        }
    }
}

int get_num_of_args(int op_code)
{
    for(int i = 0; i < SIZEOF_COMMANDS_ATTRIBUTES; i++)
    {
        if(COMMANDS_ATTRIBUTES[i].op_code == op_code)
        {
            return COMMANDS_ATTRIBUTES[i].max_num_of_args;
        }
    }
}

char* get_reg_str(int op_code)
{
    for(int i = 0; i < SIZEOF_REGISTERS_ATTRIBUTES; i++)
    {
        if(REGISTERS_ATTRIBUTES[i].register_code == op_code)
        {
            return REGISTERS_ATTRIBUTES[i].register_str;
        }
    }
}

int print_one_command(FILE* commands_file, int* command_content, FILE* listing_file, int num_of_command)
{
    fprintf(listing_file, "address = %d\n", num_of_command);
    fprintf(listing_file, "%X ", *command_content);
    int arg_mask = (*command_content) & 0x000000FF;
    int num_of_args = (*command_content >> BYTE) & 0x000000FF;
    int op_code = (*command_content >> (2 * BYTE));

    for(int i = 1; i <= num_of_args; i++)
    {
        fprintf(listing_file, "%X ", command_content[i]);
    }

    fprintf(listing_file, "\n");

    if(op_code == CMD_LBL)
    {
        fprintf(listing_file, "label with adress: %d", num_of_command);
    }
    else
    {
        fprintf(listing_file, "%s ", get_command_str(op_code));
    }

    


    int arg_counter = 1;

    char first_arg[SIZE_OF_ARG] = {};
    char second_arg[SIZE_OF_ARG] = {};

    if(arg_mask & ARG_FLAGS_1_MASK && get_num_of_args(op_code) >= 1)
    {
        char* arg_str_ptr = first_arg;
        if(GET_RAM_FLAG(arg_mask))
        {
            *arg_str_ptr = '[';
            arg_str_ptr++;
        }
        if(GET_REG_FLAG(arg_mask))
        {
            sprintf(arg_str_ptr, "%s\0", get_reg_str(command_content[arg_counter]));
            arg_str_ptr += strlen(get_reg_str(command_content[arg_counter]));
            arg_counter++;
            if(GET_NUM_FLAG(arg_mask))
            {
                sprintf(arg_str_ptr, "+%d\0", command_content[arg_counter]);
                arg_str_ptr = arg_str_ptr + strlen(arg_str_ptr);
                arg_counter++;
            }
        }
        else
        {
            if(GET_NUM_FLAG(arg_mask))
            {
                sprintf(arg_str_ptr, "%d\0", command_content[arg_counter]);
                arg_str_ptr = arg_str_ptr + strlen(arg_str_ptr);
                arg_counter++;
            }
        }

        if(GET_RAM_FLAG(arg_mask))
        {
            *arg_str_ptr = ']';
            arg_str_ptr++;
        }
        fprintf(listing_file, "%s ", first_arg);
    }

    printf("%s ", first_arg);

    arg_mask = arg_mask >> HALF_BYTE;
    printf("\n%d %d ", num_of_command, arg_mask);
    if(arg_mask & ARG_FLAGS_1_MASK && get_num_of_args(op_code) >= 2)
    {
        char* arg_str_ptr = second_arg;
        if(GET_RAM_FLAG(arg_mask))
        {
            *arg_str_ptr = '[';
            arg_str_ptr++;
        }
        if(GET_REG_FLAG(arg_mask))
        {
            sprintf(arg_str_ptr, "%s\0", get_reg_str(command_content[arg_counter]));
            arg_str_ptr += strlen(get_reg_str(command_content[arg_counter]));
            arg_counter++;
            if(GET_NUM_FLAG(arg_mask))
            {
                sprintf(arg_str_ptr, "+%d\0", command_content[arg_counter]);
                arg_str_ptr = arg_str_ptr + strlen(arg_str_ptr);
                arg_counter++;
            }
        }
        else
        {
            if(GET_NUM_FLAG(arg_mask))
            {
                sprintf(arg_str_ptr, "%d\0", command_content[arg_counter]);
                arg_str_ptr = arg_str_ptr + strlen(arg_str_ptr);
                arg_counter++;
            }
        }

        if(GET_RAM_FLAG(arg_mask))
        {
            *arg_str_ptr = ']';
            arg_str_ptr++;
        }
        fprintf(listing_file, "%s ", second_arg);
    }

    fprintf(listing_file, "\n");

    return NO_ERRORS;
}

int main(int argc, char** argv)
{
    int num_of_commands = 0; 
    if(argc < 3)
    {
        return NOT_ENOUGH_ARGV;
    }
    FILE* commands_file = fopen(argv[1], "rb");
    int command_counter = 0;
    fread(&num_of_commands, sizeof(int), 1, commands_file);

    int* command_arr = (int*)calloc(num_of_commands * SIZE_OF_COMMAND, sizeof(int));
    fread(command_arr, sizeof(int), num_of_commands * SIZE_OF_COMMAND, commands_file);

    FILE* listing_file = fopen(argv[2], "w");

    for(int i = 0; i < num_of_commands; i++)
    {
        print_one_command(commands_file, command_arr + i * SIZE_OF_COMMAND, listing_file, i);
    }
    return NO_ERRORS;
}