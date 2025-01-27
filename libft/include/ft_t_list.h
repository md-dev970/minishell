#ifndef FT_T_LIST_H
#define FT_T_LIST_H

typedef struct t_list {
        void *content;
        struct t_list *next;
} t_list;

#endif /* FT_T_LIST_H */