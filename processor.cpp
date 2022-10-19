#include <stdio.h>
#include <stdlib.h>
#include "processor.h"
#include "commands.h"
#include "stack.h"

#define GET_REG_FLAG(ARG_MASK)  ARG_MASK & 0x01;
#define GET_NUM_FLAG(ARG_MASK) (ARG_MASK & 0x02) >> 1;
#define GET_RAM_FLAG(ARG_MASK) (ARG_MASK & 0x04) >> 2;

typedef int arg_t;
typedef int reg_t;
typedef int ram_t;
typedef int num_arg_t;

int int_arg_handler_1(int* arg, args_flags curr_args_flags, reg_t reg_arr[], int* arg_counter, arg_t args[])
{
    *arg = 0;

    *arg += !curr_args_flags.reg_flag_1 ? 0 : reg_arr[args[*arg_counter]];
    *arg_counter += curr_args_flags.reg_flag_1;

    
    *arg += !curr_args_flags.num_flag_1 ? 0 : args[*arg_counter];
    *arg_counter += curr_args_flags.num_flag_1;

    return NO_ERRORS;
}

int int_arg_handler_2(int* arg, args_flags curr_args_flags, reg_t reg_arr[], int* arg_counter, arg_t args[])
{
    *arg = 0;

    *arg += !curr_args_flags.reg_flag_2 ? 0 : reg_arr[args[*arg_counter]];
    *arg_counter += curr_args_flags.reg_flag_2;

    
    *arg += !curr_args_flags.num_flag_2 ? 0 : args[*arg_counter];
    *arg_counter += curr_args_flags.num_flag_2;

    return NO_ERRORS;
}


int main(int argc, char** argv)
{
    stack int_proc_stack;
    stack_init(&int_proc_stack, sizeof(int));

    stack refund_stack;
    stack_init(&refund_stack, sizeof(int));

    int num_of_commands = 0; 
    if(argc < 2)
    {
        return NOT_ENOUGH_ARGV;
    }
    FILE* commands_file = fopen(argv[1], "rb");
    int command_counter = 0;
    fread(&num_of_commands, sizeof(int), 1, commands_file);

    int* command_arr = (int*)calloc(num_of_commands * SIZE_OF_COMMAND, sizeof(int));
    fread(command_arr, sizeof(int), num_of_commands * SIZE_OF_COMMAND, commands_file);
    
    fread(&command_counter, sizeof(int), 1, commands_file);

    for(int i = 0; i < num_of_commands; i++)
    {
        printf("%X ", command_arr[i * SIZE_OF_COMMAND]);
        for(int j = 1; j < 5; j++)
        {
            printf("%d ", command_arr[i * SIZE_OF_COMMAND + j]);
        }
        printf("\n");
    }

    reg_t reg_arr[NUM_OF_REGISTERS] = {};
    ram_t ram_arr[SIZE_OF_RAM] = {};
    printf("\n");

    while(command_counter < num_of_commands)
    {
        args_flags curr_args_flags;
        int arg_mask = 0x000000FF & command_arr[command_counter * SIZE_OF_COMMAND]; 
        int op_code = (command_arr[command_counter * SIZE_OF_COMMAND] & 0xFFFF0000) >> (BYTE * 2);
        int num_of_args = (command_arr[command_counter * SIZE_OF_COMMAND] & 0x0000FF00) >> (BYTE * 1);
        arg_t args[MAX_OF_READ_ARGS] = {};
        printf("address: %d\n", command_counter);

        for(int i = 0; i < MAX_OF_READ_ARGS; i++)
        {
            args[i] = command_arr[command_counter * SIZE_OF_COMMAND + 1 + i];
        }

        int arg_counter = 0;


        curr_args_flags.reg_flag_1 = GET_REG_FLAG(arg_mask);
        curr_args_flags.num_flag_1 = GET_NUM_FLAG(arg_mask);
        curr_args_flags.ram_flag_1 = GET_RAM_FLAG(arg_mask);

        arg_mask = arg_mask >> HALF_BYTE;

        
        curr_args_flags.reg_flag_2 = GET_REG_FLAG(arg_mask);
        curr_args_flags.num_flag_2 = GET_NUM_FLAG(arg_mask);
        curr_args_flags.ram_flag_2 = GET_RAM_FLAG(arg_mask);

        if(CMD_POP == op_code)
        {
            if(num_of_args < 1 || (!curr_args_flags.ram_flag_1 && curr_args_flags.num_flag_1))
            {
                stack_destroy(&refund_stack);
                stack_destroy(&int_proc_stack);
                free(command_arr);
                return WRONG_ARGS;
            }

            int pop;

            if(stack_pop(&int_proc_stack, &pop) == NULL_SIZE_OF_STACK)
            {
                stack_destroy(&refund_stack);
                stack_destroy(&int_proc_stack);
                free(command_arr);
                return NULL_SIZE_OF_STACK;
            }

            if(curr_args_flags.ram_flag_1)
            {
                int arg_1 = 0;
                int_arg_handler_1(&arg_1, curr_args_flags, reg_arr, &arg_counter, args);
                ram_arr[arg_1] = pop;
            }
            else
            {
                reg_arr[args[0]] = pop;
            }
        }
        if(CMD_PUSH == op_code)
        {
            if(num_of_args < 1)
            {
                stack_destroy(&refund_stack);
                stack_destroy(&int_proc_stack);
                free(command_arr);
                return WRONG_ARGS;
            }
            num_arg_t arg_1 = 0;

            int_arg_handler_1(&arg_1, curr_args_flags, reg_arr, &arg_counter, args);

            stack_push(&int_proc_stack, ((curr_args_flags.ram_flag_1 ? (num_arg_t*)(ram_arr + arg_1) : (num_arg_t*)&arg_1)));
        }
        if(CMD_HLT == op_code)
        {
            command_counter = num_of_commands + 1;
        }
        if(CMD_OUT == op_code)
        {
            if(num_of_args < 1 || (!curr_args_flags.ram_flag_1 && curr_args_flags.num_flag_1))
            {
                stack_destroy(&refund_stack);
                stack_destroy(&int_proc_stack);
                free(command_arr);
                return WRONG_ARGS;
            }
            int arg_1 = 0;
            int_arg_handler_1(&arg_1, curr_args_flags, reg_arr, &arg_counter, args);
            printf("out:%d\n", (curr_args_flags.ram_flag_1 ? ram_arr[arg_1] : reg_arr[args[0]]));
        }


        //блок комманд числовых операторов, придумать оптимизацию
        if(CMD_ADD == op_code || CMD_MUL == op_code || CMD_DIV == op_code || CMD_SUB == op_code)
        {
            if(num_of_args == 0)
            {
                int pop_1;
                int pop_2; 

                stack_pop(&int_proc_stack, &pop_1);
                if(stack_pop(&int_proc_stack, &pop_2) == NULL_SIZE_OF_STACK)
                {
                    stack_destroy(&int_proc_stack);
                    free(command_arr);
                    return NULL_SIZE_OF_STACK;
                }

                switch (op_code)
                {
                case CMD_ADD:
                    pop_1 += pop_2;
                    break;
                case CMD_DIV:
                    pop_1 *= pop_2;
                    break;
                case CMD_MUL:
                    pop_1 /= pop_2;
                    break;
                case CMD_SUB:
                    pop_1 -= pop_2;
                    break;
                default:
                    break;
                }
                stack_push(&int_proc_stack , &pop_1);
            }
            else
            {
                if(num_of_args == 1 || (curr_args_flags.num_flag_1 && !curr_args_flags.ram_flag_1))
                {
                    stack_destroy(&refund_stack);
                    stack_destroy(&int_proc_stack);
                    free(command_arr);
                    return WRONG_ARGS;
                }

                int arg_1 = 0;
                int arg_2 = 0;

                int_arg_handler_1(&arg_1, curr_args_flags, reg_arr, &arg_counter, args);
                int_arg_handler_2(&arg_2, curr_args_flags, reg_arr, &arg_counter, args);

                if(curr_args_flags.ram_flag_1)
                {
                    switch (op_code)
                    {
                    case CMD_ADD:
                        ram_arr[arg_1] = ram_arr[arg_1] +
                        (curr_args_flags.ram_flag_2 ? ram_arr[arg_2] : arg_2);
                        break;
                    case CMD_DIV:
                        ram_arr[arg_1] = ram_arr[arg_1] /
                        (curr_args_flags.ram_flag_2 ? ram_arr[arg_2] : arg_2);
                        break;
                    case CMD_MUL:
                        ram_arr[arg_1] = ram_arr[arg_1] *
                        (curr_args_flags.ram_flag_2 ? ram_arr[arg_2] : arg_2);
                        break;
                    case CMD_SUB:
                        ram_arr[arg_1] = ram_arr[arg_1] -
                        (curr_args_flags.ram_flag_2 ? ram_arr[arg_2] : arg_2);
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    switch (op_code)
                    {
                    case CMD_ADD:
                        reg_arr[args[0]] = arg_1 + (curr_args_flags.ram_flag_2 ? ram_arr[arg_2] : arg_2);
                        break;
                    case CMD_DIV:
                        reg_arr[args[0]] = arg_1 / (curr_args_flags.ram_flag_2 ? ram_arr[arg_2] : arg_2);
                        break;
                    case CMD_MUL:
                        reg_arr[args[0]] = arg_1 * (curr_args_flags.ram_flag_2 ? ram_arr[arg_2] : arg_2);
                        break;
                    case CMD_SUB:
                        reg_arr[args[0]] = arg_1 - (curr_args_flags.ram_flag_2 ? ram_arr[arg_2] : arg_2);
                        break;
                    default:
                        break;
                    }
                }

            }
        }
        if(CMD_JMP == op_code)
        {
            if(num_of_args < 1)
            {
                stack_destroy(&refund_stack);
                stack_destroy(&int_proc_stack);
                free(command_arr);
                return WRONG_ARGS;
            }
            int arg_1 = 0;

            int_arg_handler_1(&arg_1, curr_args_flags, reg_arr, &arg_counter, args);

            if(arg_1 < 0 || arg_1 >= num_of_commands)
            {
                stack_destroy(&refund_stack);
                stack_destroy(&int_proc_stack);
                free(command_arr);
                return WRONG_JMP_ADDRESS;
            }

            if(curr_args_flags.reg_flag_2 == 1) //same as jmp_label_flag
            {
                stack_push(&refund_stack, &command_counter);
            }

            command_counter = arg_1 - 1;
        }

        if(CMD_RET == op_code)
        {
            int refund_address = 0;
            if(stack_pop(&refund_stack, &refund_address) == NULL_SIZE_OF_STACK)
            {
                stack_destroy(&refund_stack);
                stack_destroy(&int_proc_stack);
                free(command_arr);
                return NULL_SIZE_OF_STACK;
            }
            command_counter = refund_address - 1;
        }
        if(op_code == CMD_JA || op_code == CMD_JAE || op_code == CMD_JB || op_code == CMD_JBE ||
           op_code == CMD_JE || op_code == CMD_JNE)
            {
                if(num_of_args < 1)
                {
                    stack_destroy(&refund_stack);
                    stack_destroy(&int_proc_stack);
                    free(command_arr);
                    return WRONG_ARGS;
                }
                
                int pop_1;
                int pop_2; 

                stack_pop(&int_proc_stack, &pop_1);
                if(stack_pop(&int_proc_stack, &pop_2) == NULL_SIZE_OF_STACK)
                {
                    stack_destroy(&refund_stack);
                    stack_destroy(&int_proc_stack);
                    free(command_arr);
                    return NULL_SIZE_OF_STACK;
                }

                int jmp_flag = 0;
                            
                switch (op_code)
                {
                case CMD_JA:
                    if(pop_1 > pop_2)
                    {
                        jmp_flag = 1;
                    }
                    break;
                case CMD_JAE:
                    if(pop_1 >= pop_2)
                    {
                        jmp_flag = 1;
                    }
                    break;
                case CMD_JB:
                    if(pop_1 < pop_2)
                    {
                        jmp_flag = 1;
                    }
                    break;
                case CMD_JBE:
                    if(pop_1 <= pop_2)
                    {
                        jmp_flag = 1;
                    }
                    break;
                case CMD_JE:
                    if(pop_1 == pop_2)
                    {
                        jmp_flag = 1;
                    }
                    break;
                case CMD_JNE:
                    if(pop_1 != pop_2)
                    {
                        jmp_flag = 1;
                    }
                    break;
                default:
                    break;
                }

                if(jmp_flag)
                {
                    int arg_1 = 0;

                    int_arg_handler_1(&arg_1, curr_args_flags, reg_arr, &arg_counter, args);

                    if(curr_args_flags.reg_flag_2) //same as jmp_label_flag
                    {
                        stack_push(&refund_stack, &command_counter);
                    }

                    if(arg_1 < 0 || arg_1 >= num_of_commands)
                    {
                        stack_destroy(&int_proc_stack);
                        free(command_arr);
                        return WRONG_JMP_ADDRESS;
                    }

                    command_counter = arg_1 - 1;
                }
            }
        
        command_counter++;
    }
    stack_destroy(&refund_stack);
    stack_destroy(&int_proc_stack);
    free(command_arr);
    return NO_ERRORS;
}