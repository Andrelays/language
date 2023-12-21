#ifndef OPERATORS_H_INCLUDED
#define OPERATORS_H_INCLUDED

enum operators_index {
    MUL         = 0,
    ADD         = 1,
    SUB         = 2,
    DIV         = 3,
    POW         = 4,
    SIN         = 5,
    COS         = 6,
    TAN         = 7,
    LOG         = 8,
    EXP         = 9,
    ASGMT       = 10,
    IF          = 11,
    BEGIN_PROG  = 12,
    END_PROG    = 13,
    OPEN_PAR    = 14,
    CLOSE_PAR   = 15,
    BEGIN_OPER  = 16,
    END_OPER    = 17,
    SEQ_EXEC    = 18,
    WHILE       = 19
};

struct operator_parametrs {
    const char *name;
    const char *real_name;
    bool        is_binary;
};

const operator_parametrs KEY_WORDS[] = {[MUL]         = {"*",       "*",                            true },
                                        [ADD]         = {"+",       "+",                            true },
                                        [SUB]         = {"-",       "-",                            true },
                                        [DIV]         = {"/",       "/",                            true },
                                        [POW]         = {"^",       "^",                            true },
                                        [SIN]         = {"sin",     "sin",                          false},
                                        [COS]         = {"cos",     "cos",                          false},
                                        [TAN]         = {"tan",     "tan",                          false},
                                        [LOG]         = {"log",     "log",                          false},
                                        [EXP]         = {"exp",     "exp",                          false},
                                        [ASGMT]       = {"=",       "=",                            false},
                                        [IF]          = {"if",      "турель в кустах",              false},
                                        [BEGIN_PROG]  = {"-",       "загрузка клиента",             false},
                                        [END_PROG]    = {"-",       "alf+f4",                       false},
                                        [OPEN_PAR]    = {"(",       "(",                            false},
                                        [CLOSE_PAR]   = {")",       ")",                            false},
                                        [BEGIN_OPER]  = {"{",       "проснулся на пляже",           false},
                                        [END_OPER]    = {"}",       "закончили упражнение",         false},
                                        [SEQ_EXEC]    = {";",       ";",                            false},
                                        [WHILE]       = {"while",   "мимо пробежал клан, человек",  false}};

#endif //OPERATORS_H_INCLUDED

