#include <stdio.h>

char *nextWord(char *phrase) {
    char *p = phrase;
    while (*p != 0x00 && *p != ' ')
        p++;

    if (*p==0x00 || *(p+1)==0x00)
        return NULL;

    return p+1;
}

int main (void) {
	char test[] = "hola esto es una prueba";
	char *p = test;
	while ((p=nextWord(p)) != NULL) {
		printf("[%s] ", p);
	}
}
