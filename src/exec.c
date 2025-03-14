#define _POSIX_C_SOURCE 200809L
#include "exec.h"
#include <errno.h>
#include <signal.h>

struct command_attrs {
        int cin;
        int cout;
        char **input;
};


static int search_dir(DIR *dir, char *target, size_t n)
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


static char *search_executable(char *exec)
{
        char **tmp = ft_split(exec, '/');
        size_t e = ft_arrsize((void **)tmp);
        ft_foreach((void **)tmp, &free);
        free(tmp);
        if (e > 1 && access(exec, F_OK) == 0)
                return ft_strdup(exec);

        char **pathenv = ft_split(getenv("PATH"), ':');
        if (!pathenv)
                return NULL;

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


static int o_flag(int f)
{
        return O_WRONLY | O_CREAT | ((f < 2) ? O_TRUNC : O_APPEND);
}


int builtin_pwd()
{
        char *cwd = getcwd(NULL, 0);
        ft_putstr_fd(cwd, STDOUT_FILENO);
        ft_putchar_fd('\n', STDOUT_FILENO);
        free(cwd);
        return 0;
}


int builtin_cd(char *input[])
{
        int s = 0;
        if (!input || !input[0] || *input[0] == '~') {
                s = chdir(getenv("HOME"));
        } else if (input[1]) {
                ft_putstr_fd("minishell: cd: too many arguments\n", STDOUT_FILENO);
                s = 1;
        } else {
                if ((s = chdir(input[0]))) {
                        ft_putstr_fd("cd: command failed\n", STDOUT_FILENO);
                        perror("error: ");
                }
        }
        return s;
}


int builtin_env()
{
        size_t i = 0;
        while (__environ[i])
                printf("%s\n", __environ[i++]);
        return 0;
}


int builtin_export(char *input[])
{
        if (!input)
                return 0;
        int s = 0;
        size_t i = 0;
        size_t v = 0;
        t_list *l = NULL;
        char **tmp;
        int valid;
        while (input[i])
        {

                if (!ft_isalpha(*input[i]) && *input[i] != '_') {
                        printf("minishell: export: '%s' is not a valid identifier\n", input[i]);
                        s = 1;
                        i++;
                        continue;
                }
                valid = 1;
                size_t j = 0;
                while (input[i][j] && input[i][j] != '=') {
                        if (!ft_isalnum(input[i][j])) {
                                valid = 0;
                                break;
                        }
                        j++;
                }
                if (!valid) {
                        printf("minishell: export: '%s' is not a valid identifier\n", input[i]);
                        s = 1;
                        i++;
                        continue;
                }
                tmp = ft_split(input[i], '=');
                if (ft_arrsize((void **)tmp) < 2) {
                        size_t j = 0;
                        while (tmp && tmp[j])
                                free(tmp[j++]);
                        free(tmp);
                        i++;
                        continue;
                }
                size_t e = 0;
                int exist = 0;
                while (__environ && __environ[e]) {
                        char **var = ft_split(__environ[e], '=');
                        if (ft_strlen(var[0]) == ft_strlen(tmp[0]) && !ft_strncmp(var[0], tmp[0], ft_strlen(tmp[0]))) {
                                for (size_t i = 0; i < ft_arrsize((void **)var); ++i)
                                        free(var[i]);
                                free(var);
                                exist = 1;
                                break;
                        }
                        for (size_t i = 0; i < ft_arrsize((void **)var); ++i)
                                free(var[i]);
                        free(var);
                        e++;
                }
                if (exist) {
                        free(__environ[e]);
                        __environ[e] = ft_strdup(input[i]);
                        size_t j = 0;
                        while (tmp && tmp[j])
                                free(tmp[j++]);
                        free(tmp);
                        break;
                }
                        
                j = 0;
                while (tmp && tmp[j])
                        free(tmp[j++]);
                free(tmp);   

                ft_lstadd_back(&l, ft_lstnew(ft_strdup(input[i])));
                v++;
                i++;
        }
        size_t e = 0;
        while (__environ && __environ[e]) {
                e++;
        }
        t_list *prev;
        char **new_environ = (char **)malloc((e + v + 1) * sizeof(char *));
        if (!new_environ) {
                #ifdef DEBUG
                printf("Error: coudn't allocate memory\n");
                #endif
                return 2;
        }
        for (i = 0; i < e; ++i) {
                new_environ[i] = ft_strdup(__environ[i]);
                free(__environ[i]);
        }
        free(__environ);

        for (i = e; i < e + v; ++i) {
                new_environ[i] = (char *)l->content;
                prev = l;
                l = l->next;
                free(prev);
        }
        new_environ[e + v] = NULL;
        __environ = new_environ;
        return s;
}


int builtin_unset(char *input[])
{
        if (!input)
                return 0;
        size_t input_size = ft_arrsize((void **)input);
        size_t env_size = ft_arrsize((void **)__environ);
        t_list *l = NULL;
        t_list *tmp = l;
        int exist = 0;
        char **var;
        for (size_t i = 0; i < input_size; ++i) {
                for (size_t j = 0; j < env_size; ++j) {
                        var = ft_split(__environ[j], '=');
                        if (ft_strlen(var[0]) == ft_strlen(input[i]) && !ft_strncmp(var[0], input[i], ft_strlen(input[i]))) {
                                exist = 0;
                                tmp = l;
                                
                                while (tmp) {
                                        if (ft_strlen(var[0]) == ft_strlen((char *)tmp->content) 
                                        && !ft_strncmp(var[0], (char *)tmp->content, ft_strlen(var[0]))) {
                                                
                                                exist = 1;
                                                break;
                                        }
                                        tmp = tmp->next;
                                }
                                if (!exist) {
                                        ft_lstadd_back(&l, ft_lstnew(ft_strdup(input[i])));
                                }
                                ft_foreach((void **)var, &free);
                                free(var);
                                        
                                break;
                        }
                        ft_foreach((void **)var, &free);
                        free(var);
                }
        }
        int lst_size = ft_lstsize(l);
        #ifdef DEBUG
        printf("vars to unset: %i\n", lst_size);
        #endif
        if (lst_size == 0) {
                ft_lstclear(l, &free);
                return 0;
        }

        char **new_environ = (char **)malloc((env_size - lst_size + 1) * sizeof(char *));
        size_t nv = 0;

        for (size_t i = 0; i < env_size; ++i) {
                exist = 0;
                tmp = l;
                var = ft_split(__environ[i], '=');
                while (tmp) {
                        if (ft_strlen(var[0]) == ft_strlen((char *)tmp->content) 
                        && !ft_strncmp(var[0], (char *)tmp->content, ft_strlen(var[0]))) {
                                exist = 1;
                                break;
                        }
                        tmp = tmp->next;
                }
                ft_foreach((void **)var, &free);
                free(var);
                if (exist)
                        continue;
                new_environ[nv++] = ft_strdup(__environ[i]);

        }
        ft_foreach((void **)__environ, &free);
        free(__environ);
        ft_lstclear(l, &free);
        new_environ[nv] = NULL;
        __environ = new_environ;
        return 0;
}


static size_t check_opt(char *opt)
{
        size_t n = ft_strlen(opt);
        if (n < 2 || opt[0] != '-')
                return 0;
        for (size_t i = 1; i < n; ++i) {
                if (opt[i] == 'n')
                        continue;
                return 0;
        }
        return 1;
}


int builtin_echo(char *input[])
{
        if (!input || !input[0]) {
                printf("\n");
                return 0;
        }
        size_t i;
        char newline = ((i = check_opt(input[0])) == 0) ? '\n' : '\0';
        while (input[i])
                printf("%s ", input[i++]);
        printf("%c", newline);
        return 0;
}


void builtin_exit(char *input[])
{
        if (!input)
                exit(0);
        ft_putstr_fd("executing built in exit\n", STDOUT_FILENO);
        exit(0);
}


void non_builtin(char *input[])
{
        #ifdef DEBUG
        printf("executing non built in\n");
        #endif
        char *pathname = search_executable(input[0]);
        if (!pathname) {
                ft_putstr_fd("minishell: ", STDOUT_FILENO);
                ft_putstr_fd(input[0], STDOUT_FILENO);
                ft_putstr_fd(": command not found\n", STDOUT_FILENO);
                ft_foreach((void **)__environ, &free);
                free(__environ);
                exit(1);
        }
        execve(pathname, input, __environ);
        ft_foreach((void **)__environ, &free);
        free(__environ);
        exit(1);
}


int execute_command(struct command_attrs *ca)
{
        if (!ca)
                return 1;
        int s = 0;
        char **input = ca->input;
        int cin  = ca->cin;
        int cout = ca->cout;
        if (!input || !input[0]) {
                ft_foreach((void **)input, &free);
                free(ca);
                exit(1);
        }
                
        
        char *cmd = input[0];
        #ifdef DEBUG
        printf("checking command %s\n", cmd);
        #endif
        switch (ft_strlen(cmd)) {
        case 2:
                if (!ft_strncmp(cmd, "cd", 2)) {
                        s = builtin_cd(input + 1);
                        goto cleanup;
                }
                        
                break;
        case 3:
                if (!ft_strncmp(cmd, "pwd", 3)) {
                        s = builtin_pwd();
                        goto cleanup;
                } else if (!ft_strncmp(cmd, "env", 3)) {
                        s = builtin_env();
                        goto cleanup;
                }
                break;
        case 4:
                if (!ft_strncmp(cmd, "echo", 4)) {
                        s = builtin_echo(input + 1);
                        goto cleanup;
                } else if (!ft_strncmp(cmd, "exit", 4)) {
                        builtin_exit(input + 1);
                        goto cleanup;
                }
                break;
        case 5:
                if (!ft_strncmp(cmd, "unset", 5)) {
                        s = builtin_unset(input + 1);
                        goto cleanup;
                }
                break;
        case 6:
                if (!ft_strncmp(cmd, "export", 6)) {
                        s = builtin_export(input + 1);
                        goto cleanup;
                }
                        
                break;
        default:
                break;
        }
        non_builtin(input);
        cleanup:
        ft_foreach((void **)input, &free);
        free(input);
        free(ca);
        dup2(cin, STDIN_FILENO);
        dup2(cout, STDOUT_FILENO);
        return s;
}


static struct command_attrs *prepare_command(int in, int out, struct args *ar)
{
        if (!ar || !ar->clargs || !ar->clargs->content)
                return NULL;

        #ifdef DEBUG
        printf("executing command %s \n", (char *)ar->clargs->content);
        #endif
        char **input = (char **)malloc((ft_lstsize(ar->clargs) + 1) * sizeof(char *));
        t_list *tmp = ar->clargs;
        size_t i = 0;
        #ifdef DEBUG
        printf("argument list size: %i\n", ft_lstsize(ar->clargs));
        #endif
        while (tmp) {
                #ifdef DEBUG
                printf("arg: %s\n", (char *)tmp->content);
                #endif
                input[i++] = ft_strdup((char *)tmp->content);
                tmp = tmp->next;
        }

        if (!input) {
                input = (char **)malloc(sizeof(char *));
        }
        input[i] = NULL;

        #ifdef DEBUG
        printf("---------------------------------\nCommand output\n");
        #endif

        tmp = ar->fileHandlers;
        int cout = dup(STDOUT_FILENO);
        int cin = dup(STDIN_FILENO);
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
                #ifdef DEBUG
                write(cout, "file to open: ", 14);
                write(cout, f->path, ft_strlen(f->path));
                write(cout, "\n", 2);
                #endif
                if (!f->flag) {
                        if ((fd = open(f->path, O_RDONLY)) < 0) {
                                #ifdef DEBUG
                                write(cout, "error opening file 0\n", 22);
                                #endif
                                exit(1);
                        }
                        close(STDIN_FILENO);
                        dup2(fd, STDIN_FILENO);
                } else {
                        if ((fd = open(f->path, o_flag(f->flag), 0664)) < 0) {
                                #ifdef DEBUG
                                write(cout, "error opening file 1\n", 22);
                                #endif
                                exit(2);
                        }
                        close(STDOUT_FILENO);
                        dup2(fd, STDOUT_FILENO);
                }
                tmp = tmp->next;
        }
        #ifdef DEBUG
        printf("beginning execution\n");
        #endif
        struct command_attrs *ret = (struct command_attrs *)malloc(sizeof(struct command_attrs));
        ret->cin = cin;
        ret->cout = cout;
        ret->input = input;
        return ret;

}


void handle_pipeline(struct node *ast, int in, t_list *l, int *last_exit_status)
{
        if (!ast)
                return;
        if (!ast->right) {
                struct sigaction old_sa;
                sigset_t s;
                sigemptyset(&s);
                old_sa.sa_mask = s;
                old_sa.sa_flags = 0;
                old_sa.sa_handler = SIG_DFL;
                pid_t id = fork();
                if (id < 0) {
                        #ifdef DEBUG
                        printf("fork failed\n");
                        #endif
                        exit(2);
                } else if (id) {
                        wait(last_exit_status);
                        if (WIFEXITED(*last_exit_status))
                                *last_exit_status = WEXITSTATUS(*last_exit_status);
                        else if (WIFSIGNALED(*last_exit_status))
                                *last_exit_status = WTERMSIG(*last_exit_status);
                        else if (WIFSTOPPED(*last_exit_status))
                                *last_exit_status = WSTOPSIG(*last_exit_status);
                        return;
                } else {
                        sigaction(SIGINT, &old_sa, NULL);
                        int s = execute_command(prepare_command(in, 1, (struct args *)l->content));
                        ft_foreach((void **)__environ, &free);
                        free(__environ);
                        exit(s);
                }
        }
                
        #ifdef DEBUG
        printf("pipline\n");
        #endif
        int p[2];
        if (pipe(p)) {
                #ifdef DEBUG
                printf("Pipe opening failed\n");
                #endif
                return;
        }
        pid_t id = fork();
        if (id < 0) {
                #ifdef DEBUG
                printf("fork failed\n");
                #endif
                exit(2);
        } else if (id) {
                close(p[1]);
                handle_pipeline(ast->right->center, p[0], l->next, last_exit_status);
                close(p[0]);
                wait(last_exit_status);
                if (WIFEXITED(*last_exit_status))
                        *last_exit_status = WEXITSTATUS(*last_exit_status);
                else if (WIFSIGNALED(*last_exit_status))
                        *last_exit_status = WTERMSIG(*last_exit_status);
                else if (WIFSTOPPED(*last_exit_status))
                        *last_exit_status = WSTOPSIG(*last_exit_status);
        } else {
                close(p[0]);
                int s = execute_command(prepare_command(in, p[1], (struct args *)l->content));
                close(p[1]);
                ft_foreach((void **)__environ, &free);
                free(__environ);
                exit(s);
        }
}


void handle_commands(struct node *ast, t_list *l, int *last_exit_status)
{
        if (!ast)
                return;
        if (ast->right) {
                handle_pipeline(ast, 0, l, last_exit_status);
                return;
        }
        char *cmd = ast->left->value;
        switch (ft_strlen(cmd)) {
        case 2:
                if (!ft_strncmp(cmd, "cd", 2)) {
                        *last_exit_status = execute_command(prepare_command(0, 1, (struct args*)l->content));
                        return;
                }
                        
                break;
        case 3:
                if (!ft_strncmp(cmd, "pwd", 3)) {
                        *last_exit_status = execute_command(prepare_command(0, 1, (struct args*)l->content));
                        return;
                } else if (!ft_strncmp(cmd, "env", 3)) {
                        *last_exit_status = execute_command(prepare_command(0, 1, (struct args*)l->content));
                        return;
                }
                break;
        case 4:
                if (!ft_strncmp(cmd, "echo", 4)) {
                        *last_exit_status = execute_command(prepare_command(0, 1, (struct args*)l->content));
                        return;
                } else if (!ft_strncmp(cmd, "exit", 4)) {
                        *last_exit_status = execute_command(prepare_command(0, 1, (struct args*)l->content));
                        return;
                }
                break;
        case 5:
                if (!ft_strncmp(cmd, "unset", 5)) {
                        *last_exit_status = execute_command(prepare_command(0, 1, (struct args*)l->content));
                        return;
                }
                break;
        case 6:
                if (!ft_strncmp(cmd, "export", 6)) {
                        *last_exit_status = execute_command(prepare_command(0, 1, (struct args*)l->content));
                        return;
                }
                break;
        default:
                break;
        }
        handle_pipeline(ast, 0, l, last_exit_status);
}