#include "token.h"


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