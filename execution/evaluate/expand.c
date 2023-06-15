/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/15 01:32:20 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/15 01:36:26 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "evaluate.h"

char	*get_var(char *name)
{
	int		i;
	t_node	**nodes;

	nodes = (t_node **)global.envirement.pointers;
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

int	is_quote(char c, char *res, int *single_quotes_ptr, int *double_quotes_ptr)
{
	if (c == '\'')
	{
		if (!(*double_quotes_ptr))
			(*single_quotes_ptr) = !(*single_quotes_ptr);
		else
		{
			res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
			res[ft_strlen(res)] = c;
		}
		return (1);
	}
	else if (c == '"')
	{
		if (!(*single_quotes_ptr))
			(*double_quotes_ptr) = !(*double_quotes_ptr);
		else
		{
			res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
			res[ft_strlen(res)] = c;
		}
		return (1);
	}
	return (0);
}

char	*is_expandable(char *value, char *res, int *ptr)
{
	int		i;
	int		j;
	char	*var;
	char	*to_add;

	i = *ptr;
	var = NULL;
	i++;
	j = i;
	while (value[j] && !ft_strchr(" \"", value[j]))
		j++;
	var = ft_calloc(j - i + 1, sizeof(char));
	ft_strncpy(var, value + i, j - i);
	to_add = get_var(var);
	res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + ft_strlen(to_add)
			+ 2);
	ft_strncpy(res + ft_strlen(res), to_add, ft_strlen(to_add));
	i = j;
	*ptr = i;
	return (res);
}

char	*new_res(char *res, char *value, int *ptr)
{
	res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
	res[ft_strlen(res)] = value[(*ptr)++];
	return (res);
}

char	*expand(char *value)
{
	int		i;
	int		single_quotes;
	int		double_quotes;
	char	*res;

	res = "";
	i = 0;
	single_quotes = 0;
	double_quotes = 0;
	while (value && value[i])
	{
		i += is_quote(value[i], res, &single_quotes, &double_quotes);
		if (value[i] == '$')
		{
			if (double_quotes || !single_quotes)
				res = is_expandable(value, res, &i);
			else if (single_quotes)
				res = new_res(res, value, &i);
		}
		else
			res = new_res(res, value, &i);
	}
	return (res);
}
