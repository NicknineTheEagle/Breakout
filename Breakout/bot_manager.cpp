#include "stdafx.h"
#include "bot_manager.h"

CBotManager g_BotManager;

CBotManager::CBotManager()
{
}

void CBotManager::OnBlockDestroyed( CBaseEntity *pBlock )
{
	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		if ( g_EntityList[i] && g_EntityList[i]->IsBot() )
		{
			CBot *pBot = static_cast<CBot *>( g_EntityList[i] );

			// If the bot is targeting the destroyed block find a new target.
			if ( pBot->m_pTargetEntity == pBlock )
			{
				pBot->m_pTargetEntity = pBot->FindTarget();
			}
		}
	}
}
