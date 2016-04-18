#include "stdafx.h"
#include "gamelogic.h"
#include "player.h"
#include "coin.h"
#include "block.h"
#include "hud_manager.h"
#include "hud_winscreen.h"

CGameLogic *g_pGameLogic = NULL;

using namespace sf;

CGameLogic::CGameLogic()
{
	m_pMainWindow = NULL;
	g_CurTime = 0.0f;
	m_flSpeedMult = 1.0f;
	m_bFullScreen = false;
	m_bPaused = false;
	m_bRestart = false;
	m_iWinningPlayer = -1;

	memset( m_aActivePlayers, 0, MAX_PLAYERS * sizeof( int ) );
	m_aActivePlayers[0] = PLAYER_HUMAN;
	m_aActivePlayers[1] = PLAYER_BOT;
	m_bLessBlocks = false;
}

CGameLogic::~CGameLogic()
{
	ClearEntityList();
	delete m_pMainWindow;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CGameLogic::Init( void )
{
	if ( !g_MainTexture.loadFromFile( "images/breakout_sprites.png" ) )
		return false;

	if ( !g_MainFont.loadFromFile( "fonts/arial.ttf" ) )
		return false;

	Vector vecCenter( g_ScreenRect.width / 2, g_ScreenRect.height / 2 );

	m_textPause[0] = Text( "PAUSED", g_MainFont, 36 );
	m_textPause[1] = Text( "Esc: Unpause", g_MainFont, 24 );
	m_textPause[2] = Text( "R: Restart", g_MainFont, 24 );
	m_textPause[3] = Text( "Q: Quit", g_MainFont, 24 );

	float flOffset = 0.0f;
	for ( size_t i = 0; i < ARRAYSIZE( m_textPause ); i++ )
	{
		float flFontHeight = (float)m_textPause[i].getCharacterSize();
		sf::FloatRect textBounds = m_textPause[i].getLocalBounds();
		m_textPause[i].setOrigin( textBounds.width / 2.0f, flFontHeight );
		m_textPause[i].setPosition( vecCenter + Vector( 0, flOffset ) );

		flOffset += flFontHeight + 5.0f;
	}

	if ( !g_HUDManager.Init() )
		return false;

	CreateMainWindow();

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
RenderWindow *CGameLogic::CreateMainWindow( void )
{
	assert( !m_pMainWindow );
	m_pMainWindow = new RenderWindow( VideoMode( 672, 768 ), "Breakout", Style::Default & ~Style::Resize );
	m_pMainWindow->setFramerateLimit( 300 );

	return m_pMainWindow;
}

//-----------------------------------------------------------------------------
// Purpose: Chooses a proper sub-loop.
//-----------------------------------------------------------------------------
void CGameLogic::MainLoop( void )
{
	assert( m_pMainWindow );
	StartGame();

	while ( m_pMainWindow->isOpen() )
	{
		Time elapsedTime = m_Clock.restart();
		g_FrameTime = elapsedTime.asSeconds() * m_flSpeedMult;

		if ( !m_bPaused )
			g_CurTime += g_FrameTime;

		if ( m_iWinningPlayer >= 0 )
			WinLoop();
		else if ( m_bPaused )
			PauseLoop();
		else
			GameLoop();

		if ( m_bRestart )
		{
			// Game restart was scheduled.
			g_CurTime = 0.0f;
			m_iWinningPlayer = -1;
			ClearEntityList();
			g_HUDManager.ResetHUD();
			StartGame();
			m_bRestart = false;
			m_bPaused = false;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Main game loop that runs all required processes
//-----------------------------------------------------------------------------
void CGameLogic::GameLoop( void )
{
	Event event;
	while ( m_pMainWindow->pollEvent( event ) )
	{
		if ( event.type == Event::Closed )
		{
			m_pMainWindow->close();
			break;
		}

#ifdef DEBUG_BUTTONS
		if ( event.type == Event::MouseButtonPressed )
		{
			if ( event.mouseButton.button == Mouse::Left )
			{
				Vector vecClick( (float)event.mouseButton.x, (float)event.mouseButton.y );
				CreateEntity<CBlock>( vecClick, vec2_origin );
			}
			else if ( event.mouseButton.button == Mouse::Right )
			{
				Vector vecClick( (float)event.mouseButton.x, (float)event.mouseButton.y );
				Vector vecFallVel( 0, COIN_SPEED );
				CreateEntity<CCoin>( vecClick, vecFallVel );
			}
		}
		else
#endif // DEBUG_BUTTONS
			if ( event.type == Event::KeyPressed )
			{
				if ( event.key.code == Keyboard::Escape )
				{
					m_bPaused = true;
					PauseDraw();
					break;
				}
#ifdef DEBUG_BUTTONS
				else if ( event.key.code == Keyboard::Add )
				{
					m_flSpeedMult += 0.1f;
				}
				else if ( event.key.code == Keyboard::Subtract )
				{
					m_flSpeedMult -= 0.1f;
				}
				else if ( event.key.code == Keyboard::BackSpace )
				{
					m_flSpeedMult = 1.0f;
				}
				else if ( event.key.code == Keyboard::Delete )
				{
					if ( g_EntityList[0] )
					{
						CPlayer *pPlayer = static_cast<CPlayer *>( g_EntityList[0] );
						pPlayer->ResetScore();
					}
				}
#endif // DEBUG_BUTTONS
			}
	}

	if ( m_bPaused )
		return;

	m_pMainWindow->clear();

	UpdateWorld();
	ProcessDeletionList();
	DrawEntities();
	g_HUDManager.DrawHUD();

#if 0
	char szBuf[256];
	sprintf( szBuf, "Current time: %.3f\nFPS: %.2f\nSpeed: %.1fx", g_CurTime, ( 1.0f / g_FrameTime ), m_flSpeedMult );
	Text textDebug( szBuf, g_MainFont, 30 );
	m_pMainWindow->draw( textDebug );

	float flScorePos = ( (float)textDebug.getCharacterSize() + 5.0f ) * 3;
#endif

	m_pMainWindow->display();

	bool bBlocksLeft = false;
	for ( int i = 0; i < MAX_ENTITIES; i++ )
	{
		if ( g_EntityList[i] && g_EntityList[i]->GetEntityID() == ENTITY_BLOCK )
		{
			bBlocksLeft = true;
			break;
		}
	}

	// If all blocks are destroyed, declare a winner.
	if ( !bBlocksLeft )
	{
		std::vector<CPlayer *> vecPlayers;
		for ( int i = 0; i < MAX_PLAYERS; i++ )
		{
			CPlayer *pPlayer = static_cast<CPlayer *>( g_EntityList[i] );
			if ( pPlayer )
				vecPlayers.push_back( pPlayer );
		}

		bool bSwapped = true;
		size_t numLength = vecPlayers.size();
		for ( size_t i = 1; ( i <= numLength ) && bSwapped; i++ )
		{
			bSwapped = false;
			for ( size_t j = 0; j < ( numLength - 1 ); j++ )
			{
				if ( vecPlayers[j + 1]->GetScore() == vecPlayers[j]->GetScore() )
				{
					m_iWinningPlayer = MAX_PLAYERS;
					break;
				}
				else if ( vecPlayers[j + 1]->GetScore() > vecPlayers[j]->GetScore() )
				{
					std::swap( vecPlayers[j], vecPlayers[j + 1] );
					bSwapped = true;
				}
			}
		}

		if ( m_iWinningPlayer == -1 )
			m_iWinningPlayer = vecPlayers[0]->entindex();

		m_bPaused = true;

		g_HUDWinScreen.SetWinScreenInfo( m_iWinningPlayer );
	}
}

void CGameLogic::WinLoop( void )
{
	Event event;
	while ( m_pMainWindow->pollEvent( event ) )
	{
		if ( event.type == Event::Closed )
		{
			m_pMainWindow->close();
			break;
		}

		if ( event.type == Event::KeyPressed )
		{
			if ( event.key.code == Keyboard::Escape )
			{
				m_pMainWindow->close();
				break;
			}
			else if ( event.key.code == Keyboard::R )
			{
				m_bRestart = true;
				break;
			}
		}
	}

	m_pMainWindow->clear();

	DrawEntities();
	g_HUDManager.DrawHUD();
	
	m_pMainWindow->display();
}

void CGameLogic::PauseLoop( void )
{
	Event event;
	while ( m_pMainWindow->pollEvent( event ) )
	{
		if ( event.type == Event::Closed )
		{
			m_pMainWindow->close();
			break;
		}

		if ( event.type == Event::KeyPressed )
		{
			if ( event.key.code == Keyboard::Q )
			{
				m_pMainWindow->close();
				break;
			}
			else if ( event.key.code == Keyboard::Escape )
			{
				m_bPaused = false;
				break;
			}
			else if ( event.key.code == Keyboard::R )
			{
				m_bRestart = true;
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CGameLogic::PauseDraw( void )
{
	m_pMainWindow->clear();
	DrawEntities();

	for ( size_t i = 0; i < ARRAYSIZE( m_textPause ); i++ )
	{
		m_pMainWindow->draw( m_textPause[i] );
	}

	m_pMainWindow->display();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CGameLogic::StartGame( void )
{
	CreatePlayers();
	CreateBlocks();
}

void CGameLogic::RestartGame( void )
{
	m_bRestart = true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CGameLogic::ClearEntityList( void )
{
	for ( int i = 0; i < MAX_ENTITIES; i++ )
	{
		if ( g_EntityList[i] )
		{
			delete g_EntityList[i];
			g_EntityList[i] = NULL;
		}
	}
}


SGameControls g_aPlayerControls[MAX_PLAYERS] =
{
	{
		Keyboard::Up,
		Keyboard::Down,
		Keyboard::Left,
		Keyboard::Right,
		Keyboard::Numpad1,
		Keyboard::Numpad3,
		Keyboard::RControl,
	},
	{
		Keyboard::W,
		Keyboard::S,
		Keyboard::A,
		Keyboard::D,
		Keyboard::Q,
		Keyboard::E,
		Keyboard::Space,
	},
	{
		Keyboard::Up,
		Keyboard::Down,
		Keyboard::Left,
		Keyboard::Right,
		Keyboard::Numpad1,
		Keyboard::Numpad3,
		Keyboard::RControl,
	},
	{
		Keyboard::Up,
		Keyboard::Down,
		Keyboard::Left,
		Keyboard::Right,
		Keyboard::Numpad1,
		Keyboard::Numpad3,
		Keyboard::RControl,
	},
};

//-----------------------------------------------------------------------------
// Purpose: Creates specified number of players.
//-----------------------------------------------------------------------------
void CGameLogic::CreatePlayers( void )
{
	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		if ( !m_aActivePlayers[i] )
			continue;

		Vector vecOrigin;
		float flAngle = 0.0f;
		switch ( i )
		{
		case 0:
			vecOrigin = Vector( g_ScreenRect.width * 0.5f, g_ScreenRect.height - 17.0f );
			flAngle = 270.0f;
			break;
		case 1:
			vecOrigin = Vector( g_ScreenRect.width * 0.5f, 17.0f );
			flAngle = 90.0f;
			break;
		case 2:
			vecOrigin = Vector( 17.0f, g_ScreenRect.height * 0.5f );
			flAngle = 0.0f;
			break;
		case 3:
			vecOrigin = Vector( g_ScreenRect.width - 17.0f, g_ScreenRect.height * 0.5f );
			flAngle = 180.0f;
			break;
		}

		CPlayer *pPlayer = NULL;
		if ( m_aActivePlayers[i] == PLAYER_HUMAN )
		{
			pPlayer = CreateEntity<CPlayer>( vecOrigin, vec2_origin, i );
		}
		else if ( m_aActivePlayers[i] == PLAYER_BOT )
		{
			pPlayer = CreateEntity<CBot>( vecOrigin, vec2_origin, i );
		}

		pPlayer->SetAngle( flAngle );
		pPlayer->SetControls( g_aPlayerControls[i] );

		// Workaround for the lack of AABB.
		if ( fmodf( flAngle, 180.0f ) == 0.0f )
		{
			pPlayer->SetCollisionBounds( -Vector( PLAYER_HEIGHT, PLAYER_WIDTH ), Vector( PLAYER_HEIGHT, PLAYER_WIDTH ) );
		}

		pPlayer->CreateBall();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Spawn blocks for balls to break.
//-----------------------------------------------------------------------------
void CGameLogic::CreateBlocks( void )
{
#if 1
	int nBlocks = m_bLessBlocks ? 11 : 22;

	// TEMP: Spawn a bunch of blocks.
	for ( int y = 4; y >= 0; y-- )
	{
		for ( int x = 0; x < nBlocks; x++ )
		{
			Vector vecOrigin( x * 30 + 21.0f + 30 * ( 11 - nBlocks / 2 ), g_ScreenRect.height * 0.5f + 30.0f * ( y - 2 ) );
			CBlock *pBlock = CreateEntity<CBlock>( vecOrigin );
			//printf( "0: %g %g\n", vecOrigin.x, vecOrigin.y );

			if ( pBlock )
			{
				pBlock->SetType( RandomInt( 0, BLOCK_COUNT ) );
			}
		}
	}
#else
	std::string strLine;
	std::ifstream fileStream;
	fileStream.open( "level.txt" );

	while ( !fileStream.eof() )
	{
		std::getline( fileStream, strLine );

		int iType = 0;
		Vector vecPos;
		sscanf( strLine.c_str(), "%d: %f %f", &iType, &vecPos.x, &vecPos.y );

		CBlock *pBlock = CreateEntity<CBlock>( vecPos );
		if ( pBlock )
		{
			pBlock->SetType( iType );
		}
	}

	fileStream.close();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Process game physics.
//-----------------------------------------------------------------------------
void CGameLogic::UpdateWorld( void )
{
	for ( int i = 0; i < MAX_ENTITIES; i++ )
	{
		CBaseEntity *pEntity = g_EntityList[i];
		if ( !pEntity )
			continue;

		pEntity->ProcessMovement();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Draw all sprites
//-----------------------------------------------------------------------------
void CGameLogic::DrawEntities( void )
{
	for ( int i = 0; i < MAX_ENTITIES; i++ )
	{
		CBaseEntity *pEntity = g_EntityList[i];
		if ( !pEntity )
			continue;

		Sprite *pSprite = pEntity->GetSprite();
		if ( pSprite )
		{
			// If the game's paused set sprite color to bluish then set it back to normal.
			if ( m_bPaused )
			{
				Color oldColor = pSprite->getColor();
				Color newColor;
				newColor.a = 127;
				newColor.r = (Uint8)( (float)oldColor.r * 0.75f );
				newColor.g = (Uint8)( (float)oldColor.g * 0.75f );
				newColor.b = oldColor.b + std::min<Uint8>( 50, UCHAR_MAX - oldColor.b );

				pSprite->setColor( newColor );
				m_pMainWindow->draw( *pSprite );
				pSprite->setColor( oldColor );
			}
			else
			{
				m_pMainWindow->draw( *pSprite );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Kill everyone that wants to die...
//-----------------------------------------------------------------------------
void CGameLogic::ProcessDeletionList( void )
{
	for ( int i = 0; i < MAX_ENTITIES; i++ )
	{
		CBaseEntity *pEntity = g_EntityList[i];
		if ( !pEntity )
			continue;

		if ( pEntity->IsMarkedForDeletion() )
		{
			// Bye!
			delete pEntity;
			g_EntityList[i] = NULL;
		}
	}
}
