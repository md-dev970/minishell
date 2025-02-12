#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef struct node {
        enum token_type type;
        char *value;
        struct node *left;
        struct node *center;
        struct node *right;
} node;

node *parser(t_list *lexems);

#endif /* PARSER_H */