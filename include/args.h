#ifndef ARGS_H
#define ARGS_H

#include "ft/libft.h"

struct fileHandler {
        char *path;
        int flag;
};

struct args {
        t_list *clargs;
        t_list *fileHandlers;
};

void free_args(void *a);

#endif /* ARGS_H */