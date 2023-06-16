/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:25:50 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 03:43:39 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NODES_H
# define NODES_H

# include "minishell.h"

t_node	*expr(void);
t_node	*and_or(void);
t_node	*pipe_node(void);
t_node	*prime(void);
t_node	*parentses(void);
t_node	*advance(t_node *node);
int		skip(t_type type);
bool	check(t_type type, t_type types[]);
bool	is_redirection(t_type type);
int		check_token_type(t_token *token);

#endif