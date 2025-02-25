#ifndef EXPANDER_H
#define EXPANDER_H

#include "node.h"
#include <readline/readline.h>
#include <sys/fcntl.h>

typedef struct file {
        char *path;
        int flag;
        mode_t mode;
} file;

typedef struct args {
        t_list *clargs;
        t_list *files;
} args;

void expander(node *ast, t_list **l);

#endif /* EXPANDER_H */