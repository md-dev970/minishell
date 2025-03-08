#ifndef FT_LSTCLEAR_H
#define FT_LSTCLEAR_H

/* Delete all the elements of a t_list */
void ft_lstclear(t_list *lst, void (*del)(void *));

#endif /* FT_LSTCLEAR_H */