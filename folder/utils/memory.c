/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:05:33 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 04:35:02 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/utils.h"

void	ft_memset(void *pointer, int c, size_t len)
{
	size_t			i;
	unsigned char	*temporary;

	temporary = (unsigned char *)pointer;
	i = 0;
	while (temporary && i < len)
	{
		temporary[i] = c;
		i++;
	}
}

void	*ft_memcpy(void *destination, void *source, size_t len)
{
	size_t			i;
	unsigned char	*pointer1;
	unsigned char	*pointer2;

	if (!destination)
		return (source);
	if (!source)
		return (destination);
	pointer1 = (unsigned char *)destination;
	pointer2 = (unsigned char *)source;
	i = 0;
	while (i < len)
	{
		pointer1[i] = pointer2[i];
		i++;
	}
	return (destination);
}

void	add_to_addresses(void *pointer)
{
	void	*temporary;

	if (global.addresses.len == 0)
	{
		global.addresses.pos = 0;
		global.addresses.len = 100;
		global.addresses.pointers = malloc(global.addresses.len
				* sizeof(void *));
	}
	global.addresses.pointers[global.addresses.pos++] = pointer;
	if (global.addresses.pos + 10 > global.addresses.len)
	{
		global.addresses.len *= 2;
		temporary = malloc(global.addresses.len * sizeof(void *));
		ft_memcpy(temporary, global.addresses.pointers, global.addresses.pos
			* sizeof(void *));
		free(global.addresses.pointers);
		global.addresses.pointers = temporary;
	}
}

void	*ft_calloc(size_t count, size_t size)
{
	void			*new;
	unsigned char	*pointer;
	size_t			i;

	new = (void *)malloc(count * size);
	add_to_addresses(new);
	if (!new)
		return (NULL);
	i = 0;
	pointer = (unsigned char *)new;
	while (i < count * size)
	{
		pointer[i] = 0;
		i++;
	}
	return (new);
}

void	*ft_realloc(void *pointer, size_t oldsize, size_t newsize)
{
	void	*new;

	new = ft_calloc(1, newsize);
	if (pointer)
		ft_memcpy(new, pointer, oldsize);
	return (new);
}
