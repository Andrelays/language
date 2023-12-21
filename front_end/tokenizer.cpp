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

 //TODO check unary-minus

static ssize_t       find_key_word           (const char *current_token);
static void          check_size_token_array  (token_info *tokens);

void split_buffer_into_tokens(tree *tree_pointer, const char *database_buffer, token_info *tokens)
{
    MYASSERT(database_buffer    != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(tree_pointer       != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(tree_pointer->info != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    while (*(database_buffer = skip_spaces(database_buffer)) != '\0')
    {
        ssize_t  operator_index = NO_KEY_WORD;
        ssize_t  variable_index = NO_VARIABLE;
        char    *end_number_ptr = NULL;
        double   current_number = 0;

        check_size_token_array(tokens);

        if ((operator_index = find_key_word(database_buffer)) != NO_KEY_WORD)
        {
            CURRENT_TOKEN.type = KEY_WORD;
            CURRENT_TOKEN.value.operator_index = operator_index;

            ++tokens->token_array_position;
            database_buffer += strlen(KEY_WORDS[operator_index].real_name);

            continue;
        }

        current_number = strtod(database_buffer, &end_number_ptr);

        if (database_buffer != end_number_ptr)
        {
            CURRENT_TOKEN.type = NUMBER;
            CURRENT_TOKEN.value.number = current_number;

            ++tokens->token_array_position;
            database_buffer = end_number_ptr;

            continue;
        }

        if (isalpha(*database_buffer))
        {
            size_t variable_len = calculate_variable_len(database_buffer);

            if ((variable_index = find_variable(database_buffer, variable_len, tree_pointer->variable_array, tree_pointer->variable_array_position)) == NO_VARIABLE)
            {
                if (!add_variable(database_buffer, variable_len, tree_pointer->variable_array, &tree_pointer->variable_array_position))
                {
                    printf(RED "Error! Too many variables\n" RESET_COLOR);
                    continue;
                }

                variable_index = tree_pointer->variable_array_position - 1;
            }

            CURRENT_TOKEN.type = VARIABLE;
            CURRENT_TOKEN.value.variable_index = variable_index;
            ++tokens->token_array_position;

            database_buffer += variable_len;

            continue;
        }

        printf(RED "Error!\nUnknown character <%c> %d\n" RESET_COLOR, *database_buffer, *database_buffer);
        break;
    }

    tokens->size_token_array = tokens->token_array_position;
    tokens->token_array_position = 0;

    return;
}

static ssize_t find_key_word(const char *current_token)
{
    MYASSERT(current_token != NULL, NULL_POINTER_PASSED_TO_FUNC, return NO_KEY_WORD);

    ssize_t size_operators_array = sizeof(KEY_WORDS) / sizeof(*KEY_WORDS);

    for (ssize_t operator_index = 0; operator_index < size_operators_array; operator_index++)
    {
        if (strncmp(KEY_WORDS[operator_index].real_name, current_token, strlen(KEY_WORDS[operator_index].real_name)) == 0) {
            return operator_index;
        }
    }

    return NO_KEY_WORD;
}

static void check_size_token_array(token_info *tokens)
{
    MYASSERT(tokens != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    if (tokens->token_array_position >= tokens->size_token_array)
    {
        tokens->size_token_array *= 2;
        tokens->token_array = (tree_node *) realloc(tokens->token_array, sizeof(tree_node) * (size_t) tokens->size_token_array);
    }
}
