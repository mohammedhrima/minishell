/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   evaluate.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 00:07:23 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 02:49:16 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/execute.h"

void	evaluate_pipe(t_node *node, t_file *input, t_file *output)
{
	int	*fd;

	fd = open_pipe();
	add_number(&g_global.fds, fd[0]);
	add_number(&g_global.fds, fd[1]);
	evaluate(node->left, input, new_file(NULL, fd[1], redir_output));
	evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
	close(fd[0]);
	close(fd[1]);
}

t_value	*evaluate(t_node *node, t_file *input, t_file *output)
{
	t_type	type;

	type = node->token->type;
	if (is_redirection(type) || type == identifier_)
		evaluate_command(node, input, output);
	else if (type == pipe_)
		evaluate_pipe(node, input, output);
	return (NULL);
}
