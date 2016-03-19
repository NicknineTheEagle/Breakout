#ifndef BOT_MANAGER_H
#define BOT_MANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "player.h"

class CBotManager
{
public:
	CBotManager();

	void OnBlockDestroyed( CBaseEntity *pBlock );
};

extern CBotManager g_BotManager;

#endif // BOT_MANAGER_H
