# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/16 04:40:36 by mhrima            #+#    #+#              #
#    Updated: 2023/06/17 11:19:02 by mhrima           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc

READLINE_INCLUDE = -I$(HOME)/.local/lib/readline/include/readline
READLINE = -lreadline -lhistory -L$(HOME)/.local/lib/readline/lib

CFLAGS = -Wall -Werror -Wextra
SAN = -fsanitize=address -fsanitize=null -g3
RM = rm -rf

SRCS = minishell.c  folder/new.c folder/list.c folder/signals.c folder/open_file.c folder/redirection.c 

PARSING = folder/parsing/tokenize.c folder/parsing/parse.c folder/parsing/check_input.c

BUILTIN = folder/built-in/cd.c folder/built-in/echo.c folder/built-in/env.c \
			folder/built-in/exit.c folder/built-in/export.c folder/built-in/pwd.c \
			folder/built-in/unset.c folder/built-in/get_built_in.c

NODES = folder/nodes/nodes_part1.c folder/nodes/nodes_part2.c

EXECUTION = folder/execution/evaluate.c folder/execution/execute.c \
			folder/execution/heredoc.c folder/execution/expand.c folder/execution/command.c

UTILS = folder/utils/convert.c folder/utils/exit.c folder/utils/memory.c folder/utils/split.c \
		folder/utils/string.c folder/utils/utils_part1.c folder/utils/utils_part2.c

OBJS_SRCS = $(SRCS:.c=.o)
OBJS_PARSING = $(PARSING:.c=.o)
OBJS_BUILTIN = $(BUILTIN:.c=.o)
OBJS_NODES = $(NODES:.c=.o)
OBJS_EXECUTION = $(EXECUTION:.c=.o)
OBJS_UTILS = $(UTILS:.c=.o)

NAME = minishell

all: $(NAME)

$(NAME): $(OBJS_SRCS) $(OBJS_PARSING) $(OBJS_BUILTIN) $(OBJS_NODES) $(OBJS_EXECUTION) $(OBJS_UTILS)
	$(CC) $(CFLAGS) $(SAN) $(READLINE_INCLUDE) $(READLINE) $(OBJS_SRCS) $(OBJS_PARSING) $(OBJS_BUILTIN) $(OBJS_NODES) $(OBJS_EXECUTION) $(OBJS_UTILS) -o $(NAME)

clean: 
	$(RM) $(OBJS_SRCS) $(OBJS_PARSING) $(OBJS_BUILTIN) $(OBJS_NODES) $(OBJS_EXECUTION) $(OBJS_UTILS)
	
fclean: clean
	$(RM) $(NAME)

re : fclean all