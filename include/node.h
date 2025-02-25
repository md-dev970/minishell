#ifndef NODE_H
#define NODE_H

#include "token.h"

typedef struct node {
        enum token_type type;
        char *value;
        struct node *left;
        struct node *center;
        struct node *right;
} node;

#endif /* NODE_H */