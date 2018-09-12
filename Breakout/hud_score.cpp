#include "stdafx.h"
#include "hud_score.h"
#include "hud_manager.h"
#include "gamelogic.h"
#include "player.h"

static CHudScore g_HudScore;

CHudScore::CHudScore()
{
	Reset();
	g_HUDManager.RegisterHUDElement( this );
}

//-----------------------------------------------------------------------------
// Purpose: Set font and offset on strings.
//-----------------------------------------------------------------------------
bool CHudScore::Init( void )
{
	float flOffset = 0.0f;

	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		m_textScores[i] = new sf::Text( "", g_MainFont, 30 );
		m_textScores[i]->setFillColor( g_aPlayerColors[i] );
		m_textScores[i]->setPosition( Vector( 0, flOffset ) );

		flOffset += 35.0f;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Don't draw on win screen
//-----------------------------------------------------------------------------
bool CHudScore::ShouldDraw( void )
{
	if ( g_pGameLogic->GetWinningPlayer() != -1 )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHudScore::Reset( void )
{
	memset( m_flScores, 0, MAX_PLAYERS * sizeof( float ) );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHudScore::Update( void )
{
	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		CPlayer *pPlayer = ToPlayer( g_EntityList[i] );
		if ( !pPlayer )
			continue;

		float flPlayerScore = (float)pPlayer->GetScore();
		if ( flPlayerScore != m_flScores[i] )
		{
			float flSpeed = flPlayerScore > m_flScores[i] ? RemapVal( m_flScores[i], flPlayerScore - 50, flPlayerScore, 100, 5 ) : 1000;
			m_flScores[i] = Approach( flPlayerScore, m_flScores[i], flSpeed * g_FrameTime );
		}

		char szScore[64];
		sprintf( szScore, "P%d SCORE: %.f", i + 1, m_flScores[i] );
		m_textScores[i]->setString( szScore );

		g_pGameLogic->GetWindow()->draw( *m_textScores[i] );
	}
}
