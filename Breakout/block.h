#ifndef BLOCK_H
#define BLOCK_H

#ifdef _WIN32
#pragma once
#endif

#include "baseentity.h"

enum
{
	BLOCK_RED = 0,
	BLOCK_PURPLE,
	BLOCK_ORANGE,
	BLOCK_BLUE,
	BLOCK_BLUE_BRIGHT,
	BLOCK_GREEN,
	BLOCK_GREEN_BRIGHT,
	BLOCK_COUNT
};

class CBlock : public CBaseEntity
{
public:
	CBlock();

	virtual int		GetEntityID( void ) { return ENTITY_BLOCK; }

	int		GetType( void );
	void	SetType( int iType );

	virtual void	Touch( CBaseEntity *pOther );

private:
	int m_iType;
};

#endif // BLOCK_H
