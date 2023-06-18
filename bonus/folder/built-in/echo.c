/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:07:55 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 03:12:59 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/builtin.h"

void	echo_func(char **arguments)
{
	int	print_new_line;
	int	i;

	if (ft_strcmp(arguments[0], "-n") == 0)
	{
		print_new_line = 0;
		i = 1;
	}
	else
	{
		print_new_line = 1;
		i = 0;
	}
	while (arguments && arguments[i])
	{
		write(OUT, arguments[i], ft_strlen(arguments[i]));
		write(OUT, " ", ft_strlen(" "));
		i++;
	}
	if (print_new_line)
		write(OUT, "\n", 1);
}
