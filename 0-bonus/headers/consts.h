/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   consts.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 02:18:15 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 03:44:33 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTS_H
# define CONSTS_H
# include "header.h"

// error messages
# define SUCCESS 0
# define NO_SUCH_FILE_OR_DIR 1
# define PERMISSION_DENIED 1
# define AMBIGIOUS_REDIRECTION 1
# define LIMIT_HEREDOC 2
# define UNEXPECTED_ERROR 258
# define COMMAND_NOT_FOUND 127
# define SYNTAX_ERROR 258
# define ERROR -1

// file descriptors
# define NOT_OPENED -200
# define IN STDIN_FILENO
# define OUT STDOUT_FILENO
# define ERR STDERR_FILENO

// signals
# define CTRL_C 2
# define CTRL_SLASH 3

# ifndef DEBUG
#  define DEBUG 1
# endif

#endif