#ifndef PLAYER_H
#define PLAYER_H

#ifdef _WIN32
#pragma once
#endif

#include "baseentity.h"
#include <SFML/Window/Keyboard.hpp>

class CBall;
class CBotManager;

struct SGameControls
{
	sf::Keyboard::Key up;
	sf::Keyboard::Key down;
	sf::Keyboard::Key left;
	sf::Keyboard::Key right;
	sf::Keyboard::Key rotate_left;
	sf::Keyboard::Key rotate_right;
	sf::Keyboard::Key launch;
};

enum PlayerStats_t
{
	STAT_BLOCKS_DESTROYED = 0,
	STAT_COINS_COLLECTED,
	STAT_BALLS_LOST,
	NUM_STATS
};

class CPlayer : public CBaseEntity
{
public:
	CPlayer();

	virtual int		GetEntityID( void ) { return ENTITY_PLAYER; }
	virtual	bool	IsPlayer( void ) { return true; }

	void	SetControls( SGameControls &controls );
	void	HandleKeys( void );
	int		GetScore( void ) { return m_nScore; }
	void	ResetScore( void );
	void	IncrementScore( int nScore );
	CBall	*GetBall( void ) { return m_pBall; }
	void	SetBall( CBall *pBall ) { m_pBall = pBall; }
	void	CreateBall( void );
	void	LaunchBall( CBall *pBall );
	void	OnBallLost( void );
	void	IncrementStat( int iStat, int iAmount = 1 );
	void	ResetStats( void );
	int		GetStat( int iStat ) { return m_Stats[iStat]; }

	virtual void	ProcessMovement( void );

protected:
	int				m_nScore;
	CBall			*m_pBall;

private:
	SGameControls	m_PlayerControls;
	int				m_Stats[NUM_STATS];
};

class CBot : public CPlayer
{
public:
	CBot();

	virtual bool	IsBot( void ) { return true; }

	void			RunAI( void );
	CBaseEntity		*FindTarget( void );
	float			GetAngleToTarget( CBaseEntity *pTarget );

	virtual void	ProcessMovement( void );

	friend class CBotManager;

private:
	float			m_flDesiredOffset;
	CBaseEntity		*m_pTargetEntity;
};

#endif // PLAYER_H
