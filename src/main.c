#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <sys/types.h>
#include "lexer.h"
#include "parser.h"


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

char **expand_input(node *ast);

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
        char **input = expand_input(ast->center);
        char path[20] = "/bin/";
        ft_strlcat(path, ast->left->value, 20);
        input[0] = path;
        printf("---------------------------------\nCommand output\n");
        int status = execve(path, input, NULL);
        
        printf("Execution status : %i\n", status);
        size_t i = 0;
        while (input && input[i])
                free(input[i++]);

        free(input);
        if (ast->right != NULL) {
                printf("pipline\n");
                execute_pipe(ast->right->center, ast->left->value);
        }
}

void execute_pipe(node *ast, char *output)
{
        if (!ast)
                return;
        printf("executing command %s with input from %s\n", ast->left->value, output);
        char **input = expand_input(ast->center);
        size_t i = 0;
        while (input && input[i])
                free(input[i++]);
        free(input);
                
        if (ast->right != NULL) {
                printf("pipline\n");
                execute_pipe(ast->right->center, ast->left->value);
        }

}

char *heredoc(char *delimiter)
{
        char *input = "";
        size_t len = 0;
        t_list *lines = NULL;
        while (
        ft_strlen(input) != ft_strlen(delimiter) ||
        ft_strncmp(input, delimiter, ft_strlen(input)) != 0) {
                
                input = readline(">");
                len += ft_strlen(input) + 1;
                ft_lstadd_back(&lines, ft_lstnew(input));
        }
        len -= ft_strlen(delimiter) + 1;
        input = (char *)malloc((len + 1) * sizeof(char));
        if (!input)
                return NULL;
        t_list *tmp = lines;
        while (tmp && tmp->next) {
                ft_strlcat(input, (char *)tmp->content, len + 1);
                ft_strlcat(input, "\n", len + 1);
                tmp = tmp->next;
        }
        ft_lstclear(lines, &free);
                
        return input;
}

void add_input(node *ast, t_list **input)
{
        if (!ast) {
                printf("null\n");
                return;
        }
        if (ast->type == IDENT)
                ft_lstadd_back(input, ft_lstnew(ft_strdup(ast->value)));
        if (ast->type == NONE && ast->left->type == DLT) {
                printf("delimiter is: %s\n", ast->center->value);
                char *text = heredoc(ast->center->value);
                ft_lstadd_back(input, ft_lstnew(text));
        }
        add_input(ast->left, input);
        add_input(ast->center, input);
}

char **expand_input(node *ast)
{
        if (!ast)
                return NULL;
        
        t_list *input = NULL;
        add_input(ast, &input);
        printf("----------------------\n Inputs: \n");
        ft_lstiter(input, &print_lexem);
        t_list *tmp = input;
        printf("Input list size: %i\n", ft_lstsize(input));
        char **args = (char **)malloc((ft_lstsize(input) + 2) * sizeof(char *));
        for (size_t i = 1; i < ft_lstsize(input) + 1; ++i)
        {
                args[i] = ft_strdup((char *)tmp->content);
                tmp = tmp->next;
        }
        args[ft_lstsize(input) + 1] = NULL;
        ft_lstclear(input, &free);
        return args;
}