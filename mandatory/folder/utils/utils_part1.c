/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_part1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:16:32 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 02:41:33 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/utils.h"

char	*type_to_string(t_type type)
{
	char	**values;
	t_type	*types;
	int		i;

	values = (char *[]){"=", "identifier", "<", ">", "<<", ">>", "|", "env",
		"echo", "cd", "pwd", "export", "unset", "exit", "end", NULL};
	types = (t_type[]){assign_, identifier_, redir_input, redir_output,
		heredoc_, append_, pipe_, env_, echo_, cd_, pwd_, export_, unset_,
		exit_, end_, 0};
	i = 0;
	while (types[i])
	{
		if (types[i] == type)
			return (values[i]);
		i++;
	}
	return (NULL);
}

int	ft_isspace(int c)
{
	return (c == '\t' || c == '\n' || c == '\f' || c == '\r' || c == '\v'
		|| c == ' ');
}

char	*strjoin(char *string1, char *string2, char *string3)
{
	char	*res;

	res = ft_calloc(ft_strlen(string1) + ft_strlen(string2) + ft_strlen(string3)
			+ 1, sizeof(char));
	if (string1)
		ft_strncpy(res, string1, ft_strlen(string1));
	if (string2)
		ft_strncpy(res + ft_strlen(res), string2, ft_strlen(string2));
	if (string3)
		ft_strncpy(res + ft_strlen(res), string3, ft_strlen(string3));
	return (res);
}

char	*ft_strdup(char *string)
{
	char	*pointer;
	int		i;

	if (!string)
		return (NULL);
	pointer = ft_calloc((ft_strlen(string) + 1), sizeof(char));
	if (!pointer)
		return (NULL);
	i = 0;
	while (string && string[i])
	{
		pointer[i] = string[i];
		i++;
	}
	pointer[i] = '\0';
	return (pointer);
}
