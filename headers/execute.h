/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 20:53:49 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 22:55:31 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTE_H
#define EXECUTE_H

#include "minishell.h"
t_value *evaluate(t_node *node, t_file *input, t_file *output);
void execute(char **arguments, t_file *input, t_file *output);

#endif