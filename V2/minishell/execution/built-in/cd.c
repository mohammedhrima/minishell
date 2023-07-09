/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:07:51 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 20:30:54 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../_headers/execution.h"

void	cd_func(char **arguments)
{
	// change PWD envirement variable
	if (arguments[0])
		chdir(arguments[0]);
	else
		chdir(getenv("HOME"));
}
