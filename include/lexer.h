#ifndef LEXER_H
#define LEXER_H

#include "token.h"

void print_lexem(void *lexem);

int lexer(t_list **lst, char* input);

#endif /* LEXER_H */