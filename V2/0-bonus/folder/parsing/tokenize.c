/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 07:40:00 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/18 22:57:37 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/parsing.h"

t_type	search_in_values(int *txt_pos_ptr, char *text)
{
	t_type	type;
	int		start;
	int		txt_pos;
	int		i;

	type = 0;
	txt_pos = *txt_pos_ptr;
	i = 0;
	while (g_global.values[i])
	{
		if (ft_strncmp(g_global.values[i], &text[txt_pos],
				ft_strlen(g_global.values[i])) == 0)
		{
			start = txt_pos;
			type = g_global.types[i];
			txt_pos += ft_strlen(g_global.values[i]);
			break ;
		}
		i++;
	}
	if (type)
		add_pointer(&g_global.tokens, new_token(type, text + start, txt_pos
				- start));
	*txt_pos_ptr = txt_pos;
	return (type);
}

int	create_identifier(int txt_pos, char *text)
{
	int	start;
	int	single_quote;
	int	double_quote;

	start = txt_pos;
	single_quote = 0;
	double_quote = 0;
	while (text[txt_pos])
	{
		if (text[txt_pos] == '\'' && !double_quote)
			single_quote = !single_quote;
		else if (text[txt_pos] == '"' && !single_quote)
			double_quote = !double_quote;
		else if (!single_quote && !double_quote && (ft_isspace(text[txt_pos])
				|| ft_strchr("|&<>()*", text[txt_pos])))
			break ;
		txt_pos++;
	}
	add_pointer(&g_global.tokens, new_token(identifier_, text + start, txt_pos
			- start));
	return (txt_pos);
}

int	build_tokens(char *text)
{
	int	txt_pos;

	txt_pos = 0;
	while (text && text[txt_pos])
	{
		if (text[txt_pos] == '\0')
			break ;
		else if (ft_isspace(text[txt_pos]))
		{
			while (ft_isspace(text[txt_pos]))
				txt_pos++;
		}
		else if (search_in_values(&txt_pos, text) == 0)
			txt_pos = create_identifier(txt_pos, text);
	}
	add_pointer(&g_global.tokens, new_token(end_, NULL, 0));
	return (SUCCESS);
}
