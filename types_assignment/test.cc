#include "complex.h"
#include "complex_inline.h"
#include <strings.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

static void
basic()
{
	struct ComplexNumber *a= ComplexNumber_alloc();
	struct ComplexNumber *b= ComplexNumber_alloc();
	struct ComplexNumber *c= ComplexNumber_alloc();
	struct ComplexNumber *witness= ComplexNumber_alloc();
	char buffer[ 256 ];

	// Test some simple equality and setting

	ComplexNumber_init( a, 1, 0 );
	ComplexNumber_init( b, 1, 0 );

	assert(  ComplexNumber_equal( a, b ) );
	assert(  ComplexNumber_equal( b, a ) );
	assert( !ComplexNumber_not_equal( a, b ) );
	assert( !ComplexNumber_not_equal( b, a ) );

	ComplexNumber_init( a, 0, 1 );
	ComplexNumber_init( b, 0, 1 );

	assert( ComplexNumber_equal( a, b ) );
	assert( ComplexNumber_equal( b, a ) );
	assert( !ComplexNumber_not_equal( a, b ) );
	assert( !ComplexNumber_not_equal( b, a ) );

	ComplexNumber_init( a, -1, 1 );
	ComplexNumber_init( b, -1, 1 );

	assert( ComplexNumber_equal( a, b ) );
	assert( ComplexNumber_equal( b, a ) );
	assert( !ComplexNumber_not_equal( a, b ) );
	assert( !ComplexNumber_not_equal( b, a ) );

	ComplexNumber_init( a, 1, 0 );
	ComplexNumber_init( b, 0, 1 );

	assert( !ComplexNumber_equal( a, b ) );
	assert( !ComplexNumber_equal( b, a ) );
	assert( ComplexNumber_not_equal( a, b ) );
	assert( ComplexNumber_not_equal( b, a ) );

	ComplexNumber_init( a, 1, -4 );
	ComplexNumber_init( b, 0, -4 );

	assert( !ComplexNumber_equal( a, b ) );
	assert( !ComplexNumber_equal( b, a ) );
	assert( ComplexNumber_not_equal( a, b ) );
	assert( ComplexNumber_not_equal( b, a ) );

	// Now that we're convinced that ComplexNumber's init and equal work,
	// let's test add, then sub, then mult. (Along with Format)

	// Add

	ComplexNumber_init( a, 4, 3 );
	ComplexNumber_init( b, 5, -7 );

	ComplexNumber_add( c, a, b );

	ComplexNumber_init( witness, 9, -4 );
	assert( ComplexNumber_equal( c, witness ) );

	bzero( buffer, sizeof( buffer ) );
	ComplexNumber_formatAppend( buffer, c );
	assert( !strcmp( "(9 + -4i)", buffer ) );

	// Sub

	ComplexNumber_init( a, 4, 3 );
	ComplexNumber_init( b, 5, -7 );

	ComplexNumber_sub( c, a, b );

	ComplexNumber_init( witness, -1, 10 );
	assert( ComplexNumber_equal( c, witness ) );

	bzero( buffer, sizeof( buffer ) );
	ComplexNumber_formatAppend( buffer, c );
	assert( !strcmp( "(-1 + 10i)", buffer ) );

	// Mult

	ComplexNumber_init( a, 4, 3 );
	ComplexNumber_init( b, 5, -7 );

	ComplexNumber_mult( c, a, b );

	ComplexNumber_init( witness, 41, -13 );
	assert( ComplexNumber_equal( c, witness ) );

	bzero( buffer, sizeof( buffer ) );
	ComplexNumber_formatAppend( buffer, c );
	assert( !strcmp( "(41 + -13i)", buffer ) );


	ComplexNumber_free( witness );
	ComplexNumber_free( c );
	ComplexNumber_free( b );
	ComplexNumber_free( a );
}

void
fast()
{
	struct ComplexNumberInline a;
	struct ComplexNumberInline b;
	struct ComplexNumberInline c;
	struct ComplexNumberInline witness;
	char buffer[ 256 ];

	// Test some simple equality and setting

	COMPLEX_NUMBER_INLINE_INIT( a, 1, 0 );
	COMPLEX_NUMBER_INLINE_INIT( b, 1, 0 );

	assert(  COMPLEX_NUMBER_INLINE_EQUAL( a, b ) );
	assert(  COMPLEX_NUMBER_INLINE_EQUAL( b, a ) );
	assert( !COMPLEX_NUMBER_INLINE_NOT_EQUAL( a, b ) );
	assert( !COMPLEX_NUMBER_INLINE_NOT_EQUAL( b, a ) );

	COMPLEX_NUMBER_INLINE_INIT( a, 0, 1 );
	COMPLEX_NUMBER_INLINE_INIT( b, 0, 1 );

	assert( COMPLEX_NUMBER_INLINE_EQUAL( a, b ) );
	assert( COMPLEX_NUMBER_INLINE_EQUAL( b, a ) );
	assert( !COMPLEX_NUMBER_INLINE_NOT_EQUAL( a, b ) );
	assert( !COMPLEX_NUMBER_INLINE_NOT_EQUAL( b, a ) );

	COMPLEX_NUMBER_INLINE_INIT( a, -1, 1 );
	COMPLEX_NUMBER_INLINE_INIT( b, -1, 1 );

	assert( COMPLEX_NUMBER_INLINE_EQUAL( a, b ) );
	assert( COMPLEX_NUMBER_INLINE_EQUAL( b, a ) );
	assert( !COMPLEX_NUMBER_INLINE_NOT_EQUAL( a, b ) );
	assert( !COMPLEX_NUMBER_INLINE_NOT_EQUAL( b, a ) );

	COMPLEX_NUMBER_INLINE_INIT( a, 1, 0 );
	COMPLEX_NUMBER_INLINE_INIT( b, 0, 1 );

	assert( !COMPLEX_NUMBER_INLINE_EQUAL( a, b ) );
	assert( !COMPLEX_NUMBER_INLINE_EQUAL( b, a ) );
	assert( COMPLEX_NUMBER_INLINE_NOT_EQUAL( a, b ) );
	assert( COMPLEX_NUMBER_INLINE_NOT_EQUAL( b, a ) );

	COMPLEX_NUMBER_INLINE_INIT( a, 1, -4 );
	COMPLEX_NUMBER_INLINE_INIT( b, 0, -4 );

	assert( !COMPLEX_NUMBER_INLINE_EQUAL( a, b ) );
	assert( !COMPLEX_NUMBER_INLINE_EQUAL( b, a ) );
	assert( COMPLEX_NUMBER_INLINE_NOT_EQUAL( a, b ) );
	assert( COMPLEX_NUMBER_INLINE_NOT_EQUAL( b, a ) );

	// Now that we're convinced that ComplexNumber's init and equal work,
	// let's test add, then sub, then mult. (Along with Format)

	// Add

	COMPLEX_NUMBER_INLINE_INIT( a, 4, 3 );
	COMPLEX_NUMBER_INLINE_INIT( b, 5, -7 );

	COMPLEX_NUMBER_INLINE_ADD( c, a, b );

	COMPLEX_NUMBER_INLINE_INIT( witness, 9, -4 );
	assert( COMPLEX_NUMBER_INLINE_EQUAL( c, witness ) );

	bzero( buffer, sizeof( buffer ) );
	COMPLEX_NUMBER_INLINE_FORMAT_APPEND( buffer, c );
	assert( !strcmp( "(9 + -4i)", buffer ) );

	// Sub

	COMPLEX_NUMBER_INLINE_INIT( a, 4, 3 );
	COMPLEX_NUMBER_INLINE_INIT( b, 5, -7 );

	COMPLEX_NUMBER_INLINE_SUB( c, a, b );

	COMPLEX_NUMBER_INLINE_INIT( witness, -1, 10 );
	assert( COMPLEX_NUMBER_INLINE_EQUAL( c, witness ) );

	bzero( buffer, sizeof( buffer ) );
	COMPLEX_NUMBER_INLINE_FORMAT_APPEND( buffer, c );
	assert( !strcmp( "(-1 + 10i)", buffer ) );

	// Mult

	COMPLEX_NUMBER_INLINE_INIT( a, 4, 3 );
	COMPLEX_NUMBER_INLINE_INIT( b, 5, -7 );

	COMPLEX_NUMBER_INLINE_MULT( c, a, b );

	COMPLEX_NUMBER_INLINE_INIT( witness, 41, -13 );
	assert( COMPLEX_NUMBER_INLINE_EQUAL( c, witness ) );

	bzero( buffer, sizeof( buffer ) );
	COMPLEX_NUMBER_INLINE_FORMAT_APPEND( buffer, c );
	assert( !strcmp( "(41 + -13i)", buffer ) );
}

int
main()
{
	basic();
	fast();
}
