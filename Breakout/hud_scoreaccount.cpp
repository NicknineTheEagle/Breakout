#include "stdafx.h"
#include "hud_scoreaccount.h"
#include "hud_manager.h"
#include "gamelogic.h"
#include "util.h"

CTextManager g_HUDScoreAccount;

CTextManager::CTextManager()
{
	m_iDeltaHead = 0;
	g_HUDManager.RegisterHUDElement( this );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTextManager::Init( void )
{
	if ( m_Font.loadFromFile( "fonts/TF2Secondary.ttf" ) == false )
		return false;
	
	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTextManager::ShouldDraw( void )
{
	if ( g_pGameLogic->IsPaused() )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Reset HUD element.
//-----------------------------------------------------------------------------
void CTextManager::Reset( void )
{
	m_iDeltaHead = 0;

	for ( int i = 0; i < 10; i++ )
	{
		m_aTexts[i].flDieTime = 0.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTextManager::AddText( const char *pszText, const Vector &vecPos, bool bMinus /*= false*/ )
{
	TextDelta_t *pNewDelta = &m_aTexts[m_iDeltaHead];
	m_iDeltaHead++;
	m_iDeltaHead %= 10;

	strncpy( pNewDelta->text, pszText, 128 );
	pNewDelta->vecPos = vecPos;
	pNewDelta->flDieTime = g_CurTime + 1.5f;
	pNewDelta->bMinus = bMinus;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTextManager::AddNumber( int iNumber, const Vector &vecPos )
{
	TextDelta_t *pNewDelta = &m_aTexts[m_iDeltaHead];
	m_iDeltaHead++;
	m_iDeltaHead %= 10;

	sprintf( pNewDelta->text, "%d", iNumber );
	pNewDelta->vecPos = vecPos;
	pNewDelta->flDieTime = g_CurTime + 1.5f;
	pNewDelta->bMinus = iNumber < 0;
}

//-----------------------------------------------------------------------------
// Purpose: Update all deltas that are not expired.
//-----------------------------------------------------------------------------
void CTextManager::Update( void )
{
	for ( int i = 0; i < 10; i++ )
	{
		if ( m_aTexts[i].flDieTime < g_CurTime )
			continue;

		sf::Color deltaColor( 255, 15, 15, 255 );

		float flLifetimePercent = ( m_aTexts[i].flDieTime - g_CurTime ) / 1.5f;

		if ( flLifetimePercent < 0.5f )
		{
			deltaColor.a = (char)( 255.0f * ( flLifetimePercent / 0.5 ) );
		}

		if ( !m_aTexts[i].bMinus )
		{
			deltaColor.r = (char)RemapValClamped( flLifetimePercent, 0.0f, 1.0f, 255, 30 );
			deltaColor.g = (char)RemapValClamped( flLifetimePercent, 0.0f, 1.0f, 30, 255 );
		}

		float flHeight = 50.0f;
		float flXPos = m_aTexts[i].vecPos.x;
		float flYPos = m_aTexts[i].vecPos.y - ( 1.0f - flLifetimePercent ) * flHeight;

		sf::Text newText( m_aTexts[i].text, m_Font );
		sf::FloatRect textBounds = newText.getLocalBounds();
		newText.setOrigin( textBounds.width / 2.0f, textBounds.height );
		newText.setPosition( flXPos, flYPos );
		newText.setFillColor( deltaColor );

		g_pGameLogic->GetWindow()->draw( newText );
	}
}
