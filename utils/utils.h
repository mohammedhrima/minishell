#ifndef UTILS_H
#define UTILS_H
#include "../header.h"
void add_pointer(t_list *list, void *value);
void add_number(t_list *list, int number);
void clear_list(t_list *list);
t_list *new_list(t_list *list, size_t size, t_type type);
t_node *new_node(t_token *token);
t_token *new_token(t_type type, char *source, int len);
t_file *new_file(char *name, int fd, t_type type);
void handle_signal(int signum);
void handle_heredoc_signal(int signum);
char *type_to_string(t_type type);
int ft_exit(int code);
int	ft_isspace(int c);
void	ft_memset(void *pointer, int c, size_t len);
void	*ft_memcpy(void *destination, void *source, size_t len);
void add_to_addresses(void *pointer);
void    *ft_calloc(size_t count, size_t size);
void *ft_realloc(void *pointer, size_t oldsize, size_t newsize);
int     ft_strlen(char *string);
void    ft_strncpy(char *destination, char *source, int len);
void    ft_strcpy(char *destination, char *source);
char	*ft_strchr(char *string, int c);
int     ft_strncmp(char *left, char *right, size_t len);
int     ft_strcmp(char *left, char *right);
char **split(char *string, char *spliter);
char **split_by_two(char *string, char c);
char *strjoin(char *string1, char *string2, char *string3);
char	*ft_strdup(char *string);
char *ft_itoa(int num);
int ft_atoi(char *str);
void ft_printf(int file_descriptor, char *fmt, ...);


#endif