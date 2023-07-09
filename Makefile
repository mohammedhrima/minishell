# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/16 04:40:36 by mhrima            #+#    #+#              #
#    Updated: 2023/07/09 13:05:57 by mhrima           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC 					= cc
NAME				= exe
READLINE_INCLUDE	= -I$(HOME)/.local/lib/readline/include/readline
READLINE			= -lreadline -lhistory -L$(HOME)/.local/lib/readline/lib
SAN 				= -fsanitize=address -fsanitize=null -g3
CFLAGS				= $(SAN) #-Wall -Werror -Wextra 
RM					= rm -rf

SRCS 		= main.c utils.c

# OBJS_SRCS 		= $(SRCS:.c=.o)

all: $(NAME)
	./exe

$(NAME): $(SRCS)
	$(CC) $(CFLAGS) $(READLINE_INCLUDE) $(READLINE) $(SRCS) -o $(NAME)

# clean: 
# 	$(RM) $(SRCS)
	
fclean:
	$(RM) $(NAME)

re : fclean all