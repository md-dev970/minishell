#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "lexer.h"
#include "parser.h"


typedef struct file {
        int fd;
        int flag;
        mode_t mode;
} file;

typedef struct args {
        t_list *clargs;
        t_list *hd;
        t_list *files;
} args;

void print_tree(node *root)
{
        if (!root)
                return;
        t_list *queue = NULL;
        ft_lstadd_back(&queue, ft_lstnew(root));
        t_list *cur;
        node *n;
        while(queue) {
                cur = ft_lstpop_front(&queue);
                n = (node *)cur->content;
                if (!n) {
                        free(cur);
                        continue;
                }
                switch (n->type)
                {
                case PIPE:
                        printf(" | ");
                        break;
                case DLT:
                        printf(" << ");
                        break;
                case DGT:
                        printf(" >> ");
                        break;
                case LT:
                        printf(" < ");
                        break;
                case GT:
                        printf(" > ");
                        break;
                case NONE:
                        printf("  ");
                        break;
                default:
                        printf(" %s ", n->value);
                }
                ft_lstadd_back(&queue, ft_lstnew(n->left));
                ft_lstadd_back(&queue, ft_lstnew(n->center));
                ft_lstadd_back(&queue, ft_lstnew(n->right));
                free(cur);
        }
}

void free_tree(node *root)
{
        if (!root)
                return;
        free_tree(root->left);
        free_tree(root->center);
        free_tree(root->right);
        free(root);
}

void execute(node *ast);

void execute_pipe(node *ast, char *output);

args *expand_input(node *ast);

int main()
{
        int quit = 0;
        char *inputBuffer;
        t_list *lexems = NULL;
        while(quit == 0) {
                inputBuffer = readline("minishell>");
                int r = lexer(&lexems, inputBuffer);
                if (r) {
                        printf("Error : unclosed quotes\n");
                        goto clean;
                }
                node *ast = parser(lexems);
                print_tree(ast);
                printf("\n");
                execute(ast);
                free_tree(ast);
                goto clean;
                clean:
                if (strcmp(inputBuffer, "exit") == 0)
                        quit = 1;
                free(inputBuffer);
                ft_lstiter(lexems, &print_token);
                ft_lstclear(lexems, &free_token);
                lexems = NULL;
        }
        rl_clear_history();

        return 0;
}

void execute(node *ast)
{
        if (!ast)
                return;

        printf("executing command %s \n", ast->left->value);
        args *ar = expand_input(ast);
        char **input = (char **)malloc((ft_lstsize(ar->clargs) + 1) * sizeof(char *));
        t_list *tmp = ar->clargs;
        size_t i = 0;
        printf("argument list size: %i\n", ft_lstsize(ar->clargs));
        while (tmp) {
                printf("arg: %s\n", (char *)tmp->content);
                input[i++] = ft_strdup((char *)tmp->content);
                tmp = tmp->next;
        }

        if (!input) {
                input = (char **)malloc(sizeof(char *));
        }
        input[i] = NULL;


        char *path = (char *)malloc(20 * sizeof(char));
        path[0] = '\0';
        ft_strlcat(path, "/bin/", 20);
        ft_strlcat(path, ast->left->value, 20);
        free(input[0]);
        input[0] = path;
        printf("---------------------------------\nCommand output\n");

        pid_t id = fork();
        if (id < 0) {
                printf("fork failed\n");
                return;
        }

        if (id == 0) {
                printf("beginning execution\n");
                tmp = ar->hd;
                while (tmp) {
                        printf("changing input fd\n");
                        close(STDIN_FILENO);
                        dup2(*(int *)tmp->content, STDIN_FILENO);
                        close(*(int *)tmp->content);
                        tmp = tmp->next;
                }


                if (execve(path, input, __environ) < 0) {
                        printf("failed executing command\n");
                        exit(1);
                }
        } else {
                int status;
                waitpid(id, &status, 0);
                printf("command executed\nstatus: %i\n", status);
        }

        ft_lstclear(ar->clargs, &free);
        ft_lstclear(ar->hd, &free);
        ft_lstclear(ar->files, &free);
        free(ar);


        i = 0;
        while (input && input[i]) {
                printf("freeing input: %s\n", input[i]);
                free(input[i]);
                i++;
        }
        free(input);
        if (ast->right != NULL) {
                printf("pipline\n");
                execute_pipe(ast->right->center, ast->left->value);
        }
}

void execute_pipe(node *ast, char *output)
{
        return;
}

int *heredoc(char *delimiter)
{
        char *input = readline(">");
        int *p = (int *)malloc(2 * sizeof(int));
        pipe(p);
        while (ft_strlen(input) != ft_strlen(delimiter) ||
                ft_strncmp(input, delimiter, ft_strlen(input)) != 0) {
                write(p[1], input, ft_strlen(input));
                write(p[1], "\n", 1);
                free(input);
                input = readline(">");
        }
        free(input);
        close(p[1]);
        return p;
}

void add_input(node *ast, args *input)
{
        if (!ast) {
                printf("null\n");
                return;
        }
        if (ast->type == IDENT)
                ft_lstadd_back(&(input->clargs), ft_lstnew(ft_strdup(ast->value)));

        if (ast->type == NONE && ast->left->type == DLT) {
                printf("delimiter is: %s\n", ast->center->value);
                ft_lstadd_back(&(input->hd), ft_lstnew(heredoc(ast->center->value)));
                return;
        }

        if (ast->type == NONE && ast->left->type == LT) {
                ft_lstadd_back(&(input->files), ft_lstnew(ast->center->value));
                return;
        }

        if (ast->type == NONE && ast->left->type == DGT) {
                ft_lstadd_back(&(input->files), ft_lstnew(ast->center->value));
                return;
        }

        if (ast->type == NONE && ast->left->type == GT) {
                ft_lstadd_back(&(input->files), ft_lstnew(ast->center->value));
                return;
        }

        add_input(ast->left, input);
        add_input(ast->center, input);
}

args *expand_input(node *ast)
{
        if (!ast)
                return NULL;

        args *a = (args *)malloc(sizeof(args));
        a->clargs = NULL;
        a->hd = NULL;
        a->fname = NULL;
        add_input(ast, a);
        return a;
}