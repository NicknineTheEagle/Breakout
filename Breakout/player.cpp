#include "stdafx.h"
#include "player.h"
#include "ball.h"
#include "gamelogic.h"
#include "hud_scoreaccount.h"

//#define DEBUG_BOTS

#define BALL_LOST_PENALTY 1000

CPlayer::CPlayer()
{
	SetSolid( SOLID_YES );
	//SetSprite( sf::IntRect( 0, 280, 128, 24 ) );
	SetSprite( sf::IntRect( 0, 200, 96, 24 ) );
	SetCollisionBounds( -Vector( PLAYER_WIDTH, PLAYER_HEIGHT ), Vector( PLAYER_WIDTH, PLAYER_HEIGHT ) );
	m_nScore = 0;
	m_flAngle = 270.0f; // Up.
}

//-----------------------------------------------------------------------------
// Purpose: Set player's control scheme.
//-----------------------------------------------------------------------------
void CPlayer::SetControls( game_controls_t &controls )
{
	m_PlayerControls = controls;
}

//-----------------------------------------------------------------------------
// Purpose: Set velocity according to keys pressed.
//-----------------------------------------------------------------------------
void CPlayer::HandleKeys( void )
{
	// Reset velocity from previous frame.
	m_vecVelocity = vec2_origin;

	if ( m_pBall && !m_pBall->IsActive() )
	{
		if ( sf::Keyboard::isKeyPressed( m_PlayerControls.launch ) )
		{
			if ( !IsSpaceEmpty( m_pBall ) )
			{
				static int nWarnings = 0;
				if ( nWarnings < 10 )
				{
					printf( "Attempted to launch the ball in solid!\n" );
					nWarnings++;
				}
			}
			else
			{
				LaunchBall( m_pBall );;
			}
		}
	}

	if ( fmodf( m_flAngle, 180.0f ) != 0.0f )
	{
		if ( sf::Keyboard::isKeyPressed( m_PlayerControls.left ) )
		{
			m_vecVelocity.x -= PLAYER_SPEED;
		}
		if ( sf::Keyboard::isKeyPressed( m_PlayerControls.right ) )
		{
			m_vecVelocity.x += PLAYER_SPEED;
		}
	}
	else
	{
		if ( sf::Keyboard::isKeyPressed( m_PlayerControls.up ) )
		{
			m_vecVelocity.y -= PLAYER_SPEED;
		}
		if ( sf::Keyboard::isKeyPressed( m_PlayerControls.down ) )
		{
			m_vecVelocity.y += PLAYER_SPEED;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CPlayer::ResetScore( void )
{
	m_nScore = 0;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CPlayer::IncrementScore( int nScore )
{
	m_nScore += nScore;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CPlayer::CreateBall( void )
{
	Vector vecDir;
	AngleVectors( m_flAngle, &vecDir );
	Vector vecBallOrigin = GetOrigin() + vecDir * 25.0f;
	CBall *pBall = CreateEntity<CBall>( vecBallOrigin, vec2_origin );
	assert( pBall );
	pBall->SetOwner( this );
	m_pBall = pBall;
}

//-----------------------------------------------------------------------------
// Purpose: Launch a ball with random angle.
//-----------------------------------------------------------------------------
void CPlayer::LaunchBall( CBall *pBall )
{
	pBall->SetActive( true );
	Vector vecDir;
	AngleVectors( RandomFloat( -45.0f, 45.0f ) + m_flAngle, &vecDir );
	pBall->SetVelocity( vecDir * BALL_SPEED );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CPlayer::OnBallLost( void )
{
	// TODO: Subtract a life.

	// Losing a ball in Versus causes a score penalty.
	m_nScore -= BALL_LOST_PENALTY;
	g_HUDScoreAccount.AddNumber( -BALL_LOST_PENALTY, m_pBall->GetOrigin() );

	// Create a new ball.
	CreateBall();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CPlayer::ProcessMovement( void )
{
	if ( !IsBot() )
		HandleKeys();

	CBaseEntity::ProcessMovement();

	// HACK: Default player sprite orientation is up, not right, so we gotta fix it here.
	if ( GetSprite() )
	{
		GetSprite()->rotate( 90.0f );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Bot
//-----------------------------------------------------------------------------
CBot::CBot()
{
	m_flDesiredOffset = 12.0f;
	m_pTargetEntity = NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBot::RunAI( void )
{
	// Reset velocity from previous frame.
	m_vecVelocity = vec2_origin;

	if ( !m_pBall )
		return;

	if ( !m_pTargetEntity )
		m_pTargetEntity = FindTarget();

	if ( !m_pBall->IsActive() )
	{
		// Don't launch the ball until at least one of the humans does.
		bool bHumanPresent = false;
		bool bHumanReady = false;
		for ( int i = 0; i < MAX_PLAYERS; i++ )
		{
			CPlayer *pPlayer = static_cast<CPlayer *>( g_EntityList[i] );
			if ( !pPlayer || pPlayer == this )
				continue;

			if ( !pPlayer->IsBot() )
			{
				bHumanPresent = true;

				if ( pPlayer->GetBall() && pPlayer->GetBall()->IsActive() )
				{
					bHumanReady = true;
					break;
				}
			}
		}

		if ( bHumanPresent && !bHumanReady )
			return;

		if ( !IsSpaceEmpty( m_pBall ) )
		{
			static int nWarnings = 0;

			if ( nWarnings < 10 )
			{
				printf( "AI attempted to launch the ball in solid!\n" );
				nWarnings++;
			}
		}
		else if ( m_pBall->GetOwner() == this )
		{
			LaunchBall( m_pBall );
		}
	}
	else
	{
		if ( m_pTargetEntity )
		{
			float flAngle = GetAngleToTarget( m_pTargetEntity );

			if ( flAngle > 45 || flAngle < -45 )
			{
				// Target is too far away, find another one.
				CBaseEntity *pNewTarget = FindTarget();
				if ( pNewTarget )
				{
					m_pTargetEntity = pNewTarget;
					flAngle = GetAngleToTarget( m_pTargetEntity );
				}
			}

#ifdef DEBUG_BOTS
			char szAngle[16];
			sprintf( szAngle, "%.f", flAngle );
			sf::Text textAngle( szAngle, g_MainFont );
			textAngle.setPosition( GetOrigin() + Vector( GetMaxs().x + 5, -15 ) );
			g_pGameLogic->GetWindow()->draw( textAngle );

			sf::Vertex line1[] =
			{
				sf::Vertex( GetOrigin(), g_aPlayerColors[entindex()] ),
				sf::Vertex( m_pTargetEntity->GetOrigin() )
			};

			g_pGameLogic->GetWindow()->draw( line1, 2, sf::Lines );
#endif // DEBUG_BOTS

			m_flDesiredOffset = RemapVal( flAngle, -45, 45, PLAYER_WIDTH, -PLAYER_WIDTH );

			if ( m_flDesiredOffset < -PLAYER_WIDTH || m_flDesiredOffset > PLAYER_WIDTH )
			{
				// Target's out of range, approuch it slowly.
				m_flDesiredOffset = clamp( m_flDesiredOffset, -12.0f, 12.0f );
			}

			Vector vecForward, vecRight;
			AngleVectors( GetAngle(), &vecForward, &vecRight );

			Vector vecTargetDir = m_pBall->GetOrigin() - GetOrigin();
			vecTargetDir = VectorProject( vecTargetDir, vecRight );
			vecTargetDir += vecRight * m_flDesiredOffset;

			float flDist = VectorNormalize( vecTargetDir );
			if ( flDist != 0.0f )
			{
				m_vecVelocity = vecTargetDir * PLAYER_SPEED;
			}
		}
		else
		{
			// Just juggle the ball back and forth if we have no target.
			Vector vecForward, vecRight;
			AngleVectors( GetAngle(), &vecForward, &vecRight );

			Vector vecTargetDir = m_pBall->GetOrigin() - GetOrigin();
			vecTargetDir = VectorProject( vecTargetDir, vecRight );

			float flDist = VectorNormalize( vecTargetDir );

			if ( flDist > 12.0f )
			{
				m_vecVelocity = vecTargetDir * PLAYER_SPEED;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Find the nearest block to aim at.
//-----------------------------------------------------------------------------
CBaseEntity *CBot::FindTarget( void )
{
	CBaseEntity *pNearest = NULL;
	float flNearest = FLT_MAX;

	for ( int i = MAX_PLAYERS; i < MAX_ENTITIES; i++ )
	{
		CBaseEntity *pEntity = g_EntityList[i];
		if ( !pEntity || pEntity->IsMarkedForDeletion() || pEntity->GetEntityID() != ENTITY_BLOCK )
			continue;

		Vector vecDir = pEntity->GetOrigin() - GetOrigin();
		float flLength = VectorLength( vecDir );

		if ( flLength < flNearest )
		{
			flNearest = flLength;
			pNearest = pEntity;
		}
	}

	return pNearest;
}

//-----------------------------------------------------------------------------
// Purpose: Calculate relative angle to bot's target.
//-----------------------------------------------------------------------------
float CBot::GetAngleToTarget( CBaseEntity *pEntity )
{
	// Get angle from paddle to target.
	Vector vecDir = pEntity->GetOrigin() - GetOrigin();
	VectorNormalize( vecDir );

	float flAngle;
	VectorAngles( vecDir, flAngle );

	// Convert to relative angle.
	flAngle -= GetAngle();

	flAngle = AngleNormalize( flAngle );

	return flAngle;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBot::ProcessMovement( void )
{
	RunAI();

	CPlayer::ProcessMovement();
}
