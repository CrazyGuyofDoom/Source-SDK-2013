//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#ifndef HUD_COUNTERDISPLAY_H
#define HUD_COUNTERDISPLAY_H

#ifdef _WIN32
#pragma once
#endif

#include "hud.h"
#include "hudelement.h"
#include "hud_numericdisplay.h"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CHudCounterDisplay : public CHudNumericDisplay, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CHudCounterDisplay, CHudNumericDisplay );

public:
	CHudCounterDisplay( const char *pElementName );
	~CHudCounterDisplay();

	void	Init( void );
	void	Reset( void );
	void	VidInit( void );
	void	OnThink( void );
	void	MsgFunc_HudCounterMsg(bf_read &msg );
	bool	ShouldDraw();

	float	GetTimeValue() { return m_flTime; }
	void	SetTimeValue( float flTime );

	float	GetTimeWarnValue() { return m_flWarnTime; }
	void	SetTimeWarnValue( float flTime );

	void	SetOn( bool bState );
	bool	IsOn() { return m_bOn; }

protected:
	void	Paint();
	void	PaintNumbers(vgui::HFont font, int xpos, int ypos, int value);

private:

	float	m_flTime;
	float	m_flWarnTime;
	bool	m_bWarningSummoned;
	bool	m_bOn;

	CPanelAnimationVar( vgui::HFont, m_hNumberFont, "NumberFont", "CountdownNumbers" );
	CPanelAnimationVar( vgui::HFont, m_hNumberGlowFont, "NumberGlowFont", "CountdownNumbersGlow" );
};

extern CHudCounterDisplay *GetHudCounter();

#endif // HUD_COUNTERDISPLAY_H