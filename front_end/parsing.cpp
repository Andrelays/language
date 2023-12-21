#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "../libraries/utilities/myassert.h"
#include "../libraries/utilities/colors.h"
#include "../libraries/stack/stack.h"
#include "../libraries/onegin/onegin.h"
#include "../libraries/utilities/utilities.h"
#include "front_end.h"
#include "../key_words.h"
#include "../libraries/tree/dsl.h"

#define syn_assert(condition)                                                                                                                                           \
 do {                                                                                                                                                                   \
    if (!(condition))                                                                                                                                                   \
    {                                                                                                                                                                   \
        tokens->is_error = true;                                                                                                                                       \
        printf(RED  "File: %s\n"                                                                                                                                        \
                    "line: %d\n"                                                                                                                                        \
                    "Function: %s\n"                                                                                                                                    \
                    "Position: %ld\n"                                                                                                                                   \
                    "The condition is not met: \"%s\"\n"RESET_COLOR, __FILE__, __LINE__, __PRETTY_FUNCTION__, tokens->token_array_position, #condition);               \
                                                                                                                                                                        \
        return 0;                                                                                                                                                       \
    }                                                                                                                                                                   \
}  while(0)

static tree_node *get_operator       (token_info *, tree *);
static tree_node *get_if             (token_info *, tree *);
static tree_node *get_while          (token_info *, tree *);
static tree_node *get_assigment      (token_info *, tree *);
static tree_node *get_sum            (token_info *, tree *);
static tree_node *get_product        (token_info *, tree *);
static tree_node *get_power          (token_info *, tree *);
static tree_node *get_negation       (token_info *, tree *);
// static tree_node *get_unary_functions(parsing_info *, tree *);
static tree_node *get_parenthesis    (token_info *, tree *);
static tree_node *get_number         (token_info *, tree *);
static tree_node *get_variable       (token_info *, tree *);

tree_node *get_grammar(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    syn_assert(IS_BEGIN_OF_PROGRAM);
    ++tokens->token_array_position;

    tree_node *tree_node_pointer = get_operator(tokens, tree_pointer);

    while (tokens->token_array_position < tokens->size_token_array - 1)
    {
        ssize_t old_token_array_position = tokens->token_array_position;
        tree_node *tree_node_pointer_2   = get_operator(tokens, tree_pointer);

        syn_assert(old_token_array_position < tokens->token_array_position);

        tree_node_pointer = create_node(KEY_WORD, {.operator_index = SEQ_EXEC}, tree_node_pointer, tree_node_pointer_2);
    }

    syn_assert(IS_END_OF_PROGRAM);

    return tree_node_pointer;
}

static tree_node *get_operator(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if (IS_BEGIN_OPERATOR)                                                                                          //FIXME error recursion
    {
        ++tokens->token_array_position;
        ++tree_pointer->size;                                                                                      //FIXME tree_pointer->size

        tree_node_pointer = get_operator(tokens, tree_pointer);

        while (!(IS_END_OPERATOR))
        {
            tree_node *tree_node_pointer_2 = get_operator(tokens, tree_pointer);

            tree_node_pointer = create_node(KEY_WORD, {.operator_index = SEQ_EXEC}, tree_node_pointer, tree_node_pointer_2);
        }

        ++tokens->token_array_position;
        return tree_node_pointer;
    }

    if (!(tree_node_pointer = get_if(tokens, tree_pointer)) && !(tree_node_pointer = get_while(tokens, tree_pointer)))
    {
        tree_node_pointer = get_assigment(tokens, tree_pointer);
        syn_assert(IS_SEQUENTIAL_EXEC_OPER);
        ++tokens->token_array_position;
    }

    return tree_node_pointer;
}

static tree_node *get_if(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if (IS_KWD_IF)
    {
        ++tokens->token_array_position;
        ++tree_pointer->size;

        syn_assert(IS_OPEN_PARENTHESIS);
        ++tokens->token_array_position;

        tree_node_pointer = get_sum(tokens, tree_pointer);

        syn_assert(IS_CLOSING_PARENTHESIS);
        ++tokens->token_array_position;

        tree_node *tree_node_pointer_2 = get_operator(tokens, tree_pointer);

        tree_node_pointer = create_node(KEY_WORD, {.operator_index = IF}, tree_node_pointer, tree_node_pointer_2);
    }

    return tree_node_pointer;
}

static tree_node *get_while(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if (IS_KWD_WHILE)
    {
        ++tokens->token_array_position;
        ++tree_pointer->size;

        syn_assert(IS_OPEN_PARENTHESIS);
        ++tokens->token_array_position;

        tree_node_pointer = get_sum(tokens, tree_pointer);

        syn_assert(IS_CLOSING_PARENTHESIS);
        ++tokens->token_array_position;

        tree_node *tree_node_pointer_2 = get_operator(tokens, tree_pointer);

        tree_node_pointer = CREATE_WHILE(tree_node_pointer, tree_node_pointer_2);
    }

    return tree_node_pointer;
}

static tree_node *get_assigment(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    ++tokens->token_array_position;                                                        //TODO check size array

    if(IS_ASSIGMENT)
    {
        --tokens->token_array_position;

        tree_node_pointer = get_variable(tokens, tree_pointer);

        ++tokens->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_sum(tokens, tree_pointer);

        tree_node_pointer = create_node(KEY_WORD, {.operator_index = ASGMT}, tree_node_pointer, tree_node_pointer_2);
    }

    else {
        --tokens->token_array_position;
    }

    return tree_node_pointer;
}

static tree_node *get_sum(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = get_product(tokens, tree_pointer);

    while (CURRENT_TOKEN.type == KEY_WORD && (CURRENT_TOKEN.value.operator_index == ADD || CURRENT_TOKEN.value.operator_index == SUB))
    {
        ssize_t operator_index = CURRENT_TOKEN.value.operator_index;
        ++tokens->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_product(tokens, tree_pointer);

        switch(operator_index)
        {
            case ADD:
                tree_node_pointer = create_node(KEY_WORD, {.operator_index = ADD}, tree_node_pointer, tree_node_pointer_2);
                break;

            case SUB:
                tree_node_pointer = create_node(KEY_WORD, {.operator_index = SUB}, tree_node_pointer, tree_node_pointer_2);
                break;

            default:
                syn_assert(0);
        }
    }

    return tree_node_pointer;
}

static tree_node *get_product(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = get_power(tokens, tree_pointer);

    while (CURRENT_TOKEN.type == KEY_WORD && (CURRENT_TOKEN.value.operator_index == MUL || CURRENT_TOKEN.value.operator_index == DIV))
    {
        ssize_t operator_index = CURRENT_TOKEN.value.operator_index;
        ++tokens->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_power(tokens, tree_pointer);

        switch(operator_index)
        {
            case MUL:
                tree_node_pointer = create_node(KEY_WORD, {.operator_index = MUL}, tree_node_pointer, tree_node_pointer_2);
                break;

            case DIV:
                tree_node_pointer = create_node(KEY_WORD, {.operator_index = DIV}, tree_node_pointer, tree_node_pointer_2);
                break;

            default:
                syn_assert(0);
        }
    }

    return tree_node_pointer;
}

static tree_node *get_power(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = get_negation(tokens, tree_pointer);

    while (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == POW)
    {
        ++tokens->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_negation(tokens, tree_pointer);

        tree_node_pointer = create_node(KEY_WORD, {.operator_index = POW}, tree_node_pointer, tree_node_pointer_2);
    }

    return tree_node_pointer;
}

static tree_node *get_negation(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    bool is_negation = false;

    if (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == SUB)
    {
        is_negation = true;

        ++tokens->token_array_position;
        ++tree_pointer->size;
    }

    tree_node *tree_node_pointer = get_parenthesis(tokens, tree_pointer);

    if (is_negation) {
        tree_node_pointer = create_node(KEY_WORD, {.operator_index = MUL}, CREATE_NUM(-1), tree_node_pointer);
    }

    return tree_node_pointer;
}



// static tree_node *get_unary_functions(token_info *tokens, tree *tree_pointer)
// {
//     MYASSERT(tokens              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
//     MYASSERT(tokens->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
//     MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
//
//     tree_node *tree_node_pointer = NULL;
//
//     if (CURRENT_TOKEN.type == KEY_WORD && !KEY_WORDS[CURRENT_TOKEN.value.operator_index].is_binary)
//     {
//         ssize_t operator_index = CURRENT_TOKEN.value.operator_index;
//         ++tokens->token_array_position;
//         ++tree_pointer->size;
//
//         tree_node_pointer = get_unary_functions(tokens, tree_pointer);                                                                                     //FIXME sin cos tan - no language
//
//         switch(operator_index)
//         {
//             case SIN:
//                 tree_node_pointer = create_node(KEY_WORD, {.operator_index = SIN}, NULL, tree_node_pointer);
//                 break;
//
//             case COS:
//                 tree_node_pointer = create_node(KEY_WORD, {.operator_index = COS}, NULL, tree_node_pointer);
//                 break;
//
//             case TAN:
//                 tree_node_pointer = create_node(KEY_WORD, {.operator_index = TAN}, NULL, tree_node_pointer);
//                 break;
//
//             case LOG:
//                 tree_node_pointer = create_node(KEY_WORD, {.operator_index = LOG}, NULL, tree_node_pointer);
//                 break;
//
//             case EXP:
//                 tree_node_pointer = create_node(KEY_WORD, {.operator_index = EXP}, NULL, tree_node_pointer);
//                 break;
//
//             default:
//                 syn_assert(0);
//         }
//
//         return tree_node_pointer;
//     }
//
//     tree_node_pointer = get_parenthesis(tokens, tree_pointer);
//
//     return tree_node_pointer;
// }

static tree_node *get_parenthesis(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if (IS_OPEN_PARENTHESIS)
    {
        ++tokens->token_array_position;

        tree_node_pointer = get_sum(tokens, tree_pointer);

        syn_assert(IS_CLOSING_PARENTHESIS);
        ++tokens->token_array_position;

        return tree_node_pointer;
    }

    if (!(tree_node_pointer = get_number(tokens, tree_pointer))) {
        tree_node_pointer   = get_variable(tokens, tree_pointer);
    }

    return tree_node_pointer;
}

static tree_node *get_number(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if(CURRENT_TOKEN.type == NUMBER)
    {
        tree_node_pointer = create_node(NUMBER, CURRENT_TOKEN.value, NULL, NULL);
        ++tokens->token_array_position;
        ++tree_pointer->size;
    }

    return tree_node_pointer;
}

tree_node *get_variable(token_info *tokens, tree *tree_pointer)
{
    MYASSERT(tokens               != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tokens->token_array  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    ssize_t string_index_old = tokens->token_array_position;

    if(CURRENT_TOKEN.type == VARIABLE)
    {
        tree_node_pointer = create_node(VARIABLE, CURRENT_TOKEN.value, NULL, NULL);
        ++tokens->token_array_position;
        ++tree_pointer->size;
    }

    syn_assert(string_index_old < tokens->token_array_position);

    return tree_node_pointer;
}
