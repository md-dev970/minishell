#include "exec.h"

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

static int o_flag(int f)
{
        return O_WRONLY | O_CREAT | ((f < 2) ? O_TRUNC : O_APPEND);
}

void builtin_pwd()
{
        char *cwd = getcwd(NULL, 0);
        ft_putstr_fd(cwd, STDOUT_FILENO);
        ft_putchar_fd('\n', STDOUT_FILENO);
        free(cwd);
}

void builtin_cd(char *input[])
{
        if (!input || !input[0] || *input[0] == '~') {
                chdir(getenv("HOME"));
        } else if (input[1]) {
                ft_putstr_fd("minishell: cd: too many arguments\n", STDOUT_FILENO);
        } else {
                int s;
                if (!(s = chdir(input[0]))) {
                        ft_putstr_fd("cd: command failed\n", STDOUT_FILENO);
                        perror("error: ");
                }
        }
}


void builtin_env()
{
        size_t i = 0;
        while (__environ[i])
                printf("%s\n", __environ[i++]);
}


void builtin_export(char *input[])
{
        if (!input)
                return;
        size_t i = 0;
        size_t v = 0;
        t_list *l = NULL;
        char **tmp;
        while (input[i]) {
                if (!ft_isalpha(*input[i]) && *input[i] != '_') {
                        printf("minishell: '%s' is not a valid identifier\n", input[i]);
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
                        
                size_t j = 0;
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
                printf("Error: coudn't allocate memory\n");
                return;
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
}


void builtin_unset(char *input[])
{
        if (!input)
                return;
        size_t input_size = ft_arrsize((void **)input);
        size_t v = 0;
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
        printf("vars to unset: %i\n", lst_size);
        if (lst_size == 0) {
                ft_lstclear(l, &free);
                return;
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
        return;
}

static size_t check_opt(char *opt)
{
        size_t n = ft_strlen(opt);
        if (n < 2 || opt[0] != '-')
                return 0;
        for (int i = 1; i < n; ++i) {
                if (opt[i] == 'n')
                        continue;
                return 0;
        }
        return 1;
}

void builtin_echo(char *input[])
{
        if (!input || !input[0]) {
                printf("\n");
                return;
        }
        size_t i;
        char newline = ((i = check_opt(input[0])) == 0) ? '\n' : '\0';
        while (input[i])
                printf("%s ", input[i++]);
        printf("%c", newline);
}


void builtin_exit(char *input[])
{
        ft_putstr_fd("executing built in exit\n", STDOUT_FILENO);
        exit(0);
}

void non_builtin(char *input[])
{
        printf("executing non built in\n");
        char *pathname = search_executable(input[0]);
        if (!pathname) {
                write(STDOUT_FILENO, "minishell: ", 12);
                write(STDOUT_FILENO, input[0], ft_strlen(input[0]) + 1);
                write(STDOUT_FILENO, ": command not found\n", 21);
                exit(1);
        }
        int s = execve(pathname, input, __environ);
        exit(s);
}

void execute_command(char *input[], int cin, int cout)
{
        if (!input || !input[0])
                exit(12);
        
        char *cmd = input[0];
        printf("checking command %s\n", cmd);
        switch (ft_strlen(cmd)) {
        case 2:
                if (!ft_strncmp(cmd, "cd", 2)) {
                        builtin_cd(input + 1);
                        goto cleanup;
                }
                        
                break;
        case 3:
                if (!ft_strncmp(cmd, "pwd", 3)) {
                        builtin_pwd();
                        goto cleanup;
                } else if (!ft_strncmp(cmd, "env", 3)) {
                        builtin_env();
                        goto cleanup;
                }
                break;
        case 4:
                if (!ft_strncmp(cmd, "echo", 4)) {
                        builtin_echo(input + 1);
                        goto cleanup;
                } else if (!ft_strncmp(cmd, "exit", 4)) {
                        builtin_exit(input + 1);
                        goto cleanup;
                }
                break;
        case 5:
                if (!ft_strncmp(cmd, "unset", 5)) {
                        builtin_unset(input + 1);
                        goto cleanup;
                }
                break;
        case 6:
                if (!ft_strncmp(cmd, "export", 6)) {
                        builtin_export(input + 1);
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
        dup2(cin, STDIN_FILENO);
        dup2(cout, STDOUT_FILENO);
}

static void prepare_command(int in, int out, struct args *ar)
{
        if (!ar || !ar->clargs || !ar->clargs->content)
                return;

        char *cmd = (char *)ar->clargs->content;

        printf("executing command %s \n", cmd);
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
                write(cout, "file to open: ", 14);
                write(cout, f->path, ft_strlen(f->path));
                write(cout, "\n", 2);
                if (!f->flag) {
                        if ((fd = open(f->path, O_RDONLY)) < 0) {
                                write(cout, "error opening file 0\n", 22);
                                exit(1);
                        }
                        close(STDIN_FILENO);
                        dup2(fd, STDIN_FILENO);
                } else {
                        if ((fd = open(f->path, o_flag(f->flag), 0664)) < 0) {
                                write(cout, "error opening file 1\n", 22);
                                exit(2);
                        }
                        close(STDOUT_FILENO);
                        dup2(fd, STDOUT_FILENO);
                }
                tmp = tmp->next;
        }
        printf("beginning execution\n");
        execute_command(input, cin, cout);

}

void handle_pipeline(struct node *ast, int in, t_list *l)
{
        if (!ast)
                return;
        if (!ast->right) {
                pid_t id = fork();
                if (id < 0) {
                        printf("fork failed\n");
                        exit(2);
                } else if (id) {
                        wait(NULL);
                        return;
                } else {
                        prepare_command(in, 1, (struct args *)l->content);
                }
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
                handle_pipeline(ast->right->center, p[0], l->next);
                close(p[0]);
                wait(NULL);
        } else {
                close(p[0]);
                prepare_command(in, p[1], (struct args *)l->content);
                close(p[1]);
                exit(0);
        }
}

void handle_commands(struct node *ast, t_list *l)
{
        if (!ast)
                return;
        if (ast->right) {
                handle_pipeline(ast, 0, l);
                return;
        }
        char *cmd = ast->left->value;
        switch (ft_strlen(cmd)) {
        case 2:
                if (!ft_strncmp(cmd, "cd", 2)) {
                        prepare_command(0, 1, (struct args*)l->content);
                        return;
                }
                        
                break;
        case 3:
                if (!ft_strncmp(cmd, "pwd", 3)) {
                        prepare_command(0, 1, (struct args*)l->content);
                        return;
                } else if (!ft_strncmp(cmd, "env", 3)) {
                        prepare_command(0, 1, (struct args*)l->content);
                        return;
                }
                break;
        case 4:
                if (!ft_strncmp(cmd, "echo", 4)) {
                        prepare_command(0, 1, (struct args*)l->content);
                        return;
                } else if (!ft_strncmp(cmd, "exit", 4)) {
                        prepare_command(0, 1, (struct args*)l->content);
                        return;
                }
                break;
        case 5:
                if (!ft_strncmp(cmd, "unset", 5)) {
                        prepare_command(0, 1, (struct args*)l->content);
                        return;
                }
                break;
        case 6:
                if (!ft_strncmp(cmd, "export", 6)) {
                        prepare_command(0, 1, (struct args*)l->content);
                        return;
                }
                break;
        default:
                break;
        }
        handle_pipeline(ast, 0, l);
}