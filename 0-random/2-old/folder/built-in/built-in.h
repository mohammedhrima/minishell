/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built-in.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/15 21:41:25 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/15 21:42:29 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILT_IN_H
#define BUILT_IN_H

#include "../../header.h"
void	echo_func(char **arguments);
void	cd_func(char **arguments);
void	pwd_func(char **arguments);
void	new_envirement_variable(char **arguments);
void	export_func(char **arguments);
void	unset_func(char **argument);
void	env_func(char **arguments);
void	exit_func(char **arguments);
void	*get_built_in_function(char *cmd);

#endif
