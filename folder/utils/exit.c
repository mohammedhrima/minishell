/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:04:29 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 04:34:57 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/utils.h"

int	ft_exit(int code)
{
	int	i;

	i = 0;
	while (i < global.fds.pos)
	{
		if (global.fds.integers[i] != OUT && global.fds.integers[i] != IN)
			close(global.fds.integers[i]);
		i++;
	}
	i = 0;
	while (i < global.addresses.pos)
	{
		free(global.addresses.pointers[i]);
		i++;
	}
	exit(code);
	return (code);
}

int	get_last_exit_code(void)
{
	int			*pids;
	int			status;
	static int	res;

	pids = global.pids.integers;
	status = 0;
	if (global.pids.pos > 0)
	{
		waitpid(pids[--global.pids.pos], &status, 0);
		res = WEXITSTATUS(status);
	}
	return (res);
}
