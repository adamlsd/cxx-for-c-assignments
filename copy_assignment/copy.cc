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

/* We'll copy 256k blocks at a time, just for efficiency. */
const size_t COPYBUFSIZE= ( 256 * 1024 );

const bool DEBUG_MODE= true;

// MemoryGuard could be modified, optionally
// to implement a reference-counting semantic
// to help with lifetime issues.
struct MemoryGuard
{
	void *p;
	size_t amount;

	size_t *refcount;

	MemoryGuard()
	{
		refcount= new size_t( 1 );
		amount= 0;
		p= 0;
	}

	MemoryGuard( size_t a )
	{
		refcount= new size_t( 1 );
		p= malloc( a );
		if( p ) amount= a;
	}

	// When a memory-guard is copied, we bump the refcount
	// by one -- this facilitates cleaning up the resource
	// when it is no longer used, yet still preserves a relatively
	// cheap copy operation.
	MemoryGuard( const MemoryGuard &copy )
	{
		p= copy.p;
		refcount= copy.refcount;
		amount= copy.amount;
		++*refcount;
	}

	MemoryGuard &
	operator= ( const MemoryGuard &copy )
	{
		if( !--*refcount )
		{
			free( p );
			delete refcount;
		}
		p= copy.p;
		refcount= copy.refcount;
		amount= copy.amount;
		++*refcount;

		return *this;
	}

	~MemoryGuard()
	{
		if( !--*refcount )
		{
			free( p );
			delete refcount;
		}
	}

	/*
	 * Returns a newly constructed memory guard which manages a distinct
	 * copy of memory from the one managed by this `MemoryGuard`.  This
	 * facilitates the creation of memory regions which duplicate existing
	 * memory, as opposed to tracking a single memory region by reference
	 * count.
	 */
	MemoryGuard
	duplicate()
	{
		MemoryGuard result( amount );
		memcpy( result.p, p, amount );
		return result;
	}

	/*
	 * Returns a newly constructed `MemoryGuard` with the specified size, containing an
	 * appropriately sized copy of the contents of this buffer.  Realloc is not an appropriate
	 * on this buffer, as this would cause issues with the sharing reference count mechanism.
	 */
	MemoryGuard
	duplicate( int a )
	{
		MemoryGuard result( a );
		memcpy( result.p, p, amount < a ? amount : a );
		return result;
	}
};

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
 * Returns a newly opened file, or prints an error and returns a
 * FileGuard in an error state.
 */
static FileGuard openFileForRead( const char *fileName );
static FileGuard openFileForWrite( const char *fileName );

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

static FileGuard
openFileWithMode( const char *fileName, const char *mode )
{
	FileGuard result( fileName, mode );
	if( result.error() ) fprintf( stderr, "Unable to open file \"%s\"\n", fileName );
	return result;
}


static FileGuard
openFileForRead( const char *fileName )
{
	return openFileWithMode( fileName, "rb" );
}

static FileGuard
openFileForWrite( const char *fileName )
{
	return openFileWithMode( fileName, "wb" );
}

/* End of sample program */

