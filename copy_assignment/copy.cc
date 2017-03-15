/*
 * This is a simple file copy program in C++.
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
const size_t COPYBUFSIZE= ( 256 * 1024 );

const bool DEBUG_MODE= true;

struct FileGuard
{
	FILE *fp;

	~FileGuard()
	{
		fclose( fp );
	}
};

struct MemoryGuard
{
	void *p;

	~MemoryGuard()
	{
		free( p );
	}
};

/*
 * Returns a newly allocated buffer in `buf`, and the amount read in `amount_read`.
 * Populates the buffer with data from the file, up to the amount read.
 */
static int read_buffer( size_t amount, FILE *fp, size_t *amount_read, void **buf );

int
main( int argcnt, char *argvec[] )
{
	FileGuard infile= { NULL };
	FileGuard outfile= { NULL };
	size_t res;

	if( argcnt != 3 )
	{
		fprintf( stderr, "%s must take two arguments: an \"infile\" "
				"and an \"outfile\"\n", argvec[ 0 ] );
		return -1;
	}

	if( ( infile.fp= fopen( argvec[ 1 ], "rb" ) ) == NULL )
	{
		fprintf( stderr, "Unable to open file \"%s\"\n", argvec[ 1 ] );
		return -1;
	}

	if( ( outfile.fp= fopen( argvec[ 2 ], "wb" ) ) == NULL )
	{
		fprintf( stderr, "Unable to open file \"%s\"\n", argvec[ 2 ] );
		return -1;
	}

	while( !feof( infile.fp ) )
	{
		MemoryGuard buf= { NULL };
		if( DEBUG_MODE ) fprintf( stderr, "Try to read %zu bytes\n", res );
		int error= read_buffer( COPYBUFSIZE, infile.fp, &res, &buf.p );
		if( error )
		{
			if( error == -2 ) fprintf( stderr, "Unable to allocate a copy buffer.\n" );
			else fprintf( stderr, "An error in reading occurred.\n" );
			return -1;
		}
		if( DEBUG_MODE ) fprintf( stderr, "Read %zu bytes\n", res );

		if( ( fwrite( buf.p, 1, res, outfile.fp ) < res ) && ferror( outfile.fp ) )
		{
			fprintf( stderr, "An error in copying occurred.\n" );
			return -1;
		}
		if( DEBUG_MODE ) fprintf( stderr, "Wrote %zu bytes\n", res );

	}

	/* Copy is complete.  Our resources are cleaned by dtors. */

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

