/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:08:09 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 20:31:22 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../_headers/execution.h"

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
