#ifndef MEM_H
#define MEM_H



/* 
* Overwrites a given number of bytes with
* zeros starting from a given position
*/
void ft_bzero(void *s, size_t n);



/* 
* Allocates a given amount of memory blocks
* and returns a pointer to the first block
*/
void *ft_calloc(size_t count, size_t size);



/* 
* Return the pointer to the first occurence of c in a byte
* string of size n if found, otherwise return NULL
*/
void *ft_memchr(const void *s, int c, size_t n);



/* Compare two byte strings of length n */
int ft_memcmp(const void *s1, const void *s2, size_t n);



/* Copy n bytes from src to dst */
void *ft_memcpy(void *dst, const void *src, size_t n);



/* Copy n bytes from src to dst, take into consideration string overlap */
void *ft_memmove(void *dst, const void *src, size_t n);



/* Fill a byte string with a byte value */
void *ft_memset(void *b, int c, size_t len);



#endif /* MEM_H */