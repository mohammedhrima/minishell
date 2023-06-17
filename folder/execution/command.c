/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 23:53:38 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 00:12:47 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/execute.h"

char	**expand_star(int *pos_ptr, int *len_ptr, char **arguments)
{
	t_token			*token;
	char			*new;
	DIR				*curr_dir;
	struct dirent	*content;

	(*pos_ptr)++;
	token = ((t_token **)global.tokens.pointers)[(*pos_ptr)];
	new = getcwd(NULL, 0);
	curr_dir = opendir(new);
	free(new);
	content = readdir(curr_dir);
	while (content)
	{
		if (content->d_name[0] != '.')
		{
			arguments = ft_realloc(arguments, (*len_ptr) * sizeof(char *),
					(*len_ptr + 2) * sizeof(char *));
			arguments[(*len_ptr)++] = ft_strdup(content->d_name);
			arguments[(*len_ptr)] = NULL;
		}
		content = readdir(curr_dir);
	}
	closedir(curr_dir);
	return (arguments);
}

char	**update_arguments(char **arguments, t_token *token, int *len_ptr,
		int *pos_ptr)
{
	char	*argument;
	int		len;
	int		pos;

	len = *len_ptr;
	pos = *pos_ptr;
	arguments = ft_realloc(arguments, len * sizeof(char *), (len + 2)
			* sizeof(char *));
	if (token->type == star_)
		arguments = expand_star(&pos, &len, arguments);
	else
	{
		argument = expand(token->value);
		arguments[len++] = argument;
	}
	arguments[len] = NULL;
	*len_ptr = len;
	*pos_ptr = pos;
	return (arguments);
}

char	**build_arguments(int *there_is_redirection_ptr, t_node *node,
		t_file *input, t_file *output)
{
	t_token	*token;
	char	**arguments;
	int		i;
	int		len;
	int		pos;

	i = 0;
	len = 0;
	pos = node->token->start;
	while (i < node->token->len)
	{
		token = ((t_token **)global.tokens.pointers)[pos];
		pos++;
		if (token->type == identifier_ || token->type == star_)
			arguments = update_arguments(arguments, token, &len, &pos);
		else if (is_redirection(token->type))
		{
			*there_is_redirection_ptr = 1;
			if (handle_redirection(&pos, token, input, output))
				return (NULL);
			i++;
		}
		i++;
	}
	return (arguments);
}

void	evaluate_command(t_node *node, t_file *input, t_file *output)
{
	char	**arguments;
	int		there_is_redirection;

	arguments = NULL;
	there_is_redirection = 0;
	arguments = build_arguments(&there_is_redirection, node, input, output);
	if (arguments && arguments[0])
		execute(arguments, input, output);
	else if (there_is_redirection)
	{
		if (create_child() == 0)
		{
			check_redirection(input, output);
			ft_exit(SUCCESS);
		}
	}
}
