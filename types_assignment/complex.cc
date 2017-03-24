#include "complex.h"
#include <stdlib.h>
#include <stdio.h>

struct ComplexNumber
{
	double real;
	double imag;
};

ComplexNumber *
ComplexNumber_alloc()
{
	return reinterpret_cast< ComplexNumber * >( malloc( sizeof( ComplexNumber ) ) );
}

void
ComplexNumber_free( ComplexNumber *p )
{
	free( p );
}

void
ComplexNumber_init( ComplexNumber *result, double real, double imag )
{
	result->real= real;
	result->imag= imag;
}

double
ComplexNumber_real( const ComplexNumber *value )
{
	return value->imag;
}

double
ComplexNumber_imag( const ComplexNumber *value )
{
	return value->imag;
}

int
ComplexNumber_equal( const ComplexNumber *left, const ComplexNumber *right )
{
	return left->real == right->real && left->imag == right->imag;
}

int
ComplexNumber_not_equal( const ComplexNumber *left, const ComplexNumber *right )
{
	return !ComplexNumber_equal( left, right );
}

void
ComplexNumber_add( ComplexNumber *result, const ComplexNumber *left, const ComplexNumber *right )
{
	result->real= left->real + right->real;
	result->imag= left->imag + right->imag;
}

void
ComplexNumber_sub( ComplexNumber *result, const ComplexNumber *left, const ComplexNumber *right )
{
	result->real= left->real - right->real;
	result->imag= left->imag - right->imag;
}

static void
multByImag( ComplexNumber *result, const ComplexNumber *left, double imag )
{
	result->real= -( imag * left->imag );
	result->imag= imag * left->real;
}

static void
multByReal( ComplexNumber *result, const ComplexNumber *left, double real )
{
	result->real= left->real * real;
	result->imag= left->imag * real;
}

void
ComplexNumber_mult( ComplexNumber *result, const ComplexNumber *left, const ComplexNumber *right )
{
	ComplexNumber first;
	ComplexNumber second;
	ComplexNumber tmp;

	multByReal( &first, left, right->real );
	multByImag( &second, left, right->imag );

	ComplexNumber_add( &tmp, &first, &second );

	result->real= tmp.real;
	result->imag= tmp.imag;
}

void
ComplexNumber_formatAppend( char *s, const ComplexNumber *value )
{
	sprintf( s, "(%g + %gi)", value->real, value->imag );
}
