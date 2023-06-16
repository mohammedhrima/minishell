/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:16:32 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 04:35:16 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/utils.h"

int	ft_isspace(int c)
{
	return (c == '\t' || c == '\n' || c == '\f' || c == '\r' || c == '\v'
		|| c == ' ');
}

char	*strjoin(char *string1, char *string2, char *string3)
{
	char	*res;

	res = ft_calloc(ft_strlen(string1) + ft_strlen(string2) + ft_strlen(string3)
			+ 1, sizeof(char));
	if (string1)
		ft_strncpy(res, string1, ft_strlen(string1));
	if (string2)
		ft_strncpy(res + ft_strlen(res), string2, ft_strlen(string2));
	if (string3)
		ft_strncpy(res + ft_strlen(res), string3, ft_strlen(string3));
	return (res);
}
