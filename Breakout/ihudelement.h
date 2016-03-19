#ifndef IHUDELEMENT_H
#define IHUDELEMENT_H

#ifdef _WIN32
#pragma once
#endif

class IHudElement
{
public:
	virtual bool Init( void ) = 0;
	virtual void Reset( void ) = 0;
	virtual void Update( void ) = 0;
	virtual bool ShouldDraw( void ) = 0;
};

#endif // IHUDELEMENT_H
