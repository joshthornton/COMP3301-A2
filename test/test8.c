#include <stdio.h>
#include <stdlib.h>
#include "../ioctl.h"

void *read(void * arg)
{
	int bid = (int)arg;
	FILE * r;
	
	r = fopen( "/dev/crypto", "r" );
	if (!r)
	{
		perror("fopen");
		exit(-1);
	}
	
	// Attach
	if (ioctl(fileno(r), CRYPTO_IOCTATTACH, bid) < 0)
	{
		perror( "attach 1");
		exit(-1);
	}
	
	// Read input
	char c;
	while ( (c = fgetc(r)) != '\0' && c != -1 && c != EOF )
	{
		if ( c == '\n' )
		{
			fclose(r);
			r = fopen( "/dev/crypto", "r" );
			if (!r)
			{
				perror("fopen");
				exit(-1);
			}
			
			// Attach
			if (ioctl(fileno(r), CRYPTO_IOCTATTACH, bid) < 0)
			{
				putchar('\n');
				exit(-1);
			}

		} else {
			putchar( c );
		}
	}
	putchar('\n');
	fclose(r);
}


int main( int argc, char *argv[] )
{

	// Open a file
	FILE * w = fopen( "/dev/crypto", "w" );

	// Create buffer
	int bid;
	if ((bid = ioctl(fileno(w), CRYPTO_IOCCREATE)) < 0)
	{
		fprintf(stderr, "create: %d\n", bid);
		exit(-1);
	}

	pthread_t rt;
	pthread_create(&rt, NULL, read, bid);

	usleep(100);

	// Write to it
	int i = 1;
	while( i < argc )
	{
		fprintf( w, "%s\n", argv[i++] );
		fflush(w);
		usleep(100);
	}

	usleep(100);
	fclose(w);
	usleep(100);
	pthread_join(rt, NULL);
	
	return EXIT_SUCCESS;

}
