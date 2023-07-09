/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:44:06 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 20:15:21 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../_headers/execution.h"

void	open_input(t_file *file)
{
	if (access(file->name, F_OK))
	{
		printf("minishell: %s No such file or directory\n", file->name);
		ft_exit(NO_SUCH_FILE_OR_DIR);
	}
	else if (access(file->name, R_OK))
	{
		printf("minishell: %s Permission denied\n", file->name);
		ft_exit(PERMISSION_DENIED);
	}
	else
		file->fd = open(file->name, O_RDONLY);
}

void	open_output(t_file *file)
{
	if (access(file->name, F_OK) == 0 && access(file->name, W_OK))
	{
		printf("minishell: %s Permission denied\n", file->name);
		ft_exit(PERMISSION_DENIED);
	}
	else
		file->fd = open(file->name, O_WRONLY | O_CREAT | O_TRUNC);
}

void	open_append(t_file *file)
{
	if (access(file->name, F_OK) == 0 && access(file->name, W_OK))
	{
		printf("minishell: %s Permission denied\n", file->name);
		ft_exit(PERMISSION_DENIED);
	}
	else
		file->fd = open(file->name, O_WRONLY | O_CREAT | O_APPEND);
}

void	open_file(t_file *file)
{
	if (file->fd == NOT_OPENED)
	{
		if (file->type == redir_input)
			open_input(file);
		if (file->type == redir_output && file->name)
			open_output(file);
		if (file->type == append_ && file->name)
			open_append(file);
	}
}
