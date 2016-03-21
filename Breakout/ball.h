#ifndef BALL_H
#define BALL_H

#ifdef _WIN32
#pragma once
#endif

#include "baseentity.h"

class CPlayer;

class CBall : public CBaseEntity
{
public:
	CBall();

	virtual int		GetEntityID( void ) { return ENTITY_BALL; }
	virtual bool	IsBall( void ) { return true; }

	CPlayer *GetOwner( void );
	void	SetOwner( CPlayer *pPlayer );
	bool	IsActive( void );
	void	SetActive( bool bActive );
	void	Bounce( int iSide );
	void	Killed( void );

	virtual void	UpdateOnRemove( void );
	virtual void	ProcessMovement( void );
	virtual void	Touch( CBaseEntity *pOther );
	virtual void	TouchScreenEdge( const Vector &vecPoint );

private:
	CPlayer	*m_pOwner;
	bool	m_bActive;
	bool	m_bDeflected;
};

#endif // BALL_H
