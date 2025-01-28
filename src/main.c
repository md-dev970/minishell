#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../libft/include/libft.h"

char *b[7] = {"echo", "cd", "pwd", "env", "export", "unset", "exit"};
char *s[8] = {" ", ">", "<", "<<", ">>", "|"};

int is_separator(char *str, size_t i)
{
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
                        j = i + 1;
                        while (j < len && (!is_separator(input, j) || (open_quote))) {
                                if (input[j] == '\"' || input[j] == '\'') {
                                        if (!open_quote)
                                                open_quote = input[j];
                                        else if (open_quote == input[j])
                                                open_quote = '\0';
                                }
                                j++;
                        }
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, j - i)));
                        i = j - 1;
                        break;
                }
        }
        return (open_quote) ? -1 : 0;
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