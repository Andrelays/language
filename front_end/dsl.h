#ifndef DSL_H_INCLUDED
#define DSL_H_INCLUDED

#define  CURRENT_VARIABLE            variable_array[tree_node_pointer->value.variable_index]
#define  CURRENT_CALCULATE_FUNC      OPERATORS[tree_node_pointer->value.operator_index].calculate_func
#define  CURRENT_OPERATOR            OPERATORS[tree_node_pointer->value.operator_index]
#define  CURRENT_DIFFERENTIATE_FUNC  OPERATORS[tree_node_pointer->value.operator_index].differentiate_func
#define  CURRENT_TOKEN               parsing->token_array[parsing->token_array_position]
#define  VALUE_OF_NODE               tree_node_pointer->value
#define  OPERATOR_PRIORITY(node)     OPERATORS[(node)->value.operator_index].priority
#define  DIF_BY_CUR_VAR(node)        differentiate_node_by_variable(node, variable_index)

#define  CREATE_NUM(num)                   create_node(NUMBER,   {.number = num},         NULL,      NULL)
#define  CREATE_MUL(left_node, right_node) create_node(OPERATOR, {.operator_index = MUL}, left_node, right_node)
#define  CREATE_ADD(left_node, right_node) create_node(OPERATOR, {.operator_index = ADD}, left_node, right_node)
#define  CREATE_SUB(left_node, right_node) create_node(OPERATOR, {.operator_index = SUB}, left_node, right_node)
#define  CREATE_DIV(left_node, right_node) create_node(OPERATOR, {.operator_index = DIV}, left_node, right_node)
#define  CREATE_POW(left_node, right_node) create_node(OPERATOR, {.operator_index = POW}, left_node, right_node)
#define  CREATE_LOG(node)                  create_node(OPERATOR, {.operator_index = LOG}, NULL,      node)
#define  CREATE_EXP(node)                  create_node(OPERATOR, {.operator_index = EXP}, NULL,      node)
#define  CREATE_COS(node)                  create_node(OPERATOR, {.operator_index = COS}, NULL,      node)
#define  CREATE_SIN(node)                  create_node(OPERATOR, {.operator_index = SIN}, NULL,      node)


#define  IS_KWD_IF                  CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == IF
#define  IS_OPEN_PARENTHESIS        CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == OPEN_PAR
#define  IS_CLOSING_PARENTHESIS     CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == CLOSE_PAR
#define  IS_BEGIN_OF_PROGRAM        CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == BEGIN_PROG
#define  IS_END_OF_PROGRAM          CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == END_PROG
#define  IS_ASSIGMENT               CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == ASGMT
#define  IS_BEGIN_OPERATOR          CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == BEGIN_OPER
#define  IS_END_OPERATOR            CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == END_OPER
#define  IS_SEQUENTIAL_EXEC_OPER    CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == SEQ_EXEC




#endif
