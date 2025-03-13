#define _POSIX_C_SOURCE 200809L


#include <stdio.h>
#include <stdlib.h>
#include <readline/history.h>
#include <signal.h>

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "expander.h"
#include "exec.h"

void prompt_int_handler(int signum)
{
        if (signum != SIGINT)
                return;
        printf("\n");
        rl_replace_line("", 1);
        rl_on_new_line();
        rl_redisplay();
}

int main()
{
        struct sigaction prompt_sa, old_sa, ign_sa;
        prompt_sa.sa_flags = 0;
        old_sa.sa_flags = 0;
        ign_sa.sa_flags = 0;
        sigset_t s;
        sigemptyset(&s);
        old_sa.sa_mask = s;
        prompt_sa.sa_mask = s;
        ign_sa.sa_mask = s;
        prompt_sa.sa_handler = &prompt_int_handler;
        old_sa.sa_handler = SIG_DFL;
        ign_sa.sa_handler = SIG_IGN;
        // sa.sa_mask = ss;
        sigaction(SIGINT, &prompt_sa, &old_sa);
        sigaction(SIGQUIT,  &prompt_sa, &old_sa);
        int quit = 0;
        char *inputBuffer;
        t_list *lexems = NULL;

        inputBuffer = readline("minishell>");
        sigaction(SIGINT, &ign_sa, &prompt_sa);
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
        int last_exit_status = 0;
        while(quit == 0) {
                if (!inputBuffer ||  (ft_strlen(inputBuffer) == 4 && !ft_strncmp(inputBuffer, "exit", 4))) {
                        break;
                }
                add_history(inputBuffer);    
                int r = lexer(&lexems, inputBuffer);
                if (r) {
                        printf("Error : unclosed quotes\n");
                        goto clean;
                }
                struct node *ast = parser(lexems);
                #ifdef DEBUG
                print_tree(ast);
                printf("\n");
                #endif
                t_list *l = NULL;
                expander(ast, &l, last_exit_status);
                handle_commands(ast, l, &last_exit_status);
                ft_lstclear(l, &free_args);
                free_tree(ast);
                clean:
                ft_lstclear(lexems, &free_token);
                lexems = NULL;
                free(inputBuffer);
                sigaction(SIGINT, &prompt_sa, &ign_sa);
                inputBuffer = readline("minishell>");
                sigaction(SIGINT, &ign_sa, &prompt_sa);
        }
        rl_clear_history();
        free(inputBuffer);
        e = ft_arrsize((void **)__environ);
        for (size_t i = 0; i < e; ++i) {
                free(__environ[i]);
        }
        free(__environ);

        return 0;
}