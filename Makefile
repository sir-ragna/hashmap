default:
	gcc -g -Werror -Wall main.c -o main

test:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main