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

#define COMPARISON_OPERATOR(name, condition)                                                                                                        \
case (name):                                                                                                                                        \
{                                                                                                                                                   \
    counter_comparison++;                                                                                                                           \
                                                                                                                                                    \
    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);                                                           \
    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);                                                           \
                                                                                                                                                    \
    fprintf(file_output, "%s compare_true_%ld:\n"                                                                                                   \
                         "push 0\n"                                                                                                                 \
                         "jmp end_compare_false_%ld:\n"                                                                                             \
                         "compare_true_%ld:\n"                                                                                                      \
                         "push 1\n"                                                                                                                 \
                         "end_compare_false_%ld:\n", condition, counter_comparison, counter_comparison, counter_comparison, counter_comparison);    \
                                                                                                                                                    \
    break;                                                                                                                                          \
}

#define ARITHMETIC_OPERATOR(name, command)                                                      \
case (name):                                                                                    \
{                                                                                               \
    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);       \
    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);       \
                                                                                                \
    fprintf(file_output, "%s\n", command);                                                      \
    break;                                                                                      \
}

void print_node(FILE *file_output, const tree_node *tree_node_pointer, const variable_parametrs *variable_array, const tree_node *parent_tree_node_pointer)
{
    MYASSERT(file_output                != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(parent_tree_node_pointer   != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(variable_array             != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    static ssize_t counter_if         = 0;
    static ssize_t counter_while      = 0;
    static ssize_t counter_comparison = 0;
    static ssize_t counter_and        = 0;
    static ssize_t counter_or         = 0;

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
                ARITHMETIC_OPERATOR(ADD, "add")
                ARITHMETIC_OPERATOR(DIV, "div")
                ARITHMETIC_OPERATOR(SUB, "sub")
                ARITHMETIC_OPERATOR(MUL, "mul")

                case SEQ_EXEC:
                {
                    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);
                    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

                    break;
                }

                case ASSIGMENT:
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

                    fprintf(file_output, "begin_while_%ld:\n", counter_while);

                    print_node(file_output, tree_node_pointer->left, variable_array, tree_node_pointer);

                    fprintf(file_output, "push 0\n"
                                         "je end_while_%ld:\n", counter_while);

                    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

                    fprintf(file_output,  "jmp begin_while_%ld:\n"
                                          "end_while_%ld:\n", counter_while, counter_while);

                    break;
                }

                COMPARISON_OPERATOR(BELOW,          "jb")
                COMPARISON_OPERATOR(MORE,           "ja")
                COMPARISON_OPERATOR(EQUAL,          "je")
                COMPARISON_OPERATOR(MORE_OR_EQUAL,  "jae")
                COMPARISON_OPERATOR(BELOW_OR_EQUAL, "jbe")
                COMPARISON_OPERATOR(NOT_EQUAL,      "jne")

                case AND:
                {
                    counter_and++;

                    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);

                    fprintf(file_output, "push 0\n"
                                         "je compare_and_false_%ld:\n", counter_and);

                    print_node(file_output, tree_node_pointer->right,  variable_array, tree_node_pointer);

                    fprintf(file_output, "push 0\n"
                                         "je compare_and_false_%ld:\n", counter_and);

                    fprintf(file_output, "push 1\n"
                                         "jmp compare_and_true_%ld:\n"
                                         "compare_and_false_%ld:\n"
                                         "push 0\n"
                                         "compare_and_true_%ld:\n", counter_and, counter_and, counter_and);

                    break;
                }

                case OR:
                {
                    counter_or++;

                    print_node(file_output, tree_node_pointer->left,  variable_array, tree_node_pointer);

                    fprintf(file_output, "push 0\n"
                                         "jne compare_or_true_%ld:\n", counter_or);

                    print_node(file_output, tree_node_pointer->right,  variable_array, tree_node_pointer);

                    fprintf(file_output, "push 0\n"
                                         "jne compare_or_true_%ld:\n", counter_or);

                    fprintf(file_output, "push 0\n"
                                         "jmp compare_or_false_%ld:\n"
                                         "compare_or_true_%ld:\n"
                                         "push 1\n"
                                         "compare_or_false_%ld:\n", counter_or, counter_or, counter_or);

                    break;
                }

                default:
                {
                    printf(RED "ERROR! Incorrect KEY WORD <%ld> when output to a file!\n" RESET_COLOR, tree_node_pointer->value.operator_index);
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

#undef COMPARISON_OPERATOR
#undef ARITHMETIC_OPERATOR

