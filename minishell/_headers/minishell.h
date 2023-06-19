/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 22:27:14 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 04:57:42 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H


# include <dirent.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdarg.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>

# include "structs.h"
# include "consts.h"
# include "utils.h"
# include "execution.h"
# include "parsing.h"

void	rl_replace_line(const char *text, int clear_undo);
void	init_global(char **envp);
void	reset(char *line);
char	*expand(char *value);
bool	is_redirection(t_type type);

#endif