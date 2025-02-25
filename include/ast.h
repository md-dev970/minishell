#ifndef AST_H
#define AST_H

#include "token.h"
#include "node.h"
#include "libft.h"

void print_tree(struct node *root);

void free_tree(struct node *root);

#endif /* AST_H */