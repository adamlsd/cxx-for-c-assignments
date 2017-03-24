#ifndef COMPLEX_INLINE_NUMBER_HEADER
#define COMPLEX_INLINE_NUMBER_HEADER

/*
 * This is a basic complex number type with an inline design.
 * This complex number has the following advantages and disadvantages:
 *
 * Advantages:
 * - Direct calls to library will be inlined and will be very fast.
 * - ComplexNumber "objects" can be locals with automatic lifetime.
 * - No lost size and optimization opportunities from `ComplexNumber` layout being hidden from compiler.
 * - Very minimal in-memory size: just raw `ComplexNumber` objects themselves.  No extra overhead.
 * - Potentially smaller binary code size, due to optimization opportunities of inlining
 *   lots of simple arithmetic operations.
 * - Potentially smaller binary code size, due to eliminating instruction overhead of a function call and
 *   prologue and epilogue being larger than the actual implementation instructions.
 * - Compiler may use intrinsics and asm instructions to keep an entire complex number in a pair of
 *   FPU registers.
 *
 * Disadvantages:
 * - API is ugly as sin -- we cannot represent mathematical equations using sensible notation.
 * - Binary incompatible with client code, after layout changes -- all code using it must be recompiled.
 * - Potentially larger binary code size, due to instruction overhead of multiple inline
 *   emissions of common code.
 * - Potentially slower performance due to lots of cache-line consumption due to more inlined
 *   copies of common code.
 * - Potentially larger binary code size, due to missed optimization opportunities by inlining
 *   lots of simple arithmetic operations, in favor of possibly more important operations.
 * - Must recompile whole codebase to replace implementation for new functionality for performance
 *   and for bugs.
 * - Library internal layout is accessible to client code, so invariants cannot be guaranteed immune
 *   from unauthorized alterations.
 * - Dangerous to use, because of potential macro-expansion and parameter reevaluation.
 */

struct ComplexNumberInline
{
	double real;
	double imag;
};

#define COMPLEX_NUMBER_INLINE_INIT( res, r, i ) \
		do{ ComplexNumberInline *p= &(res); p->real= r; p->imag=i; } while( 0 )

#define COMPLEX_NUMBER_INLINE_REAL( res ) ( (res).real )
#define COMPLEX_NUMBER_INLINE_IMAG( res ) ( (res).imag )


// Avoid side-effects -- multiple evaluation!
#define COMPLEX_NUMBER_INLINE_EQUAL( left, right ) \
		( ( left ).real == ( right ).real && ( left ).imag == ( right ).imag )

// Avoid side-effects -- multiple evaluation!
#define COMPLEX_NUMBER_INLINE_NOT_EQUAL( left, right ) \
		( !COMPLEX_NUMBER_INLINE_EQUAL( ( left ), ( right ) ) )

#define COMPLEX_NUMBER_INLINE_ADD( result, left, right ) \
	do \
	{ \
		ComplexNumberInline *l= &( left ); \
		ComplexNumberInline *r= &( right ); \
		ComplexNumberInline *res= &( result ); \
		res->real= l->real + r->real;\
		res->imag= l->imag + r->imag;\
	} \
	while( 0 )

#define COMPLEX_NUMBER_INLINE_SUB( result, left, right ) \
	do \
	{ \
		ComplexNumberInline *l= &( left ); \
		ComplexNumberInline *r= &( right ); \
		ComplexNumberInline *res= &( result ); \
		res->real= l->real - r->real;\
		res->imag= l->imag - r->imag;\
	} \
	while( 0 )

#define COMPLEX_NUMBER_INLINE_IMPL_MULT_BY_IMAG( result, left, iii ) \
do \
{ \
	ComplexNumberInline tmp; \
	ComplexNumberInline *res= &( result ); \
	ComplexNumberInline *l= &( left ); \
	tmp.real= -( iii * l->imag ); \
	tmp.imag= iii * l->real; \
	res->real= tmp.real; \
	res->imag= tmp.imag; \
} \
while( 0 )

#define COMPLEX_NUMBER_INLINE_IMPL_MULT_BY_REAL( result, left, rrr ) \
do \
{ \
	ComplexNumberInline tmp; \
	ComplexNumberInline *res= &( result ); \
	ComplexNumberInline *l= &( left ); \
	tmp.real= l->real * rrr; \
	tmp.imag= l->imag * rrr; \
	res->real= tmp.real; \
	res->imag= tmp.imag; \
} \
while( 0 )

#define COMPLEX_NUMBER_INLINE_MULT( result, left, right ) \
	do \
	{ \
		ComplexNumberInline *lhs= &( left ); \
		ComplexNumberInline *rhs= &( right ); \
		ComplexNumberInline *res= &( result ); \
		ComplexNumberInline first; \
		ComplexNumberInline second; \
		ComplexNumberInline tmp; \
		double rr= rhs->real; \
		double ri= rhs->imag; \
		COMPLEX_NUMBER_INLINE_IMPL_MULT_BY_REAL( first, *lhs, rr ); \
		COMPLEX_NUMBER_INLINE_IMPL_MULT_BY_IMAG( second, *lhs, ri ); \
		COMPLEX_NUMBER_INLINE_ADD( tmp, first, second ); \
		res->real= tmp.real; \
		res->imag= tmp.imag; \
	} \
	while( 0 )

#define COMPLEX_NUMBER_INLINE_FORMAT_APPEND( s, value ) \
do \
{ \
	ComplexNumberInline *v= &( value ); \
	sprintf( ( s ), "(%g + %gi)", v->real, v->imag ); \
} \
while( 0 )

#endif /*** COMPLEX_INLINE_NUMBER_HEADER ***/
