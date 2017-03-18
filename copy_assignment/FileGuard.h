#ifndef DEMONSTRATION_FILE_GUARD_HEADER
#define DEMONSTRATION_FILE_GUARD_HEADER

#include <stdio.h>
#include <stdlib.h>

#include "MemoryGuard.h"

// File guard could be reference counted, to make a handy file-opening
// function which prints errors... and the guard can have an "error"
// state function.
struct FileGuard
{
	FILE *fp;
	size_t *refcount;

	FileGuard()
	{
		refcount= new size_t( 1 );
		fp= 0;
	}

	FileGuard( const char *name, const char *mode )
	{
		refcount= new size_t( 1 );
		fp= fopen( name, mode );
	}

	FileGuard( const FileGuard &copy )
	{
		fp= copy.fp;
		refcount= copy.refcount;
		++*refcount;
	}

	FileGuard &operator= ( const FileGuard &copy )
	{
		if( !--*refcount )
		{
			fclose( fp );
			delete refcount;
		}
		fp= copy.fp;
		refcount= copy.refcount;
		++*refcount;

		return *this;
	}

	~FileGuard()
	{
		if( !--*refcount )
		{
			fclose( fp );
			delete refcount;
		}
	}

	// Returns true if there is no file open, or
	// if there is an error on the file pointer itself.
	bool error() { return !fp || ferror( fp ); }

	// Returns true when a file is at the end (or no file is open.)
	bool eof() { return !fp || feof( fp ); }

	// Copy data from the file into the specified buffer (and update
	// its size).  Returns true when an error in reading has occurred
	// and false otherwise.
	bool
	read( MemoryGuard *guard )
	{
		if( !fp ) return true;
		size_t amount= guard->amount;
		guard->amount= fread( guard->p, 1, amount, fp );
		return guard->amount < amount && ( error() || !eof() );
	}

	// Copy data from the specified buffer into this file.  Returns true
	// when an error in writing has occurred and false otherwise.
	bool
	write( const MemoryGuard *guard )
	{
		if( !fp ) return true;
		size_t amount= fwrite( guard->p, 1, guard->amount, fp );
		return amount < guard->amount || error();
	}
};

/*
 * Returns a newly opened file, or prints an error and returns a
 * FileGuard in an error state.
 */
inline FileGuard
openFileWithMode( const char *fileName, const char *mode )
{
	FileGuard result( fileName, mode );
	if( result.error() ) fprintf( stderr, "Unable to open file \"%s\"\n", fileName );
	return result;
}

/*
 * Returns a newly opened file, or prints an error and returns a
 * FileGuard in an error state.
 */
inline FileGuard
openFileForRead( const char *fileName )
{
	return openFileWithMode( fileName, "rb" );
}

/*
 * Returns a newly opened file, or prints an error and returns a
 * FileGuard in an error state.
 */
inline FileGuard
openFileForWrite( const char *fileName )
{
	return openFileWithMode( fileName, "wb" );
}

#endif /*** DEMONSTRATION_FILE_GUARD_HEADER ***/
