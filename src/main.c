#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include "lexer.h"
#include "parser.h"
#include "expander.h"


void print_tree(struct node *root)
{
        if (!root)
                return;
        t_list *queue = NULL;
        ft_lstadd_back(&queue, ft_lstnew(root));
        t_list *cur;
        struct node *n;
        while(queue) {
                cur = ft_lstpop_front(&queue);
                n = (struct node *)cur->content;
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

void free_tree(struct node *root)
{
        if (!root)
                return;
        free_tree(root->left);
        free_tree(root->center);
        free_tree(root->right);
        free(root);
}

void free_args(void *a)
{
        struct args *ar = (struct args *)a;
        ft_lstclear(ar->clargs, &free);
        ft_lstclear(ar->fileHandlers, &free);
        free(ar);
}

void execute(struct node *ast, int in, int out, struct args *ar);

void execute_pipe(struct node *ast, int in, t_list *l);

int main()
{
        int quit = 0;
        char *inputBuffer;
        t_list *lexems = NULL;
        while(quit == 0) {
                inputBuffer = readline("minishell>");
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
                execute_pipe(ast, 0, l);
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

int search_dir(DIR *dir, char *target, size_t n)
{
        struct dirent *d;
        while ((d = readdir(dir)) != NULL) {
                if (ft_strlen(d->d_name) != n)
                        continue;
                
                if (ft_strncmp(d->d_name, target, n))
                        continue;
                closedir(dir);
                return 1;
        }
        closedir(dir);
        return 0;
}

char *search_executable(char *exec)
{
        if (access(exec, F_OK) == 0)
                return ft_strdup(exec);

        char **pathenv = ft_split(getenv("PATH"), ':');
        if (!pathenv)
                return NULL;
        size_t j = 0;
        DIR *dir;
        size_t n = ft_strlen(exec);
        char *fullpath = NULL;
        for (size_t j = 0; pathenv[j]; ++j) {
                dir = opendir(pathenv[j]);
                if (!search_dir(dir, exec, n))
                        continue;
                        
                size_t len = ft_strlen(pathenv[j]) + n + 2;
                fullpath = (char *)malloc(len * sizeof(char));
                fullpath[0] = '\0';
                ft_strlcat(fullpath, pathenv[j], len);
                ft_strlcat(fullpath, "/", len);
                ft_strlcat(fullpath, exec, len);
                break;
        }
        ft_foreach((void **)pathenv, &free);
        free(pathenv);
        return fullpath;
}

int o_flag(int f)
{
        return O_WRONLY | O_CREAT | ((f < 2) ? O_TRUNC : O_APPEND);
}

void execute(struct node *ast, int in, int out, struct args *ar)
{
        if (!ast)
                return;


        printf("executing command %s \n", ast->left->value);
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

        printf("---------------------------------\nCommand output\n");

        pid_t id = fork();
        if (id < 0) {
                printf("fork failed\n");
                return;
        }
        if (id) {
                int status;
                waitpid(id, &status, 0);
                printf("command executed\nstatus: %i\n", status);
                ft_foreach((void **)input, &free);
                free(input);
                return;
        }
        tmp = ar->fileHandlers;
        int con = dup(STDOUT_FILENO);
        dup2(in, STDIN_FILENO);
        dup2(out, STDOUT_FILENO);
        int fd;
        while (tmp) {
                struct fileHandler *f = (struct fileHandler *)tmp->content;
                if (!f->path) {
                        close(STDIN_FILENO);
                        dup2(f->flag, STDIN_FILENO);
                        tmp = tmp->next;
                        continue;
                }
                write(con, "file to open: ", 14);
                write(con, f->path, ft_strlen(f->path));
                write(con, "\n", 2);
                if (!f->flag) {
                        if ((fd = open(f->path, O_RDONLY)) < 0) {
                                write(con, "error opening file 0\n", 22);
                                exit(1);
                        }
                        close(STDIN_FILENO);
                        dup2(fd, STDIN_FILENO);
                } else {
                        if ((fd = open(f->path, o_flag(f->flag), 0664)) < 0) {
                                write(con, "error opening file 1\n", 22);
                                exit(2);
                        }
                        close(STDOUT_FILENO);
                        dup2(fd, STDOUT_FILENO);
                }
                tmp = tmp->next;
        }
        printf("beginning execution\n");
        char *pathname = search_executable(ast->left->value);
        printf("pathname: %s\n", pathname);
        if (!pathname) {
                write(con, "minishell:", 11);
                ft_putstr_fd(ast->left->value, con);
                write(con, ": command not found", 20);
                exit(3);
        }
        if (execve(pathname, input, __environ) < 0) {
                printf("failed executing command\n");
                exit(4);
        }

}

void execute_pipe(struct node *ast, int in, t_list *l)
{
        if (!ast->right) {
                execute(ast, in, 1, (struct args *)l->content);
                return;
        }
                

        printf("pipline\n");
        int p[2];
        pipe(p);
        pid_t id = fork();
        if (id < 0) {
                printf("fork failed\n");
                exit(2);
        } else if (id) {
                close(p[1]);
                execute_pipe(ast->right->center, p[0], l->next);
                close(p[0]);
                waitpid(id, NULL, 0);
        } else {
                close(p[0]);
                execute(ast, in, p[1], (struct args *)l->content);
                close(p[1]);
                exit(0);
        }
}
