.phony: test

Release: main.c
	gcc -o mergeSort main.c

Debug: main.c
	gcc -o mergeSort main.c -O0 -g -fsanitize=address

test:
	./mergeSort
