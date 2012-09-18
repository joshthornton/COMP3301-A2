#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ioctl.h"
#include <sys/mman.h>

int main( int argc, char *argv[] )
{

	// Open a file
	FILE * w = fopen( "/dev/crypto", "r+" );
    int len = strlen(argv[1]);

	// Create buffer
    int bid;
	if ((bid = ioctl(fileno(w), CRYPTO_IOCCREATE)) < 0)
	{
		fprintf(stderr, "create: %d\n", bid);
		exit(-1);
	}

	// Write to it
	unsigned int i = strlen(argv[1]) + 1;
	fprintf( w, "%s\n", argv[1]);
    while (i < 4096 && i++)
        fprintf( w, "x");
	fprintf( w, "TEST NINE:\tPASSED" );
	fflush(w);

    // Read part of the buffer via mmap
    char* v = mmap (0, len, PROT_READ, MAP_SHARED, fileno(w), 4096);
    if (v < 0) {
        perror("mmap");
        exit(0);
    }
	for (i = 0; i < len; i++) {
        printf("%c", *(v + i));
    }
	putchar('\n');
	return EXIT_SUCCESS;

}
