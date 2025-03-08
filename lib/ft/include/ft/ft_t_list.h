#ifndef FT_T_LIST_H
#define FT_T_LIST_H

/* t_list structure */
typedef struct t_list {
        /* content of the t_list element */
        void *content;
        /* pointer to the next element */
        struct t_list *next;
} t_list;

#endif /* FT_T_LIST_H */