#include <stdio.h>
#include <errno.h>
#define NDEBUG
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "assembler.h"
#include "commands.h"

const char* out_file_name = "processor.out";

int get_num_of_args(int command_code)
{
    for(int i = 0; i < sizeof(COMMANDS_ATTRIBUTES) / sizeof(command_asm_code); i++)
    {
        if(COMMANDS_ATTRIBUTES[i].command_code == command_code)
        {
            return COMMANDS_ATTRIBUTES[i].max_num_of_args;
        }
    }
}

char* get_command_str(int command_code)
{
    for(int i = 0; i < sizeof(COMMANDS_ATTRIBUTES) / sizeof(command_asm_code); i++)
    {
        if(COMMANDS_ATTRIBUTES[i].command_code == command_code)
        {
            return COMMANDS_ATTRIBUTES[i].command_str;
        }
    }
}

int get_file_size(char* file_name, int* size_of_file)
{
    assert(file_name != NULL);

    if(file_name == NULL || size_of_file == NULL)
    {
        errno = NULL_POINTER;
        return NULL_POINTER;
    }
    else
    {
        struct stat st = {};;
        stat(file_name, &st);
        *size_of_file = st.st_size;
    }
    return NO_ERRORS;
}

int read_file(char* file_name, char** str_ptr, int* size_of_file)
{
    assert(str_ptr != NULL);
    assert(*str_ptr != NULL);
    assert(size_of_file != NULL);
    assert(file_name != NULL);

    if(str_ptr == NULL || size_of_file == NULL || file_name == NULL)
    {
        errno = NULL_POINTER;
        return NULL_POINTER;
    }

    int file_desc = open(file_name, O_RDONLY);

    *str_ptr = (char*)calloc(*size_of_file + 2, sizeof(char));
    *size_of_file = read(file_desc, *str_ptr, *size_of_file);

    close(file_desc);

    return NO_ERRORS;
}

int liner_text(char* file, int size_of_file, int* num_of_lines, line_poz** lines, int space_divide_flag)
{
    assert(file != NULL);
    assert(num_of_lines != NULL);
    assert(lines != NULL);

    if(num_of_lines == NULL || file == NULL || lines == NULL)
    {
        errno = NULL_POINTER;
        return NULL_POINTER;
    }

    *num_of_lines = 0;
    (*lines)[*num_of_lines].start = file;
    bool space_flag = true;

    for(int i = 0; i < size_of_file; i++)
    {
        if((file[i] == '\n' || file[i] == '\r' || (space_divide_flag && file[i] == ' ')) && !space_flag)
        {
            file[i] = '\0';
            (*lines)[*num_of_lines].length = strlen((*lines)[*num_of_lines].start);
            (*num_of_lines)++;
            space_flag = true;
        }

        if(!isspace(file[i]) && file[i] != '\0' && space_flag)
        {
            space_flag = false;
            (*lines)[*num_of_lines].start = file + i;
            (*lines)[*num_of_lines].length = strlen((*lines)[*num_of_lines].start);
        }
    }
    (*num_of_lines)++;

    *lines = (line_poz*)realloc((*lines), (*num_of_lines) * sizeof(line_poz));
    return NO_ERRORS;
}

int read_command_arg(line_poz arg_str, label labels_arr[], int num_of_labels, int command_code, int* num_arg, int* reg_arg,
int* num_arg_flag, int* reg_arg_flag, int* ram_arg_flag)
{
    if(num_arg == NULL || reg_arg == NULL || num_arg_flag == NULL ||
    reg_arg_flag == NULL || ram_arg_flag == NULL)
    {
        return NULL_POINTER;
    }

    *num_arg = 0;
    *reg_arg = 0;
    *num_arg_flag = 0;
    *reg_arg_flag = 0;
    *ram_arg_flag = 0;
    if(sscanf(arg_str.start, "%d", num_arg))
    {
        *num_arg_flag = 1;
        return NO_ERRORS;
    }
    if(command_code == CMD_JMP)
    {
        for(int i = 0; i < num_of_labels; i++)
        {
            if(strcmp(labels_arr[i].label_name, arg_str.start) == 0)
            {
                *num_arg_flag = 1;
                *num_arg = labels_arr[i].address;
                return NO_ERRORS;
            }
        }
        return WRONG_LABEL_NAME;
    }

}

int asm_to_file(FILE* file_ptr, line_poz* lines_command_arr, int num_of_lines)
{
    label labels_arr[num_of_lines];
    int num_of_labels = 0;

    for(int i = 0; i < num_of_lines; i++)
    {
        line_poz* command_content = (line_poz*)calloc(3, sizeof(line_poz));
        int num_of_args = 0;
        liner_text(lines_command_arr[i].start, strlen(lines_command_arr[i].start),
        &num_of_args, &command_content, 1);

        int curr_command_code = 0;

        int label_flag = 1;
        for (int j = 0; j < sizeof(COMMANDS_ATTRIBUTES) / sizeof(command_asm_code); j++)
        {
            if(strcmp(COMMANDS_ATTRIBUTES[j].command_str, command_content[0].start) == 0)
            {
                curr_command_code = COMMANDS_ATTRIBUTES[j].command_code;
                label_flag = 0;
            }
        }
        if(label_flag)
        {
            if(command_content[0].length >= 2 && (*(command_content[0].start + command_content[0].length - 1) == ':'))
            {
                *(command_content[0].start + command_content[0].length - 1) = '\0';
                labels_arr[num_of_labels].address = i;
                labels_arr[num_of_labels].label_name = command_content[0].start;
                curr_command_code = CMD_LBL;
                num_of_labels++;
            }
        }

        printf("%d ", curr_command_code);

        if(num_of_args > get_num_of_args(curr_command_code) + 1)
        {
            free(command_content);
            return WRONG_NUM_OF_ARGS;
        }

        for(int j = 1; j < num_of_args; j++)
        {
            int num_arg = 0;
            int reg_arg = 0;
            int num_arg_flag = 0;
            int reg_arg_flag = 0;
            int ram_arg_flag = 0;
            read_command_arg(command_content[j], labels_arr, num_of_labels, curr_command_code,
            &num_arg, &reg_arg, &num_arg_flag, &reg_arg_flag, &ram_arg_flag);
            printf("%d %d ", num_arg, reg_arg);
        }

        

        printf("\n");


        free(command_content);
    }

    for(int i = 0; i < num_of_labels; i++)
    {
        printf("%d %s\n", labels_arr[i].address, labels_arr[i].label_name);
    }
    return NO_ERRORS;   
}

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        return NOT_ENOUGH_ARGV;
    }

    int size_of_file = 0;
    get_file_size(argv[1], &size_of_file);

    char* file_str = NULL;
    read_file(argv[1], &file_str, &size_of_file);

    int num_of_commands = 0;
    line_poz* lines_command_arr = (line_poz*)calloc(size_of_file, sizeof(line_poz));
    liner_text(file_str, size_of_file, &num_of_commands, &lines_command_arr, 0);



    for(int i = 0; i < num_of_commands; i++)
    {
       // printf("%s\n", lines_command_arr[i].start);
    }

    FILE* proc_command_file = fopen(argv[2], "w");

    asm_to_file(proc_command_file, lines_command_arr, num_of_commands);


    free(lines_command_arr);
    free(file_str);

    return NO_ERRORS;
}