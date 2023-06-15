CC = cc

READLINE_INCLUDE = -I$(HOME)/.local/lib/readline/include/readline
READLINE = -lreadline -lhistory -L$(HOME)/.local/lib/readline/lib

#CFLAGS = -Wall -Werror -Wextra $(READLINE_INCLUDE)
SAN = -fsanitize=address -fsanitize=null -g3
RM = rm -rf

BUILTIN = folder/built-in/cd.c folder/built-in/echo.c folder/built-in/env.c folder/built-in/exit.c \
		  folder/built-in/export.c folder/built-in/pwd.c folder/built-in/unset.c folder/built-in/get_built_in.c

EVALUATE = folder/evaluate/evaluate.c

SRC = minishell.c $(BUILTIN) $(EVALUATE) folder/list.c folder/new.c folder/remove.c folder/signals.c folder/utils.c \
	folder/nodes.c folder/tokenize.c folder/expand.c folder/open_file.c folder/parse.c \
	folder/heredoc.c folder/command.c

NAME = exe

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(READLINE_INCLUDE) $(READLINE) $(SRC) -o $(NAME)

clean: 
	$(RM) $(NAME)
	
fclean: clean
# 	$(RM) $(NAME)

re : fclean all