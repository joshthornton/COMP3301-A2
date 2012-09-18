#include <stdio.h>
#include <stdlib.h>
#include "../ioctl.h"

void *read(void * arg)
{
	// Read input
	char c;
	while ( (c = fgetc( (FILE *)arg ) ) != '\0' && c != -1 && c != EOF && c != '\n' )
		putchar( c );
	putchar( '\n' );
}

int main( int argc, char *argv[] )
{

	// Open a file
	FILE * r = fopen( "/dev/crypto", "r" );
	FILE * w = fopen( "/dev/crypto", "w" );

	// Create buffer
	int bid;
	if ((bid = ioctl(fileno(r), CRYPTO_IOCCREATE)) < 0)
	{
		fprintf(stderr, "create: %d\n", bid);
		exit(-1);
	}

	// Attach
	if (ioctl(fileno(w), CRYPTO_IOCTATTACH, bid) < 0)
	{
		perror( "attach" );
		exit(-1);
	}

	pthread_t rt;
	pthread_create(&rt, NULL, read, r);

	usleep(100);

	// Write to it
	if ( argc > 1 )
		fprintf( w, "%s", argv[1] );
	else
		fprintf( w, "blah" );
	fflush(w);
	fclose(w);

	pthread_join(rt, NULL);
	
	return EXIT_SUCCESS;

}
