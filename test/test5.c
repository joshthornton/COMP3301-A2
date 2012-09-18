#include <stdio.h>
#include <stdlib.h>
#include "../ioctl.h"

int main( int argc, char *argv[] )
{

	// Open a file
	FILE * f = fopen( "/dev/crypto", "r+" );

	// Create buffer
	int bid;
	if ((bid = ioctl(fileno(f), CRYPTO_IOCCREATE)) < 0)
	{
		fprintf(stderr, "create: %d\n", bid);
		exit(-1);
	}

	// Set encryption 
	struct crypto_smode mode;
	mode.dir = CRYPTO_READ;
	mode.mode = CRYPTO_ENC;
	mode.key = "abcdefghijklmnopqrstuvwxyz";

	if ( ioctl(fileno(f), CRYPTO_IOCSMODE, &mode) )
	{
		perror( "Crypto Mode" );
		exit(-1);
	}

	// Write to it
	if ( argc > 1 )
		fprintf( f, "%s\n", argv[1] );
	else
		fprintf( f, "blah\n" );
	
	// Read input
	char c;
	while ( (c = fgetc( f ) ) != '\0' && c != -1 && c != EOF && c != '\n' )
		putchar( c );
	
	putchar( '\n' );

	return EXIT_SUCCESS;

}
