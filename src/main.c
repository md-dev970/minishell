#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../libft/include/libft.h"

char *b[7] = {"echo", "cd", "pwd", "env", "export", "unset", "exit"};
char *s[8] = {" ", ">", "<", "<<", ">>", "|"};

void print_lexems(void *lexems)
{
        char *s = (char *)lexems;
        printf("%s\n", s);
}

int is_separator(char *str, size_t i)
{
        for (size_t j = 0; j < 6; ++j) {
                if (str[i] == *s[j])
                        return 1;     
        }
        return 0;
}

void expander(const char *s)
{
        char q = '\0';
        size_t len = ft_strlen(s);
        t_list *tmp = NULL;
        char *str;
        /* Lord forgive for what I'm about to write */
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
                        ft_lstadd_back(&tmp, ft_lstnew(ft_substr(s, i, j - i)));
                        i = j;
                } else if (q == '\"') {
                        while (j < len && s[j] != q) {
                                if (s[j] == '$') {
                                        ft_lstadd_back(&tmp, ft_lstnew(ft_substr(s, i, j - i)));
                                        i = j;
                                        size_t k = j + 1;
                                        while (k < len && s[k] != '=' && s[k] != '\'' && s[k] != ' ' && s[k] != '\"')
                                                k++;
                                        ft_lstadd_back(&tmp, ft_lstnew(ft_substr(s, j, k - j)));
                                        j = k - 1;
                                        i = j;
                                }
                                j++;
                        }
                        if (j < len)
                                q = '\0';
                        i = j;
                } else {
                        while (j < len && s[j] != '\'' && s[j] != '\"') {
                                if (s[j] == '$') {
                                        ft_lstadd_back(&tmp, ft_lstnew(ft_substr(s, i, j - i)));
                                        i = j;
                                        size_t k = j + 1;
                                        while (k < len && s[k] != '=' && s[k] != '\'' && s[k] != ' ' && s[k] != '\"')
                                                k++;
                                        ft_lstadd_back(&tmp, ft_lstnew(ft_substr(s, j, k - j)));
                                        j = k;
                                        i = j;
                                }
                                j++;
                        }
                        ft_lstadd_back(&tmp, ft_lstnew(ft_substr(s, i, j - i)));
                        i = j - 1;
                }
        }
        ft_lstiter(tmp, &print_lexems);
        ft_lstclear(tmp, &free);

}

int lexer(t_list **lst, char* input)
{
        size_t len = ft_strlen(input);
        size_t j;
        char open_quote = '\0';
        t_list *here_doc = NULL;
        for(size_t i = 0; i < len; ++i) {
                switch (input[i])
                {
                case ' ':
                        break;

                case '|':
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 1)));
                        break;

                case '<':
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
                        expander(ft_substr(input, i, j - i));
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, j - i)));
                        i = j - 1;
                        break;
                }
        }
        return (open_quote) ? -1 : 0;
}

void parser(char **lexems)
{
        /* TODO : parse the token list */
        return;
}

int main()
{
        int quit = 0;
        char *inputBuffer;
        t_list *lexems = NULL;

        while(quit == 0) {

                inputBuffer = readline("minishell>");

                int r = lexer(&lexems, inputBuffer);
                if (r)
                        printf("Error : unclosed quotes\n");

                if (strcmp(inputBuffer, "exit") == 0)
                        quit = 1;
                free(inputBuffer);
                ft_lstiter(lexems, &print_lexems);
                ft_lstclear(lexems, &free);
                lexems = NULL;
        }
        rl_clear_history();

        return 0;
}