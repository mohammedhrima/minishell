#ifndef HEADER_H
#define HEADER_H

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

void    ft_printf(int file_descriptor, char *fmt, ...);
int     ft_isnum(int c);
int     ft_isupper(int c);
int     ft_islower(int c);
int     ft_isalpha(int c);
int     ft_isalphanum(int c);
int     ft_isspace(int c);
void    ft_memset(void *pointer, int c, size_t len);
void    *ft_memcpy(void *destination, void *source, size_t len);
void    *ft_calloc(size_t count, size_t size);
void    *ft_realloc(void *pointer, size_t old_size, size_t new_size);
int     ft_strlen(char *string);
void    ft_strncpy(char *destination, char *source, int len);
void    ft_strcpy(char *destination, char *source);
char    *ft_strchr(char *string, int c);
int     ft_strncmp(char *left, char *right, size_t len);
int     ft_strcmp(char *left, char *right);
char    **split(char *string, char *spliter);
char    **split_by_two(char *string, char c);
char    *strjoin(char *string1, char *string2, char *string3);
char    *charjoin(char *string, char c);
char    *ft_strdup(char *string);
char    *ft_itoa(int num);
void    ft_putchar(int file_descriptor, int c);
void    ft_putstr(int file_descriptor, char *str);
void    ft_putnbr(int file_descriptor, long num);
void    print_space(int file_descriptor, int len);
void    ft_printf(int file_descriptor, char *fmt, ...);
void    ft_exit(int code);
int     get_last_exit_code();
void    handle_signal(int signum) ;
void    handle_heredoc_signal(int signum);
char    *ft_readline(char *msg);
void    add_to_addresses(void *pointer);

#endif