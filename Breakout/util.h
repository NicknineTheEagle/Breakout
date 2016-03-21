#ifndef UTIL_H
#define UTIL_H

#ifdef _WIN32
#pragma once
#endif

// Using ARRAYSIZE implementation from winnt.h:
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif

template <typename T, size_t N>
char( *RtlpNumberOf( T( &)[N] ) )[N];

#define RTL_NUMBER_OF_V2(A) (sizeof(*RtlpNumberOf(A)))
#define RTL_NUMBER_OF_V1(A) (sizeof(A)/sizeof((A)[0]))

// ARRAYSIZE is more readable version of RTL_NUMBER_OF_V2
// _ARRAYSIZE is a version useful for anonymous types
#define ARRAYSIZE(A)	RTL_NUMBER_OF_V2(A)
#define _ARRAYSIZE(A)	RTL_NUMBER_OF_V1(A)


#define XRES(x)	( x  * ( ( float )g_ScreenRect.width / 1024.0 ) )
#define YRES(y)	( y  * ( ( float )g_ScreenRect.height / 768.0 ) )


#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.

// NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system.
#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif


template< class T >
inline T clamp( T const &val, T const &minVal, T const &maxVal )
{
	if ( maxVal < minVal )
		return maxVal;
	else if ( val < minVal )
		return minVal;
	else if ( val > maxVal )
		return maxVal;
	else
		return val;
}

//-----------------------------------------------------------------------------
// Remap a value in the range [A,B] to [C,D].
//-----------------------------------------------------------------------------
inline float RemapVal( float val, float A, float B, float C, float D )
{
	if ( A == B )
		return val >= B ? D : C;
	return C + ( D - C ) * ( val - A ) / ( B - A );
}

inline float RemapValClamped( float val, float A, float B, float C, float D )
{
	if ( A == B )
		return val >= B ? D : C;
	float cVal = ( val - A ) / ( B - A );
	cVal = clamp( cVal, 0.0f, 1.0f );

	return C + ( D - C ) * cVal;
}

//-----------------------------------------------------------------------------
// Vector
//-----------------------------------------------------------------------------
inline float VectorLength( const Vector &vec )
{
	return sqrtf( vec.x*vec.x + vec.y*vec.y );
}

inline float VectorLengthSqr( const Vector &vec )
{
	return vec.x*vec.x + vec.y*vec.y;
}

inline float VectorNormalize( Vector &vec )
{
	float radius = VectorLength( vec );

	// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
	float iradius = 1.f / ( radius + FLT_EPSILON );

	vec.x *= iradius;
	vec.y *= iradius;

	return radius;
}

void inline SinCos( float radians, float *sine, float *cosine )
{
	*sine = sinf( radians );
	*cosine = cosf( radians );
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
inline float DotProduct( const Vector& a, const Vector& b )
{
	return ( a.x*b.x + a.y*b.y );
}

// projection of vector v1 onto v2
inline Vector VectorProject( const Vector& v1, const Vector& v2 )
{
	float v2_ls = VectorLengthSqr( v2 );
	return v2 * ( DotProduct( v2, v1 ) / v2_ls );
}

void AngleVectors( const float flAngle, Vector *forward );
void AngleVectors( const float flAngle, Vector *forward, Vector *right );
void VectorAngles( const Vector &forward, float &flAngle, bool bNormalize = true );

float Approach( float target, float value, float speed );

float AngleNormalize( float angle );
// ensure that 0 <= angle <= 360
float AngleNormalizePositive( float angle );

//-----------------------------------------------------------------------------
// Random
//-----------------------------------------------------------------------------
inline int RandomInt( int min, int max )
{
	return ( rand() % ( max - min ) + min );
}

inline float RandomFloat( float min, float max )
{
	return ( min + static_cast<float>( rand() ) / ( static_cast<float>( RAND_MAX / ( max - min ) ) ) );
}

#endif // UTIL_H
