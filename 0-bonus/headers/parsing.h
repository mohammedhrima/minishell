/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 02:03:28 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 02:05:05 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_H
# define PARSING_H

# include "minishell.h"

int		build_tokens(char *text);
bool	is_inclosed(char *text);
void	reset(char *text);
void	evaluate_input(char *text);
void	handle_input(char *text);

#endif
