#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../libft/include/libft.h"

char *b[7] = {"echo", "cd", "pwd", "env", "export", "unset", "exit"};
char *s[8] = {" ", ">", "<", "<<", ">>", "|", "\"", "'"};

t_list *lexer(char* input)
{
        t_list *lst = NULL;
        size_t len = ft_strlen(input);
        size_t j;
        char open_quote = '\0';
        for(size_t i = 0; i < len; ++i) {
                switch (input[i])
                {
                case ' ':
                        break;
                case '|':
                case '$':
                        ft_lstadd_back(&lst, ft_lstnew(ft_substr(input, i, 1)));
                        break;
                case '<':
                case '>':
                        if (i == len - 1 || input[i + 1] != input[i]) {
                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(input, i, 1)));
                                break;
                        }
                        ft_lstadd_back(&lst, ft_lstnew(ft_substr(input, i, 2)));
                        i++;
                        break;
                case '\'':
                case '\"':
                        if (!open_quote)
                                open_quote = input[i];
                        else if (open_quote == input[i])
                                open_quote = '\0';
                        ft_lstadd_back(&lst, ft_lstnew(ft_substr(input, i, 1)));
                        break;
                default:
                        j = i + 1;
                        while (j < len && is_separator(input, j, len) == -1)
                                j++;
                        ft_lstadd_back(&lst, ft_lstnew(ft_substr(input, i, j - i)));
                        i = j - 1;
                        break;
                }
        }
        return lst;
}

void print_lexems(void *lexems)
{
        char *s = (char *)lexems;
        printf("%s\n", s);
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
        t_list *lexems;

        while(quit == 0) {

                inputBuffer = readline("minishell>");

                lexems = lexer(inputBuffer);

                if (strcmp(inputBuffer, "exit") == 0)
                        quit = 1;
                free(inputBuffer);
                ft_lstiter(lexems, &print_lexems);
                ft_lstclear(lexems, &free);
        }
        rl_clear_history();

        return 0;
}