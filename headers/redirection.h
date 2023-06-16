/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 22:43:39 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 22:47:36 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "minishell.h"

int check_redirection_errors(t_token *token, bool is_child_process);
void check_redirection(t_file *input, t_file *output);
void do_redirection(t_type type, char *name, t_file*input, t_file *output);
bool	is_redirection(t_type type);

#endif