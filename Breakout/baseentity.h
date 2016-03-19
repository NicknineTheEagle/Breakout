#ifndef BASEENTITY_H
#define BASEENTITY_H

#ifdef _WIN32
#pragma once
#endif

#include "shareddefs.h"
#include "util.h"

class CBaseEntity
{
public:
	CBaseEntity();
	virtual ~CBaseEntity();

	int				entindex( void ) { return m_iEntIndex; }
	int				RegisterEntity( int iIndexOverride = -1 );
	const char		*GetClassname( void );

	void			Remove( void );
	virtual void	UpdateOnRemove( void );
	bool			IsMarkedForDeletion( void ) { return m_bKillMe; }

	const Vector	&GetOrigin( void ) { return m_vecOrigin; }
	void			SetOrigin( const Vector &newOrigin ) { m_vecOrigin = newOrigin; }
	const Vector	&GetVelocity( void ) { return m_vecVelocity; }
	void			SetVelocity( const Vector &newVel ) { m_vecVelocity = newVel; }
	float			GetAngle( void ) { return m_flAngle; }
	void			SetAngle( float flAngle ) { m_flAngle = flAngle; }

	const Vector	&GetMins( void ) { return m_vecMins; }
	const Vector	&GetMaxs( void ) { return m_vecMaxs; }
	void			SetCollisionBounds( const Vector &vecMins, const Vector &vecMaxs );
	void			GetAbsBounds( Vector *vecMins, Vector *vecMaxs );

	SolidType_t		GetSolidType( void ) { return m_nSolidType; }
	void			SetSolid( SolidType_t nSolid ) { m_nSolidType = nSolid; }
	bool			IsSolid( void ) { return ( m_nSolidType == SOLID_YES ); }

	void			SetSprite( const sf::IntRect &rect );
	sf::Sprite		*GetSprite( void ) { return m_pSprite; }

	virtual int		GetEntityID( void );
	virtual bool	IsPlayer( void ) { return false; }
	virtual bool	IsBot( void ) { return false; }
	virtual bool	IsBall( void ) { return false; }

	virtual void	ProcessMovement( void );
	void			OnCollide( CBaseEntity *pOther );
	virtual void	Touch( CBaseEntity *pOther );
	virtual void	TouchScreenEdge( const Vector &vecPoint );

protected:
	Vector	m_vecOrigin;
	Vector	m_vecVelocity;

	float	m_flAngle;

private:
	int			m_iEntIndex;

	bool		m_bKillMe;

	sf::Sprite	*m_pSprite;

	Vector		m_vecMins;
	Vector		m_vecMaxs;

	SolidType_t	m_nSolidType;
};

extern CBaseEntity *g_EntityList[MAX_ENTITIES];

template<class T>
T *CreateEntity( const Vector &vecOrigin = vec2_origin, const Vector &vecVelocity = vec2_origin, int iIndexOverride = -1 )
{
	T *newEnt = new T;

	// Add it to list.
	int index = newEnt->RegisterEntity( iIndexOverride );
	if ( index == -1 )
	{
		printf( "Failed to create entity!!!\n" );
		delete newEnt;
		return NULL;
	}

	newEnt->SetOrigin( vecOrigin );
	newEnt->SetVelocity( vecVelocity );

	return newEnt;
}

bool IsBoxIntersectingBox( const Vector& boxMin1, const Vector& boxMax1,
	const Vector& boxMin2, const Vector& boxMax2 );

bool IsSpaceEmpty( CBaseEntity *pEntity );

#endif // BASEENTITY_H
