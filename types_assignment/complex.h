#ifndef COMPLEX_NUMBER_HEADER
#define COMPLEX_NUMBER_HEADER

/*
 * This is a basic complex number type, with a private-implementation design.
 * This complex number has the following advantages and disadvantages:
 *
 * Advantages:
 * - Binary compatible with client code, after layout changes.
 * - Single compiled library to replace for new functionality for performance and for bugs.
 * - Library internal layout is inaccessible to client code, so invariants can be guaranteed immune
 *   from unauthorized alterations.
 * - Potentially smaller binary code size, due to single implementation in the codebase.
 * - Safe to use, because there is no macro-expansion and parameter reevaluation.
 *
 * Disadvantages:
 * - Indirect calls to library cannot be inlined and will be slow
 *   - Calling this library by shared-object (DLL or .so) will be VERY slow
 * - API is ugly as sin -- we cannot represent mathematical equations using sensible notation
 * - ComplexNumber "objects" must be allocated
 * - ComplexNumber "objects" must be deallocated
 * - Potentially larger binary code size, due to instruction overhead of a function call and
 *   prologue and epilogue being larger than the actual implementation instructions.
 * - Potentially larger binary code size, due to missed optimization opportunities from inlining
 *   simple arithmetic operations.
 * - Lost size and optimization opportunities from `ComplexNumber` layout being hidden from compiler.
 * - Larger in-memory size than raw `ComplexNumber` objects themselves -- each Complex number now
 *   consumes an additional pointer.  With 12-byte floating point numbers this is an extra 14% or
 *   so on 32-bit platforms and 25% on 64-bit platforms.
 */
struct ComplexNumber;

extern ComplexNumber *ComplexNumber_alloc();
extern void ComplexNumber_free( ComplexNumber * );

extern void ComplexNumber_init( ComplexNumber *result, double real, double imag );

extern double ComplexNumber_real( const ComplexNumber *value );
extern double ComplexNumber_imag( const ComplexNumber *value );

extern int ComplexNumber_equal( const ComplexNumber *left, const ComplexNumber *right );
extern int ComplexNumber_not_equal( const ComplexNumber *left, const ComplexNumber *right );

extern void ComplexNumber_add( ComplexNumber *result, const ComplexNumber *left, const ComplexNumber *right );

extern void ComplexNumber_sub( ComplexNumber *result, const ComplexNumber *left, const ComplexNumber *right );

extern void ComplexNumber_mult( ComplexNumber *result, const ComplexNumber *left, const ComplexNumber *right );

extern void ComplexNumber_formatAppend( char *s, const ComplexNumber *value );

#endif /*** COMPLEX_NUMBER_HEADER ***/
