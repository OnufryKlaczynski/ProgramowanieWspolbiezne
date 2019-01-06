#include "utils.h"


/*
Funkcja vchar usuwa z bufora klawiatury znaki rozdzielajšce (CR, LF, SP, TAB, ...)
i przekazuje jako wynik kod pierwszego napotkanego znaku widocznego.
*/
char vchar()
{
	char cc;
	while ((cc = getchar()) <= ' ');
	return cc;
}

void print_error(){
	printf("%s\n", strerror(errno));
}

