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
        exit(0);
}

void builtin_cd(char *input[])
{
        if (!input || !input[0]) {
                chdir(getenv("HOME"));
                builtin_pwd();
                exit(0);
        }
                
        if (input[1]) {
                ft_putstr_fd("minishell: cd: too many arguments\n", STDOUT_FILENO);
                exit(-1);
        }
        int s;
        if (!(s = chdir(input[0]))) {
                ft_putstr_fd("cd: command failed\n", STDOUT_FILENO);
                perror("error: ");
        }
        exit(s);
}


void builtin_env()
{
        ft_putstr_fd("executing built in env\n", STDOUT_FILENO);
        exit(0);
}


void builtin_export(char *input[])
{
        ft_putstr_fd("executing built in export\n", STDOUT_FILENO);
        exit(0);
}


void builtin_unset(char *input[])
{
        ft_putstr_fd("executing built in unset\n", STDOUT_FILENO);
        exit(0);
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
                exit(0);
        }
        size_t i;
        char newline = ((i = check_opt(input[0])) == 0) ? '\n' : '\0';
        while (input[i])
                printf("%s ", input[i++]);
        printf("%c", newline);
        exit(0);
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

void execute_command(char *input[])
{
        if (!input || !input[0])
                exit(12);
        
        char *cmd = input[0];
        
        switch (ft_strlen(cmd)) {
        case 2:
                if (!ft_strncmp(cmd, "cd", 2))
                        builtin_cd(input + 1);
                break;
        case 3:
                if (!ft_strncmp(cmd, "pwd", 3))
                        builtin_pwd();
                else if (!ft_strncmp(cmd, "env", 3))
                        builtin_env();
                break;
        case 4:
                if (!ft_strncmp(cmd, "echo", 4))
                        builtin_echo(input + 1);
                else if (!ft_strncmp(cmd, "exit", 4))
                        builtin_exit(input + 1);
                break;
        case 5:
                if (!ft_strncmp(cmd, "unset", 5))
                        builtin_unset(input + 1);
                break;
        case 6:
                if (!ft_strncmp(cmd, "export", 6))
                        builtin_export(input + 1);
                break;
        default:
                break;
        }
        non_builtin(input);
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
        execute_command(input);

}

void handle_commands(struct node *ast, int in, t_list *l)
{
        if (!ast)
                return;
        if (!ast->right) {
                prepare_command(in, 1, (struct args *)l->content);
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
                handle_commands(ast->right->center, p[0], l->next);
                close(p[0]);
                waitpid(id, NULL, 0);
        } else {
                close(p[0]);
                prepare_command(in, p[1], (struct args *)l->content);
                close(p[1]);
                exit(0);
        }
}