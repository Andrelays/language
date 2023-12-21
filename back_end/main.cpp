#include <string.h>
#include <stdio.h>
#include "../libraries/utilities/myassert.h"
#include "../libraries/utilities/utilities.h"
#include "back_end.h"

int main(int argc, const char *argv[])
{
    const int CORRECT_NUMBER_OF_ARGC = 5;

    if(!check_argc(argc, CORRECT_NUMBER_OF_ARGC)) {
        return INCORRECT_NUMBER_OF_ARGC;
    }

    const char *file_name_logs        = argv[1];
    const char *file_name_input       = argv[2];
    const char *file_name_output_tree = argv[3];
    const char *file_name_input_var   = argv[4];

    Global_logs_pointer_tree = check_isopen_html(file_name_logs,   "w");
    FILE *file_input         = check_isopen(file_name_input,       "r");
    FILE *file_output_tree   = check_isopen(file_name_output_tree, "w");
    FILE *file_input_var     = check_isopen(file_name_input_var,   "r");

    MYASSERT(Global_logs_pointer_tree != NULL, COULD_NOT_OPEN_THE_FILE , return COULD_NOT_OPEN_THE_FILE);
    MYASSERT(file_input               != NULL, COULD_NOT_OPEN_THE_FILE , return COULD_NOT_OPEN_THE_FILE);
    MYASSERT(file_output_tree         != NULL, COULD_NOT_OPEN_THE_FILE , return COULD_NOT_OPEN_THE_FILE);
    MYASSERT(file_input_var           != NULL, COULD_NOT_OPEN_THE_FILE , return COULD_NOT_OPEN_THE_FILE);

    tree *tree_pointer = input_tree_from_database(file_input, file_input_var);

    print_node(file_output_tree, tree_pointer->root, tree_pointer->variable_array, tree_pointer->root);

    tree_destructor(tree_pointer);

    MYASSERT(check_isclose(Global_logs_pointer_tree), COULD_NOT_CLOSE_THE_FILE , return COULD_NOT_CLOSE_THE_FILE);
    MYASSERT(check_isclose(file_input),               COULD_NOT_CLOSE_THE_FILE , return COULD_NOT_CLOSE_THE_FILE);
    MYASSERT(check_isclose(file_output_tree),         COULD_NOT_CLOSE_THE_FILE , return COULD_NOT_CLOSE_THE_FILE);
    MYASSERT(check_isclose(file_input_var),           COULD_NOT_CLOSE_THE_FILE , return COULD_NOT_CLOSE_THE_FILE);

    return 0;
}
