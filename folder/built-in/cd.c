/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:07:51 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 03:13:02 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/builtin.h"

void	cd_func(char **arguments)
{
	if (arguments[0])
		chdir(arguments[0]);
	else
		chdir(getenv("HOME"));
}
