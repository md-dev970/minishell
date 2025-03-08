#include "lexer.h"

void print_lexem(void *lexem)
{
        char *s = (char *)lexem;
        printf("%s\n", s ? s : "");
}

static int is_separator(char *str, size_t i)
{

        char *s[8] = {" ", ">", "<", "<<", ">>", "|"};
        for (size_t j = 0; j < 6; ++j) {
                if (str[i] == *s[j])
                        return 1;     
        }
        return 0;
}


int lexer(t_list **lst, char* input)
{
        size_t len = ft_strlen(input);
        size_t j;
        char open_quote = '\0';
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
        #ifdef DEBUG
        ft_lstiter(tmp_lst, &print_lexem);
        #endif
        ft_lstclear(tmp_lst, &free);
        return (open_quote) ? -1 : 0;
}