#ifndef HUD_SCOREACCOUNT_H
#define HUD_SCOREACCOUNT_H

#ifdef _WIN32
#pragma once
#endif

#include "shareddefs.h"
#include "ihudelement.h"

struct TextDelta_t
{
	TextDelta_t()
	{
		text[0] = '\0';
		flDieTime = 0.0f;
		bMinus = false;
	}

	char text[32];
	Vector vecPos;
	float flDieTime;
	bool bMinus;
};

class CTextManager : public IHudElement
{
public:
	CTextManager();

	bool Init( void );
	bool ShouldDraw( void );
	void Reset( void );
	void AddText( const char *pszText, const Vector &vecPos, bool bMinus = false );
	void AddNumber( int iNumber, const Vector &vecPos );
	void Update( void );

private:
	TextDelta_t m_aTexts[10];
	int m_iDeltaHead;
	sf::Font m_Font;
};

extern CTextManager g_HUDScoreAccount;

#endif // HUD_SCOREACCOUNT_H
