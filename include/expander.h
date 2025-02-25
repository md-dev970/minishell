#ifndef EXPANDER_H
#define EXPANDER_H

#include "node.h"
#include <readline/readline.h>
#include <sys/fcntl.h>

struct fileHandler {
        char *path;
        int flag;
};

struct args {
        t_list *clargs;
        t_list *fileHandlers;
};

void expander(struct node *ast, t_list **l);

#endif /* EXPANDER_H */