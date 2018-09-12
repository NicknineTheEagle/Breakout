#ifndef HUD_WINSCREEN_H
#define HUD_WINSCREEN_H

#ifdef _WIN32
#pragma once
#endif

#include "ihudelement.h"
#include "shareddefs.h"

class CWinScreen : public IHudElement
{
public:
	CWinScreen();

	bool	Init( void );
	bool	ShouldDraw( void );
	void	Reset( void );
	void	Update( void );
	void	SetWinScreenInfo( int iWinningPlayer );

private:
	sf::Text *m_textWins[MAX_PLAYERS];
	sf::Text *m_textWinHeader;
	sf::Text *m_textWinScores[MAX_PLAYERS * 2];
	sf::Text *m_textWinKeys[2];
	sf::Text *m_textCountdown;

	sf::Text *m_textStats[MAX_PLAYERS];

	float m_flScores[MAX_PLAYERS];
	bool m_bFinishedCounting;
	bool m_bShouldAutoRestart;

	sf::Clock m_Clock;
};

extern CWinScreen g_HUDWinScreen;

#endif // HUD_WINSCREEN_H
