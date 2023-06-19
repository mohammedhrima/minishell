/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 22:12:40 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 06:59:45 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "_headers/minishell.h"

void build_envirement(char **envp)
{
	int i;
	char **array;
	t_node *node;

	i = 0;
	while (envp && envp[i])
	{
		array = split_by_two(envp[i], '=');
		node = new_node(new_token(assign_, NULL, 0));
		node->left = new_node(new_token(identifier_, NULL, 0));
		node->left->token->value = array[0];
		node->right = new_node(new_token(identifier_, NULL, 0));
		node->right->token->value = array[1];
		add_pointer(&g_global.envirement, node);
		i++;
	}
	g_global.tokens.pos = 0;
}

int main(int argc, char **argv, char **envp)
{
	char *line;
	t_token **tokens;
	t_node *node;
	t_file *input;
	t_file *output;

	struct termios	term;

	tcgetattr(0, &term);
	term.c_cc[VQUIT] = _POSIX_VDISABLE;
	term.c_lflag &= ~ECHOCTL;
	tcsetattr(0, TCSANOW, &term);
	signal(SIGINT, handle_signal);
	
	// test heredoc
	init_global(envp);
	build_envirement(envp);
	while (argc && argv)
	{
		input = new_file(NULL, IN, 0);
		output = new_file(NULL, OUT, 0);
		line = readline(("minishell $> "));
		if (line == NULL)
			break;
		add_history(line);
		if (is_inclosed(line))
			printf("minishell: syntax error\n");
		else
		{
			// printf("start tokenizing\n");
			build_tokens(line);
			// printf("finish tokenizing in pos: %d\n", g_global.tokens.pos);
			g_global.tokens.pos = 0;
			tokens = (t_token **)g_global.tokens.pointers;
			printf("start executing from pos: %d\n", g_global.tokens.pos);
			while (tokens[g_global.tokens.pos]->type != end_)
			{
				printf("loop\n");
				node = expr_node(); // add evaluate
				if (node == NULL)
					break;
				evaluate(node, input, output);
			}
			printf("finish executing\n");
		}
		reset(line);
	}
}
