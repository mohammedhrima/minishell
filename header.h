#ifndef HEADER_H
#define HEADER_H

# include <dirent.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>

// error messages
# define SUCCESS 0
# define NO_SUCH_FILE_OR_DIR 1
# define PERMISSION_DENIED 1
# define AMBIGIOUS_REDIRECTION 1
# define LIMIT_HEREDOC 2
# define UNEXPECTED_ERROR 258
# define COMMAND_NOT_FOUND 127
# define SYNTAX_ERROR 258
# define ERROR -1

// file descriptors
# define NOT_OPENED -200
# define IN STDIN_FILENO
# define OUT STDOUT_FILENO
# define ERR STDERR_FILENO

// signals
# define CTRL_C 2
# define CTRL_SLASH 3

// structs, enums
typedef enum    Type    Type;
typedef struct  Token	Token;
typedef struct  Token	Value;
typedef struct  Node	Node;
typedef struct  File	File;
typedef struct  List	List;

enum    Type
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
	single_quote_,
	double_quote_,
	end_
};

struct  Token
{
	Type    type;
	char    *content;
	int     start;
	int     len;
};

struct  Node
{
	Node    *left;
	Node    *right;
	Token   *token;
};

struct File
{
    char    *name;
    int     fd;
    Type type;
};

struct  List
{
    Type    type;
    union
    {
        void    **pointers;
        int     *integers;
    };
    size_t  size;
    int     pos;
    int     len;
};

struct
{
    List    addresses;
    List    envirement;
    List    tokens;
    List    fds;
    List    pids;
    bool    add_to_tokens;
    // int     inside_pipe;
    // comment it because It could be updated everytime
    // rebuild env form envirement List;
    // char    **env; 
    char    **values;
    Type   *types;
} Global;

// utils
void    ft_memset(void *pointer, int c, size_t len);
void	*ft_memcpy(void *destination, void *source, size_t len);
void	*ft_calloc(size_t count, size_t size);
void	*ft_realloc(void *pointer, size_t oldlen, size_t newlen, size_t size);
int     ft_strlen(char *string);
void	ft_strncpy(char *destination, char *source, int len);
char	*ft_strchr(char *string, int c);
int     ft_strncmp(char *left, char *right, size_t len);
int     ft_strcmp(char *left, char *right);
char    **split(char*string, char *spliter);
bool	ft_isspace(int c);
int		new_child_process(void);
void	ft_exit(int code);
char	*open_heredoc(char *delimiter);
void	add_pointer(List *list, void *value);
void	add_number(List *list, int number);
void	clear_list(List *list);
bool	is_redirection(Type type);
char	*strjoin(char *string1, char *string2, char *string3);
char	**split_by_two(char *str, char c);
char	*ft_itoa(int num);
int		ft_atoi(char *str);
char	*ft_strdup(char *string, int len);
char	*ft_strnstr(char *string, char *to_find);
bool	ft_isalpha(int c);
char	*expand(char *content);
#endif
