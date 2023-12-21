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

static tree_node *saving_node_from_database(tree *tree_pointer, const char *database_buffer, ssize_t *buffer_position);
static void       input_variables          (tree *tree_pointer, const char *database_buffer_variables);
static char      *saving_buffer            (FILE *database_file);

tree *input_tree_from_database(FILE *file_input, FILE *file_input_var)
{
    MYASSERT(file_input         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(file_input_var     != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree *tree_pointer = new_pointer_tree();
    TREE_CONSTRUCTOR(tree_pointer);

    char *database_buffer = saving_buffer(file_input);
    MYASSERT(database_buffer != NULL, FAILED_TO_ALLOCATE_DYNAM_MEMOR, return NULL);

    ssize_t buffer_position = 0;

    tree_pointer->root = saving_node_from_database(tree_pointer, database_buffer, &buffer_position);

    char *database_buffer_variables = saving_buffer(file_input_var);
    MYASSERT(database_buffer_variables != NULL, FAILED_TO_ALLOCATE_DYNAM_MEMOR, return NULL);

    input_variables(tree_pointer, database_buffer_variables);

    free(database_buffer);
    free(database_buffer_variables);

    VERIFY_TREE(tree_pointer);

    return tree_pointer;
}

static void input_variables(tree *tree_pointer, const char *database_buffer_variables)
{
    MYASSERT(tree_pointer               != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(database_buffer_variables  != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    while(*database_buffer_variables != '\0')
    {
        char *end_of_num = NULL;

        size_t index_variable = (size_t) strtol(database_buffer_variables, &end_of_num, 10);
        MYASSERT(database_buffer_variables != end_of_num, INPUT_ERROR, return);

        database_buffer_variables = end_of_num;
        MYASSERT(*database_buffer_variables == ' ', INPUT_ERROR, return);

        database_buffer_variables += 1;
        MYASSERT(*database_buffer_variables == '<', INPUT_ERROR, return);

        database_buffer_variables += 1;
        size_t variable_len = calculate_variable_len(database_buffer_variables);

        MYASSERT(index_variable < SIZE_ARRAY_OF_VARIABLE, GOING_BEYOUND_BOUNDARY_ARRAY, return);
        MYASSERT(variable_len   < MAX_SIZE_NAME_VARIABLE, GOING_BEYOUND_BOUNDARY_ARRAY, return);

        strncpy(tree_pointer->variable_array[index_variable].name, database_buffer_variables, variable_len);

        tree_pointer->variable_array_position++;
        database_buffer_variables += variable_len + 1;

        database_buffer_variables = skip_spaces(database_buffer_variables);
    }
}

static tree_node *saving_node_from_database(tree *tree_pointer, const char *database_buffer, ssize_t *buffer_position)
{
    MYASSERT(database_buffer != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(buffer_position != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    // printf("1: <%s> %ld\n", database_buffer + *buffer_position, *buffer_position);

    if (database_buffer[*buffer_position] == '\0')
    {
        printf(RED "ERROR! File input error!\n" RESET_COLOR);
        return NULL;
    }

    if (database_buffer[*buffer_position] == '(')
    {
        *buffer_position += 2;

        // printf("2: <%s> %ld\n", database_buffer + *buffer_position, *buffer_position);

        char *end_of_num = NULL;

        type_node  type  = (type_node) strtol(database_buffer + *buffer_position, &end_of_num, 10);
        value_node value = {};

        tree_pointer->size++;

        // printf("\n\n%ld\n\n", strtol(database_buffer + *buffer_position, &end_of_num, 10));

        *buffer_position = end_of_num - database_buffer + 1;

        // printf("3: <%s> %ld\n", database_buffer + *buffer_position, *buffer_position);

        if (type == NUMBER) {
            value.number = strtod(database_buffer + *buffer_position, &end_of_num);
        }

        else if (type == KEY_WORD){
            value.operator_index = strtol(database_buffer + *buffer_position, &end_of_num, 10);
        }

        else {
            value.variable_index = strtol(database_buffer + *buffer_position, &end_of_num, 10);
        }

        *buffer_position = end_of_num - database_buffer + 1;

        // printf("4: %f <%s> %ld\n", value.number,  database_buffer + *buffer_position, *buffer_position);

        tree_node *left_node  = saving_node_from_database(tree_pointer, database_buffer, buffer_position);
        tree_node *right_node = saving_node_from_database(tree_pointer, database_buffer, buffer_position);

        return create_node(type, value, left_node, right_node);
    }

    if (database_buffer[*buffer_position] == '_')
    {
        *buffer_position += 2;
        return NULL;
    }

    if (database_buffer[*buffer_position] == ')')
    {
        *buffer_position += 2;
        return saving_node_from_database(tree_pointer, database_buffer, buffer_position);
    }

    return NULL;
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


