/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 01:47:11 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 01:49:21 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/parsing.h"

bool	error_left_parent(char *text, int *ptr, int *is_parent_ptr)
{
	int	i;

	i = *ptr;
	(*is_parent_ptr)++;
	i++;
	while (ft_isspace(text[i]))
		i++;
	if (text[i] == ')')
		return (true);
	i--;
	*ptr = i;
	return (false);
}

bool	error_right_parent(char *text, int *ptr, int *is_parent_ptr)
{
	if (*ptr == 0)
		return (true);
	if (text[*ptr])
		(*is_parent_ptr)--;
	return (false);
}

void	skip_space(char *text, int *ptr)
{
	while (ft_isspace(text[*ptr]))
		(*ptr)++;
}

void	check_quote_and_operator(char c, int *quote_ptr, int *is_operator_ptr)
{
	if (ft_strchr("\"\'", c) && (*quote_ptr) == 0)
		(*quote_ptr) = c;
	else if (c == (*quote_ptr))
		(*quote_ptr) = 0;
	else if (ft_strchr("&|", c))
		(*is_operator_ptr) = 1;
	else if (c && c != '\n')
		(*is_operator_ptr) = 0;
}

bool	is_inclosed(char *text)
{
	int	quote;
	int	is_operator;
	int	is_parent;
	int	i;

	quote = 0;
	is_operator = 0;
	is_parent = 0;
	i = 0;
	while (text && text[i])
	{
		skip_space(text, &i);
		if (
			(text[i] == '(' && error_left_parent(text, &i, &is_parent))
			|| (text[i] == ')' && error_right_parent(text, &i, &is_parent)))
			return (true);
		else
			check_quote_and_operator(text[i], &quote, &is_operator);
		i++;
	}
	if (quote || is_operator || is_parent)
		return (true);
	return (false);
}
