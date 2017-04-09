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

	FileGuard() { fp= 0; }

	FileGuard( const char *name, const char *mode )
	{
		fp= fopen( name, mode );
	}

	~FileGuard()
	{
		fclose( fp );
	}
};

struct MemoryGuard
{
	void *p;

	MemoryGuard() { p= 0; }

	MemoryGuard( size_t amount ) { p= malloc( amount ); }

	~MemoryGuard()
	{
		free( p );
	}
};

/*
 * Returns a newly allocated buffer in `buf`, and the amount read in `amount_read`.
 * Populates the buffer with data from the file, up to the amount read.
 */
static int read_buffer( size_t amount, FILE *fp, size_t *amount_read, MemoryGuard *buf );

int
main( int argcnt, char *argvec[] )
{
	size_t res;

	if( argcnt != 3 )
	{
		fprintf( stderr, "%s must take two arguments: an \"infile\" "
				"and an \"outfile\"\n", argvec[ 0 ] );
		return -1;
	}

	FileGuard infile( argvec[ 1 ], "rb" );
	if( infile.fp == NULL )
	{
		fprintf( stderr, "Unable to open file \"%s\"\n", argvec[ 1 ] );
		return -1;
	}

	FileGuard outfile( argvec[ 2 ], "wb" );
	if( outfile.fp == NULL )
	{
		fprintf( stderr, "Unable to open file \"%s\"\n", argvec[ 2 ] );
		return -1;
	}

	while( !feof( infile.fp ) )
	{
		if( DEBUG_MODE ) fprintf( stderr, "Try to read %zu bytes\n", res );
		MemoryGuard buf;
		int error= read_buffer( COPYBUFSIZE, infile.fp, &res, &buf );
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
read_buffer( size_t amount, FILE *infile, size_t *amount_read, MemoryGuard *buf )
{
	assert( buf );
	assert( amount_read );
	if( DEBUG_MODE ) fprintf( stderr, "Try to allocate %zu bytes\n", amount );
	MemoryGuard internal( amount );
	if( internal.p == NULL )
	{
		*amount_read= 0;
		return -2;
	}
	if( DEBUG_MODE ) fprintf( stderr, "allocated %zu bytes\n", amount );

	*amount_read= fread( internal.p, 1, amount, infile );
	if( DEBUG_MODE ) fprintf( stderr, "just read %zu bytes\n", *amount_read );

	if( *amount_read < COPYBUFSIZE && ( ferror( infile ) || !feof( infile ) ) )
	{
		return -1;
	}

	// Transfer the ownership to the supplied memory guard.
	buf->p= internal.p;
	internal.p= NULL;

	return 0;
}

/* End of sample program */

