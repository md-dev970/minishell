#ifndef FT_LSTDELONE_H
#define FT_LSTDELONE_H

/* Delete one element of a t_list */
void ft_lstdelone(t_list *lst, void (*del)(void *));

#endif /* FT_LSTDELONE_H */