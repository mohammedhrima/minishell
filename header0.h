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

// error messages
#define SUCCESS 0
#define NO_SUCH_FILE_OR_DIR 1
#define PERMISSION_DENIED 1
#define AMBIGIOUS_REDIRECTION 1
#define UNEXPECTED_ERROR 258
#define COMMAND_NOT_FOUND 127
#define SYNTAX_ERROR 258

// file descriptors
#define NOT_OPENED -200
#define in STDIN_FILENO
#define out STDOUT_FILENO
#define err STDERR_FILENO

// signals
#define CTRL_C 2
#define CTRL_SLASH 3

#ifndef DEBUG
#define DEBUG 1
#endif

typedef enum e_type t_type;
typedef struct s_token t_token;
typedef struct s_token t_value;
typedef struct s_node t_node;
typedef struct s_file t_file;
typedef struct s_list t_list;
typedef struct s_global t_global;


// types
enum					e_type
{
	integers_,
	pointers_,
	assign_,
	identifier_,
	lparent_,
	rparent_,
	redir_input,
	redir_output,
	heredoc_,
	append_,
	pipe_,
	star_,
	redir_parent,
	and_,
	or_,
	env_,
	echo_,
	cd_,
	pwd_,
	export_,
	unset_,
	exit_,
	end_
};

// structs
struct s_token
{
    t_type type;

    char *value;
    int start;
    int len;
    // int process_id;
};

struct s_node
{
    t_node *left;
    t_node *right;
    t_token *token;
};

struct s_file
{
    char *name;
    int fd;
    t_type type;
};

struct s_list
{
    t_type type;
    union
    {
        void **pointers;
        int *integers;
    };
    size_t size;
    int pos;
    int len;
};

struct s_global
{
    t_list addresses;
    t_list envirement; // when get freed, make sure to free left and right with there token
    t_list tokens; // must have type tokens
    t_list fds;
    t_list pids;
    char **path; // handle if path is NULL
    int inside_pipe;
    // int inside_child_process;
    char **env;
};

t_global global;

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
void    add_to_list(t_list *list, void *value);
void    ft_exit(int code);
t_node    *expr();        // expression
t_node    *and_or();      // || &&
t_node    *pipe_node();   // |
t_node    *redirection();    // < > << >> 
t_node    *prime();       // files, command, argument, (), built in commands: echo, cd, pwd, export, unset, env, exit
t_node    *new_node(t_token *token);
char*   type_to_string(t_type type);
int     get_last_exit_code();
void    handle_signal(int signum) ;
void    handle_heredoc_signal(int signum);
void    clear_list(t_list *list);
char    *ft_readline(char *msg);
void    add_to_addresses(void *pointer);
int ft_atoi(char *str) ;
#endif