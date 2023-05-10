all:
	gcc -fsanitize=address -g3 001.c
	./a.out