#ifndef EXEC_H
#define EXEC_H

#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include "node.h"
#include "args.h"

void handle_commands(struct node *ast, int in, t_list *l);

#endif /* EXEC_H */