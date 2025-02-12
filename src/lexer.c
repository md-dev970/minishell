#include "lexer.h"

void print_lexem(void *lexem)
{
        char *s = (char *)lexem;
        printf("%s\n", s ? s : "");
}

static int is_separator(char *str, size_t i)
{
        char *b[7] = {"echo", "cd", "pwd", "env", "export", "unset", "exit"};
        char *s[8] = {" ", ">", "<", "<<", ">>", "|"};
        for (size_t j = 0; j < 6; ++j) {
                if (str[i] == *s[j])
                        return 1;     
        }
        return 0;
}

static void expander(char **str)
{
        if (!str || !*str)
                return;
        char *s = *str;
        printf("string to expand : %s[end]\n", s);
        char q = '\0';
        size_t len = ft_strlen(s);
        t_list *lst = NULL;
        char *tmp;
        /* Lord forgive me for what I'm about to write */
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
                return;
        }
        ret[0] = '\0';
        tmp_lst = lst;
        while (tmp_lst) {
                ft_strlcat(ret, (char *)tmp_lst->content, n + 1);
                tmp_lst = tmp_lst->next;
        }
        printf("result: %s[end]\n", ret);
        free(s);
        *str = ret;
        ft_lstclear(lst, &free);
}

int lexer(t_list **lst, char* input)
{
        size_t len = ft_strlen(input);
        size_t j;
        char open_quote = '\0';
        t_list *here_doc = NULL;
        char *tmp;
        for(size_t i = 0; i < len; ++i) {
                switch (input[i])
                {
                case ' ':
                        break;

                case '|':
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 1)));
                        break;

                case '<':
                        if (i == len - 1 || input[i + 1] != input[i]) {
                                ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 1)));
                                break;
                        }
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 2)));
                        i++;
                        break;
                case '>':
                        if (i == len - 1 || input[i + 1] != input[i]) {
                                ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 1)));
                                break;
                        }
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 2)));
                        i++;
                        break;

                default:
                        j = i;
                        while (j < len && (!is_separator(input, j) || (open_quote))) {
                                if (input[j] == '\"' || input[j] == '\'') {
                                        if (!open_quote)
                                                open_quote = input[j];
                                        else if (open_quote == input[j])
                                                open_quote = '\0';
                                }
                                j++;
                        }
                        tmp = ft_substr(input, i, j - i);
                        expander(&tmp);
                        if (ft_strlen(tmp))
                                ft_lstadd_back(lst, ft_lstnew(tmp));
                        else
                                free(tmp);
                        i = j - 1;
                        break;
                }
        }
        t_list *tmp_lst = *lst;
        *lst = ft_lstmap(*lst, &generate_token, &free_token);
        ft_lstclear(tmp_lst, &free);
        return (open_quote) ? -1 : 0;
}