# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/16 04:40:36 by mhrima            #+#    #+#              #
#    Updated: 2023/06/19 20:32:03 by mhrima           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC 					= cc
NAME				= minishell
READLINE_INCLUDE	= -I$(HOME)/.local/lib/readline/include/readline
READLINE			= -lreadline -lhistory -L$(HOME)/.local/lib/readline/lib
SAN 				= -fsanitize=address -fsanitize=null -g3
CFLAGS				= $(SAN) #-Wall -Werror -Wextra 
RM					= rm -rf

SRCS 		= main.c
PARSING 	= parsing/input.c parsing/nodes_1.c parsing/nodes_2.c parsing/tokenize.c  
BUILTIN		= execution/built-in/cd.c execution/built-in/echo.c execution/built-in/env.c \
			  execution/built-in/exit.c execution/built-in/export.c execution/built-in/pwd.c \
			  execution/built-in/unset.c execution/built-in/get_built_in.c
EXECUTION 	= $(BUILTIN) evaluate.c heredoc.c execution/open_file.c
UTILS 		= utils/convert.c utils/exit.c utils/expand.c utils/global.c utils/list.c \
			  utils/memory.c utils/split.c utils/string_1.c utils/string_2.c utils/utils.c \
			  utils/redirection.c utils/signals.c

OBJS_SRCS 		= $(SRCS:.c=.o)
OBJS_PARSING 	= $(PARSING:.c=.o)
OBJS_EXECUTION 	= $(EXECUTION:.c=.o)
OBJS_UTILS 		= $(UTILS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS_SRCS) $(OBJS_PARSING) $(OBJS_EXECUTION) $(OBJS_UTILS)
	$(CC) $(CFLAGS) $(READLINE_INCLUDE) $(READLINE) $(OBJS_SRCS) $(OBJS_PARSING) $(OBJS_EXECUTION) $(OBJS_UTILS) -o $(NAME)

clean: 
	$(RM) $(OBJS_SRCS) $(OBJS_PARSING) $(OBJS_EXECUTION) $(OBJS_UTILS)
	
fclean: clean
	$(RM) $(NAME)

re : fclean all