/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   evaluate.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/15 01:37:16 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/15 02:02:44 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVALUATE_H
# define EVALUATE_H

# include "../../header.h"

char	*get_var(char *name);
int		is_quote(char c, char *res, int *single_quotes_ptr,
			int *double_quotes_ptr);
char	*is_expandable(char *value, char *res, int *ptr);
char	*new_res(char *res, char *value, int *ptr);
char	*expand(char *value);
t_file	*open_heredoc(char *delimiter, t_file *input);
int		skip(t_type type);
bool	check(t_type type, t_type types[]);
t_node	*expr(void);
t_node	*and_or(void);
t_node	*pipe_node(void);
bool	is_redirection(t_type type);
t_node	*prime(void);
t_node	*parentses(void);
t_node	*advance(t_node *node);
void	open_input(t_file *file);
void	open_output(t_file *file);
void	open_append(t_file *file);
void	open_file(t_file *file);
char	*get_command_path(char *cmd);
int		get_last_exit_code(void);
t_value	*evaluate(t_node *node, t_file *input, t_file *output);
void	call_built_in_function(void (*func)(char **), char **arguments,
			t_file *input, t_file *output);
char	*get_command_path(char *cmd);
int		get_last_exit_code(void);
void	do_redirection(t_file *input, t_file *output);
void	call_built_in_function(void (*func)(char **), char **arguments,
			t_file *input, t_file *output);
void	execute_command(t_file *input, t_file *output, char **arguments);
void	open_parenteses(t_node *node, t_file *input, t_file *output);
int skip(t_type type);
bool check(t_type type, t_type types[]);
bool is_redirection(t_type type);
int check_token_type(t_token *token);
#endif