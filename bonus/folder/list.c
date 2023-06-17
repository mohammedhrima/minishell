/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:17:35 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 03:20:50 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/list.h"

void	add_pointer(t_list *list, void *value)
{
	list->pointers[list->pos++] = value;
	if (list->pos + 10 > list->len)
	{
		list->len *= 2;
		list->pointers = ft_realloc(list->pointers, list->pos * list->size,
				list->len * list->size);
	}
}

void	add_number(t_list *list, int number)
{
	list->integers[list->pos++] = number;
	if (list->pos + 10 > list->len)
	{
		list->len *= 2;
		list->integers = ft_realloc(list->integers, list->pos * list->size,
				list->len * list->size);
	}
}

void	clear_list(t_list *list)
{
	t_type	type;

	type = list->type;
	if (type == integers_)
		ft_memset(list->integers, 0, list->len * list->size);
	else if (type == pointers_)
		ft_memset(list->pointers, 0, list->len * list->size);
	list->pos = 0;
}
