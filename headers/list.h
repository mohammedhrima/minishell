/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:20:14 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 04:38:16 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIST_H
# define LIST_H

# include "minishell.h"

void	add_pointer(t_list *list, void *value);
void	add_number(t_list *list, int number);
void	clear_list(t_list *list);

#endif