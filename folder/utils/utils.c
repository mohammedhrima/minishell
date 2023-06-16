/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:16:32 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 22:29:54 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/utils.h"

int ft_isspace(int c)
{
	return (c == '\t' || c == '\n' || c == '\f' || c == '\r' || c == '\v' || c == ' ');
}

char *strjoin(char *string1, char *string2, char *string3)
{
	char *res;

	res = ft_calloc(ft_strlen(string1) + ft_strlen(string2) + ft_strlen(string3) + 1, sizeof(char));
	if (string1)
		ft_strncpy(res, string1, ft_strlen(string1));
	if (string2)
		ft_strncpy(res + ft_strlen(res), string2, ft_strlen(string2));
	if (string3)
		ft_strncpy(res + ft_strlen(res), string3, ft_strlen(string3));
	return (res);
}

char *ft_strdup(char *string)
{
	char *pointer;
	int i;

	if (!string)
		return (NULL);
	pointer = ft_calloc((ft_strlen(string) + 1), sizeof(char));
	if (!pointer)
		return (NULL);
	i = 0;
	while (string && string[i])
	{
		pointer[i] = string[i];
		i++;
	}
	pointer[i] = '\0';
	return (pointer);
}

int create_child(void)
{
	int pid;

	pid = fork();
	if (pid < 0)
	{
		ft_printf(OUT, "Error forking\n");
		ft_exit(ERROR);
	}
	add_number(&global.pids, pid);
	return pid;
}

int *open_pipe(void)
{
	int *fd;

	fd = ft_calloc(2, sizeof(int));
	if (pipe(fd))
	{
		ft_printf(OUT, "Error openning pipe\n");
		ft_exit(ERROR);
	}
	return fd;
}

void close_fds(void)
{
	int i;
	i = 0;
	while (i < global.fds.pos)
	{
		if (global.fds.integers[i] != IN && global.fds.integers[i] != OUT)
			close(global.fds.integers[i]);
		i++;
	}
}