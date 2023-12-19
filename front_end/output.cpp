#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "libraries/utilities/myassert.h"
#include "libraries/utilities/colors.h"
#include "libraries/stack/stack.h"
#include "libraries/onegin/onegin.h"
#include "libraries/utilities/utilities.h"
#include "front_end.h"
#include "key_words.h"
#include "dsl.h"

void print_node(FILE *file_output, const tree_node *tree_node_pointer, const variable_parametrs *variable_array, const tree_node *parent_tree_node_pointer)
{
    MYASSERT(file_output                != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(parent_tree_node_pointer   != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(variable_array             != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    if (!tree_node_pointer)
    {
        fprintf(file_output,  "_ ");
        return;
    }

    fprintf(file_output,  "( ");

    switch(tree_node_pointer->type)
    {
        case VARIABLE:
        {
            fprintf(file_output,  "3 %ld ", tree_node_pointer->value.variable_index);
            break;
        }

        case KEY_WORD:
        {
            fprintf(file_output,  "1 %ld " , tree_node_pointer->value.operator_index);
            break;
        }

        case NUMBER:
        {
            fprintf(file_output,"2 " FORMAT_SPECIFIERS_TREE " ", tree_node_pointer->value.number);
            break;
        }

        case NO_TYPE:
        default:
        {
            printf(RED "ERROR! Incorrect data type when output to a file!\n" RESET_COLOR);
            return;
        }
    }

    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);
    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

    fprintf(file_output, ") ");
}

void print_variable_array(FILE *file_output, const variable_parametrs *variable_array, ssize_t variable_array_size)
{
    MYASSERT(file_output    != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(variable_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    for (ssize_t index_variable = 0; index_variable < variable_array_size; index_variable++) {
        fprintf(file_output, "%ld <%s>\n", index_variable, variable_array->name);
    }
}
