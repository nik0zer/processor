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
            (*lines)[*num_of_lines].length = &file[i] - (*lines)[*num_of_lines].start + 1;
            file[i] = '\0';
            (*num_of_lines)++;
            space_flag = true;
        }

        if(!isspace(file[i]) && file[i] != '\0' && space_flag)
        {
            space_flag = false;
            (*lines)[*num_of_lines].start = file + i;
        }
    }
    (*num_of_lines)++;

    *lines = (line_poz*)realloc((*lines), (*num_of_lines) * sizeof(line_poz));
    return NO_ERRORS;
}

int asm_to_file(FILE* file_ptr, line_poz* lines_command_arr, int num_of_lines)
{
    label labels_arr[num_of_lines];
    int num_of_labels = 0;

    for(int i = 0; i < num_of_lines; i++)
    {
        line_poz* command_content = (line_poz*)calloc(3, sizeof(line_poz));
        int num_of_args;
        liner_text(lines_command_arr[i].start, strlen(lines_command_arr[i].start),
        &num_of_args, &command_content, 1);
        int label_flag = 1;
        for (int j = 0; j < sizeof(commands_attributes) / sizeof(command_asm_code); j++)
        {
            if(strcmp(commands_attributes[j].command_str, command_content[0].start) == 0)
            {
                printf("%d\n", commands_attributes[j].command_code);
                label_flag = 0;
            }
        }
        if(label_flag)
        {
            
        }
        free(command_content);
    }

    
        
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