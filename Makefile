all:
	gcc -fsanitize=address -fsanitize=null -g3 bonus.c
	./a.out