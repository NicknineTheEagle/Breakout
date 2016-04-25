#include "stdafx.h"
#include "coin.h"
#include "player.h"
#include "hud_scoreaccount.h"

#define COIN_ANIM_FRAMES 16
static const sf::IntRect s_CoinSprite( 0, 376, 24, 24 );

#define COIN_SCORE 100

CCoin::CCoin()
{
	SetSprite( s_CoinSprite );
	SetSolid( SOLID_TRIGGER );
	SetCollisionBounds( -Vector( 12, 12 ), Vector( 12, 12 ) );
	m_flAnimTime = 0.0f;
	m_iAnimFrame = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Set the sprite to the next animation frame.
//-----------------------------------------------------------------------------
void CCoin::AnimFrameAdvance( void )
{
	m_iAnimFrame++;
	m_iAnimFrame %= COIN_ANIM_FRAMES;

	sf::IntRect newSprite = s_CoinSprite;
	newSprite.left = m_iAnimFrame * s_CoinSprite.width;
	SetSprite( newSprite );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCoin::ProcessMovement( void )
{
	if ( m_flAnimTime < g_CurTime )
	{
		AnimFrameAdvance();
		m_flAnimTime = g_CurTime + 0.05f;
	}

	CBaseEntity::ProcessMovement();
}

//-----------------------------------------------------------------------------
// Purpose: Increment player's score.
//-----------------------------------------------------------------------------
void CCoin::Touch( CBaseEntity *pOther )
{
	if ( pOther->IsPlayer() )
	{
		CPlayer *pPlayer = static_cast<CPlayer *>( pOther );
		pPlayer->IncrementScore( COIN_SCORE );
		pPlayer->IncrementStat( STAT_COINS_COLLECTED );
		g_HUDScoreAccount.AddNumber( COIN_SCORE, GetOrigin() );

		Remove();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCoin::TouchScreenEdge( const Vector &vecPoint )
{
	// Remove coin once it's completely off-screen.
	Vector vecMins = vecPoint + GetMins();
	Vector vecMaxs = vecPoint + GetMaxs();
	Vector vecScreenMins = Vector( g_ScreenRect.left, g_ScreenRect.top );
	Vector vecScreenMaxs = Vector( g_ScreenRect.width, g_ScreenRect.height );

	if ( !IsBoxIntersectingBox( vecMins, vecMaxs, vecScreenMins, vecScreenMaxs ) )
	{
		Remove();
	}
}
