/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 02:00:19 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 04:29:23 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../_headers/utils.h"

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

int	skip_spaces(char *text, int txt_pos)
{
	while (ft_isspace(text[txt_pos]))
		txt_pos++;
	return (txt_pos);
}

t_file	*new_file(char *name, int fd, t_type type)
{
	t_file	*new;

	new = ft_calloc(1, sizeof(t_file));
	new->name = name;
	new->fd = fd;
	new->type = type;
	add_number(&g_global.fds, new->fd);
	return (new);
}
