/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 19:33:48 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 03:48:27 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/execute.h"

char	*get_var(char *name)
{
	int		i;
	t_node	**nodes;

	nodes = (t_node **)g_global.envirement.pointers;
	i = 0;
	if (ft_strcmp(name, "?") == 0)
		return (ft_itoa(get_last_exit_code()));
	while (nodes && nodes[i])
	{
		if (nodes[i]->left->token->value
			&& ft_strcmp(nodes[i]->left->token->value, name) == 0)
			return (nodes[i]->right->token->value);
		i++;
	}
	return ("");
}

int	update_res(char **res_ptr, char c)
{
	char	*res;

	res = *res_ptr;
	res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
	res[ft_strlen(res)] = c;
	*res_ptr = res;
	return (1);
}

int	check_quotes(char c, int *double_quotes_ptr, int *single_quotes_ptr)
{
	if (c == '\'' && !(*double_quotes_ptr))
	{
		(*single_quotes_ptr) = !(*single_quotes_ptr);
		return (1);
	}
	else if (c == '\"' && !(*single_quotes_ptr))
	{
		(*double_quotes_ptr) = !(*double_quotes_ptr);
		return (1);
	}
	return (0);
}

int	expand_var(int start, char **res_ptr, char *value)
{
	int		end;
	char	*var;
	char	*to_add;
	char	*res;

	res = *res_ptr;
	start++;
	end = start;
	while (value[end] && !ft_strchr(" \"\'$", value[end]))
		end++;
	var = ft_calloc(end - start + 1, sizeof(char));
	ft_strncpy(var, value + start, end - start);
	to_add = get_var(var);
	res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + ft_strlen(to_add)
			+ 2);
	ft_strncpy(res + ft_strlen(res), to_add, ft_strlen(to_add));
	*res_ptr = res;
	return (end);
}

char	*expand(char *value)
{
	char	*res;
	int		i;
	int		single_quotes;
	int		double_quotes;

	res = "";
	i = 0;
	single_quotes = 0;
	double_quotes = 0;
	while (value && value[i])
	{
		if (check_quotes(value[i], &double_quotes, &single_quotes))
			i++;
		else if (value[i] == '$')
		{
			if (double_quotes || !single_quotes)
				i = expand_var(i, &res, value);
			else if (single_quotes)
				i += update_res(&res, value[i]);
		}
		else
			i += update_res(&res, value[i]);
	}
	return (res);
}
