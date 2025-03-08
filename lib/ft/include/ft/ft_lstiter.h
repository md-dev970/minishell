#ifndef FT_LSTITER_H
#define FT_LSTITER_H

/* Apply a given function to the content of each element */
void ft_lstiter(t_list *lst, void (*f)(void *));

#endif /* FT_LSTITER_H */