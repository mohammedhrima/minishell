/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 00:31:42 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 20:47:01 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../_headers/utils.h"

void init_global(char **envp)
{
	g_global.addresses.pointers = NULL;
	g_global.addresses.pos = 0;
	// g_global.addresses.pointer = NULL;
	g_global.values = ft_calloc(11, sizeof(char *));
	g_global.types = ft_calloc(11, sizeof(t_type));
	ft_memcpy(g_global.values, (char *[]){"<<", ">>", "<", ">", "&&", "||", "(", ")", "|", "*", 0}, 11 * sizeof(char *));
	ft_memcpy(g_global.types, (t_type[]){heredoc_, append_, redir_input, redir_output, and_, or_, lparent_, rparent_, pipe_, star_, 0}, 11 * sizeof(t_type));
	new_list(&g_global.envirement, sizeof(t_node *), pointers_);
	new_list(&g_global.tokens, sizeof(t_token *), pointers_);
	new_list(&g_global.pids, sizeof(int), integers_);
	new_list(&g_global.fds, sizeof(int), integers_);
	g_global.tokens.pos = 0;
	g_global.env = envp;
	g_global.add_to_tokens = true;
}

void	reset(char *line)
{
	while (g_global.pids.pos > 0)
		get_last_exit_code();
	free(line);
	clear_list(&g_global.tokens);
	clear_list(&g_global.pids);
	clear_list(&g_global.fds);
}