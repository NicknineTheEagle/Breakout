#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#ifdef _WIN32
#pragma once
#endif

#include "shareddefs.h"

class CGameLogic
{
public:
	CGameLogic();
	~CGameLogic();

	bool Init( void );
	sf::RenderWindow *CreateMainWindow( void );
	sf::RenderWindow *GetWindow( void ) { return m_pMainWindow; }

	void	MainLoop( void );
	void	GameLoop( void );
	void	WinLoop( void );
	void	PauseLoop( void );
	void	PauseDraw( void );
	void	StartGame( void );
	void	RestartGame( void );
	void	ClearEntityList( void );
	void	CreatePlayers( void );
	void	CreateBlocks( void );
	void	UpdateWorld( void );
	void	DrawEntities( void );
	void	ProcessDeletionList( void );

	bool	IsPaused( void ) { return m_bPaused; }
	int		GetWinningPlayer( void ) { return m_iWinningPlayer; }

public:
	enum
	{
		PLAYER_NONE,
		PLAYER_HUMAN,
		PLAYER_BOT,
	};

	int		m_aActivePlayers[MAX_PLAYERS];
	bool	m_bLessBlocks;
	int		m_iWins[MAX_PLAYERS];

private:
	sf::RenderWindow *m_pMainWindow;
	sf::Clock m_Clock;

	float	m_flSpeedMult;
	bool	m_bFullScreen;
	bool	m_bPaused;
	bool	m_bRestart;
	int		m_iWinningPlayer;

	sf::Text *m_textPause[4];
};

extern CGameLogic *g_pGameLogic;

#endif // GAMELOGIC_H
