#include "stdafx.h"
#include "baseentity.h"

#include <typeinfo>

bool IsBoxIntersectingBox( const Vector& boxMin1, const Vector& boxMax1,
	const Vector& boxMin2, const Vector& boxMax2 )
{
	if ( ( boxMin1.x > boxMax2.x ) || ( boxMax1.x < boxMin2.x ) )
		return false;
	if ( ( boxMin1.y > boxMax2.y ) || ( boxMax1.y < boxMin2.y ) )
		return false;

	return true;
}

bool IsSpaceEmpty( CBaseEntity *pEntity )
{
	bool bEmpty = true;

	Vector vecMins, vecMaxs;
	pEntity->GetAbsBounds( &vecMins, &vecMaxs );

	// World bounds!
	if ( !g_ScreenRect.contains( vecMins ) || !g_ScreenRect.contains( vecMaxs ) )
	{
		// Screen edges are always solid.
		bEmpty = false;
	}

	for ( int i = 0; i < MAX_ENTITIES; i++ )
	{
		CBaseEntity *pOther = g_EntityList[i];
		if ( !pOther || pOther == pEntity || pOther->GetSolidType() == SOLID_NO )
			continue;

		Vector vecOtherMins, vecOtherMaxs;
		pOther->GetAbsBounds( &vecOtherMins, &vecOtherMaxs );

		if ( IsBoxIntersectingBox( vecMins, vecMaxs, vecOtherMins, vecOtherMaxs ) )
		{
			if ( pOther->IsSolid() )
			{
				bEmpty = false;
			}
		}
	}

	return bEmpty;
}

CBaseEntity::CBaseEntity()
{
	m_bKillMe = false;
	m_pSprite = NULL;
	m_flAngle = 0.0f;
	m_nSolidType = SOLID_NO;
	m_iEntIndex = -1;
}

CBaseEntity::~CBaseEntity()
{
	delete m_pSprite;
}

//-----------------------------------------------------------------------------
// Purpose: Add entity to entity list so it can be accessed by iterators.
//-----------------------------------------------------------------------------
int CBaseEntity::RegisterEntity( int iIndexOverride /*= -1*/ )
{
	if ( iIndexOverride != -1 )
	{
		assert( g_EntityList[iIndexOverride] == NULL );

		g_EntityList[iIndexOverride] = this;
		m_iEntIndex = iIndexOverride;
	}
	else
	{
		// 0 to MAX_PLAYERS are reserved for players.
		for ( int i = MAX_PLAYERS; i < MAX_ENTITIES; i++ )
		{
			if ( g_EntityList[i] == NULL )
			{
				g_EntityList[i] = this;
				m_iEntIndex = i;
				break;
			}
		}
	}

	return m_iEntIndex;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CBaseEntity::GetClassname( void )
{
	static char szName[256];
	strncpy( szName, typeid( *this ).name(), sizeof( szName ) );

	return szName;
}

//-----------------------------------------------------------------------------
// Purpose: Sets up entity for removal at the end of this frame.
//-----------------------------------------------------------------------------
void CBaseEntity::Remove( void )
{
	if ( !m_bKillMe )
	{
		m_bKillMe = true;
		UpdateOnRemove();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBaseEntity::UpdateOnRemove( void )
{
	// Kill the sprite so it won't get drawn anymore.
	delete m_pSprite;
	m_pSprite = NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBaseEntity::SetCollisionBounds( const Vector &vecMins, const Vector &vecMaxs )
{
	m_vecMins = vecMins;
	m_vecMaxs = vecMaxs;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBaseEntity::GetAbsBounds( Vector *vecMins, Vector *vecMaxs )
{
	*vecMins = m_vecOrigin + m_vecMins;
	*vecMaxs = m_vecOrigin + m_vecMaxs;
}

//-----------------------------------------------------------------------------
// Purpose: Sets sprite bounds. Currently only supporting one texture.
//-----------------------------------------------------------------------------
void CBaseEntity::SetSprite( const sf::IntRect &rect )
{
	if ( m_pSprite )
	{
		m_pSprite->setTextureRect( rect );
	}
	else
	{
		m_pSprite = new sf::Sprite( g_MainTexture, rect );
	}

	Vector vecSpriteOrigin( (float)rect.width / 2, (float)rect.height / 2 );
	m_pSprite->setOrigin( vecSpriteOrigin );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CBaseEntity::GetEntityID( void )
{
	assert( false );
	return ENTITY_INVALID;
}

//-----------------------------------------------------------------------------
// Purpose: "Physics" simulation.
//-----------------------------------------------------------------------------
void CBaseEntity::ProcessMovement( void )
{
	// Test new position.
	Vector vecNewOrigin = m_vecOrigin + m_vecVelocity * g_FrameTime;
	Vector vecMins = vecNewOrigin + m_vecMins;
	Vector vecMaxs = vecNewOrigin + m_vecMaxs;

	// Test collision against other entities.
	// Since movement amount depends on frame time and we only test the final position
	// this whole system goes out of the window at low framerates.
	bool bCanMove = true;

	if ( GetSolidType() != SOLID_NO )
	{
		// World bounds!
		if ( !g_ScreenRect.contains( vecMins ) || !g_ScreenRect.contains( vecMaxs ) )
		{
			TouchScreenEdge( vecNewOrigin );

			// Screen edges are always solid.
			if ( IsSolid() )
			{
				bCanMove = false;
			}
		}

		for ( int i = 0; i < MAX_ENTITIES; i++ )
		{
			CBaseEntity *pEntity = g_EntityList[i];
			if ( !pEntity || pEntity == this || pEntity->GetSolidType() == SOLID_NO )
				continue;

			Vector vecOtherMins, vecOtherMaxs;
			pEntity->GetAbsBounds( &vecOtherMins, &vecOtherMaxs );

			if ( IsBoxIntersectingBox( vecMins, vecMaxs, vecOtherMins, vecOtherMaxs ) )
			{
				OnCollide( pEntity );

				if ( IsSolid() && pEntity->IsSolid() )
				{
					bCanMove = false;
				}
			}
		}
	}

	if ( bCanMove )
		m_vecOrigin = vecNewOrigin;

	// Move the sprite if we have one.
	if ( m_pSprite )
	{
		m_pSprite->setPosition( m_vecOrigin );
		m_pSprite->setRotation( m_flAngle );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called each frame when two entities are touching each other.
//-----------------------------------------------------------------------------
void CBaseEntity::OnCollide( CBaseEntity *pOther )
{
	if ( !pOther )
		return;

	// If either ents are marked for deletion they can't touch each other.
	if ( IsMarkedForDeletion() || pOther->IsMarkedForDeletion() )
		return;

	pOther->Touch( this );
	Touch( pOther );
}

void CBaseEntity::Touch( CBaseEntity *pOther )
{
}

void CBaseEntity::TouchScreenEdge( const Vector &vecPoint )
{
}
