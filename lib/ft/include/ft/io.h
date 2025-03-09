#ifndef IO_H
#define IO_H



/* Write a character to a file descriptor */
int ft_putchar_fd(char c, int fd);



/* Write a newline character to a file descriptor */
int ft_putendl_fd(char *s, int fd);



/* Write an integer to a file descriptor */
int ft_putnbr_fd(int n, int fd);



/* Write a string to a file descriptor */
int ft_putstr_fd(char *s, int fd);



#endif /* IO_H */