/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 02:52:09 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 03:55:25 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_H
# define PARSING_H

# include "minishell.h"

// reading input
char	*read_input(void);
bool	is_inclosed(char *text);

// building tokens
void	build_tokens(char *text);
t_token	*new_token(t_type type, char *source, int len);

// parsing
t_node	*new_node(t_token *token);
t_node	*expr_node(void);
t_node	*logic_node(void);
t_node	*pipe_node(void);
t_node	*prime_node(void);
t_node	*heredoc_node(void);
t_node	*parentses_node(void);
t_node	*advance(t_node *node);
int		skip(t_type type);
int		check_token_type(t_token *token);

#endif