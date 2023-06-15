CC = cc

READLINE_INCLUDE = -I$(HOME)/.local/lib/readline/include/readline
READLINE = -lreadline -lhistory -L$(HOME)/.local/lib/readline/lib

#CFLAGS = -Wall -Werror -Wextra $(READLINE_INCLUDE)
SAN = -fsanitize=address -fsanitize=null -g3
RM = rm -rf

SRC = minishell.c folder/list.c folder/new.c \
	folder/remove.c folder/signals.c folder/utils.c \
	folder/builtin.c folder/nodes.c folder/tokenize.c \
	folder/expand.c folder/open_file.c folder/parse.c

NAME = exe

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(READLINE_INCLUDE) $(READLINE) $(SRC) -o $(NAME)

clean: 
	$(RM) $(NAME)
	
fclean: clean
# 	$(RM) $(NAME)

re : fclean all