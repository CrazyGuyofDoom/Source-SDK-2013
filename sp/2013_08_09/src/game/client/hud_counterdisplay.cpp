//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include "vgui_controls/AnimationController.h"
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"

#include "hud_counterdisplay.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

static CHudCounterDisplay *g_sHudCounter = NULL;

CHudCounterDisplay *GetHudCounter()
{
	return g_sHudCounter;
}

DECLARE_HUDELEMENT( CHudCounterDisplay );
DECLARE_HUD_MESSAGE( CHudCounterDisplay, HudCounterMsg );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudCounterDisplay::CHudCounterDisplay( const char *pElementName ) : BaseClass(NULL, "HudCounter"), CHudElement( pElementName )
{
	SetHiddenBits( HIDEHUD_HEALTH | HIDEHUD_NEEDSUIT );

	g_sHudCounter = this;
	m_bIsTime = true;

	m_bWarningSummoned = false;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CHudCounterDisplay::~CHudCounterDisplay()
{
	g_sHudCounter = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCounterDisplay::SetOn( bool bState )
{
	m_bOn = bState;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCounterDisplay::Init( void )
{
	HOOK_HUD_MESSAGE( CHudCounterDisplay, HudCounterMsg);
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCounterDisplay::Reset( void )
{
	SetLabelText(g_pVGuiLocalize->Find("#Valve_Hud_COUNTER"));
	SetDisplayValue( (int)m_flTime );

	if ( m_flTime >= m_flWarnTime )
	{
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("CountdownTimerAboveWarning");
	}
	else
	{
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("CountdownTimerBelowWarning");
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("CountdownTimerLow");
		m_bWarningSummoned = true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCounterDisplay::VidInit( void )
{
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CHudCounterDisplay::ShouldDraw( void )
{
	if ( !m_bOn || m_flTime <= 0.0f )
		return false;

	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCounterDisplay::OnThink( void )
{
	if (  m_flTime >= m_flWarnTime )
	{
		if ( m_bWarningSummoned )
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("CountdownTimerAboveWarning");
			m_bWarningSummoned = false;
		}
	}
	else if ( m_flTime > 0 )
	{
		if ( !m_bWarningSummoned )
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("CountdownTimerBelowWarning");
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("CountdownTimerLow");
			m_bWarningSummoned = true;
		}
	}

	SetDisplayValue( (int)m_flTime );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCounterDisplay::MsgFunc_HudCounterMsg( bf_read &msg )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCounterDisplay::SetTimeValue( float flTime )
{
	m_flTime = flTime;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCounterDisplay::SetTimeWarnValue( float flTime )
{
	m_flWarnTime = flTime;
}

//-----------------------------------------------------------------------------
// Purpose: paints a number at the specified position
//-----------------------------------------------------------------------------
void CHudCounterDisplay::Paint( void )
{
	if (m_bDisplayValue)
	{
		// draw our numbers
		surface()->DrawSetTextColor(GetFgColor());
		PaintNumbers(m_hNumberFont, digit_xpos, digit_ypos, m_iValue);

		// draw the overbright blur
		for (float fl = m_flBlur; fl > 0.0f; fl -= 1.0f)
		{
			if (fl >= 1.0f)
			{
				PaintNumbers(m_hNumberGlowFont, digit_xpos, digit_ypos, m_iValue);
			}
			else
			{
				// draw a percentage of the last one
				Color col = GetFgColor();
				col[3] *= fl;
				surface()->DrawSetTextColor(col);
				PaintNumbers(m_hNumberGlowFont, digit_xpos, digit_ypos, m_iValue);
			}
		}
	}

	// total ammo
	if (m_bDisplaySecondaryValue)
	{
		surface()->DrawSetTextColor(GetFgColor());
		PaintNumbers(m_hSmallNumberFont, digit2_xpos, digit2_ypos, m_iSecondaryValue);
	}

	PaintLabel();
}

//-----------------------------------------------------------------------------
// Purpose: paints a number at the specified position
//-----------------------------------------------------------------------------
void CHudCounterDisplay::PaintNumbers(vgui::HFont font, int xpos, int ypos, int value)
{
	surface()->DrawSetTextFont(font);
	wchar_t unicode[6];
	if ( !m_bIsTime )
	{
		V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d", value);
	}
	else
	{
		int iMinutes = value / 60;
		int iSeconds = value - iMinutes * 60;

		if ( iSeconds < 10 )
			V_snwprintf( unicode, ARRAYSIZE(unicode), L"%d:0%d", iMinutes, iSeconds );
		else
			V_snwprintf( unicode, ARRAYSIZE(unicode), L"%d:%d", iMinutes, iSeconds );
	}

	// adjust the position to take into account 3 characters
	int charWidth = surface()->GetCharacterWidth(font, '0');
	if (value < 100 && m_bIndent)
	{
		xpos += charWidth;
	}
	if (value < 10 && m_bIndent)
	{
		xpos += charWidth;
	}

	surface()->DrawSetTextPos(xpos, ypos);
	surface()->DrawUnicodeString( unicode );
}