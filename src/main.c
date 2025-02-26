#include <stdio.h>
#include <stdlib.h>
#include <readline/history.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "expander.h"
#include "exec.h"




int main()
{
        int quit = 0;
        char *inputBuffer;
        t_list *lexems = NULL;
        while(quit == 0) {
                inputBuffer = readline("minishell>");
                if (*inputBuffer == '\0')
                        goto clean;
                add_history(inputBuffer);
                if (strcmp(inputBuffer, "exit") == 0) {
                        quit = 1;
                        goto clean;
                }
                        
                int r = lexer(&lexems, inputBuffer);
                if (r) {
                        printf("Error : unclosed quotes\n");
                        goto clean;
                }
                struct node *ast = parser(lexems);
                print_tree(ast);
                printf("\n");
                t_list *l = NULL;
                expander(ast, &l);
                execute(ast, 0, l);
                ft_lstclear(l, &free_args);
                free_tree(ast);
                goto clean;
                clean:
                free(inputBuffer);
                ft_lstiter(lexems, &print_token);
                ft_lstclear(lexems, &free_token);
                lexems = NULL;
        }
        rl_clear_history();

        return 0;
}