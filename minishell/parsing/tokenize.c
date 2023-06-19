/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 04:20:03 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 04:20:59 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../_headers/parsing.h"

t_token	*new_token(t_type type, char *source, int len)
{
	t_token	*new;

	new = ft_calloc(1, sizeof(t_token));
	new->type = type;
	if (source)
	{
		new->value = ft_calloc(len + 1, sizeof(char));
		ft_strncpy(new->value, source, len);
	}
	printf("new token with type %s value %s\n", type_to_string(type),
		new->value);
	add_pointer(&g_global.tokens, new);
	return (new);
}

char	*search_in_values(char *text)
{
	int	i;

	i = 0;
	while (g_global.values[i])
	{
		if (ft_strncmp(g_global.values[i], text,
				ft_strlen(g_global.values[i])) == 0)
		{
			new_token(g_global.types[i], text, ft_strlen(g_global.values[i]));
			return (g_global.values[i]);
		}
		i++;
	}
	return (NULL);
}

int	get_identifier(char *text, int txt_pos)
{
	int	squote;
	int	dquote;
	int	start;

	squote = 0;
	dquote = 0;
	start = txt_pos;
	while (text && text[txt_pos])
	{
		if (text[txt_pos] == '\'' && !dquote)
			squote = !squote;
		else if (text[txt_pos] == '"' && !squote)
			dquote = !dquote;
		else if (!squote && !dquote && ft_strchr("|&<>()*\t\n\f\r\v ",
				text[txt_pos]))
			break ;
		txt_pos++;
	}
	if (txt_pos > start)
		new_token(identifier_, &text[start], txt_pos - start);
	return (txt_pos);
}

void	build_tokens(char *text)
{
	int		txt_pos;
	char	*value;

	txt_pos = 0;
	while (text && text[txt_pos])
	{
		if (text[txt_pos] == '\0')
			break ;
		else if (ft_isspace(text[txt_pos]))
			txt_pos = skip_spaces(text, txt_pos);
		else
		{
			value = search_in_values(&text[txt_pos]);
			if (value)
				txt_pos += ft_strlen(value);
			else
				txt_pos = get_identifier(text, txt_pos);
		}
	}
	new_token(end_, NULL, 0);
}
