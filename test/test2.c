#include <stdio.h>
#include <stdlib.h>
#include "../ioctl.h"

int main( int argc, char *argv[] )
{

	// Open a file for writing only and reading only
	FILE * w = fopen( "/dev/crypto", "w" );
	FILE * r = fopen( "/dev/crypto", "r" );

	// Create buffer
	int bid;
	if ((bid = ioctl(fileno(r), CRYPTO_IOCCREATE)) < 0)
	{
		fprintf(stderr, "create: %d\n", bid);
		exit(-1);
	}

	// attach to buffer
	if ( ioctl(fileno(w), CRYPTO_IOCTATTACH, bid) /*|| ioctl(fileno(r), CRYPTO_IOCTATTACH, bid)*/ )
	{
		perror( "attach" );
		exit(-1);
	}

	// Write to it
	if ( argc > 1 )
		fprintf( w, "%s\n", argv[1] );
	else
		fprintf( w, "blah\n" );
	
	// Force write
	fflush(w);

	// Read input
	char c;
	while ( (c = fgetc( r ) ) != '\0' && c != -1 && c != EOF && c != '\n' )
		putchar( c );
	
	putchar( '\n' );

	fclose(w);
	fclose(r);

	return EXIT_SUCCESS;

}
