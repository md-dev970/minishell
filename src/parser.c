#include "parser.h"

static node *B(t_list **l);
static node *S(t_list **l);
static node *P(t_list **l);
static node *A(t_list **l);
static node *I(t_list **l);
static node *O(t_list **l);
static node *F(t_list **l);

node *parser(t_list *lexems)
{
        return B(&lexems);
}

static node *B(t_list **l)
{
        printf("currently in B\n");
        if (!l || !(*l))
                return NULL;
        return S(l);
}


static node *S(t_list **l)
{
        printf("currently in S\n");
        if (!(*l)) {
                printf("Error\n");
                return NULL;
        }
        struct token *t = (struct token *)(*l)->content;
        if (t->type != IDENT)
                return NULL;
        node *root = (node *)malloc(sizeof(node));
        root->type = NONE;
        root->left = (node *)malloc(sizeof(node));
        root->left->type = IDENT;
        root->left->value = t->value;
        root->left->left = NULL;
        root->left->right = NULL;
        root->left->center = NULL;
        *l = (*l)->next;
        root->center = A(l);
        root->right = P(l);
        return root;
}


static node *P(t_list **l)
{
        printf("currently in P\n");
        if (!(*l))
                return NULL;
        struct token *t = (struct token *)(*l)->content;

        if (t->type == PIPE) {
                node *root = (node *)malloc(sizeof(node));
                root->type = NONE;
                root->left = (node *)malloc(sizeof(node));
                root->left->type = PIPE;
                root->left->left = NULL;
                root->left->right = NULL;
                root->left->center = NULL;
                *l = (*l)->next;
                root->center = S(l);
                root->right = P(l);
                return root;
        }
        printf("Error\n");
        return NULL;
}


static node *A(t_list **l)
{
        printf("currently in A\n");
        if (!(*l))
                return NULL;
        struct token *t = (struct token *)(*l)->content;
        node *root = (node *)malloc(sizeof(node));
        root->type = NONE;
        root->right = NULL;

        switch (t->type) {
        case DGT:
        case GT:
                root->left = O(l);
                break;
        case LT:
        case DLT:
                root->left = I(l);
                break;
        case IDENT:
                (*l) = (*l)->next;
                root->left = (node *)malloc(sizeof(node));
                root->left->type = IDENT;
                root->left->value = t->value;
                root->left->left = NULL;
                root->left->right = NULL;
                root->left->center = NULL;
                break;
        default:
                free(root);
                return NULL;
        }
        root->center = A(l);

        return root;
}


static node *I(t_list **l)
{
        printf("currently in I\n");
        if (!(*l))
                return NULL;
        struct token *t = (struct token *)(*l)->content;
        node *root = (node *)malloc(sizeof(node));
        root->type = NONE;
        root->left = (node *)malloc(sizeof(node));
        root->left->type = t->type;
        root->left->left = NULL;
        root->left->right = NULL;
        root->left->center = NULL;
        root->right = NULL;
        *l = (*l)->next;
        root->center = F(l);
        return root;
}


static node *O(t_list **l)
{
        printf("currently in O\n");
        if (!(*l))
                return NULL;
        struct token *t = (struct token *)(*l)->content;
        node *root = (node *)malloc(sizeof(node));
        root->type = NONE;
        root->left = (node *)malloc(sizeof(node));
        root->left->type = t->type;
        root->left->left = NULL;
        root->left->right = NULL;
        root->left->center = NULL;
        root->right = NULL;
        *l = (*l)->next;
        root->center = F(l);
        return root;
}


static node *F(t_list **l)
{
        if (!(*l)) {
                printf("Error\n");
                return NULL;
        }

        struct token *t = (struct token *)(*l)->content;
        if (t->type != IDENT) {
                printf("Error\n");
                return NULL;
        }
        printf("currently in F\n");
        node *root = (node *)malloc(sizeof(node));
        root->type = IDENT;
        root->value = t->value;
        root->left = NULL;
        root->right = NULL;
        root->center = NULL;
        *l = (*l)->next;
        return root;
}