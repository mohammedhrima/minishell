/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/15 21:35:20 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/15 21:35:21 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	init_global(char **envp)
{
	int	i;

	global.values = ft_calloc(11, sizeof(char *));
	global.types = ft_calloc(11, sizeof(t_type));
	ft_memcpy(global.values, (char *[]){"<<", ">>", "<", ">", "&&", "||", "(",
		")", "|", "*", 0}, 11 * sizeof(char *));
	ft_memcpy(global.types, (t_type[]){heredoc_, append_, redir_input,
		redir_output, and_, or_, lparent_, rparent_, pipe_, star_, 0}, 11
		* sizeof(t_type));
	global.env = envp;
	new_list(&global.envirement, sizeof(t_node *), pointers_);
	new_list(&global.tokens, sizeof(t_token *), pointers_);
	new_list(&global.pids, sizeof(int), integers_);
	new_list(&global.fds, sizeof(int), integers_);
	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp("PATH", envp[i], ft_strlen("PATH")) == 0)
		{
			global.path = split(envp[i], ":");
			break ;
		}
		i++;
	}
	clear_list(&global.tokens);
}

void	build_envirement(char **envp)
{
	int		i;
	char	**array;
	t_node	*node;

	i = 0;
	while (envp && envp[i])
	{
		array = split_by_two(envp[i], '=');
		node = new_node(new_token(assign_, NULL, 0));
		node->left = new_node(new_token(identifier_, NULL, 0));
		node->left->token->value = array[0];
		node->right = new_node(new_token(identifier_, NULL, 0));
		node->right->token->value = array[1];
		add_pointer(&global.envirement, node);
		i++;
	}
}

int	main(int argc, char **argv, char **envp)
{
	char	*text;

	init_global(envp);
	build_envirement(envp);
	while (argc && argv)
	{
		signal(SIGINT, handle_signal);
		signal(SIGQUIT, handle_signal);
		text = readline("minishell $> ");
		if (text)
			handle_input(text);
		else
			break ;
	}
	ft_exit(0);
}
