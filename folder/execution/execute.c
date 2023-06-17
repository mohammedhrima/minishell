/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 23:05:24 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 23:05:46 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/execute.h"

char	*get_command_path(char *cmd)
{
	char	*res;
	int		i;

	if (cmd == NULL || ft_strchr(cmd, '/') || ft_strlen(cmd) == 0)
		return (cmd);
	i = 0;
	while (global.path && global.path[i])
	{
		res = strjoin(global.path[i], "/", cmd);
		if (access(res, F_OK) == 0 && access(res, X_OK) == 0)
			return (res);
		i++;
	}
	return (cmd);
}

void	call_func(void (*func)(char **), char **args, t_file *in, t_file *out)
{
	if (global.inside_pipe)
	{
		if (create_child() == 0)
		{
			check_redirection(in, out);
			func(&args[1]);
			ft_exit(SUCCESS);
		}
	}
	else
	{
		check_redirection(in, out);
		func(&args[1]);
		if (create_child() == 0)
			ft_exit(SUCCESS);
	}
}

void	execute_command(char **arguments, t_file *input, t_file *output)
{
	char	*argument;

	argument = get_command_path(arguments[0]);
	if (create_child() == 0)
	{
		check_redirection(input, output);
		close_fds();
		if (execve(argument, arguments, global.env) < 0)
		{
			if (ft_strchr(argument, '/'))
				printf("minishell: '%s' no such file or directory\n", argument);
			else
				printf("minishell: '%s' command not found\n", argument);
			ft_exit(COMMAND_NOT_FOUND);
		}
	}
}

void	execute(char **arguments, t_file *input, t_file *output)
{
	void	(*func)(char **);

	func = get_built_in_function(arguments[0]);
	if (func)
		call_func(func, arguments, input, output);
	else
		execute_command(arguments, input, output);
}
