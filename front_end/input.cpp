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

const ssize_t NO_KEY_WORD = -1;
const ssize_t NO_VARIABLE = -1;

#define syn_assert(condition)                                                                                                                                           \
 do {                                                                                                                                                                   \
    if (!(condition))                                                                                                                                                   \
    {                                                                                                                                                                   \
        printf(RED  "File: %s\n"                                                                                                                                        \
                    "line: %d\n"                                                                                                                                        \
                    "Function: %s\n"                                                                                                                                    \
                    "Position: %ld\n"                                                                                                                                   \
                    "The condition is not met: \"%s\"\n"RESET_COLOR, __FILE__, __LINE__, __PRETTY_FUNCTION__, parsing->token_array_position, #condition);               \
                                                                                                                                                                        \
        return 0;                                                                                                                                                       \
    }                                                                                                                                                                   \
}  while(0)

static tree_node *get_grammar        (parsing_info *, tree *);
static tree_node *get_operator       (parsing_info *, tree *);
static tree_node *get_if             (parsing_info *, tree *);
static tree_node *get_assigment      (parsing_info *, tree *);
static tree_node *get_sum            (parsing_info *, tree *);
static tree_node *get_product        (parsing_info *, tree *);
static tree_node *get_power          (parsing_info *, tree *);
// static tree_node *get_unary_functions(parsing_info *, tree *);
static tree_node *get_parenthesis    (parsing_info *, tree *);
static tree_node *get_number         (parsing_info *, tree *);
static tree_node *get_variable       (parsing_info *, tree *);

static char         *saving_buffer           (FILE *database_file, size_t *buffer_size);                //TODO del buf_size - unused
static parsing_info *parsing_info_constructor(tree *tree_pointer, const char *database_buffer);
static ssize_t       find_key_word           (const char *current_token);
static ssize_t       find_variable           (const char *current_token, size_t variable_len, variable_parametrs *variable_array, ssize_t variable_array_size);
static bool          add_variable            (const char *current_token, size_t variable_len, variable_parametrs *variable_array, ssize_t *variable_array_position_pointer);
static void          check_size_token_array  (parsing_info *parsing);
static void          parsing_info_destructor (parsing_info *parsing);
static size_t        calculate_variable_len  (const char *current_token);
static void          print_parsing_info      (const parsing_info *parsing);

ssize_t input_tree_from_database(FILE *database_file, tree *tree_pointer)
{
    MYASSERT(database_file      != NULL, NULL_POINTER_PASSED_TO_FUNC, return DATABASE_FILE_IS_NULL);
    MYASSERT(tree_pointer       != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_IS_NULL);
    MYASSERT(tree_pointer->info != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_INFO_IS_NULL);

    size_t buffer_size = 0;

    char *database_buffer = saving_buffer(database_file, &buffer_size);
    MYASSERT(database_buffer != NULL, NULL_POINTER_PASSED_TO_FUNC, return DATABASE_BUFFER_IS_NULL);

    delete_node(tree_pointer->root);
    tree_pointer->root = NULL;
    tree_pointer->size = 0;
    tree_pointer->variable_array_position = 0;

    parsing_info *parsing = parsing_info_constructor(tree_pointer, database_buffer);

    print_parsing_info(parsing);

    tree_pointer->root = get_grammar(parsing, tree_pointer);

    parsing_info_destructor(parsing);
    free(database_buffer);

    return VERIFY_TREE(tree_pointer);;
}

static void print_parsing_info(const parsing_info *parsing)
{
    printf("size = %ld\n", parsing->size_token_array);

    for (ssize_t token_array_position = 0; token_array_position < parsing->size_token_array; token_array_position++)
    {
        switch(parsing->token_array[token_array_position].type)
        {
            case VARIABLE:
            {
                printf("VAR %ld pos = %ld\n", parsing->token_array[token_array_position].value.variable_index, token_array_position);
                break;
            }

            case KEY_WORD:
            {
                printf("KWD <%s> pos = %ld\n", KEY_WORDS[parsing->token_array[token_array_position].value.operator_index].name, token_array_position);
                break;
            }

            case NUMBER:
            {
                printf("NUM " FORMAT_SPECIFIERS_TREE " pos = %ld\n", parsing->token_array[token_array_position].value.number, token_array_position);
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

static parsing_info *parsing_info_constructor(tree *tree_pointer, const char *database_buffer)                  //TODO rename
{
    MYASSERT(database_buffer    != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer       != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer->info != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    const ssize_t INITIAL_SIZE_TOKENS_ARRAY = 100;

    parsing_info *parsing = (parsing_info *) calloc(1, sizeof(parsing_info));;

    parsing->size_token_array = INITIAL_SIZE_TOKENS_ARRAY;
    parsing->token_array_position = 0;

    parsing->token_array = (tree_node *) calloc(INITIAL_SIZE_TOKENS_ARRAY, sizeof(tree_node));

    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    while (*(database_buffer = skip_spaces(database_buffer)) != '\0')
    {
        ssize_t  operator_index = NO_KEY_WORD;
        ssize_t  variable_index = NO_VARIABLE;
        char    *end_number_ptr = NULL;
        double   current_number = 0;

        check_size_token_array(parsing);

        // printf("<%c> %d\n", *database_buffer, *database_buffer);

        if ((operator_index = find_key_word(database_buffer)) != NO_KEY_WORD)
        {
            CURRENT_TOKEN.type = KEY_WORD;
            CURRENT_TOKEN.value.operator_index = operator_index;

            ++parsing->token_array_position;
            database_buffer += strlen(KEY_WORDS[operator_index].name);

            continue;
        }

        current_number = strtod(database_buffer, &end_number_ptr);

        if (database_buffer != end_number_ptr)
        {
            CURRENT_TOKEN.type = NUMBER;
            CURRENT_TOKEN.value.number = current_number;

            ++parsing->token_array_position;
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
            ++parsing->token_array_position;

            database_buffer += variable_len;

            continue;
        }

        printf(RED "Error!\nUnknown character <%c> %d\n" RESET_COLOR, *database_buffer, *database_buffer);
        break;
    }

    parsing->size_token_array = parsing->token_array_position;
    parsing->token_array_position = 0;

    return parsing;
}

static size_t calculate_variable_len(const char *current_token)
{
    MYASSERT(current_token  != NULL, NULL_POINTER_PASSED_TO_FUNC, return 0);

    size_t index_variable_name = 0;

    while (isalnum(current_token[index_variable_name])) {
        ++index_variable_name;
    }

    return index_variable_name;
}

static void parsing_info_destructor(parsing_info *parsing)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    free(parsing->token_array);

    parsing->token_array          = NULL;
    parsing->size_token_array     = -1;
    parsing->token_array_position = -1;

    free(parsing);

    parsing = NULL;
}

static ssize_t find_key_word(const char *current_token)
{
    MYASSERT(current_token != NULL, NULL_POINTER_PASSED_TO_FUNC, return NO_KEY_WORD);

    ssize_t size_operators_array = sizeof(KEY_WORDS) / sizeof(*KEY_WORDS);

    for (ssize_t operator_index = 0; operator_index < size_operators_array; operator_index++)
    {
        if (strncmp(KEY_WORDS[operator_index].name, current_token, strlen(KEY_WORDS[operator_index].name)) == 0) {
            return operator_index;
        }
    }

    return NO_KEY_WORD;
}

static ssize_t find_variable(const char *current_token, size_t variable_len, variable_parametrs *variable_array, ssize_t variable_array_size)
{
    MYASSERT(current_token  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NO_VARIABLE);
    MYASSERT(variable_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NO_VARIABLE);

    for (ssize_t variable_index = 0; variable_index < variable_array_size; variable_index++)                            //TODO check variable_len > MAX_LEN
    {
        if (strncmp((variable_array[variable_index]).name, current_token, variable_len) == 0) {
            return variable_index;
        }
    }

    return NO_VARIABLE;
}

static bool add_variable(const char *current_token, size_t variable_len, variable_parametrs *variable_array, ssize_t *variable_array_position_pointer)
{
    MYASSERT(current_token  != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);
    MYASSERT(variable_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);

    ssize_t variable_array_position = *variable_array_position_pointer;

    if ((size_t) variable_array_position >= SIZE_ARRAY_OF_VARIABLE)
    {
        printf(RED "ERROR! Too many variable. SIZE_ARRAY_OF_VARIABLE = %lu.\n" RESET_COLOR, SIZE_ARRAY_OF_VARIABLE);
        return false;
    }

    strncpy(variable_array[variable_array_position].name, current_token, variable_len);

    ++(*variable_array_position_pointer);

    return true;
}

static void check_size_token_array(parsing_info *parsing)
{
    MYASSERT(parsing != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    if (parsing->token_array_position >= parsing->size_token_array)
    {
        parsing->size_token_array *= 2;
        parsing->token_array = (tree_node *) realloc(parsing->token_array, sizeof(tree_node) * (size_t) parsing->size_token_array);
    }
}

static char *saving_buffer(FILE *database_file, size_t *buffer_size)
{
    MYASSERT(database_file != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    *buffer_size = determine_size(database_file);

    char *database_buffer = (char *)calloc(*buffer_size + 1, sizeof(char));

    MYASSERT(database_buffer != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    *buffer_size = fread(database_buffer, sizeof(char), *buffer_size, database_file);
    database_buffer[*buffer_size] = '\0';

    return database_buffer;
}

static tree_node *get_grammar(parsing_info *parsing, tree *tree_pointer)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    syn_assert(IS_BEGIN_OF_PROGRAM);
    ++parsing->token_array_position;

    tree_node *tree_node_pointer = get_operator(parsing, tree_pointer);

    while (parsing->token_array_position < parsing->size_token_array - 1)
    {
        ssize_t old_token_array_position = parsing->token_array_position;
        tree_node *tree_node_pointer_2 = get_operator(parsing, tree_pointer);

        syn_assert(old_token_array_position < parsing->token_array_position);

        tree_node_pointer = create_node(KEY_WORD, {.operator_index = SEQ_EXEC}, tree_node_pointer, tree_node_pointer_2);
    }

    syn_assert(IS_END_OF_PROGRAM);

    return tree_node_pointer;
}

static tree_node *get_operator(parsing_info *parsing, tree *tree_pointer)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    printf("pos operator = %ld\n", parsing->token_array_position);

    if (IS_BEGIN_OPERATOR)                                                                                          //FIXME error recursion
    {
        ++parsing->token_array_position;
        tree_node_pointer = get_operator(parsing, tree_pointer);

        while (!(IS_END_OPERATOR))
        {
            ++tree_pointer->size;

            printf("pos = %ld\n", parsing->token_array_position);

            tree_node *tree_node_pointer_2 = get_operator(parsing, tree_pointer);

            tree_node_pointer = create_node(KEY_WORD, {.operator_index = SEQ_EXEC}, tree_node_pointer, tree_node_pointer_2);
        }

        ++parsing->token_array_position;
        return tree_node_pointer;
    }

    if (!(tree_node_pointer = get_if(parsing, tree_pointer)))
    {
        tree_node_pointer   = get_assigment(parsing, tree_pointer);
        syn_assert(IS_SEQUENTIAL_EXEC_OPER);
        ++parsing->token_array_position;
    }

    return tree_node_pointer;
}

static tree_node *get_if(parsing_info *parsing, tree *tree_pointer)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if (IS_KWD_IF)
    {
        ++parsing->token_array_position;
        ++tree_pointer->size;

        syn_assert(IS_OPEN_PARENTHESIS);
        ++parsing->token_array_position;

        tree_node_pointer = get_sum(parsing, tree_pointer);

        syn_assert(IS_CLOSING_PARENTHESIS);
        ++parsing->token_array_position;

        printf("pos if = %ld\n", parsing->token_array_position);

        tree_node *tree_node_pointer_2 = get_operator(parsing, tree_pointer);

        tree_node_pointer = create_node(KEY_WORD, {.operator_index = IF}, tree_node_pointer, tree_node_pointer_2);
    }

    return tree_node_pointer;
}

static tree_node *get_assigment(parsing_info *parsing, tree *tree_pointer)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    ++parsing->token_array_position;                                                        //TODO check size array

    if(IS_ASSIGMENT)
    {
        --parsing->token_array_position;

        tree_node_pointer = get_variable(parsing, tree_pointer);

        ++parsing->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_sum(parsing, tree_pointer);

        tree_node_pointer = create_node(KEY_WORD, {.operator_index = ASGMT}, tree_node_pointer, tree_node_pointer_2);
    }

    else {
        --parsing->token_array_position;
    }

    return tree_node_pointer;
}

static tree_node *get_sum(parsing_info *parsing, tree *tree_pointer)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = get_product(parsing, tree_pointer);

    while (CURRENT_TOKEN.type == KEY_WORD && (CURRENT_TOKEN.value.operator_index == ADD || CURRENT_TOKEN.value.operator_index == SUB))
    {
        ssize_t operator_index = CURRENT_TOKEN.value.operator_index;
        ++parsing->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_product(parsing, tree_pointer);

        printf("type = %d Value = %f\n", CURRENT_TOKEN.type, CURRENT_TOKEN.value.number);

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

static tree_node *get_product(parsing_info *parsing, tree *tree_pointer)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = get_power(parsing, tree_pointer);

    while (CURRENT_TOKEN.type == KEY_WORD && (CURRENT_TOKEN.value.operator_index == MUL || CURRENT_TOKEN.value.operator_index == DIV))
    {
        ssize_t operator_index = CURRENT_TOKEN.value.operator_index;
        ++parsing->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_power(parsing, tree_pointer);

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

static tree_node *get_power(parsing_info *parsing, tree *tree_pointer)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = get_parenthesis(parsing, tree_pointer);

    while (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == POW)
    {
        ++parsing->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_parenthesis(parsing, tree_pointer);

        tree_node_pointer = create_node(KEY_WORD, {.operator_index = POW}, tree_node_pointer, tree_node_pointer_2);
    }

    return tree_node_pointer;
}

// static tree_node *get_unary_functions(parsing_info *parsing, tree *tree_pointer)
// {
//     MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
//     MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
//     MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
//
//     tree_node *tree_node_pointer = NULL;
//
//     if (CURRENT_TOKEN.type == KEY_WORD && !KEY_WORDS[CURRENT_TOKEN.value.operator_index].is_binary)
//     {
//         ssize_t operator_index = CURRENT_TOKEN.value.operator_index;
//         ++parsing->token_array_position;
//         ++tree_pointer->size;
//
//         tree_node_pointer = get_unary_functions(parsing, tree_pointer);                                                                                     //FIXME sin cos tan - no language
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
//     tree_node_pointer = get_parenthesis(parsing, tree_pointer);
//
//     return tree_node_pointer;
// }

static tree_node *get_parenthesis(parsing_info *parsing, tree *tree_pointer)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if (IS_OPEN_PARENTHESIS)
    {
        ++parsing->token_array_position;

        tree_node_pointer = get_sum(parsing, tree_pointer);

        syn_assert(IS_CLOSING_PARENTHESIS);
        ++parsing->token_array_position;

        return tree_node_pointer;
    }

    if (!(tree_node_pointer = get_number(parsing, tree_pointer))) {
        tree_node_pointer   = get_variable(parsing, tree_pointer);
    }

    return tree_node_pointer;
}

static tree_node *get_number(parsing_info *parsing, tree *tree_pointer)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if(CURRENT_TOKEN.type == NUMBER)
    {
        tree_node_pointer = create_node(NUMBER, CURRENT_TOKEN.value, NULL, NULL);
        ++parsing->token_array_position;
        ++tree_pointer->size;
    }

    return tree_node_pointer;
}

tree_node *get_variable(parsing_info *parsing, tree *tree_pointer)
{
    MYASSERT(parsing              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer         != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    ssize_t string_index_old = parsing->token_array_position;

    if(CURRENT_TOKEN.type == VARIABLE)
    {
        tree_node_pointer = create_node(VARIABLE, CURRENT_TOKEN.value, NULL, NULL);
        ++parsing->token_array_position;
        ++tree_pointer->size;
    }

    syn_assert(string_index_old < parsing->token_array_position);

    return tree_node_pointer;
}
