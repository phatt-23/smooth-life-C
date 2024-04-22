all:
	gcc main.c -lm -O3 -o main
	./main
	rm main
