#include <stdio.h>
#include <stdlib.h>
#include "../ioctl.h"
#include <sys/mman.h>
#include <string.h>

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
    int i = 0;
    for (i = 0; i < 8190; i++)
        fprintf( w, "x");
	fflush(w);

    // Read a couple characters
    char c[2];
	read(fileno(w), c, 2);

    int e = write(fileno(w), "OOOOOOOOO", 8);

    if (e == 4)
        printf("TEST TEN:\tPASSED\n");
    else
        printf("Test 10 failed - e=%i\n",e);
	return EXIT_SUCCESS;

}
