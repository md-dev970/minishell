#ifndef FT_MEMMOVE_H
#define FT_MEMMOVE_H

/* Copy n bytes from src to dst, take into consideration string overlap */
void *ft_memmove(void *dst, const void *src, size_t n);

#endif /* FT_MEMMOVE_H */