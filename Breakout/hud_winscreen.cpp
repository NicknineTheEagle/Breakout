#include "stdafx.h"
#include "hud_winscreen.h"
#include "hud_manager.h"
#include "gamelogic.h"
#include "player.h"

CWinScreen g_HUDWinScreen;

const char *g_aStatNames[] =
{
	"Blocks",
	"Coins",
	"Deaths"
};

CWinScreen::CWinScreen()
{
	Reset();
	g_HUDManager.RegisterHUDElement( this );
}

//-----------------------------------------------------------------------------
// Purpose: Set fonts, sizes etc.
//-----------------------------------------------------------------------------
bool CWinScreen::Init( void )
{
	Vector vecCenter( g_ScreenRect.width / 2, g_ScreenRect.height / 2 );

	// Winned rounds counters.
	for ( size_t i = 0; i < MAX_PLAYERS; i++ )
	{
		m_textWins[i] = sf::Text( "0", g_MainFont, 40 );
		m_textWins[i].setColor( g_aPlayerColors[i] );

		float flFontHeight = (float)m_textWins[i].getCharacterSize();
		sf::FloatRect textBounds = m_textWins[i].getLocalBounds();
		m_textWins[i].setOrigin( textBounds.width / 2, flFontHeight );
	}

	// Winner name.
	m_textWinHeader = sf::Text( "", g_MainFont, 36 );
	m_textWinHeader.setPosition( vecCenter );

	float flOffset = 41.0f;
	for ( size_t i = 0; i < MAX_PLAYERS * 2; i++ )
	{
		const float flFontHeight = 30.0f;
		m_textWinScores[i].setFont( g_MainFont );
		m_textWinScores[i].setCharacterSize( 30 );

		if ( i % 2 == 0 )
		{
			// Player name.
			char szScore[32];
			sprintf( szScore, "P%d SCORE:", i / 2 + 1 );
			m_textWinScores[i].setString( szScore );

			sf::FloatRect textBounds = m_textWinScores[i].getLocalBounds();
			m_textWinScores[i].setOrigin( 1.0f, flFontHeight );
			m_textWinScores[i].setPosition( vecCenter + Vector( -113, flOffset ) );
			m_textWinScores[i].setColor( g_aPlayerColors[i / 2] );
		}
		else
		{
			// Their score.
			m_textWinScores[i].setOrigin( 1.0f, flFontHeight );
			Vector vecOffset( m_textWinScores[i - 1].getLocalBounds().width + 10, 0 );
			m_textWinScores[i].setPosition( m_textWinScores[i - 1].getPosition() + vecOffset );
			m_textWinScores[i].setColor( g_aPlayerColors[( i - 1 ) / 2] );

			flOffset += flFontHeight + 5.0f;
		}
	}

	// Keys.
	m_textWinKeys[0] = sf::Text( "R: Restart", g_MainFont, 24 );
	m_textWinKeys[1] = sf::Text( "Esc: Quit", g_MainFont, 24 );

	for ( size_t i = 0; i < ARRAYSIZE( m_textWinKeys ); i++ )
	{
		float flFontHeight = (float)m_textWinKeys[i].getCharacterSize();
		sf::FloatRect textBounds = m_textWinKeys[i].getLocalBounds();
		m_textWinKeys[i].setOrigin( textBounds.width / 2, flFontHeight );
	}

	// Restart countdown.
	m_textCountdown = sf::Text( "Restarting in 5", g_MainFont, 16 );
	float flFontHeight = (float)m_textCountdown.getCharacterSize();
	sf::FloatRect textBounds = m_textCountdown.getLocalBounds();
	m_textCountdown.setOrigin( textBounds.width / 2, flFontHeight );

	// Stats.
	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		m_textStats[i] = sf::Text( "", g_MainFont, 20 );
		m_textStats[i].setColor( g_aPlayerColors[i] );
		m_textStats[i].setPosition( Vector( (float)( 128 * i ), 0 ) );
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Draw this once a match is finished.
//-----------------------------------------------------------------------------
bool CWinScreen::ShouldDraw( void )
{
	if ( g_pGameLogic->GetWinningPlayer() != -1 )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWinScreen::Reset( void )
{
	memset( m_flScores, 0, sizeof( float ) * MAX_PLAYERS );
	m_bShouldAutoRestart = false;
	m_bFinishedCounting = false;
}

//-----------------------------------------------------------------------------
// Purpose: Draw panel, count scores up.
//-----------------------------------------------------------------------------
void CWinScreen::Update( void )
{
	sf::RenderWindow *pWindow = g_pGameLogic->GetWindow();

	pWindow->draw( m_textWinHeader );

	bool bFinished = true;
	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		CPlayer *pPlayer = ToPlayer( g_EntityList[i] );
		if ( !pPlayer )
			continue;

		pWindow->draw( m_textWins[i] );

		int index = i * 2;

		float flPlayerScore = (float)pPlayer->GetScore();
		if ( flPlayerScore != m_flScores[i] )
		{
			m_flScores[i] = Approach( flPlayerScore, m_flScores[i], 1000.0f * g_FrameTime );
			bFinished = false;
		}

		char szScore[8];
		sprintf( szScore, "%.f", m_flScores[i] );
		m_textWinScores[index + 1].setString( szScore );

		pWindow->draw( m_textWinScores[index] );
		pWindow->draw( m_textWinScores[index + 1] );

		pWindow->draw( m_textStats[i] );
	}

	if ( bFinished )
	{
		// If there're no human players restart the game in 5 seconds.
		if ( m_bShouldAutoRestart )
		{
			if ( !m_bFinishedCounting )
			{
				m_bFinishedCounting = true;
				m_Clock.restart();
			}
			else
			{
				char szTime[64];
				sprintf( szTime, "Restarting in %.f", ceil( 5.0f - m_Clock.getElapsedTime().asSeconds() ) );
				m_textCountdown.setString( szTime );

				pWindow->draw( m_textCountdown );
			}
		}

		for ( size_t i = 0; i < ARRAYSIZE( m_textWinKeys ); i++ )
		{
			pWindow->draw( m_textWinKeys[i] );
		}
	}

	if ( m_bShouldAutoRestart && m_bFinishedCounting && m_Clock.getElapsedTime().asSeconds() >= 5.0f )
	{
		g_pGameLogic->RestartGame();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Set panel parameters.
//-----------------------------------------------------------------------------
void CWinScreen::SetWinScreenInfo( int iWinningPlayer )
{
	Vector vecCenter( g_ScreenRect.width / 2, g_ScreenRect.height / 2 );

	// Set winner's name.
	char szWinner[32];
	if ( iWinningPlayer < MAX_PLAYERS )
	{
		sprintf( szWinner, "PLAYER %d WINS", iWinningPlayer + 1 );
	}
	else
	{
		const char *pszString = "DRAW";
		strcpy( szWinner, pszString );
	}
	m_textWinHeader.setString( szWinner );

	float flFontHeight = (float)m_textWinHeader.getCharacterSize();
	sf::FloatRect textBounds = m_textWinHeader.getLocalBounds();
	m_textWinHeader.setOrigin( textBounds.width / 2.0f, flFontHeight );
	if ( iWinningPlayer < MAX_PLAYERS )
	{
		m_textWinHeader.setColor( g_aPlayerColors[iWinningPlayer] );
	}
	else
	{
		m_textWinHeader.setColor( sf::Color::White );
	}

	float flOffset = (float)m_textWinHeader.getCharacterSize() + 5.0f;
	int iPlayers = CountPlayers();
	float flWinsOffset = -64.0f * (float)( iPlayers - 1 );
	float flWinsPos = 0.0f;

	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		CPlayer *pPlayer = ToPlayer( g_EntityList[i] );
		if ( !pPlayer )
			continue;

		int index = i * 2;

		// Wins counter.
		m_textWins[i].setPosition( vecCenter + Vector( flWinsOffset + flWinsPos, -128 ) );
		flWinsPos += 128.0f;

		char szWins[8];
		sprintf( szWins, "%d", g_pGameLogic->m_iWins[i] );
		m_textWins[i].setString( szWins );

		// Player name.
		m_textWinScores[index].setPosition( vecCenter + Vector( -113, flOffset ) );

		// Their score.
		Vector vecOffset( m_textWinScores[index].getLocalBounds().width + 10, 0 );
		m_textWinScores[index + 1].setPosition( m_textWinScores[index].getPosition() + vecOffset );

		flOffset += (float)m_textWinScores[index].getCharacterSize() + 5.0f;

		// Stats.
		char szStats[256];
		sprintf( szStats, "P%d\n", i + 1 );
		for ( int stat = 0; stat < NUM_STATS; stat++ )
		{
			char szBuf[64];
			sprintf( szBuf, "%s: %d\n", g_aStatNames[stat], pPlayer->GetStat( stat ) );
			strcat( szStats, szBuf );
		}
		m_textStats[i].setString( szStats );
	}

	// Keys.
	flOffset += 20.0f;
	for ( size_t i = 0; i < ARRAYSIZE( m_textWinKeys ); i++ )
	{
		m_textWinKeys[i].setPosition( vecCenter + Vector( 0, flOffset ) );
		flOffset += (float)m_textWinKeys[i].getCharacterSize() + 5.0f;
	}

	// Restart countdown.
	flOffset += 10.0f;
	m_textCountdown.setPosition( vecCenter + Vector( 0, flOffset ) );

	// Auto-restart the game if no human player is present.
	m_bShouldAutoRestart = true;
	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		CBaseEntity *pPlayer = g_EntityList[i];

		if ( pPlayer && !pPlayer->IsBot() )
		{
			m_bShouldAutoRestart = false;
			break;
		}
	}
}
