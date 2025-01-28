#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../libft/include/libft.h"

char *b[7] = {"echo", "cd", "pwd", "env", "export", "unset", "exit"};
char *s[8] = {" ", ">", "<", "<<", ">>", "|", "\"", "'"};

int is_separator(char *str, size_t i)
{
        for (size_t j = 0; j < 8; ++j) {
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
                case '$':
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
                case '\'':
                case '\"':
                        j = i + 1;
                        while (j < len && input[j] != input[i])
                                j++;
                        if (j == len)
                                return -1;
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i + 1, j - i - 1)));
                        i = j;
                        break;
                default:
                        j = i + 1;
                        while (j < len && !is_separator(input, j))
                                j++;
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, j - i)));
                        i = j - 1;
                        break;
                }
        }
        return 0;
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