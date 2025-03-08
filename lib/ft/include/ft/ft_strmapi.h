#ifndef FT_STRMAPI_H
#define FT_STRMAPI_H

/* 
* Return the resulting string of applying a specified
* function f to each character of the input string
*/
char *ft_strmapi(const char *s, char (*f)(unsigned int, char));

#endif /* FT_STRMAPI_H */