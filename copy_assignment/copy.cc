/*
 * This is a simple file copy program in C, but it can be compiled in C++.
 *
 * The design of this program is not that of a typical copy program.
 * This program's design uses a heap allocated memory block to store
 * the data while it is being copied from the source file to the
 * destination file.  This memory block is released and then reallocated
 * on each successive block read.  The reason that we have done this is
 * to give us many different scenarios in which to apply C++ resource
 * management techniques.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

/* We'll copy 256k blocks at a time, just for efficiency. */
#define COPYBUFSIZE ( 256 * 1024 )

#define DEBUG_MODE 1

/*
 * Returns a newly allocated buffer in `buf`, and the amount read in `amount_read`.
 * Populates the buffer with data from the file, up to the amount read.
 */
static int read_buffer( size_t amount, FILE *fp, size_t *amount_read, void **buf );

int
main( int argcnt, char *argvec[] )
{
	FILE *infile;
	FILE *outfile;
	void *buf;
	size_t res;
	int error;

	if( argcnt != 3 )
	{
		fprintf( stderr, "%s must take two arguments: an \"infile\" "
				"and an \"outfile\"\n", argvec[ 0 ] );
		return -1;
	}

	if( ( infile= fopen( argvec[ 1 ], "rb" ) ) == NULL )
	{
		fprintf( stderr, "Unable to open file \"%s\"\n", argvec[ 1 ] );
		return -1;
	}

	if( ( outfile= fopen( argvec[ 2 ], "wb" ) ) == NULL )
	{
		fprintf( stderr, "Unable to open file \"%s\"\n", argvec[ 2 ] );
		fclose( infile );
		return -1;
	}

	while( !feof( infile ) )
	{
		if( DEBUG_MODE ) fprintf( stderr, "Try to read %zu bytes\n", res );
		error= read_buffer( COPYBUFSIZE, infile, &res, &buf );
		if( error )
		{
			if( error == -2 ) fprintf( stderr, "Unable to allocate a copy buffer.\n" );
			else fprintf( stderr, "An error in reading occurred.\n" );

			free( buf );
			fclose( outfile );
			fclose( infile );
			return -1;
		}
		if( DEBUG_MODE ) fprintf( stderr, "Read %zu bytes\n", res );

		if( ( fwrite( buf, 1, res, outfile ) < res ) && ferror( outfile ) )
		{
			fprintf( stderr, "An error in copying occurred.\n" );
			free( buf );
			fclose( outfile );
			fclose( infile );
			return -1;
		}
		if( DEBUG_MODE ) fprintf( stderr, "Wrote %zu bytes\n", res );

		free( buf );
		if( DEBUG_MODE ) fprintf( stderr, "Freed %zu bytes\n", res );
	}

	/* Copy is complete.  Free our resources. */
	fclose( outfile );
	fclose( infile );

	return 0;
}


static int
read_buffer( size_t amount, FILE *infile, size_t *amount_read, void **buf )
{
	assert( buf );
	assert( amount_read );
	if( DEBUG_MODE ) fprintf( stderr, "Try to allocate %zu bytes\n", amount );
	if( ( *buf= malloc( amount ) ) == NULL )
	{
		*amount_read= 0;
		return -2;
	}
	if( DEBUG_MODE ) fprintf( stderr, "allocated %zu bytes\n", amount );

	*amount_read= fread( *buf, 1, amount, infile );
	if( DEBUG_MODE ) fprintf( stderr, "just read %zu bytes\n", *amount_read );

	if( *amount_read < COPYBUFSIZE && ( ferror( infile ) || !feof( infile ) ) )
	{
		return -1;
	}

	return 0;
}

/* End of sample program */

