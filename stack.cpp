#include "stack.h"
#include <string.h>
#define NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

const int CANARY_STRUCT_VAL = 0;
const int CANARY_BUF_VAL = 0;

enum stack_parameters
{
    START_STACK_SIZE =  2,
    STACK_MULTIPLY_CONST =  2,
    STACK_DIVIDE_CONST =  2,
    STACK_DIVIDE_TRIGGER =  4,
    NUM_OF_BUF_CANARY = 2
};

int stack_init(stack* my_stack, int size_of_elem)
{
    errno = NO_ERRORS;

    my_stack->size = 0;
    my_stack->offset = 0;
    my_stack->size_of_elem = size_of_elem;
    my_stack->capasity = 0;

    #ifdef CANARY_PROT
    my_stack->struct_canary_2 = 0;
    my_stack->struct_canary_1 = 0;
    #endif

    my_stack->capasity = START_STACK_SIZE;
    my_stack->size_of_buf = ON_CANARY_PROT(sizeof(CANARY_BUF_VAL) * NUM_OF_BUF_CANARY +) START_STACK_SIZE * size_of_elem;

    ON_CANARY_PROT(my_stack->offset = sizeof(CANARY_BUF_VAL);)

    my_stack->buf_ptr = calloc(my_stack->size_of_buf, 1);

    assert(my_stack != NULL);
    if(my_stack == NULL)
    {
        my_stack->capasity = 0;
        my_stack->size_of_buf = 0;
        errno = CANT_ALLOCATE_MEMORY;
        return CANT_ALLOCATE_MEMORY;
    }

    #ifdef CANARY_PROT
    memcpy(my_stack->buf_ptr, &CANARY_BUF_VAL, sizeof(CANARY_BUF_VAL));
    memcpy(my_stack->buf_ptr + my_stack->size_of_buf - sizeof(CANARY_BUF_VAL), &CANARY_BUF_VAL,
    sizeof(CANARY_BUF_VAL));
    #endif

    if(check_stack_valid(my_stack) != 0)
    {
        errno = INCORRECT_STACK_VALIDATION;
        return INCORRECT_STACK_VALIDATION;
    }

    return NO_ERRORS;
}

int check_stack_valid(stack* my_stack)
{
    assert(my_stack != NULL);
    if(my_stack == NULL)
    {
        errno = NULL_STACK_PTR;
        return ERROR_IN_CHECK_FUNC;
    }

    #ifdef CANARY_PROT
    if(my_stack->struct_canary_2 != CANARY_STRUCT_VAL || my_stack->struct_canary_1 != CANARY_STRUCT_VAL)
    {
        return STRUCT_CANARIES_INVALID;
    }

    if((memcmp(my_stack->buf_ptr, &CANARY_BUF_VAL, sizeof(CANARY_BUF_VAL)) 
    || memcmp(my_stack->buf_ptr + my_stack->size_of_buf - sizeof(CANARY_BUF_VAL), &CANARY_BUF_VAL,
    sizeof(CANARY_BUF_VAL))))
    {
        return STACK_MEM_CANARIES_INVALID;
    }
    #endif

    return STACK_VALID;
}

int stack_push(stack* my_stack, void* elem)
{
    errno = NO_ERRORS;

    assert(elem != NULL);
    if(elem == NULL)
    {
        errno = NULL_POINTER_OF_ELEMENT;
        return NULL_POINTER_OF_ELEMENT;
    }

    assert(my_stack != NULL);
    if(my_stack == NULL)
    {
        errno = NULL_STACK_PTR;
        return NULL_STACK_PTR;
    }

    my_stack->offset = ON_CANARY_PROT(sizeof(CANARY_BUF_VAL) +) my_stack->size * my_stack->size_of_elem;

    if(my_stack->size >= my_stack->capasity)
    {
        my_stack->capasity *= STACK_MULTIPLY_CONST;
        my_stack->size_of_buf =ON_CANARY_PROT( NUM_OF_BUF_CANARY * sizeof(CANARY_BUF_VAL) +) my_stack->capasity * my_stack->size_of_elem;
        my_stack->buf_ptr = realloc(my_stack->buf_ptr, my_stack->size_of_buf);

        assert(my_stack->buf_ptr != NULL);
        if(my_stack->buf_ptr == NULL)
        {
            errno = CANT_ALLOCATE_MEMORY;
            return CANT_REALLOCATE_MEMORY;
        }
        
        #ifdef CANARY_PROT
        memcpy(my_stack->buf_ptr, &CANARY_BUF_VAL, sizeof(CANARY_BUF_VAL));
        memcpy(my_stack->buf_ptr + my_stack->size_of_buf - sizeof(CANARY_BUF_VAL), &CANARY_BUF_VAL,
        sizeof(CANARY_BUF_VAL));
        #endif
    }

    memcpy(my_stack->buf_ptr + my_stack->offset, elem, my_stack->size_of_elem);
    my_stack->size++;

    if(check_stack_valid(my_stack) != 0)
    {
        errno = INCORRECT_STACK_VALIDATION;
        return INCORRECT_STACK_VALIDATION;
    }


    return NO_ERRORS;
}

int stack_pop(stack* my_stack, void* return_elem)
{
    errno = NO_ERRORS;

    assert(my_stack != NULL);
    if(my_stack == NULL)
    {
        return NULL_STACK_PTR;
    }

    assert(return_elem != NULL);
    if(return_elem == NULL)
    {
        return NULL_POINTER_OF_ELEMENT;
    }

    assert(my_stack->size != NULL);
    if(my_stack->size == 0)
    {
        errno = NULL_SIZE_OF_STACK;
        return NULL_SIZE_OF_STACK;
    }

    my_stack->size--;

    my_stack->offset = ON_CANARY_PROT(sizeof(CANARY_BUF_VAL) +) my_stack->size * my_stack->size_of_elem;

    memcpy(return_elem, my_stack->buf_ptr + my_stack->offset, my_stack->size_of_elem);

    if(my_stack->size <= my_stack->capasity / STACK_DIVIDE_TRIGGER 
    && my_stack->capasity / STACK_DIVIDE_TRIGGER != 0)
    {
        my_stack->capasity /= STACK_DIVIDE_CONST;
        my_stack->size_of_buf = ON_CANARY_PROT(NUM_OF_BUF_CANARY * sizeof(CANARY_BUF_VAL) +) my_stack->capasity * my_stack->size_of_elem;
        my_stack->buf_ptr = realloc(my_stack->buf_ptr, my_stack->size_of_buf);
        
        assert(my_stack->buf_ptr != NULL);
        if(my_stack->buf_ptr == NULL)
        {
            errno = CANT_ALLOCATE_MEMORY;
            return CANT_REALLOCATE_MEMORY;
        }

        #ifdef CANARY_PROT
        memcpy(my_stack->buf_ptr, &CANARY_BUF_VAL, sizeof(CANARY_BUF_VAL));
        memcpy(my_stack->buf_ptr + my_stack->size_of_buf - sizeof(CANARY_BUF_VAL), &CANARY_BUF_VAL,
        sizeof(CANARY_BUF_VAL));
        #endif
    }

    if(check_stack_valid(my_stack) != 0)
    {
        errno = INCORRECT_STACK_VALIDATION;
        return INCORRECT_STACK_VALIDATION;
    }


    return NO_ERRORS;
}

int stack_destroy(stack* my_stack)
{
    errno = NO_ERRORS;

    assert(my_stack != NULL);
    if(my_stack == NULL)
    {
        errno = NULL_STACK_PTR;
        return NULL_STACK_PTR;
    }

    free(my_stack->buf_ptr);

    my_stack->size = 0;
    my_stack->offset = 0;
    my_stack->size_of_elem = 0;
    my_stack->capasity = 0;
    #ifdef CANARY_PROT
    my_stack->struct_canary_2 = 0;
    my_stack->struct_canary_1 = 0;
    #endif

    return NO_ERRORS;
}

void print_stack_error(int error_code)
{
    switch (error_code)
    {
        case NO_ERRORS:
            printf("NO ERRORS");
            break;
        case NULL_POINTER_OF_ELEMENT:
            printf("NULL POINTER OF ELEMENT");
            break;
        case NULL_STACK_PTR:
            printf("NULL STACK POINTER");
            break;
        case NULL_SIZE_OF_STACK:
            printf("NULL NUMBERS OF ELEMENTS IN STACK");
            break;
        case CANT_ALLOCATE_MEMORY:
            printf("CANT ALLOCATE MEMORY");
            break;
        case CANT_REALLOCATE_MEMORY:
            printf("CANT REALLOCATE MEMORY");
            break;
        case INCORRECT_STACK_VALIDATION:
            printf("INCORRECT STACK VALIDATION");
            break;
        case ERROR_IN_TEST_FILE_NAME:
            printf("ERROR IN NAME OF FILE WITH TEST DATA");
            break;
        default:
            printf("NOT ERROR MESSAGE");
            break;
    }
}

void print_stack_valid_check_message(int message_code)
{
    switch (message_code)
    {
        case ERROR_IN_CHECK_FUNC:
            printf("ERROR IN VALIDATION FUNCTION");
            break;
        case STACK_VALID:
            printf("STACK VALID");
            break;
        case STRUCT_CANARIES_INVALID:
            printf("INVALID VALUE OF STRUCT CANARIES");
            break;
        case STACK_MEM_CANARIES_INVALID:
            printf("INVALID VALUE OF STACK MEMORY CANARIES");
            break;
        default:
            printf("NOT VALIDATION FUNCTION MESSAGE");
            break;
    }
}