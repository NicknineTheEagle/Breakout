#include "stdafx.h"
#include "block.h"
#include "ball.h"
#include "coin.h"
#include "player.h"
#include "hud_scoreaccount.h"
#include "bot_manager.h"

static int s_aBlockScores[] =
{
	50,
	60,
	70,
	80,
	90,
	100,
	110,
	120,
};

static const sf::IntRect s_BlockSprite( 0, 0, 31, 31 );

CBlock::CBlock()
{
	SetSolid( SOLID_YES );
	SetSprite( s_BlockSprite );
	SetCollisionBounds( -Vector( 15, 15 ), Vector( 15, 15 ) );
	m_iType = BLOCK_RED;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CBlock::GetType( void )
{
	return m_iType;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBlock::SetType( int iType )
{
	assert( iType >= 0 && iType < BLOCK_COUNT );
	m_iType = iType;

	// Update the sprite according to the type.
	sf::IntRect newRect = s_BlockSprite;
	newRect.left = 40 * m_iType;
	SetSprite( newRect );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBlock::Touch( CBaseEntity *pOther )
{
	if ( pOther->GetEntityID() == ENTITY_BALL )
	{
		CBall *pBall = static_cast<CBall *>( pOther );
		CPlayer *pOwner = pBall->GetOwner();

		if ( pOwner )
		{
			int iScore = s_aBlockScores[GetType()];
			pOwner->IncrementScore( iScore );
			g_HUDScoreAccount.AddNumber( iScore, GetOrigin() );
		}

		if ( RandomInt( 0, 4 ) == 0 )
		{
			Vector vecOrigin = GetOrigin();
			Vector vecDir( 0, 1 ); // Down.

			// Move coin towards the player that destroyed the block.
			if ( pOwner )
			{
				AngleVectors( pOwner->GetAngle(), &vecDir );
				vecDir = -vecDir;
			}

			CreateEntity<CCoin>( vecOrigin, vecDir * COIN_SPEED );
		}

		Remove();

		// Tell bots that I got destroyed.
		g_BotManager.OnBlockDestroyed( this );
	}
}
