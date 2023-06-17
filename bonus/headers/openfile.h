/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   openfile.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:43:13 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 04:52:48 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OPENFILE_H
# define OPENFILE_H

# include "minishell.h"

void	open_input(t_file *file);
void	open_output(t_file *file);
void	open_append(t_file *file);
void	open_file(t_file *file);

#endif