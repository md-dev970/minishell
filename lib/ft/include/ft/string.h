#ifndef STRING_H
#define STRING_H



/* Converts a valid string of ascii characters to an integer */
int ft_atoi(const char *str);



/* 
* Checks if an integer is the ascii code of an
* alphanumeric character (letter or digit)
*/
int ft_isalnum(const int arg);



/* 
* Check if an integer is a valid ascii code for
* a letter of the alphabet (both upper and lowercase)
*/
int ft_isalpha(const int arg);


/* 
* Check if an integer is a valid 
* ascii code (between 0 and 127) 
*/
int ft_isascii(const int arg);



/* Check if an integer is a valid ascii code for a digit */
int ft_isdigit(const int arg);



/* 
* Check if an integer is a valid 
* ascii code of a printable character 
*/
int ft_isprint(const int arg);



/* Convert an integer to a string of ascii characters */
char *ft_itoa(int n);



/* Split a string into an array of words using a character c as separator */
char **ft_split(const char *s, char c);



/* Find the first occurence of a character in a string */
char *ft_strchr(const char *s, int c);



/* Duplicate a string */
char *ft_strdup(const char *s);



/* Apply a function to each character of a string */
void ft_striteri(char *s, void (*f)(unsigned int, char *));



/* Return a new string that is the concatenation of s1 an s2 */
char *ft_strjoin(const char *s1, const char *s2);



/* Concatenate src string to dst string */
size_t ft_strlcat(char *dst, const char *src, size_t dstsize);



/* Copy src string into dst string (overwrites dst) */
size_t ft_strlcpy(char *dst, const char *src, size_t dstsize);



/* Return lenght of a string */
size_t ft_strlen(const char *s);



/* 
* Return the resulting string of applying a specified
* function f to each character of the input string
*/
char *ft_strmapi(const char *s, char (*f)(unsigned int, char));



/* Compare two strings */
int ft_strncmp(const char *s1, const char *s2, size_t n);



/*
* Find the first occurence of a substring 
* needle in the string haystack
*/
char *ft_strnstr(const char *haystack, const char *needle, size_t len);



/* Find the last occurence of a character in a string */
char *ft_strrchr(const char *s, int c);



/* 
* Return the resulting string of removing the characters
* in set from the beginning and end of the string s
*/
char *ft_strtrim(const char *s, const char *set);



/* Creates substring of length len */
char *ft_substr(const char *s, unsigned int start, size_t len);



/* Convert lowercase ascii letter to uppercase */
void ft_toupper(char *c);



/* Convert uppercase ascii letter to lowercase */
void ft_tolower(char *c);

#endif /* STRING_H */