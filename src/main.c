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

        inputBuffer = readline("minishell>");
        char **tmp = __environ;
        size_t e = 0;
        while (__environ && __environ[e]) {
                e++;
        }
        char **env = (char **)malloc((e + 1) * sizeof(char *));
        env[e] = NULL;
        for (size_t i = 0; i < e; ++i) {
                env[i] = ft_strdup(__environ[i]);
        }

        __environ = env;
        while(quit == 0) {
                add_history(inputBuffer);
                if (strcmp(inputBuffer, "exit") == 0) {
                        break;
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
                handle_commands(ast, l);
                ft_lstclear(l, &free_args);
                free_tree(ast);
                clean:
                ft_lstclear(lexems, &free_token);
                lexems = NULL;
                free(inputBuffer);
                inputBuffer = readline("minishell>");
        }
        rl_clear_history();
        free(inputBuffer);
        e = 0;
        while (__environ && __environ[e]) {
                e++;
        }
        for (size_t i = 0; i < e; ++i) {
                free(__environ[i]);
        }
        free(__environ);
        __environ = tmp;

        return 0;
}