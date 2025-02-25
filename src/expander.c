#include "expander.h"

static void print_lexem(void *lexem)
{
        char *s = (char *)lexem;
        printf("%s\n", s ? s : "");
}

static int heredoc(char *delimiter)
{
        char *input = readline(">");
        int p[2];
        pipe(p);
        while (ft_strlen(input) != ft_strlen(delimiter) ||
                ft_strncmp(input, delimiter, ft_strlen(input)) != 0) {
                write(p[1], input, ft_strlen(input));
                write(p[1], "\n", 1);
                free(input);
                input = readline(">");
        }
        free(input);
        close(p[1]);
        return p[0];
}

static char *expand_env(char *str)
{
        if (!str)
                return NULL;
        char *s = str;
        printf("string to expand : %s[end]\n", s);
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
                        while (j < len && s[j] != q) {
                                j++;
                        }
                        if (j < len)
                                q = '\0';
                        if (j > i)
                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                        i = j;
                } else if (q == '\"') {
                        while (j < len && s[j] != q) {
                                if (s[j] == '$') {
                                        if (j > i)
                                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                                        i = j;
                                        size_t k = j + 1;
                                        while (k < len && s[k] != '=' && s[k] != '\'' && s[k] != ' ' && s[k] != '\"')
                                                k++;
                                        tmp = ft_substr(s, j + 1, k - j - 1);
                                        if (k - j > 1)
                                                ft_lstadd_back(&lst, ft_lstnew(ft_strdup(getenv(tmp))));
                                        free(tmp);
                                        j = k - 1;
                                        i = j + 1;
                                }
                                j++;
                        }
                        if (j < len)
                                q = '\0';
                        if (j > i)
                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                        i = j;
                } else {
                        while (j < len && s[j] != '\'' && s[j] != '\"') {
                                if (s[j] == '$') {
                                        if (j > i)
                                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                                        i = j;
                                        size_t k = j + 1;
                                        while (k < len && s[k] != '=' && s[k] != '\'' && s[k] != ' ' && s[k] != '\"')
                                                k++;
                                        tmp = ft_substr(s, j + 1, k - j - 1);
                                        if (k - j > 1)
                                                ft_lstadd_back(&lst, ft_lstnew(ft_strdup(getenv(tmp))));
                                        free(tmp);
                                        j = k + 1;
                                        i = j;
                                }
                                j++;
                        }
                        if (j > i)
                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                        i = j - 1;
                }
        }
        ft_lstiter(lst, &print_lexem);
        printf("list size: %i\n", ft_lstsize(lst));
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
        printf("result: %s[end]\n", ret);
        ft_lstclear(lst, &free);
        return ret;
}

static void add_input(node *ast, args *input)
{
        if (!ast) {
                printf("null\n");
                return;
        }
        if (ast->type == IDENT)
                ft_lstadd_back(&(input->clargs), ft_lstnew(expand_env(ast->value)));

        if (ast->type == NONE && ast->left->type == DLT) {
                printf("delimiter is: %s\n", ast->center->value);
                file *f = (file *)malloc(sizeof(file));
                f->flag = heredoc(ast->center->value);
                f->path = NULL;
                ft_lstadd_back(&(input->files), ft_lstnew(f));
                return;
        }

        if (ast->type == NONE && ast->left->type == LT) {
                file *f = (file *)malloc(sizeof(file));
                f->flag = 0;
                f->path = ast->center->value;
                ft_lstadd_back(&(input->files), ft_lstnew(f));
                return;
        }

        if (ast->type == NONE && ast->left->type == DGT) {
                file *f = (file *)malloc(sizeof(file));
                f->flag = 2;
                f->path = ast->center->value;
                ft_lstadd_back(&(input->files), ft_lstnew(f));
                return;
        }

        if (ast->type == NONE && ast->left->type == GT) {
                file *f = (file *)malloc(sizeof(file));
                f->flag = 1;
                f->path = ast->center->value;
                ft_lstadd_back(&(input->files), ft_lstnew(f));
                return;
        }

        add_input(ast->left, input);
        add_input(ast->center, input);
}

static args *expand_input(node *ast)
{
        if (!ast)
                return NULL;

        args *a = (args *)malloc(sizeof(args));
        a->clargs = NULL;
        a->files = NULL;
        add_input(ast, a);
        return a;
}

void expander(node *ast, t_list **l)
{
        if (!ast)
                return;
        args *ar = expand_input(ast);
        ft_lstadd_back(l, ft_lstnew(ar));
        if (ast->right)
                expander(ast->right->center, l);
}