/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:01:09 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 06:34:42 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../_headers/utils.h"

char	*ft_itoa(int num)
{
	char	*res;
	char	*left;

	if (num < 10)
	{
		res = ft_calloc(2, sizeof(char));
		res[0] = num + '0';
		return (res);
	}
	left = ft_itoa(num / 10);
	left = ft_realloc(left, ft_strlen(left), ft_strlen(left) + 2, sizeof(char));
	left[ft_strlen(left)] = num % 10 + '0';
	return (left);
}

int	ft_atoi(char *str)
{
	int	result;
	int	sign;
	int	i;

	result = 0;
	sign = 1;
	i = 0;
	if (str == NULL)
		return (0);
	while (ft_isspace(str[i]))
		i++;
	if (ft_strchr("+-", str[i]))
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (sign * result);
}
