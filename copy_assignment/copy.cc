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
#include <string.h>

#include "MemoryGuard.h"
#include "FileGuard.h"

/* We'll copy 256k blocks at a time, just for efficiency. */
const size_t COPYBUFSIZE= ( 256 * 1024 );

const bool DEBUG_MODE= true;

/*
 * MemoryGuard can be combined with an error code, to facilitate returning
 * from functions and reporting errors.
 */
struct MemoryGuardReturn
{
	int errorCode;
	MemoryGuard memory;

	MemoryGuardReturn( int code )
	{
		errorCode= code;
	}

	MemoryGuardReturn( MemoryGuard m )
	{
		errorCode= 0;
		memory= m;
	}
};

/*
 * Returns a newly allocated buffer, and indicates the amount read in the
 * return value's `MemoryGuard.memory.amount` member.
 * Populates the buffer with data from the `file`, up to the amount read.
 */
static MemoryGuardReturn read_buffer( size_t amount, FileGuard file );

int
main( int argcnt, char *argvec[] )
{
	if( argcnt != 3 )
	{
		fprintf( stderr, "%s must take two arguments: an \"infile\" "
				"and an \"outfile\"\n", argvec[ 0 ] );
		return -1;
	}

	FileGuard infile= openFileForRead( argvec[ 1 ] );
	if( infile.error() ) return -1;

	FileGuard outfile= openFileForWrite( argvec[ 2 ] );
	if( outfile.error() ) return -1;

	while( !infile.eof() )
	{
		MemoryGuardReturn bufWithStatus= read_buffer( COPYBUFSIZE, infile );
		if( bufWithStatus.errorCode )
		{
			fprintf( stderr, bufWithStatus.errorCode == -2
					? "Unable to allocate a copy buffer.\n"
					: "An error in reading occurred.\n" );
			return -1;
		}
		MemoryGuard buf= bufWithStatus.memory;
		if( DEBUG_MODE ) fprintf( stderr, "Read %zu bytes\n", buf.amount );

		if( outfile.write( &buf ) )
		{
			fprintf( stderr, "An error in copying occurred.\n" );
			return -1;
		}
		if( DEBUG_MODE ) fprintf( stderr, "Wrote %zu bytes\n", buf.amount );

	}

	/* Copy is complete.  Our resources are cleaned by dtors. */

	return 0;
}


static MemoryGuardReturn
read_buffer( size_t amount, FileGuard infile )
{
	if( DEBUG_MODE ) fprintf( stderr, "Try to allocate %zu bytes\n", amount );
	MemoryGuard internal( amount );
	if( internal.p == NULL ) return MemoryGuardReturn( -2 );
	if( DEBUG_MODE ) fprintf( stderr, "allocated %zu bytes\n", amount );

	if( infile.read( &internal ) ) return MemoryGuardReturn( -1 );
	if( DEBUG_MODE ) fprintf( stderr, "just read %zu bytes\n", internal.amount );

	// Transfer the ownership to the caller
	return MemoryGuardReturn( internal );
}

/* End of sample program */

