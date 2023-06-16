/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 02:27:36 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 02:28:48 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/new.h"

t_list	*new_list(t_list *list, size_t size, t_type type)
{
	list->pos = 0;
	list->len = 100;
	list->type = type;
	list->size = size;
	if (list->type == pointers_)
		list->pointers = ft_calloc(list->len, list->size);
	if (list->type == integers_)
		list->integers = ft_calloc(list->len, list->size);
	return (list);
}

t_node	*new_node(t_token *token)
{
	t_node	*new;

	new = ft_calloc(1, sizeof(t_node));
	new->token = token;
	return (new);
}

t_token	*new_token(t_type type, char *source, int len)
{
	t_token	*new;

	new = ft_calloc(1, sizeof(t_token));
	new->type = type;
	if (source)
	{
		new->value = ft_calloc(len + 1, sizeof(char));
		ft_strncpy(new->value, source, len);
	}
	return (new);
}

t_file	*new_file(char *name, int fd, t_type type)
{
	t_file	*new;

	new = ft_calloc(1, sizeof(t_file));
	new->name = name;
	new->fd = fd;
	new->type = type;
	add_number(&global.fds, new->fd);
	return (new);
}
