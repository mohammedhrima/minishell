/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 02:11:55 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 03:43:35 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NEW_H
# define NEW_H

# include "structs.h"
# include "minishell.h"

t_list	*new_list(t_list *list, size_t size, t_type type);
t_node	*new_node(t_token *token);
t_token	*new_token(t_type type, char *source, int len);
t_file	*new_file(char *name, int fd, t_type type);

#endif