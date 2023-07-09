/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_built_in.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:08:06 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 20:31:18 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../_headers/execution.h"

void	*get_built_in_function(char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0)
		return (&echo_func);
	if (ft_strcmp(cmd, "cd") == 0)
		return (&cd_func);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (&pwd_func);
	if (ft_strcmp(cmd, "export") == 0)
		return (&export_func);
	if (ft_strcmp(cmd, "unset") == 0)
		return (&unset_func);
	if (ft_strcmp(cmd, "env") == 0)
		return (&env_func);
	if (ft_strcmp(cmd, "exit") == 0)
		return (&exit_func);
	return (NULL);
}
