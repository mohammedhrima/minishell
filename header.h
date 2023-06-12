/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/02 13:20:36 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/12 01:02:51 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H
#define HEADER_H

#include <dirent.h>
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// error messages
#define SUCCESS 0
#define NO_SUCH_FILE_OR_DIR 1
#define PERMISSION_DENIED 1
#define AMBIGIOUS_REDIRECTION 1
#define LIMIT_HEREDOC 2
#define UNEXPECTED_ERROR 258
#define COMMAND_NOT_FOUND 127
#define SYNTAX_ERROR 258
#define ERROR -1

// file descriptors
#define NOT_OPENED -200
#define IN STDIN_FILENO
#define OUT STDOUT_FILENO
#define ERR STDERR_FILENO

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
enum e_type
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

struct s_token
{
	t_type type;
	char *value;
	int start;
	int len;
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

// when get freed,make sure to free left and right with there token
// handle if path is NULL
struct s_global
{
	t_list addresses;
	t_list envirement;
	t_list tokens;
	t_list fds;
	t_list pids;
	char **path;
	int inside_pipe;
	char **env;
	char **values;
	t_type *types;
};
t_global global;

void ft_printf(int file_descriptor, char *fmt, ...);
int ft_isspace(int c);
void *ft_calloc(size_t count, size_t size);
void ft_strncpy(char *destination, char *source, int len);
void ft_strcpy(char *destination, char *source);
char **split_by_two(char *string, char c);
void ft_memset(void *pointer, int c, size_t len);
void *ft_memcpy(void *destination, void *source, size_t len);
char *ft_strchr(char *string, int c);
int ft_strncmp(char *left, char *right, size_t len);
int ft_strcmp(char *left, char *right);
int ft_strlen(char *string);
char *strjoin(char *string1, char *string2, char *string3);
char *ft_strdup(char *string);
char *ft_readline(char *text);
char *type_to_string(t_type type);
void add_pointer(t_list *list, void *value);
void add_number(t_list *list, int number);
char *ft_itoa(int num);
void *ft_realloc(void *pointer, size_t old_size, size_t new_size);
int ft_atoi(char *str);
char *ft_itoa(int num);
void ft_exit(int code);
t_node *expr(void);
t_node *and_or(void);
t_node *pipe_node(void);
t_node *redirection(void);
t_node *prime(void);
t_node *new_node(t_token *token);
int get_last_exit_code(void);
char **split(char *string, char *spliter);
void add_to_addresses(void *pointer);

#endif