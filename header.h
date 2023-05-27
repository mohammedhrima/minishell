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

// macros
#define SUCCESS 0
#define NO_SUCH_FILE_OR_DIR 1
#define PERMISSION_DENIED 1
#define NOT_OPENED -200
#define COMMAND_NOT_FOUND 127
#define CTRL_C 2
#define CTRL_SLASH 3

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
    integers_, pointers_, tokens_, nodes_,
    // =                      (        )
    assign_, identifier_, lparent_, rparent_,
    //  <           >             <<       >>      |      *
    redir_input, redir_output, heredoc_, append_, pipe_, all_,
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
    // pid_t process_id;
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
#if 1
    union
    {
        void **pointers;
        int *integers;
    };
#else
    uintptr_t pointer;
#endif
    size_t size;
    int pos;
    int len;
};

struct
{
    // List addresses;
    List envirement; // when get freed, make sur to free left and right with there token
    List tokens; // must have type tokens
    List fds;
    List pids;
    // List exist_status;
    // List pipes;
    char **path; // handle if path is NULL
    char **env;
    int inside_pipe;
    int inside_heredoc;
    // int signum;
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
// void    add_pointer_to_list(List *list, void *pointer);
// void    add_integer_to_list(List *list, int number);
void add_to_list(List *list, void *value);
void    ft_exit(int code);
Node    *expr();        // expression
Node    *and_or();          // ||
// Node    *and();         // &&
Node    *pipe_node();   // |
Node    *prime();       // files, command, argument, (), built in commands: echo, cd, pwd, export, unset, env, exit
Node *new_node(Token *token);
char*   type_to_string(Type type);
int get_last_exit_code();
void handle_signal(int signum) ;
void handle_heredoc_signal(int signum);


#endif