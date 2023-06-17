/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 20:53:49 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 00:11:15 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTE_H
# define EXECUTE_H

# include "minishell.h"

void	execute(char **arguments, t_file *input, t_file *output);
void	evaluate_command(t_node *node, t_file *input, t_file *output);
t_value	*evaluate(t_node *node, t_file *input, t_file *output);
char	*expand(char *value);
t_file	*open_heredoc(char *delimiter);

#endif