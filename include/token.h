#ifndef TOKEN_H
#define TOKEN_H


#include <stdio.h>
#include "ft/libft.h"


enum token_type {
        PIPE,
        GT,
        LT,
        DGT,
        DLT,
        IDENT,
        NONE
};

struct token {
        enum token_type type;
        char *value;
};

void *generate_token(void *value);

void free_token(void *token);

void print_token(void *token);

#endif /* TOKEN_H */