#ifndef OPERATORS_H_INCLUDED
#define OPERATORS_H_INCLUDED

#include "front_end.h"

struct operator_parametrs {
    const char *name;
    bool        is_binary;
};

const operator_parametrs KEY_WORDS[] = {[MUL]   = {"*",              true },
                                        [ADD]   = {"+",       true },
                                        [SUB]   = {"-",           true },
                                        [DIV]   = {"/",              true },
                                        [POW]   = {"^",              true },
                                        [SIN]   = {"sin",            false},
                                        [COS]   = {"cos",            false},
                                        [TAN]   = {"tan",            false},
                                        [LOG]   = {"log",            false},
                                        [EXP]   = {"exp",            false},
                                        [ASGMT] = {"=",              false},
                                        [IF]    = {"турель в кустах",false},
                                        [BEGIN_PROG]   = {"загрузка клиента", false},
                                        [END_PROG]   = {"alf+f4", false},
                                        [OPEN_PAR]  = {"(", false},
                                        [CLOSE_PAR]  = {")", false},
                                        [BEGIN_OPER]  = {"проснулся на пляже", false},
                                        [END_OPER]  = {"закончили упражнение", false},
                                        [SEQ_EXEC]  = {";", false}};

#endif //OPERATORS_H_INCLUDED

