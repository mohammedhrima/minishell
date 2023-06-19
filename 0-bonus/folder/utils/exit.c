/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:04:29 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 02:08:40 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/utils.h"

int	ft_exit(int code)
{
	int	i;

	close_fds();
	i = 0;
	while (i < g_global.addresses.pos)
	{
		free(g_global.addresses.pointers[i]);
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

	pids = g_global.pids.integers;
	status = 0;
	if (g_global.pids.pos > 0)
	{
		waitpid(pids[--g_global.pids.pos], &status, 0);
		res = WEXITSTATUS(status);
	}
	return (res);
}
