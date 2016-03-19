#include "stdafx.h"
#include "hud_manager.h"

CHudManager g_HUDManager;

CHudManager::CHudManager()
{
}

//-----------------------------------------------------------------------------
// Purpose: Perform all operations that need to be performed after CGameLogic is initialized.
//-----------------------------------------------------------------------------
bool CHudManager::Init( void )
{
	for ( size_t i = 0; i < m_HudElements.size(); i++ )
	{
		if ( m_HudElements[i]->Init() == false )
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Draw all HUD elements.
//-----------------------------------------------------------------------------
void CHudManager::DrawHUD( void )
{
	for ( size_t i = 0; i < m_HudElements.size(); i++ )
	{
		IHudElement *pElement = m_HudElements[i];

		if ( pElement->ShouldDraw() )
			pElement->Update();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Reset HUD elements to initial state on game restart.
//-----------------------------------------------------------------------------
void CHudManager::ResetHUD( void )
{
	for ( size_t i = 0; i < m_HudElements.size(); i++ )
	{
		m_HudElements[i]->Reset();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHudManager::RegisterHUDElement( IHudElement *pElement )
{
	m_HudElements.push_back( pElement );
}
