#ifndef NODE_H
#define NODE_H

#include "token.h"

struct node {
        enum token_type type;
        char *value;
        struct node *left;
        struct node *center;
        struct node *right;
};

#endif /* NODE_H */