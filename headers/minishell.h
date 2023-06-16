/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:10:27 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 04:49:17 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "header.h"
# include "builtin.h"
# include "consts.h"
# include "new.h"
# include "openfile.h"
# include "remove.h"
# include "signals.h"
# include "structs.h"
# include "utils.h"
# include "nodes.h"
# include "list.h"

void	rl_replace_line(const char *text, int clear_undo);
t_node	*expr(void);
t_node	*and_or(void);
t_node	*pipe_node(void);
bool	is_redirection(t_type type);
t_node	*prime(void);
void	add_number(t_list *list, int number);
int		get_last_exit_code(void);

#endif