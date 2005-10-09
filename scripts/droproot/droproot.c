#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {

	int number;
	if ( argc < 3 ) {
		printf("syntax: %s <number> \"<command to run>\"\n", argv[0]);
		return 1;
	}
	number = atoi(argv[1]);
	if ( number <1 || number >65535 ) {
		printf("syntax: %s <number> \"<command to run>\"\n", argv[0]);
		printf("<number> must be between 1 and 65535\n");
		return 1;
	}
	setuid(number);
	setgid(number);
	seteuid(number);
	setegid(number);

	return system(argv[2]);
}

