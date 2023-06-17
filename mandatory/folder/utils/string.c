/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:15:51 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 04:35:12 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/utils.h"

int	ft_strlen(char *string)
{
	int	i;

	i = 0;
	while (string && string[i])
		i++;
	return (i);
}

void	ft_strncpy(char *destination, char *source, int len)
{
	int	i;

	i = 0;
	while (source[i] && i < len)
	{
		destination[i] = source[i];
		i++;
	}
}

char	*ft_strchr(char *string, int c)
{
	int	i;

	i = 0;
	if (!string)
		return (NULL);
	while (string[i])
	{
		if (string[i] == (char)c)
			return (string + i);
		i++;
	}
	if (c == 0 && string[i] == 0)
		return (string + i);
	return (NULL);
}

int	ft_strncmp(char *left, char *right, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len && left[i] && right[i] && left[i] == right[i])
		i++;
	if (i == len)
		return (0);
	return (left[i] - right[i]);
}

int	ft_strcmp(char *left, char *right)
{
	size_t	i;

	i = 0;
	while (left[i] && right[i] && left[i] == right[i])
		i++;
	return (left[i] - right[i]);
}
