#ifndef FRONT_END_H_INCLUDED
#define FRONT_END_H_INCLUDED

#include <stdlib.h>
#include "../libraries/tree/tree.h"

struct token_info {
    tree_node               *token_array;
    ssize_t                  size_token_array;
    ssize_t                  token_array_position;
    bool                     is_error;
};

tree *input_tree_from_database(FILE *file_input, FILE *file_input_var);

void print_node(FILE *file_output, const tree_node *tree_node_pointer, const variable_parametrs *variable_array, const tree_node *parent_tree_node_pointer);

#endif //FRONT_END_H_INCLUDED
