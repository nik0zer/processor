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

const char* MAIN_LABEL_NAME = "main";

int get_num_of_args(int op_code)
{
    for(int i = 0; i < sizeof(COMMANDS_ATTRIBUTES) / sizeof(commands_informations); i++)
    {
        if(COMMANDS_ATTRIBUTES[i].op_code == op_code)
        {
            return COMMANDS_ATTRIBUTES[i].max_num_of_args;
        }
    }
}

int get_min_num_of_args(int op_code)
{
    for(int i = 0; i < sizeof(COMMANDS_ATTRIBUTES) / sizeof(commands_informations); i++)
    {
        if(COMMANDS_ATTRIBUTES[i].op_code == op_code)
        {
            return COMMANDS_ATTRIBUTES[i].min_num_of_args;
        }
    }
}

char* get_command_str(int op_code)
{
    for(int i = 0; i < sizeof(COMMANDS_ATTRIBUTES) / sizeof(commands_informations); i++)
    {
        if(COMMANDS_ATTRIBUTES[i].op_code == op_code)
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

    struct stat st = {};
    stat(file_name, &st);
    *size_of_file = st.st_size;
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

int liner_text(char* file, int size_of_file, int* num_of_lines, line_poz** lines_ptr, int space_divide_flag)
{
    assert(file != NULL);
    assert(num_of_lines != NULL);
    assert(lines != NULL);

    if(num_of_lines == NULL || file == NULL || lines_ptr == NULL)
    {
        errno = NULL_POINTER;
        return NULL_POINTER;
    }

    *num_of_lines = 0;
    auto lines = *lines_ptr;

    for(int i = 0; i < *num_of_lines; i++)
    {
        lines[i].start = NULL;
    }
    
    lines[*num_of_lines].start;
    bool space_flag = true;

    for(int i = 0; i < size_of_file; i++)
    {
        if((file[i] == '\n' || file[i] == '\r' || (space_divide_flag && file[i] == ' ')) && !space_flag)
        {
            file[i] = '\0';
            lines[*num_of_lines].length = strlen(lines[*num_of_lines].start);
            (*num_of_lines)++;
            space_flag = true;
        }

        if(!isspace(file[i]) && file[i] != '\0' && space_flag)
        {
            space_flag = false;
            lines[*num_of_lines].start = file + i;
            lines[*num_of_lines].length = strlen(lines[*num_of_lines].start);
        }
    }
    (*num_of_lines)++;

    (*lines_ptr) = (line_poz*)realloc(lines, (*num_of_lines) * sizeof(line_poz));
    return NO_ERRORS;
}

int read_command_arg(line_poz arg_str, label labels_arr[], int num_of_labels, int op_code, arg_val* read_arg_val, int* jmp_label_flag)
{
    if(read_arg_val == NULL)
    {
        return NULL_POINTER;
    }

    read_arg_val->num_arg = 0;
    read_arg_val->reg_arg = 0;
    read_arg_val->num_arg_flag = 0;
    read_arg_val->reg_arg_flag = 0;
    read_arg_val->ram_arg_flag = 0;

    if(sscanf(arg_str.start, "%d", &(read_arg_val->num_arg)))
    {
        read_arg_val->num_arg_flag = 1;
        return NO_ERRORS;
    }

    if(arg_str.start[0] == '[' && arg_str.start[arg_str.length - 1] == ']')
    {
        read_arg_val->ram_arg_flag = 1;
        arg_str.start[arg_str.length - 1] = '\0';
        arg_str.start++;
        arg_str.length = strlen(arg_str.start);
    }

    if(sscanf(arg_str.start, "%d", &(read_arg_val->num_arg)))
    {
        read_arg_val->num_arg_flag = 1;
        return NO_ERRORS;
    }

    char* divide_symbol_ptr = strchr(arg_str.start, '+');

    if(divide_symbol_ptr != NULL)
    {
        *divide_symbol_ptr = '\0';
        divide_symbol_ptr++;
        arg_str.length = strlen(arg_str.start);
        if(sscanf(divide_symbol_ptr, "%d", &(read_arg_val->num_arg)))
        {
            read_arg_val->num_arg_flag = 1;
        }
        else
        {
            return WRONG_NUM_ARG;
        }
    }

    for(int i = 0; i < sizeof(REGISTERS_ATTRIBUTES) / sizeof(registers_informations); i++)
    { 
        
        if(strcmp(arg_str.start, REGISTERS_ATTRIBUTES[i].register_str) == 0)
        {
            read_arg_val->reg_arg_flag = 1;
            read_arg_val->reg_arg = REGISTERS_ATTRIBUTES[i].register_code;
            return NO_ERRORS;
        }
    }

    if(op_code == CMD_JMP || op_code == CMD_JA || op_code == CMD_JAE || op_code == CMD_JB || op_code == CMD_JBE ||
    op_code == CMD_JE || op_code == CMD_JNE)
    {
        for(int i = 0; i < num_of_labels; i++)
        {
            if(strcmp(labels_arr[i].label_name, arg_str.start) == 0)
            {
                *jmp_label_flag = 1;
                read_arg_val->num_arg_flag = 1;
                read_arg_val->num_arg = labels_arr[i].address;
                return NO_ERRORS;
            }
        }
        return WRONG_LABEL_NAME;
    }

    return WRONG_REG_ARG;
}

int asm_to_file(FILE* file_ptr, line_poz* lines_command_arr, int num_of_lines)
{
    int main_address = -1;
    
    label labels_arr[num_of_lines];
    int num_of_labels = 0;

    for(int i = 0; i < num_of_lines; i++)
    {
        line_poz* command_content = (line_poz*)calloc(3, sizeof(line_poz));

        printf("%s\n", lines_command_arr[i].start);
        
        int num_of_args = 0;
        liner_text(lines_command_arr[i].start, strlen(lines_command_arr[i].start),
        &num_of_args, &command_content, 1);

        int op_code = 0;

        int label_flag = 1;
        for (int j = 0; j < sizeof(COMMANDS_ATTRIBUTES) / sizeof(commands_informations); j++)
        {
            if(strcmp(COMMANDS_ATTRIBUTES[j].command_str, command_content[0].start) == 0)
            {
                op_code = COMMANDS_ATTRIBUTES[j].op_code;
                label_flag = 0;
            }
        }
        if(label_flag)
        {
            if(command_content[0].length >= 2 && (*(command_content[0].start + command_content[0].length - 1) == ':'))
            {
                command_content[0].start[command_content[0].length - 1] = '\0';
                labels_arr[num_of_labels].address = i;
                labels_arr[num_of_labels].label_name = command_content[0].start;

                if(strcmp(MAIN_LABEL_NAME, labels_arr[num_of_labels].label_name) == 0)
                {
                    main_address = labels_arr[num_of_labels].address;
                }

                for(int j = 0; j < num_of_labels; j++)
                {
                    if(strcmp(labels_arr[j].label_name, labels_arr[num_of_labels].label_name) == 0)
                    {
                        return WRONG_LABEL_NAME;
                    }
                }

                op_code = CMD_LBL;
                num_of_labels++;
            }
        }

        printf("%d ", op_code);

        num_of_args--;
        line_poz* args = command_content + 1;
        if(num_of_args > get_num_of_args(op_code))
        {
            free(command_content);
            return WRONG_NUM_OF_ARGS;
        }

        int num_of_read_args = 0;
        char arg_mask = 0;
        

        int read_args[MAX_OF_READ_ARGS] = {};

        for(int j = 0; j < num_of_args; j++)
        {
            if(command_content[j].start == NULL)
            {
                break;
            }
            int jmp_label_flag = 0;
            arg_val read_arg_val;
            read_command_arg(args[j], labels_arr, num_of_labels, op_code,
            &read_arg_val, &jmp_label_flag);

            arg_mask |= !read_arg_val.reg_arg_flag ? 0 : (0x01 << (HALF_BYTE * j));
            read_args[num_of_read_args] = !read_arg_val.reg_arg ? 0 : read_arg_val.reg_arg;
            num_of_read_args += read_arg_val.reg_arg_flag;
            
            arg_mask |= !read_arg_val.num_arg_flag ? 0 : (0x02 << (HALF_BYTE * j));
            read_args[num_of_read_args] = !read_arg_val.num_arg_flag ? 0 : read_arg_val.num_arg;
            arg_mask |= !read_arg_val.ram_arg_flag ? 0 : (0x04 << (HALF_BYTE * j));
            num_of_read_args += read_arg_val.num_arg_flag;

            arg_mask |= !jmp_label_flag ? 0 : (0x01 << HALF_BYTE);
        }

        if(num_of_read_args < get_min_num_of_args(op_code))
        {
            free(command_content);
            return WRONG_NUM_OF_ARGS;
        }

        

        op_code = (op_code << BYTE * 2) | (num_of_read_args << BYTE) | arg_mask;

        if(fwrite(&op_code, sizeof(int), 1, file_ptr) != 1)
        {
            free(command_content);
            return WRITE_TO_FILE_ERROR;
        }

        if(fwrite(read_args, sizeof(int), MAX_OF_READ_ARGS, file_ptr) != MAX_OF_READ_ARGS)
        {
            free(command_content);
            return WRITE_TO_FILE_ERROR;
        }

        printf("%X ", op_code);

        for(int j = 0; j < MAX_OF_READ_ARGS; j++)
        {
            printf("%X", read_args[j]);
        }

        printf("\n");
        free(command_content);
    }
    if(main_address < 0)
    {
        return NO_MAIN_LABEL;
    }
    fwrite(&main_address, sizeof(int), 1, file_ptr);
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

    FILE* proc_command_file = fopen(argv[2], "wb");

    fwrite(&num_of_commands, sizeof(int), 1, proc_command_file);

    int err_code = asm_to_file(proc_command_file, lines_command_arr, num_of_commands);

    if(err_code != NO_ERRORS);
    {
        fclose(proc_command_file);
        free(lines_command_arr);
        free(file_str);
        return err_code;
    }

    fclose(proc_command_file);
    free(lines_command_arr);
    free(file_str);

    return NO_ERRORS;
}