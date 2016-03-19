#ifndef HUD_MANAGER_H
#define HUD_MANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "ihudelement.h"

class CHudManager
{
public:
	CHudManager();

	bool Init( void );
	void ResetHUD( void );
	void DrawHUD( void );
	void RegisterHUDElement( IHudElement *pElement );

private:
	std::vector<IHudElement *> m_HudElements;
};

extern CHudManager g_HUDManager;

#endif // HUD_MANAGER_H
