#ifndef COIN_H
#define COIN_H

#ifdef _WIN32
#pragma once
#endif

#include "baseentity.h"

class CCoin : public CBaseEntity
{
public:
	CCoin();

	virtual int		GetEntityID( void ) { return ENTITY_COIN; }

	void			AnimFrameAdvance( void );
	virtual void	ProcessMovement( void );
	virtual void	Touch( CBaseEntity *pOther );
	virtual void	TouchScreenEdge( const Vector &vecPoint );

private:
	float	m_flAnimTime;
	int		m_iAnimFrame;
};

#endif // COIN_H
