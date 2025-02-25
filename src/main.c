#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include "lexer.h"
#include "parser.h"


typedef struct file {
        char *path;
        int flag;
        mode_t mode;
} file;

typedef struct args {
        t_list *clargs;
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

void free_args(void *a)
{
        args *ar = (args *)a;
        ft_lstclear(ar->clargs, &free);
        ft_lstclear(ar->files, &free);
        free(ar);
}

void expand(node *ast, t_list **l);

void execute(node *ast, int in, int out, args *ar);

void execute_pipe(node *ast, int in, t_list *l);

args *expand_input(node *ast);

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
                node *ast = parser(lexems);
                print_tree(ast);
                printf("\n");
                t_list *l = NULL;
                expand(ast, &l);
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

void execute(node *ast, int in, int out, args *ar)
{
        if (!ast)
                return;


        printf("executing command %s \n", ast->left->value);
        // args *ar = expand_input(ast);
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

        if (id == 0) {
                printf("beginning execution\n");
                tmp = ar->files;
                int con = dup(STDOUT_FILENO);
                dup2(in, STDIN_FILENO);
                printf("reached this point\n");
                dup2(out, STDOUT_FILENO);
                write(con,"reached this point 2\n", 22);
                int fd;
                while (tmp) {
                        file *f = (file *)tmp->content;

                        if (f->path) {
                                write(con, "file to open: ", 14);
                                write(con, f->path, ft_strlen(f->path));
                                write(con, "\n", 2);
                                if (f->flag == 0) {
                                        fd = open(f->path, O_RDONLY);
                                        if (fd < 0) {
                                                write(con, "error opening file 0\n", 22);
                                                exit(1);
                                        }
                                        close(STDIN_FILENO);
                                        dup2(fd, STDIN_FILENO);
                                } else if (f->flag == 1) {
                                        fd = open(f->path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
                                        if (fd < 0) {
                                                write(con, "error opening file 1\n", 22);
                                                exit(2);
                                        }
                                        close(STDOUT_FILENO);
                                        dup2(fd, STDOUT_FILENO);
                                } else {
                                        fd = open(f->path, O_WRONLY | O_CREAT | O_APPEND, 0644);
                                        if (fd < 0) {
                                                write(con, "error opening file 2\n", 22);
                                                exit(3);
                                        }
                                        close(STDOUT_FILENO);
                                        dup2(fd, STDOUT_FILENO);
                                }
                        } else {
                                close(STDIN_FILENO);
                                dup2(f->flag, STDIN_FILENO);
                        }
                        tmp = tmp->next;
                }
                char *fullpath = ast->left->value;
                if (access(fullpath, F_OK) == 0)
                        goto straight;
                char **pathenv = ft_split(getenv("PATH"), ':');
                size_t j = 0;
                DIR *dir;
                struct dirent *d;
                while (pathenv && pathenv[j]) {
                        dir = opendir(pathenv[j]);
                        while ((d = readdir(dir)) != NULL) {
                                if (ft_strlen(d->d_name) == ft_strlen(ast->left->value)
                                && !ft_strncmp(d->d_name, ast->left->value, ft_strlen(ast->left->value))) {
                                        fullpath = (char *)malloc((ft_strlen(pathenv[j]) + 
                                        ft_strlen(ast->left->value) + 2) * sizeof(char));
                                        fullpath[0] = '\0';
                                        ft_strlcat(fullpath, pathenv[j], ft_strlen(pathenv[j]) + 
                                        ft_strlen(ast->left->value) + 2);
                                        ft_strlcat(fullpath, "/", ft_strlen(pathenv[j]) + 
                                        ft_strlen(ast->left->value) + 2);
                                        ft_strlcat(fullpath, ast->left->value, ft_strlen(pathenv[j]) + 
                                        ft_strlen(ast->left->value) + 2);
                                        closedir(dir);
                                        goto execute;
                                }
                                        
                        }
                        closedir(dir);
                        j++;
                }
                j = 0;
                while (pathenv && pathenv[j])
                        free(pathenv[j++]);
                free(pathenv);
                printf("minishell: command %s not found\n", ast->left->value);
                exit(5);
                execute:
                j = 0;
                while (pathenv && pathenv[j])
                        free(pathenv[j++]);
                free(pathenv);
                straight:
                if (execve(fullpath, input, __environ) < 0) {
                        printf("failed executing command\n");
                        exit(4);
                }

        } else {
                int status;
                waitpid(id, &status, 0);
                printf("command executed\nstatus: %i\n", status);
        }


        i = 0;
        while (input && input[i]) {
                printf("freeing input: %s\n", input[i]);
                free(input[i]);
                i++;
        }
        free(input);
}

void execute_pipe(node *ast, int in, t_list *l)
{
        if (ast->right != NULL) {
                printf("pipline\n");
                int p[2];
                pipe(p);
                pid_t id = fork();
                if (id > 0) {
                        close(p[1]);
                        execute_pipe(ast->right->center, p[0], l->next);
                        close(p[0]);
                        waitpid(id, NULL, 0);
                } else {
                        close(p[0]);
                        execute(ast, in, p[1], (args *)l->content);
                        close(p[1]);
                        exit(0);
                }
                return;
        }
        execute(ast, in, 1, (args *)l->content);
}

void expand(node *ast, t_list **l)
{
        if (!ast)
                return;
        args *ar = expand_input(ast);
        ft_lstadd_back(l, ft_lstnew(ar));
        if (ast->right)
                expand(ast->right->center, l);
}

int heredoc(char *delimiter)
{
        char *input = readline(">");
        int p[2];
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
        return p[0];
}

char *expander(char *str)
{
        if (!str)
                return NULL;
        char *s = str;
        printf("string to expand : %s[end]\n", s);
        char q = '\0';
        size_t len = ft_strlen(s);
        t_list *lst = NULL;
        char *tmp;
        /* Needs refactoring */
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
                        if (j > i)
                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                        i = j;
                } else if (q == '\"') {
                        while (j < len && s[j] != q) {
                                if (s[j] == '$') {
                                        if (j > i)
                                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                                        i = j;
                                        size_t k = j + 1;
                                        while (k < len && s[k] != '=' && s[k] != '\'' && s[k] != ' ' && s[k] != '\"')
                                                k++;
                                        tmp = ft_substr(s, j + 1, k - j - 1);
                                        if (k - j > 1)
                                                ft_lstadd_back(&lst, ft_lstnew(ft_strdup(getenv(tmp))));
                                        free(tmp);
                                        j = k - 1;
                                        i = j + 1;
                                }
                                j++;
                        }
                        if (j < len)
                                q = '\0';
                        if (j > i)
                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                        i = j;
                } else {
                        while (j < len && s[j] != '\'' && s[j] != '\"') {
                                if (s[j] == '$') {
                                        if (j > i)
                                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                                        i = j;
                                        size_t k = j + 1;
                                        while (k < len && s[k] != '=' && s[k] != '\'' && s[k] != ' ' && s[k] != '\"')
                                                k++;
                                        tmp = ft_substr(s, j + 1, k - j - 1);
                                        if (k - j > 1)
                                                ft_lstadd_back(&lst, ft_lstnew(ft_strdup(getenv(tmp))));
                                        free(tmp);
                                        j = k + 1;
                                        i = j;
                                }
                                j++;
                        }
                        if (j > i)
                                ft_lstadd_back(&lst, ft_lstnew(ft_substr(s, i, j - i)));
                        i = j - 1;
                }
        }
        ft_lstiter(lst, &print_lexem);
        printf("list size: %i\n", ft_lstsize(lst));
        t_list *tmp_lst = lst;
        size_t n = 0;
        while (tmp_lst) {
                n += ft_strlen((char *)tmp_lst->content);
                tmp_lst = tmp_lst->next;
        }
        char *ret = (char *)malloc((n + 1) * sizeof(char));
        if (ret == NULL) {
                ft_lstclear(lst, &free);
                return NULL;
        }
        ret[0] = '\0';
        tmp_lst = lst;
        while (tmp_lst) {
                ft_strlcat(ret, (char *)tmp_lst->content, n + 1);
                tmp_lst = tmp_lst->next;
        }
        printf("result: %s[end]\n", ret);
        ft_lstclear(lst, &free);
        return ret;
}

void add_input(node *ast, args *input)
{
        if (!ast) {
                printf("null\n");
                return;
        }
        if (ast->type == IDENT)
                ft_lstadd_back(&(input->clargs), ft_lstnew(expander(ast->value)));

        if (ast->type == NONE && ast->left->type == DLT) {
                printf("delimiter is: %s\n", ast->center->value);
                file *f = (file *)malloc(sizeof(file));
                f->flag = heredoc(ast->center->value);
                f->path = NULL;
                ft_lstadd_back(&(input->files), ft_lstnew(f));
                return;
        }

        if (ast->type == NONE && ast->left->type == LT) {
                file *f = (file *)malloc(sizeof(file));
                f->flag = 0;
                f->path = ast->center->value;
                ft_lstadd_back(&(input->files), ft_lstnew(f));
                return;
        }

        if (ast->type == NONE && ast->left->type == DGT) {
                file *f = (file *)malloc(sizeof(file));
                f->flag = 2;
                f->path = ast->center->value;
                ft_lstadd_back(&(input->files), ft_lstnew(f));
                return;
        }

        if (ast->type == NONE && ast->left->type == GT) {
                file *f = (file *)malloc(sizeof(file));
                f->flag = 1;
                f->path = ast->center->value;
                ft_lstadd_back(&(input->files), ft_lstnew(f));
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
        a->files = NULL;
        add_input(ast, a);
        return a;
}