#include "stdafx.h"
#include "shareddefs.h"

const Vector vec2_origin( 0, 0 );
const sf::FloatRect g_ScreenRect( 0, 0, 672, 768 );

sf::Texture g_MainTexture;
sf::Font g_MainFont;

const sf::Color g_aPlayerColors[MAX_PLAYERS] =
{
	sf::Color( 255, 0, 0, 255 ),
	sf::Color( 127, 127, 255, 255 ),
	sf::Color( 0, 255, 0, 255 ),
	sf::Color( 255, 255, 0, 255 ),
};
