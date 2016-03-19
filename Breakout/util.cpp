#include "stdafx.h"
#include "util.h"

//-----------------------------------------------------------------------------
// Angle -> Basis Vectors
//-----------------------------------------------------------------------------

void AngleVectors( const float flAngle, Vector *forward )
{
	assert( forward );

	float sin, cos;

	SinCos( DEG2RAD( flAngle ), &sin, &cos );

	forward->x = cos;
	forward->y = sin;
}

void AngleVectors( const float flAngle, Vector *forward, Vector *right )
{
	float sin, cos;

	SinCos( DEG2RAD( flAngle ), &sin, &cos );

	if ( forward )
	{
		forward->x = cos;
		forward->y = sin;
	}

	if ( right )
	{
		right->x = -sin;
		right->y = cos;
	}
}

//-----------------------------------------------------------------------------
// Forward direction vector -> Angle
//-----------------------------------------------------------------------------

void VectorAngles( const Vector& forward, float &flAngle, bool bNormalize /*= true*/ )
{
	if ( forward.y == 0 && forward.x == 0 )
	{
		flAngle = 0;
	}
	else
	{
		flAngle = atan2( forward.y, forward.x ) * ( 180.0f / M_PI_F );
		
		if ( bNormalize && flAngle < 0 )
			flAngle += 360;
		
	}
}

float Approach( float target, float value, float speed )
{
	float delta = target - value;

	if ( delta > speed )
		value += speed;
	else if ( delta < -speed )
		value -= speed;
	else
		value = target;

	return value;
}

float AngleNormalize( float angle )
{
	angle = fmodf( angle, 360.0f );
	if ( angle > 180 )
	{
		angle -= 360;
	}
	if ( angle < -180 )
	{
		angle += 360;
	}
	return angle;
}

//--------------------------------------------------------------------------------------------------------------
// ensure that 0 <= angle <= 360
float AngleNormalizePositive( float angle )
{
	angle = fmodf( angle, 360.0f );

	if ( angle < 0.0f )
	{
		angle += 360.0f;
	}

	return angle;
}
