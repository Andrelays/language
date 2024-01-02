#ifndef DSL_H_INCLUDED
#define DSL_H_INCLUDED

#define  CURRENT_VARIABLE            variable_array[tree_node_pointer->value.variable_index]
#define  CURRENT_CALCULATE_FUNC      OPERATORS[tree_node_pointer->value.operator_index].calculate_func
#define  CURRENT_OPERATOR            OPERATORS[tree_node_pointer->value.operator_index]
#define  CURRENT_DIFFERENTIATE_FUNC  OPERATORS[tree_node_pointer->value.operator_index].differentiate_func
#define  CURRENT_TOKEN               tokens->token_array[tokens->token_array_position]
#define  VALUE_OF_NODE               tree_node_pointer->value
#define  OPERATOR_PRIORITY(node)     OPERATORS[(node)->value.operator_index].priority
#define  DIF_BY_CUR_VAR(node)        differentiate_node_by_variable(node, variable_index)

#define  CREATE_NUM(num)                                create_node(NUMBER,   {.number = num},                    NULL,      NULL)
#define  CREATE_MUL(left_node, right_node)              create_node(KEY_WORD, {.operator_index = MUL},            left_node, right_node)
#define  CREATE_ADD(left_node, right_node)              create_node(KEY_WORD, {.operator_index = ADD},            left_node, right_node)
#define  CREATE_SUB(left_node, right_node)              create_node(KEY_WORD, {.operator_index = SUB},            left_node, right_node)
#define  CREATE_DIV(left_node, right_node)              create_node(KEY_WORD, {.operator_index = DIV},            left_node, right_node)
#define  CREATE_IF(left_node, right_node)               create_node(KEY_WORD, {.operator_index = IF},             left_node, right_node)
#define  CREATE_WHILE(left_node, right_node)            create_node(KEY_WORD, {.operator_index = WHILE},          left_node, right_node)
#define  CREATE_POW(left_node, right_node)              create_node(KEY_WORD, {.operator_index = POW},            left_node, right_node)
#define  CREATE_LOG(node)                               create_node(KEY_WORD, {.operator_index = LOG},            NULL,      node)
#define  CREATE_EXP(node)                               create_node(KEY_WORD, {.operator_index = EXP},            NULL,      node)
#define  CREATE_COS(node)                               create_node(KEY_WORD, {.operator_index = COS},            NULL,      node)
#define  CREATE_SIN(node)                               create_node(KEY_WORD, {.operator_index = SIN},            NULL,      node)
#define  CREATE_BELOW(left_node, right_node)            create_node(KEY_WORD, {.operator_index = BELOW},          left_node, right_node)
#define  CREATE_MORE(left_node, right_node)             create_node(KEY_WORD, {.operator_index = MORE},           left_node, right_node)
#define  CREATE_EQUAL(left_node, right_node)            create_node(KEY_WORD, {.operator_index = EQUAL},          left_node, right_node)
#define  CREATE_MORE_OR_EQUAL(left_node, right_node)    create_node(KEY_WORD, {.operator_index = MORE_OR_EQUAL},  left_node, right_node)
#define  CREATE_BELOW_OR_EQUAL(left_node, right_node)   create_node(KEY_WORD, {.operator_index = BELOW_OR_EQUAL}, left_node, right_node)
#define  CREATE_NOT_EQUAL(left_node, right_node)        create_node(KEY_WORD, {.operator_index = NOT_EQUAL},      left_node, right_node)
#define  CREATE_SEQ_EXEC(left_node, right_node)         create_node(KEY_WORD, {.operator_index = SEQ_EXEC},       left_node, right_node)
#define  CREATE_ASSIGMENT(left_node, right_node)        create_node(KEY_WORD, {.operator_index = ASSIGMENT},      left_node, right_node)
#define  CREATE_AND(left_node, right_node)              create_node(KEY_WORD, {.operator_index = AND},            left_node, right_node)
#define  CREATE_OR(left_node, right_node)               create_node(KEY_WORD, {.operator_index = OR},             left_node, right_node)


#define  IS_KWD_IF                 (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == IF)
#define  IS_KWD_WHILE              (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == WHILE)
#define  IS_OPEN_PARENTHESIS       (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == OPEN_PAR)
#define  IS_CLOSING_PARENTHESIS    (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == CLOSE_PAR)
#define  IS_BEGIN_OF_PROGRAM       (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == BEGIN_PROG)
#define  IS_END_OF_PROGRAM         (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == END_PROG)
#define  IS_ASSIGMENT              (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == ASSIGMENT)
#define  IS_BEGIN_OPERATOR         (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == BEGIN_OPER)
#define  IS_END_OPERATOR           (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == END_OPER)
#define  IS_SEQUENTIAL_EXEC_OPER   (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == SEQ_EXEC)
#define  IS_AND_OPER               (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == AND)
#define  IS_OR_OPER                (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == OR)
#define  IS_BELOW_OPER             (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == BELOW)
#define  IS_MORE_OPER              (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == MORE)
#define  IS_EQUAL_OPER             (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == EQUAL)
#define  IS_MORE_OR_EQUAL_OPER     (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == MORE_OR_EQUAL)
#define  IS_BELOW_OR_EQUAL_OPER    (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == BELOW_OR_EQUAL)
#define  IS_NOT_EQUAL_OPER         (CURRENT_TOKEN.type == KEY_WORD && CURRENT_TOKEN.value.operator_index == NOT_EQUAL)

#endif
