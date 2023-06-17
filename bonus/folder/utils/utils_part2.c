/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_part2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 02:00:19 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 02:09:18 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/utils.h"

int	create_child(void)
{
	int	pid;

	pid = fork();
	if (pid < 0)
	{
		printf("Error forking\n");
		ft_exit(ERROR);
	}
	add_number(&g_global.pids, pid);
	return (pid);
}

int	*open_pipe(void)
{
	int	*fd;

	fd = ft_calloc(2, sizeof(int));
	if (pipe(fd))
	{
		printf("Error openning pipe\n");
		ft_exit(ERROR);
	}
	return (fd);
}

void	close_fds(void)
{
	int	i;

	i = 0;
	while (i < g_global.fds.pos)
	{
		if (g_global.fds.integers[i] != IN && g_global.fds.integers[i] != OUT)
			close(g_global.fds.integers[i]);
		i++;
	}
}
