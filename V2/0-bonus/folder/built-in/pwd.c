/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:08:09 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 03:12:46 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/builtin.h"

void	pwd_func(char **arguments)
{
	char	*new;

	if (arguments || arguments == NULL)
	{
		new = getcwd(NULL, 0);
		printf("%s\n", new);
		free(new);
	}
}
