#ifndef HUD_SCORE_H
#define HUD_SCORE_H

#ifdef _WIN32
#pragma once
#endif

#include "ihudelement.h"
#include "shareddefs.h"

class CHudScore : public IHudElement
{
public:
	CHudScore();

	bool Init( void );
	bool ShouldDraw( void );
	void Reset( void );
	void Update( void );

private:
	float m_flScores[MAX_PLAYERS];
	sf::Text *m_textScores[MAX_PLAYERS];
};

#endif HUD_SCORE_H
