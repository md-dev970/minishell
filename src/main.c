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

node *I(t_list *l, t_list **heredoc);

node *S(t_list *l, t_list **heredoc);

node *W(t_list *l, t_list **heredoc);

node *R(t_list *l, t_list **heredoc);

node *F(t_list *l, t_list **heredoc);

node *H(t_list *l, t_list **heredoc);

node *parser(t_list *lexems, t_list **heredoc)
{
        return I(lexems, heredoc);
}

int main()
{
        int quit = 0;
        char *inputBuffer;
        t_list *lexems = NULL;
        t_list *heredoc = NULL;
        while(quit == 0) {
                inputBuffer = readline("minishell>");
                int r = lexer(&lexems, inputBuffer);
                if (r) {
                        printf("Error : unclosed quotes\n");
                        goto clean;
                }
                node *ast = parser(lexems, &heredoc);
                goto clean;
                t_list *doc = NULL;
                t_list *tmp_lex = lexems;
                t_list *tmp_hd = heredoc;
                while(tmp_hd) {
                        free(inputBuffer);
                        
                        inputBuffer = readline(">");
                        printf("delimiter %s[end]\n", (char *)tmp_hd->content);
                        printf("buffer %s[end]\n", inputBuffer);
                        printf("diff %i\n", strncmp(inputBuffer, (char *)tmp_hd->content, ft_strlen(inputBuffer)));
                        if (ft_strlen(inputBuffer) != ft_strlen((char *)tmp_hd->content) 
                        || strncmp(inputBuffer, (char *)tmp_hd->content, ft_strlen(inputBuffer)) != 0) {
                                ft_lstadd_back(&doc, ft_lstnew(ft_strdup(inputBuffer)));
                                continue;
                        }
                                
                        printf("reached this point\n");
                        size_t n = 0;
                        t_list *tmp_lst = doc;
                        while (tmp_lst) {
                                n += ft_strlen((char *)tmp_lst->content);
                                tmp_lst = tmp_lst->next;
                        }
                        printf("heredoc lines : %i\n", ft_lstsize(doc));
                        if (n + ft_lstsize(doc) == 0) {
                                tmp_hd = tmp_hd->next;
                                continue;
                        }

                        char *ret = (char *)malloc((n + ft_lstsize(doc)) * sizeof(char));
                        if (ret == NULL) {
                                ft_lstclear(doc, &free);
                                printf("malloc failed : in heredoc treatment\n");
                                return -1;
                        }
                        ret[0] = '\0';
                        tmp_lst = doc;
                        while (tmp_lst) {
                                ft_strlcat(ret, (char *)tmp_lst->content, n + ft_lstsize(doc));
                                if (tmp_lst->next)
                                        ft_strlcat(ret, "\n", n + ft_lstsize(doc));
                                tmp_lst = tmp_lst->next;
                        }
                        ft_lstclear(doc, &free);
                        doc = NULL;
                        tmp_lex = lexems;
                        while(tmp_lex) {
                                if (((struct token *)tmp_lex->content)->type == DLT) {
                                        char *tmp = ((struct token *)tmp_lex->next->content)->value;
                                        ((struct token *)tmp_lex->next->content)->value = ret;
                                        free(tmp);
                                        break;
                                }
                                tmp_lex = tmp_lex->next;
                        }
                        tmp_hd = tmp_hd->next;
                }
                if (strcmp(inputBuffer, "exit") == 0)
                        quit = 1;
                clean:
                free(inputBuffer);
                ft_lstiter(lexems, &print_token);
                ft_lstclear(heredoc, &free);
                heredoc = NULL;
                ft_lstclear(lexems, &free_token);
                lexems = NULL;
        }
        rl_clear_history();

        return 0;
}

node *I(t_list *l, t_list **heredoc)
{
        printf("currently in I\n");
        if (!l)
                return NULL;
        return S(l, heredoc);
}

node *S(t_list *l, t_list **heredoc)
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
        root->right = W(l->next, heredoc);
        return root;
}

node *W(t_list *l, t_list **heredoc)
{
        printf("currently in W\n");
        if (!l)
                return NULL;
        struct token *t = (struct token *)l->content;
        
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
                root->right = W(l->next, heredoc);
                return root;
        }
        
        return R(l, heredoc);
}

node *R(t_list *l, t_list **heredoc)
{
        printf("currently in R\n");
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
                root->right = S(l->next, heredoc);
                break;
        case DLT:
                root->right = H(l->next, heredoc);
                break;
        default:
                root->right = F(l->next, heredoc);
        }

        return root;
}

node *F(t_list *l, t_list **heredoc)
{
        printf("currently in F\n");
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
        root->right = W(l->next, heredoc);
        return root;
}

node *H(t_list *l, t_list **heredoc)
{
        if (!l)
                return NULL;
        struct token *t = (struct token *)l->content;
        if (t->type != IDENT)
                return NULL;
        ft_lstadd_back(heredoc, ft_lstnew(ft_strdup(t->value)));

        node *root = (node *)malloc(sizeof(node));
        root->type = NONE;
        root->center = NULL;
        root->left = (node *)malloc(sizeof(node));
        root->left->type = IDENT;
        root->left->value = t->value;
        root->left->left = NULL;
        root->left->right = NULL;
        root->left->center = NULL;
        root->right = W(l->next, heredoc);
        return root;
}