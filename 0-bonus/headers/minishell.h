/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:10:27 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 01:55:48 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "header.h"
# include "builtin.h"
# include "consts.h"
# include "new.h"
# include "parsing.h"
# include "openfile.h"
# include "signals.h"
# include "structs.h"
# include "utils.h"
# include "nodes.h"
# include "list.h"
# include "execute.h"
# include "redirection.h"

void	rl_replace_line(const char *text, int clear_undo);

#endif