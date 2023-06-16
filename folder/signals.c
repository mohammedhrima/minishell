/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:07:09 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 03:07:12 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/signals.h"

void	handle_signal(int signum)
{
	if (signum == CTRL_C)
	{
		printf("\n");
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
	if (signum == CTRL_SLASH)
	{
		printf("");
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
}

void	handle_heredoc_signal(int signum)
{
	if (signum == CTRL_C)
	{
		printf("\n");
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
		ft_exit(SUCCESS);
	}
	if (signum == CTRL_SLASH)
	{
		printf("");
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
}
