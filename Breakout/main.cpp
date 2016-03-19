#include "stdafx.h"
#include "baseentity.h"
#include "gamelogic.h"

CBaseEntity *g_EntityList[MAX_ENTITIES] = { };

float g_CurTime;
float g_FrameTime;

int main( int argc, char **argv )
{
	srand( (unsigned)time( NULL ) );

	g_pGameLogic = new CGameLogic;

	for ( int i = 1; i < argc; i++ )
	{
		if ( strcmp( argv[i], "-players" ) == 0 && i + 1 < argc )
		{
			size_t len = strlen( argv[i+1] );
			for ( size_t c = 0; c < len && c < MAX_PLAYERS; c++ )
			{
				switch ( argv[i+1][c] )
				{
				case 'h':
					g_pGameLogic->m_aActivePlayers[c] = CGameLogic::PLAYER_HUMAN;
					break;
				case 'b':
					g_pGameLogic->m_aActivePlayers[c] = CGameLogic::PLAYER_BOT;
					break;
				default:
					g_pGameLogic->m_aActivePlayers[c] = CGameLogic::PLAYER_NONE;
					break;
				}
			}

			i++;
		}
		else if ( strcmp( argv[i], "-lessblocks" ) == 0 )
		{
			g_pGameLogic->m_bLessBlocks = true;
		}
	}

	if ( !g_pGameLogic->Init() )
	{
		delete g_pGameLogic;
		return -1;
	}

	g_pGameLogic->MainLoop();
	delete g_pGameLogic;

	return 0;
}
