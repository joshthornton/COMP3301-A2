#include <stdio.h>
#include <stdlib.h>
#include "../ioctl.h"

int main( int argc, char *argv[] )
{

	// Open a file
	FILE * f = fopen( "/dev/crypto", "r+" );
	FILE * f2 = fopen( "/dev/crypto", "r+" );

	// Create buffer
	int bid;
	if ((bid = ioctl(fileno(f), CRYPTO_IOCCREATE)) < 0)
	{
		fprintf(stderr, "create: %d\n", bid);
		exit(-1);
	}
	int bid2;
	if ((bid2 = ioctl(fileno(f2), CRYPTO_IOCCREATE)) < 0)
	{
		fprintf(stderr, "create: %d\n", bid);
		exit(-1);
	}
	// attach to buffer
	//if ( ioctl(fileno(f), CRYPTO_IOCTATTACH, bid) )
	//{
	//	perror( "attach" );
	//	exit(-1);
	//}
	//if ( ioctl(fileno(f2), CRYPTO_IOCTATTACH, bid2) )
	//{
	//	perror( "attach" );
	//	exit(-1);
	//}

	// Write to it
	if ( argc > 1 )
		fprintf( f, "%s\n", argv[1] );
	else
		fprintf( f, "blah\n" );
	if ( argc > 2 )
		fprintf( f2, "%s\n", argv[2] );
	else
		fprintf( f2, "blah\n" );
	
	// Read input
	char c;
	while ( (c = fgetc( f ) ) != '\0' && c != -1 && c != EOF && c != '\n' )
		putchar( c );
	while ( (c = fgetc( f2 ) ) != '\0' && c != -1 && c != EOF && c != '\n' )
		putchar( c );
	putchar( '\n' );

	fclose(f);
	fclose(f2);

	return EXIT_SUCCESS;

}
