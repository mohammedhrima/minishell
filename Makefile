all:
	gcc -fsanitize=address -fsanitize=null -g3 001.c
	./a.out
	rm -r a.out*