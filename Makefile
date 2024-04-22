all:
	gcc main.c -lm -O3 -Wall -Wextra -o main
	./main
	rm main
