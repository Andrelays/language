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

tree *input_tree_from_database(FILE *database_file);

void print_node(FILE *file_output, const tree_node *tree_node_pointer, const variable_parametrs *variable_array, const tree_node *parent_tree_node_pointer);

void print_variable_array(FILE *file_output, const variable_parametrs *variable_array, ssize_t variable_array_size);

void split_buffer_into_tokens(tree *tree_pointer, const char *database_buffer, token_info *tokens);

tree_node *get_grammar(token_info *, tree *);

#endif //FRONT_END_H_INCLUDED
