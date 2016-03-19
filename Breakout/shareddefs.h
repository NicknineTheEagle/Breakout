#ifndef SHAREDDEFS_H
#define SHAREDDEFS_H

#ifdef _WIN32
#pragma once
#endif

#define MAX_ENTITIES 512
#define MAX_PLAYERS 4

#define PLAYER_SPEED 640.0f
#define BALL_SPEED 512.0f
#define COIN_SPEED 128.0f

#define PLAYER_WIDTH 48.0f
#define PLAYER_HEIGHT 12.0f

extern float g_CurTime;
extern float g_FrameTime;

extern const Vector vec2_origin;
extern const sf::FloatRect g_ScreenRect;

extern sf::Texture g_MainTexture;
extern sf::Font g_MainFont;

enum SolidType_t
{
	SOLID_NO = 0,	// Does not collide with anything.
	SOLID_TRIGGER,	// Does not collide but still triggers touch functions.
	SOLID_YES		// Collides with solid entities.
};

enum
{
	ENTITY_INVALID = 0,
	ENTITY_PLAYER,
	ENTITY_BALL,
	ENTITY_COIN,
	ENTITY_BLOCK,
};

extern const sf::Color g_aPlayerColors[MAX_PLAYERS];

#endif // SHAREDDEFS_H
