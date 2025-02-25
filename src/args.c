#include "args.h"

void free_args(void *a)
{
        struct args *ar = (struct args *)a;
        ft_lstclear(ar->clargs, &free);
        ft_lstclear(ar->fileHandlers, &free);
        free(ar);
}