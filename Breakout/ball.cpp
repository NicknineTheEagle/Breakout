#include "stdafx.h"
#include "ball.h"
#include "player.h"

static const sf::IntRect s_BallSprite( 160, 240, 24, 24 );

CBall::CBall()
{
	m_pOwner = NULL;
	m_bActive = false;
	m_bDeflected = false;
	SetSprite( s_BallSprite );
	SetSolid( SOLID_NO );
	SetCollisionBounds( -Vector( 12, 12 ), Vector( 12, 12 ) );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CPlayer *CBall::GetOwner( void )
{
	return m_pOwner;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBall::SetOwner( CPlayer *pPlayer )
{
	m_pOwner = pPlayer;

	if ( GetSprite() )
	{
		GetSprite()->setColor( g_aPlayerColors[pPlayer->entindex()] );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CBall::IsActive( void )
{
	return m_bActive;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBall::SetActive( bool bActive )
{
	m_bActive = bActive;
	SetSolid( bActive ? SOLID_YES : SOLID_NO );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBall::Killed( void )
{
    m_vecVelocity = vec2_origin;

    if ( m_pOwner )
    {
        m_pOwner->OnBallLost();
    }

    Remove();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBall::Bounce( int iSide )
{
	if ( IsMarkedForDeletion() )
		return;

	// 0, 1 - bottom, top, 2, 3 - left, right.
	if ( g_EntityList[iSide] && g_EntityList[iSide] == GetOwner() )
	{
		// DED.
		Killed();
	}
	else
	{
		if ( iSide < 2 )
		{
			m_vecVelocity.y *= -1.0f;
		}
		else
		{
			m_vecVelocity.x *= -1.0f;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBall::UpdateOnRemove( void )
{
	if ( m_pOwner && m_pOwner->GetBall() == this )
	{
		m_pOwner->SetBall( NULL );
	}

	CBaseEntity::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBall::ProcessMovement( void )
{
    // Follow player until we go active.
    if ( !IsActive() && m_pOwner )
    {
        m_vecVelocity = vec2_origin;
        Vector vecDir;
        AngleVectors( m_pOwner->GetAngle(), &vecDir );
        Vector vecBallOrigin = m_pOwner->GetOrigin() + vecDir * 25.0f;
        SetOrigin( vecBallOrigin );
    }

	m_bDeflected = false;

    CBaseEntity::ProcessMovement();
}

//-----------------------------------------------------------------------------
// Purpose: Bounce off.
//-----------------------------------------------------------------------------
void CBall::Touch( CBaseEntity *pOther )
{
	if ( pOther->IsPlayer() )
	{
		Vector vecDir, vecToBall, vecProjected, vecForward, vecRight;
		AngleVectors( pOther->GetAngle(), &vecForward, &vecRight );

		// Project the ball direction onto paddle axis.
		vecToBall = GetOrigin() - pOther->GetOrigin();
		vecProjected = VectorProject( vecToBall, vecRight );

		// Now figure out how far the ball is from the paddle center
		// and calc the relative angle.
		float flDist = VectorNormalize( vecProjected );
		flDist *= DotProduct( vecProjected, vecRight );
		float flAngle = RemapValClamped( flDist, -PLAYER_WIDTH, PLAYER_WIDTH, -45, 45 );

		if ( DotProduct( vecToBall, vecForward ) < 0 )
		{
			// The ball is behind the paddle, mirror the angle.
			flAngle = 180 - flAngle;
		}

		// Convert to absolute angle.
		flAngle += pOther->GetAngle();

		//flAngle = AngleNormalizePositive( flAngle );

		AngleVectors( flAngle, &vecDir );
		SetVelocity( vecDir * BALL_SPEED );
	}
	else if ( pOther->IsSolid() && !m_bDeflected )
	{
		// Find collision point.
		const Vector vecRight( 1, 0 ), vecDown( 0, 1 );
		Vector vecPoint, vecOtherMins, vecOtherMaxs;
		pOther->GetAbsBounds( &vecOtherMins, &vecOtherMaxs );

		vecPoint.x = clamp( GetOrigin().x, vecOtherMins.x, vecOtherMaxs.x );
		vecPoint.y = clamp( GetOrigin().y, vecOtherMins.y, vecOtherMaxs.y );

		// Need to make sure that reflection angle is reasonable.
		float flDotRight = DotProduct( m_vecVelocity, vecRight );
		float flDotDown = DotProduct( m_vecVelocity, vecDown );

		if ( ( vecPoint.y == vecOtherMins.y && -flDotDown <= 0 ) ||
			( vecPoint.y == vecOtherMaxs.y && flDotDown <= 0 ) )
		{
			// Vertical bounce.
			m_vecVelocity.y *= -1.0f;
		}
		else if ( ( vecPoint.x == vecOtherMins.x && -flDotRight <= 0 ) ||
			( vecPoint.x == vecOtherMaxs.x && flDotRight <= 0 ) )
		{
			// Horizontal bounce.
			m_vecVelocity.x *= -1.0f;
		}

		m_bDeflected = true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Bounce off screen edge.
//-----------------------------------------------------------------------------
void CBall::TouchScreenEdge( const Vector &vecPoint )
{
	// Figure out collision point.
	Vector vecMins = vecPoint + GetMins();
	Vector vecMaxs = vecPoint + GetMaxs();

	if ( vecMaxs.y > g_ScreenRect.height )
    {
        Bounce( 0 );
    }
	else if ( vecMins.y < 0 )
	{
		Bounce( 1 );
	}

	if ( vecMins.x < 0 )
	{
		Bounce( 2 );
	}
	else if ( vecMaxs.x > g_ScreenRect.width )
	{
		Bounce( 3 );
	}
}
