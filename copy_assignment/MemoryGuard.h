#ifndef DEMONSTRATION_MEMORY_GUARD_HEADER
#define DEMONSTRATION_MEMORY_GUARD_HEADER

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

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

#endif /*** DEMONSTRATION_MEMORY_GUARD_HEADER ***/
