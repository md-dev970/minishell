#include "expander.h"

static int heredoc(char *delimiter)
{
        char *input = readline(">");
        int p[2];
        if (pipe(p)) {
                #ifdef DEBUG
                printf("Pipe opening failed\n");
                #endif
                return - 1;
        }
        while (ft_strlen(input) != ft_strlen(delimiter) ||
                ft_strncmp(input, delimiter, ft_strlen(input)) != 0) {
                ft_putstr_fd(input, p[1]);
                ft_putchar_fd('\n', p[1]);
                free(input);
                input = readline(">");
        }
        free(input);
        close(p[1]);
        return p[0];
}

static char *expand_env(char *str, int last_exit_status)
{
        if (!str)
                return NULL;
        char *s = str;
        #ifdef DEBUG
        printf("string to expand : %s[end]\n", s);
        #endif
        char q = '\0';
        size_t len = ft_strlen(s);
        t_list *lst = NULL;
        char *tmp;
        /* Needs refactoring */
        for (size_t i = 0; i < len; ++i) {
                if ((s[i] == '\'' || s[i] == '\"') && !q) {
                        q = s[i];
                        continue;
                }
                size_t j = i;
                if (q == '\'') {
                        while (j < len && s[j] != q)
                                j++;

                        if (j < len)
                                q = '\0';

                        if (j > i)
                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));

                        i = j;


                } else if (q == '\"') {
                        while (j < len && s[j] != q) {
                                if (s[j] != '$') {
                                        j++;
                                        continue;
                                }
                                        
                                if (j > i)
                                        ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                                i = j;
                                size_t k = j + 1;
                                while (k < len && (ft_isalnum(s[k]) || s[k] == '_'))
                                        k++;
                                if (k - j > 1) {
                                        tmp = ft_substr(s, j + 1, k - j - 1);
                                        ft_lstadd_back(&lst, ft_lstnew(ft_strdup(getenv(tmp))));
                                        free(tmp);
                                } else if (s[k] == '?') {
                                        ft_lstadd_back(&lst, ft_lstnew(ft_itoa(last_exit_status)));
                                        k++;
                                }
                                j = k;
                                i = j;
                        }
                        if (j < len)
                                q = '\0';
                        if (j > i)
                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                        i = j;
                } else {
                        while (j < len && s[j] != '\'' && s[j] != '\"') {
                                if (s[j] != '$') {
                                        j++;
                                        continue;
                                }
                                        
                                if (j > i)
                                        ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                                i = j;
                                size_t k = j + 1;
                                while (k < len && (ft_isalnum(s[k]) || s[k] == '_'))
                                        k++;

                                
                                if (k - j > 1) {
                                        tmp = ft_substr(s, j + 1, k - j - 1);
                                        ft_lstadd_back(&lst, ft_lstnew(ft_strdup(getenv(tmp))));
                                        free(tmp);
                                } else if (s[k] == '?') {
                                        ft_lstadd_back(&lst, ft_lstnew(ft_itoa(last_exit_status)));
                                        k++;
                                }
                                j = k;
                                i = k;
                        }
                        if (j > i)
                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                        i = j;
                }
        }
        #ifdef DEBUG
        printf("list size: %i\n", ft_lstsize(lst));
        #endif
        t_list *tmp_lst = lst;
        size_t n = 0;
        while (tmp_lst) {
                n += ft_strlen((char *)tmp_lst->content);
                tmp_lst = tmp_lst->next;
        }
        char *ret = (char *)malloc((n + 1) * sizeof(char));
        if (ret == NULL) {
                ft_lstclear(lst, &free);
                return NULL;
        }
        ret[0] = '\0';
        tmp_lst = lst;
        while (tmp_lst) {
                ft_strlcat(ret, (char *)tmp_lst->content, n + 1);
                tmp_lst = tmp_lst->next;
        }
        #ifdef DEBUG
        printf("result: %s[end]\n", ret);
        #endif
        ft_lstclear(lst, &free);
        return ret;
}

static void add_input(struct node *ast, struct args *input, int last_exit_status)
{
        if (!ast) {
                #ifdef DEBUG
                printf("null\n");
                #endif
                return;
        }
        if (ast->type == IDENT)
                ft_lstadd_back(&(input->clargs), ft_lstnew(expand_env(ast->value, last_exit_status)));

        if (ast->type == NONE && ast->left->type == DLT) {
                #ifdef DEBUG
                printf("delimiter is: %s\n", ast->center->value);
                #endif
                struct fileHandler *f = (struct fileHandler *)malloc(sizeof(struct fileHandler));
                f->flag = heredoc(ast->center->value);
                f->path = NULL;
                ft_lstadd_back(&(input->fileHandlers), ft_lstnew(f));
                return;
        }

        if (ast->type == NONE && ast->left->type == LT) {
                struct fileHandler *f = (struct fileHandler *)malloc(sizeof(struct fileHandler));
                f->flag = 0;
                f->path = ast->center->value;
                ft_lstadd_back(&(input->fileHandlers), ft_lstnew(f));
                return;
        }

        if (ast->type == NONE && ast->left->type == DGT) {
                struct fileHandler *f = (struct fileHandler *)malloc(sizeof(struct fileHandler));
                f->flag = 2;
                f->path = ast->center->value;
                ft_lstadd_back(&(input->fileHandlers), ft_lstnew(f));
                return;
        }

        if (ast->type == NONE && ast->left->type == GT) {
                struct fileHandler *f = (struct fileHandler *)malloc(sizeof(struct fileHandler));
                f->flag = 1;
                f->path = ast->center->value;
                ft_lstadd_back(&(input->fileHandlers), ft_lstnew(f));
                return;
        }

        add_input(ast->left, input, last_exit_status);
        add_input(ast->center, input, last_exit_status);
}

static struct args *expand_input(struct node *ast, int last_exit_status)
{
        if (!ast)
                return NULL;

        struct args *a = (struct args *)malloc(sizeof(struct args));
        a->clargs = NULL;
        a->fileHandlers = NULL;
        add_input(ast, a, last_exit_status);
        return a;
}

void expander(struct node *ast, t_list **l, int last_exit_status)
{
        if (!ast)
                return;
        struct args *ar = expand_input(ast, last_exit_status);
        ft_lstadd_back(l, ft_lstnew(ar));
        if (ast->right)
                expander(ast->right->center, l, last_exit_status);
}