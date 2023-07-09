/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:07:34 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 04:35:43 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/utils.h"

int	get_start_and_end(int *start_ptr, int i, char *str, char *spliter)
{
	while (ft_strncmp(str + i, spliter, ft_strlen(spliter)) == 0)
		i += ft_strlen(spliter);
	*start_ptr = i;
	while (str[i] && ft_strncmp(str + i, spliter, ft_strlen(spliter)) != 0)
		i++;
	return (i);
}

char	**allocate_res(char **res, int j)
{
	if (res == NULL)
		res = ft_calloc(j + 2, sizeof(char *));
	else
		res = ft_realloc(res, j * sizeof(char *), (j + 2) * sizeof(char *));
	return (res);
}

char	**split(char *str, char *spliter)
{
	int		i;
	int		j;
	char	**res;
	int		start;

	i = 0;
	j = 0;
	res = NULL;
	start = 0;
	if (ft_strncmp(str + i, spliter, ft_strlen(spliter)) == 0)
		i += ft_strlen(spliter);
	start = i;
	while (str && str[i])
	{
		i = get_start_and_end(&start, i, str, spliter);
		res = allocate_res(res, j);
		res[j] = ft_calloc(i - start + 2, sizeof(char));
		ft_strncpy(res[j], str + start, i - start);
		j++;
		res[j] = NULL;
	}
	return (res);
}

char	**split_by_two(char *str, char c)
{
	int		i;
	char	**res;

	i = 0;
	res = ft_calloc(2, sizeof(char *));
	while (str && str[i])
	{
		if (str[i] == c)
		{
			res[0] = ft_calloc(i + 1, sizeof(char));
			ft_strncpy(res[0], str, i);
			i++;
			res[1] = ft_calloc(ft_strlen(str) - i + 1, sizeof(char));
			ft_strncpy(res[1], str + i, ft_strlen(str) - i);
			break ;
		}
		i++;
	}
	return (res);
}
