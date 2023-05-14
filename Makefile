all:
	gcc -fsanitize=address -fsanitize=null -g3 minishell.c
	./a.out