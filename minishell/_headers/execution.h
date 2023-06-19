/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 02:50:13 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 20:32:27 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTION_H
# define EXECUTION_H

# include "minishell.h"

char *open_heredoc(char *delimiter);
t_value *evaluate(t_node *node, t_file *input, t_file *output);
void	open_file(t_file *file);
void	cd_func(char **arguments);
void	echo_func(char **arguments);
void	env_func(char **arguments);
void	exit_func(char **arguments);
void    export_func(char **arguments);
void	pwd_func(char **arguments);
void	unset_func(char **argument);
void	*get_built_in_function(char *cmd);
#endif