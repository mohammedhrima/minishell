# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/16 04:40:36 by mhrima            #+#    #+#              #
#    Updated: 2023/06/16 23:05:55 by mhrima           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc

READLINE_INCLUDE = -I$(HOME)/.local/lib/readline/include/readline
READLINE = -lreadline -lhistory -L$(HOME)/.local/lib/readline/lib

CFLAGS = -Wall -Werror -Wextra
#SAN = -fsanitize=address -fsanitize=null -g3
RM = rm -rf

BUILTIN = folder/built-in/cd.c folder/built-in/echo.c folder/built-in/env.c \
			folder/built-in/exit.c folder/built-in/export.c folder/built-in/pwd.c \
			folder/built-in/unset.c folder/built-in/get_built_in.c

NODES = folder/nodes/nodes_part1.c folder/nodes/nodes_part2.c

PARSE = folder/tokenize.c
EXECUTE = evaluate.c folder/execute/execute.c folder/execute/heredoc.c folder/execute/expand.c 

UTILS = folder/utils/convert.c folder/utils/exit.c folder/utils/memory.c folder/utils/split.c \
		folder/utils/string.c folder/utils/utils.c

SRC = minishell.c remove.c $(BUILTIN) $(NODES) $(UTILS) $(EXECUTE) $(PARSE) folder/new.c folder/list.c \
 		folder/signals.c folder/open_file.c folder/redirection.c 

NAME = minishell

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(CFLAGS) $(SAN) $(READLINE_INCLUDE) $(READLINE) $(SRC) -o $(NAME)

clean: 
	$(RM) $(NAME)
	
fclean: clean

re : fclean all