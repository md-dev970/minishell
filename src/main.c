#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../libft/include/libft.h"

char *built_ins[7] = {"echo", "cd", "pwd", "env", "export", "unset", "exit"};
char *separators[8] = {" ", ">", "<", "<<", ">>", "|", "\"", "'"};

char **lexer(char* input)
{
        t_list *lst = NULL;
        for(size_t i = 0; input[i]; ++i) {
                for (size_t j = 0; j < 8; ++j) {
                        // TODO : check for separators
                        continue;
                }
        }
        return ft_split(input, ' ');
}

void parser(char **lexems)
{
        printf("refuse\n");
}

int main()
{
        int quit = 0;
        char *inputBuffer;
        char **lexems;

        while(quit == 0) {

                inputBuffer = readline("minishell>");

                lexems = lexer(inputBuffer);

                if (strcmp(inputBuffer, "exit") == 0)
                        quit = 1;

                for (size_t i = 0; lexems[i]; ++i) {
                        printf("%s\n", lexems[i]);
                        free(lexems[i]);
                }

                free(lexems);
        }

        free(inputBuffer);
        return 0;
}