CC = cc

READLINE_INCLUDE = -I$(HOME)/.local/lib/readline/include/readline
READLINE = -lreadline -lhistory -L$(HOME)/.local/lib/readline/lib

#CFLAGS = -Wall -Werror -Wextra $(READLINE_INCLUDE)
SAN = -fsanitize=address -fsanitize=null -g3
RM = rm -rf

SRC = main.c

PARSING = parsing/tokenize.c parsing/read_input.c

BUILTIN = execution/builtin/func.c execution/builtin/cd.c execution/builtin/echo.c execution/builtin/env.c \
		  execution/builtin/exit.c execution/builtin/export.c execution/builtin/pwd.c execution/builtin/unset.c \

EVALUATE = execution/evaluate/command.c execution/evaluate/evaluate.c execution/evaluate/expand.c \
			execution/evaluate/heredoc.c execution/evaluate/nodes.c execution/evaluate/open_file.c \
			execution/evaluate/parenteses.c execution/evaluate/tools.c 

UTILS = utils/utils.c utils/new.c utils/signals.c utils/list.c

OBJS_SRC = $(SRC:.c=.o)
OBJS_PARSING = $(PARSING:.c=.o)
OBJS_BUILTIN = $(BUILTIN:.c=.o)
OBJS_EVALUATE = $(EVALUATE:.c=.o)
OBJS_UTILS = $(UTILS:.c=.o)

NAME = mandatory
NAME_BONUS = bonus

all: $(NAME)

$(NAME): $(OBJS_SRC) $(OBJS_PARSING) $(OBJS_BUILTIN) $(OBJS_EVALUATE) $(OBJS_UTILS)
	$(CC) $(CFLAGS) $(READLINE_INCLUDE) $(READLINE) $(OBJS_SRC) $(OBJS_PARSING) $(OBJS_BUILTIN) $(OBJS_EVALUATE) $(OBJS_UTILS) -o $(NAME)

clean: 
	$(RM) $(OBJS_SRC) $(OBJS_PARSING) $(OBJS_BUILTIN) $(OBJS_EVALUATE) $(OBJS_UTILS)
	
fclean: clean
	$(RM) $(NAME) $(NAME_BONUS)

re : fclean all