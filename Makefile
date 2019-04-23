build: hangman.o
	gcc -Wall hangman.o -o hangman -lncurses
encoder.o: hangman.c
	gcc -Wall -c hangman.c -o hangman.o -lncurses
clean:
	rm -f hangman.o
	rm -f hangman
