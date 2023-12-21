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

static void tokens_destructor(token_info *tokens);
static void tokens_constructor(token_info *tokens);
static token_info *new_tokens();
static char *saving_buffer(FILE *database_file);
static void print_tokens(const token_info *tokens);

tree *input_tree_from_database(FILE *database_file)
{
    MYASSERT(database_file != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree *tree_pointer = new_pointer_tree();
    TREE_CONSTRUCTOR(tree_pointer);

    char *database_buffer = saving_buffer(database_file);
    MYASSERT(database_buffer != NULL, FAILED_TO_ALLOCATE_DYNAM_MEMOR, return NULL);

    token_info *tokens = new_tokens();
    tokens_constructor(tokens);

    split_buffer_into_tokens(tree_pointer, database_buffer, tokens);

    print_tokens(tokens);

    tree_pointer->root = get_grammar(tokens, tree_pointer);

    if (tokens->is_error) {
        delete_node(tree_pointer->root);
        tree_pointer->root = NULL;
        tree_pointer->size = 0;
        tree_pointer->variable_array_position = 0;
    }

    tokens_destructor(tokens);
    free(database_buffer);

    return tree_pointer;
}

static char *saving_buffer(FILE *database_file)
{
    MYASSERT(database_file != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    size_t buffer_size = determine_size(database_file);

    char *database_buffer = (char *)calloc(buffer_size + 1, sizeof(char));

    MYASSERT(database_buffer != NULL, FAILED_TO_ALLOCATE_DYNAM_MEMOR, return NULL);

    buffer_size = fread(database_buffer, sizeof(char), buffer_size, database_file);
    database_buffer[buffer_size] = '\0';

    return database_buffer;
}

static token_info *new_tokens()
{
    token_info *tokens = (token_info *) calloc(1, sizeof(token_info));

    MYASSERT(tokens != NULL, FAILED_TO_ALLOCATE_DYNAM_MEMOR, return NULL);

    return tokens;
}

static void tokens_constructor(token_info *tokens)
{
    MYASSERT(tokens != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    const ssize_t INITIAL_SIZE_TOKENS_ARRAY = 100;

    tokens->size_token_array = INITIAL_SIZE_TOKENS_ARRAY;
    tokens->token_array_position = 0;

    tokens->token_array = (tree_node *) calloc(INITIAL_SIZE_TOKENS_ARRAY, sizeof(tree_node));

    MYASSERT(tokens->token_array != NULL, FAILED_TO_ALLOCATE_DYNAM_MEMOR, return);
}


static void tokens_destructor(token_info *tokens)
{
    MYASSERT(tokens              != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(tokens->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    free(tokens->token_array);

    tokens->token_array          = NULL;
    tokens->size_token_array     = -1;
    tokens->token_array_position = -1;

    free(tokens);

    tokens = NULL;
}

void print_tokens(const token_info *tokens)
{
    printf("size = %ld\n", tokens->size_token_array);

    for (ssize_t token_array_position = 0; token_array_position < tokens->size_token_array; token_array_position++)
    {
        switch(tokens->token_array[token_array_position].type)
        {
            case VARIABLE:
            {
                printf("VAR %ld pos = %ld\n", tokens->token_array[token_array_position].value.variable_index, token_array_position);
                break;
            }

            case KEY_WORD:
            {
                printf("KWD <%s> pos = %ld\n", KEY_WORDS[tokens->token_array[token_array_position].value.operator_index].real_name, token_array_position);
                break;
            }

            case NUMBER:
            {
                printf("NUM " FORMAT_SPECIFIERS_TREE " pos = %ld\n", tokens->token_array[token_array_position].value.number, token_array_position);
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
}
