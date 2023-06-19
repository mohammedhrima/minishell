/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 02:14:28 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 06:10:48 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H

# include "minishell.h"

typedef enum e_type		t_type;
typedef struct s_token	t_token;
typedef struct s_token	t_value;
typedef struct s_node	t_node;
typedef struct s_file	t_file;
typedef struct s_list	t_list;
typedef struct s_global	t_global;
typedef struct s_arg	t_arg;

enum					e_type
{
	integers_,
	pointers_,
	assign_,
	identifier_,
	lparent_,
	rparent_,
	redir_input,
	redir_output,
	heredoc_,
	append_,
	pipe_,
	star_,
	redir_parent,
	and_,
	or_,
	env_,
	echo_,
	cd_,
	pwd_,
	export_,
	unset_,
	exit_,
	single_quote_,
	double_quote_,
	end_
};

struct					s_token
{
	t_type				type;
	char				*value;
	int					start;
	int					len;
};

struct					s_node
{
	t_node				*left;
	t_node				*right;
	t_token				*token;
};

struct					s_file
{
	char				*name;
	int					fd;
	t_type				type;
};

struct					s_list
{
	t_type				type;
	union
	{
		void			**pointers;
		int				*integers;
	};
	size_t				size;
	int					pos;
	int					len;
};

struct					s_global
{
	t_list				addresses;
	t_list				envirement;
	t_list				tokens;
	t_list				fds;
	t_list				pids;
	// t_list				nodes;
	// int					status;
	int					inside_pipe;
	char				**env;
	char				**values;
	t_type				*types;
};
t_global				g_global;

#endif