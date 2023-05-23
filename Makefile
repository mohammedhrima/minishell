
	

all:
	export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/Users/mhrima/Desktop/minishell/readline/lib
	gcc file.c -L./readline/lib -lreadline -I./readline/include
	./a.out


#RLFLAGS = -L/Users/jbalahce/.local/opt/homebrew/opt/readline/lib -lreadline
#INCLUDES = -I./minishell.h -I/Users/jbalahce/.local/opt/homebrew/opt/readline/include