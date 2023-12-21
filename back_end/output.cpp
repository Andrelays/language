#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "../libraries/utilities/myassert.h"
#include "../libraries/utilities/colors.h"
#include "../libraries/stack/stack.h"
#include "../libraries/onegin/onegin.h"
#include "../libraries/utilities/utilities.h"
#include "back_end.h"
#include "../key_words.h"
#include "../libraries/tree/dsl.h"

void print_node(FILE *file_output, const tree_node *tree_node_pointer, const variable_parametrs *variable_array, const tree_node *parent_tree_node_pointer)
{
    MYASSERT(file_output                != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(parent_tree_node_pointer   != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(variable_array             != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    static ssize_t counter_if    = 0;
    static ssize_t counter_while = 0;

    if (!tree_node_pointer)
    {
        return;
    }

    switch(tree_node_pointer->type)
    {
        case VARIABLE:
        {
            fprintf(file_output, "push [%ld]\n", tree_node_pointer->value.variable_index);
            break;
        }

        case KEY_WORD:
        {
            switch(tree_node_pointer->value.operator_index)
            {
                case ADD:
                {
                    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);
                    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

                    fprintf(file_output, "add\n");
                    break;
                }

                case DIV:
                {
                    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);
                    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

                    fprintf(file_output, "div\n");
                    break;
                }

                case SUB:
                {
                    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);
                    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

                    fprintf(file_output, "sub\n");
                    break;
                }

                case MUL:
                {
                    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);
                    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

                    fprintf(file_output, "mul\n");
                    break;
                }

                case SEQ_EXEC:
                {
                    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);
                    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

                    break;
                }

                case ASGMT:
                {
                    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);
                    fprintf(file_output, "pop [%ld]\n", tree_node_pointer->left->value.variable_index);

                    break;
                }

                case IF:
                {
                    counter_if++;

                    print_node(file_output, tree_node_pointer->left, variable_array, tree_node_pointer);
                    fprintf(file_output, "push 0\n"
                                         "je end_if_%ld:\n", counter_if);

                    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

                    fprintf(file_output, "end_if_%ld:\n", counter_if);

                    break;
                }

                case WHILE:
                {
                    counter_while++;

                    print_node(file_output, tree_node_pointer->left, variable_array, tree_node_pointer);
                    fprintf(file_output, "push 0\n"
                                         "je end_while_%ld:\n", counter_while);

                    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

                    fprintf(file_output, "end_while_%ld:\n", counter_while);

                    break;
                }

                default:
                {
                    printf(RED "ERROR! Incorrect KEY WORD when output to a file!\n" RESET_COLOR);
                    return;
                }

            }
            break;
        }

        case NUMBER:
        {
            fprintf(file_output, "push %g\n", tree_node_pointer->value.number);
            break;
        }

        case NO_TYPE:
        default:
        {
            printf(RED "ERROR! Incorrect data type when output to a file!\n" RESET_COLOR);
            return;
        }
    }
}
