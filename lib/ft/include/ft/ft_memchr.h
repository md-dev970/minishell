#ifndef FT_MEMCHR_H
#define FT_MEMCHR_h

/* 
* Return the pointer to the first occurence of c in a byte
* string of size n if found, otherwise return NULL
*/
void *ft_memchr(const void *s, int c, size_t n);

#endif /* FT_MEMCHR_H */