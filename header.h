#ifndef HEADER_H
#define HEADER_H

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
typedef enum Type Type;
typedef struct Token Token;
typedef struct Token Value;
typedef struct Node Node;
typedef struct File File;
typedef struct List List;

// types
enum Type
{
    start_ = 11,
    integers_, pointers_,
    // =                      (        )
    assign_, identifier_, lparent_, rparent_,
    //  <           >             <<       >>      |
    redir_input, redir_output, heredoc_, append_, pipe_,
    // and or
    and_, or_,
    // built in
    env_, echo_, cd_, pwd_, export_, unset_, exit_,
    // processes id
    pid_,
    // end
    end_
};

// structs
struct Token
{
    Type type;

    char *value;
    int start;
    int len;
    pid_t process_id;
};

struct Node
{
    Node *left;
    Node *right;
    Token *token;
};

struct File
{
    char *name;
    int fd;
    Type type;
};

struct List
{
    Type type;
    
    union
    {
        void **pointer;
        int *integers;
    };
    size_t size;
    int pos;
    int len;
};

struct
{
    List addresses;
    List envirement;
    List tokens;
    List files;
    List pids;
    List exist_status;
    List pipes;
    char **path;
    char **env;
    int inside_pipe;
} global;

// macros
#define in STDIN_FILENO
#define out STDOUT_FILENO
#define err STDERR_FILENO

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
// void    *ft_calloc2(size_t count, size_t size);
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
void    add_pointer_to_list(List *list, void *pointer);
void    add_integer_to_list(List *list, int number);
void    ft_exit(int code);
Node    *expr();        // expression
Node    *and_or();          // ||
// Node    *and();         // &&
Node    *pipe_node();   // |
Node    *prime();       // files, command, argument, (), built in commands: echo, cd, pwd, export, unset, env, exit
Node *new_node(Token *token);
char*   type_to_string(Type type);
#endif