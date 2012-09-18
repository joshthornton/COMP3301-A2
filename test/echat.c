#include <stdio.h>
#include <stdlib.h>
#include "../ioctl.h"
#include <pthread.h>
#include <unistd.h>

void create_user(FILE* read, FILE* write);
void* handle_buffer(void* args);
void person_one(char* key);
void person_two(char* key, int buf1, int buf2);

void person_one(char* key)
{
	/* Open a file */
	FILE * p1r = fopen( "/dev/crypto", "r" );
	FILE * p1w = fopen( "/dev/crypto", "w" );

	/* Create buffer */
	int p1p2;
	if ((p1p2 = ioctl(fileno(p1w), CRYPTO_IOCCREATE)) < 0) {
		exit(-1);
	}

    int p2p1;
	if ((p2p1 = ioctl(fileno(p1r), CRYPTO_IOCCREATE)) < 0) {
		exit(-1);
	}

    /* Create keys */
    struct crypto_smode p1w_key, p1r_key;

    p1w_key.dir = CRYPTO_WRITE;
    p1w_key.mode = CRYPTO_ENC;
    p1w_key.key = key;

    p1r_key.dir = CRYPTO_READ;
    p1r_key.mode = CRYPTO_DEC;
    p1r_key.key = key;

	if ( ioctl(fileno(p1w), CRYPTO_IOCSMODE, &p1w_key) ) {
		exit(-1);
	}
	if ( ioctl(fileno(p1r), CRYPTO_IOCSMODE, &p1r_key) ) {
		exit(-1);
	}
    
    fprintf(stderr, "first_buffer_id: %i, second_buffer_id: %i\n", p1p2, p2p1);
	
    /* Create handler */
    create_user(p1r, p1w);
}

void person_two(char* key, int p1p2, int p2p1)
{
	/* Open a file */
	FILE * p2r = fopen( "/dev/crypto", "r" );
	FILE * p2w = fopen( "/dev/crypto", "w" );

	/* Attach to buffer */
    if ( ioctl(fileno(p2r), CRYPTO_IOCTATTACH, p1p2) ) {
		exit(-1);
	}
	if ( ioctl(fileno(p2w), CRYPTO_IOCTATTACH, p2p1) ) {
		exit(-1);
	}

    /* Create keys */
    struct crypto_smode p2w_key, p2r_key;

    p2w_key.dir = CRYPTO_WRITE;
    p2w_key.mode = CRYPTO_ENC;
    p2w_key.key = key;

    p2r_key.dir = CRYPTO_READ;
    p2r_key.mode = CRYPTO_DEC;
    p2r_key.key = key;

	if ( ioctl(fileno(p2w), CRYPTO_IOCSMODE, &p2w_key) ) {
		exit(-1);
	}
	if ( ioctl(fileno(p2r), CRYPTO_IOCSMODE, &p2r_key) ) {
		exit(-1);
	}

    /* Create handler */
    create_user(p2r, p2w);
}

int main( int argc, char *argv[] )
{
    if (argc == 2)
        person_one(argv[1]);
    else if (argc == 4)
        person_two(argv[1], atoi(argv[2]), atoi(argv[3]));

	return EXIT_SUCCESS;

}

void create_user(FILE* r, FILE* w)
{
    /* Create thread which will handle outputing buffer of other process */
    pthread_t input;
    if (pthread_create(&input, NULL, handle_buffer, r)) {
        exit(-1);
    }
    
    /* Read from stdin until EOF */
    char c[10];
    while (fgets(c, 10, stdin) != NULL && !feof(stdin)) {
        fprintf(w,"%s", c);
        fflush(w);
    }

    /* Close write end now that input has finished. */
    fclose(w);

    /* Exit chat */
    exit(0);
}

void* handle_buffer(void* args)
{
    char c[10];
    while (fgets(c, 10, (FILE*) args) != NULL)
        printf("%s", c);

    /* Close the read file pointer as there will be no more input */
	fclose((FILE*) args);

    /* Exit chat */
    exit(0);
    
    return 0;
}
