#ifndef EXPANDER_H
#define EXPANDER_H

#include "node.h"
#include "args.h"
#include <readline/readline.h>
#include <sys/fcntl.h>

void expander(struct node *ast, t_list **l, int last_exit_status);

#endif /* EXPANDER_H */