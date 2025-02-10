#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../libft/include/libft.h"

char *b[7] = {"echo", "cd", "pwd", "env", "export", "unset", "exit"};
char *s[8] = {" ", ">", "<", "<<", ">>", "|"};

enum token_type {
        PIPE,
        GT,
        LT,
        DGT,
        DLT,
        IDENT,
        NONE
};

struct token {
        enum token_type type;
        char *value;
};

typedef struct node {
        enum token_type type;
        char *value;
        struct node *left;
        struct node *center;
        struct node *right;
} node;

void print_tree(node *root)
{
        if (!root)
                return;
        
        switch (root->type)
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
                printf(" %s ", root->value);
        }

        print_tree(root->left);
        print_tree(root->center);
        print_tree(root->right);
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

void free_token(void *token)
{
        struct token *t = (struct token *)token;
        free(t->value);
        free(t);
}

void print_token(void *token)
{
        struct token *t = (struct token *)token;
        switch (t->type)
        {
        case PIPE:
                printf("token type: PIPE\n");
                break;
        case GT:
                printf("token type: GT\n");
                break;
        case DGT:
                printf("token type: DGT\n");
                break;
        case LT:
                printf("token type: LT\n");
                break;
        case DLT:
                printf("token type: DLT\n");
                break;
        case IDENT:
                printf("token type: IDENT\n");
                break;
        default:
                break;
        }
        printf("token value: %s\n", t->value);
}

void print_lexem(void *lexem)
{
        char *s = (char *)lexem;
        printf("%s\n", s ? s : "");
}

void *generate_token(void *value)
{
        char *s = (char *)value;
        struct token *t = (struct token *)malloc(sizeof(struct token));
        switch (ft_strlen(s))
        {
        case 2:
                if (ft_strncmp(s, "<<", 2) == 0)
                        t->type = DLT;
                else if (ft_strncmp(s, ">>", 2) == 0)
                        t->type = DGT;
                else
                        t->type = IDENT;
                break;
        case 1:
                switch (*s)
                {
                case '<':
                        t->type = LT;
                        break;
                case '>':
                        t->type = GT;
                        break;
                case '|':
                        t->type = PIPE;
                        break;
                default:
                        t->type = IDENT;
                        break;
                }
                break;
        default:
                t->type = IDENT;
                break;
        }
        t->value = ft_strdup(s);
        return t;
}

int is_separator(char *str, size_t i)
{
        for (size_t j = 0; j < 6; ++j) {
                if (str[i] == *s[j])
                        return 1;     
        }
        return 0;
}

void expander(char **str)
{
        if (!str || !*str)
                return;
        char *s = *str;
        printf("string to expand : %s[end]\n", s);
        char q = '\0';
        size_t len = ft_strlen(s);
        t_list *lst = NULL;
        char *tmp;
        /* Lord forgive me for what I'm about to write */
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
                return;
        }
        ret[0] = '\0';
        tmp_lst = lst;
        while (tmp_lst) {
                ft_strlcat(ret, (char *)tmp_lst->content, n + 1);
                tmp_lst = tmp_lst->next;
        }
        printf("result: %s[end]\n", ret);
        free(s);
        *str = ret;
        ft_lstclear(lst, &free);
}

int lexer(t_list **lst, char* input)
{
        size_t len = ft_strlen(input);
        size_t j;
        char open_quote = '\0';
        t_list *here_doc = NULL;
        char *tmp;
        for(size_t i = 0; i < len; ++i) {
                switch (input[i])
                {
                case ' ':
                        break;

                case '|':
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 1)));
                        break;

                case '<':
                        if (i == len - 1 || input[i + 1] != input[i]) {
                                ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 1)));
                                break;
                        }
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 2)));
                        i++;
                        break;
                case '>':
                        if (i == len - 1 || input[i + 1] != input[i]) {
                                ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 1)));
                                break;
                        }
                        ft_lstadd_back(lst, ft_lstnew(ft_substr(input, i, 2)));
                        i++;
                        break;

                default:
                        j = i;
                        while (j < len && (!is_separator(input, j) || (open_quote))) {
                                if (input[j] == '\"' || input[j] == '\'') {
                                        if (!open_quote)
                                                open_quote = input[j];
                                        else if (open_quote == input[j])
                                                open_quote = '\0';
                                }
                                j++;
                        }
                        tmp = ft_substr(input, i, j - i);
                        expander(&tmp);
                        if (ft_strlen(tmp))
                                ft_lstadd_back(lst, ft_lstnew(tmp));
                        else
                                free(tmp);
                        i = j - 1;
                        break;
                }
        }
        t_list *tmp_lst = *lst;
        *lst = ft_lstmap(*lst, &generate_token, &free_token);
        ft_lstclear(tmp_lst, &free);
        return (open_quote) ? -1 : 0;
}

node *B(t_list **l);

node *S(t_list **l);

node *P(t_list **l);

node *A(t_list **l);

node *I(t_list **l);

node *O(t_list **l);

node *parser(t_list *lexems)
{
        return B(lexems);
}

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

node *B(t_list **l)
{
        printf("currently in B\n");
        if (!l || !(*l))
                return NULL;
        return S(l);
}

node *S(t_list **l)
{
        printf("currently in S\n");
        if (!l)
                return NULL;
        struct token *t = (struct token *)l->content;
        if (t->type != IDENT)
                return NULL;
        node *root = (node *)malloc(sizeof(node));
        root->type = NONE;
        root->center = NULL;
        root->left = (node *)malloc(sizeof(node));
        root->left->type = IDENT;
        root->left->value = t->value;
        root->left->left = NULL;
        root->left->right = NULL;
        root->left->center = NULL;
        root->right = P(l->next);
        return root;
}

node *P(t_list **l)
{
        printf("currently in P\n");
        if (!l)
                return NULL;
        struct token *t = (struct token *)->content;
        
        if (t->type == IDENT) {
                node *root = (node *)malloc(sizeof(node));
                root->type = NONE;
                root->center = NULL;
                root->left = (node *)malloc(sizeof(node));
                root->left->type = IDENT;
                root->left->value = t->value;
                root->left->left = NULL;
                root->left->right = NULL;
                root->left->center = NULL;
                root->right = P(l->next);
                return root;
        }
        
        return A(l);
}

node *A(t_list **l)
{
        printf("currently in A\n");
        if (!l)
                return NULL;
        struct token *t = (struct token *)l->content;
        node *root = (node *)malloc(sizeof(node));
        root->type = NONE;
        root->center = NULL;
        root->left = (node *)malloc(sizeof(node));
        root->left->type = t->type;
        root->left->left = NULL;
        root->left->right = NULL;
        root->left->center = NULL;

        switch (t->type) {
        case PIPE:
                root->right = S(l->next);
                break;
        case DLT:
                root->right = O(l->next);
                break;
        default:
                root->right = I(l->next);
        }

        return root;
}

node *I(t_list **l)
{
        printf("currently in I\n");
        if (!l)
                return NULL;
        struct token *t = (struct token *)l->content;
        if (t->type != IDENT)
                return NULL;
        node *root = (node *)malloc(sizeof(node));
        root->type = NONE;
        root->center = NULL;
        root->left = (node *)malloc(sizeof(node));
        root->left->type = IDENT;
        root->left->value = t->value;
        root->left->left = NULL;
        root->left->right = NULL;
        root->left->center = NULL;
        root->right = P(l->next);
        return root;
}

node *O(t_list **l)
{
        if (!l)
                return NULL;
        struct token *t = (struct token *)l->content;
        if (t->type != IDENT)
                return NULL;

        node *root = (node *)malloc(sizeof(node));
        root->type = NONE;
        root->center = NULL;
        root->left = (node *)malloc(sizeof(node));
        root->left->type = IDENT;
        root->left->value = t->value;
        root->left->left = NULL;
        root->left->right = NULL;
        root->left->center = NULL;
        root->right = P(l->next);
        return root;
}