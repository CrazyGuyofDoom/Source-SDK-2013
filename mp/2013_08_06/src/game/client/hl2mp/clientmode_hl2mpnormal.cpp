//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Draws the normal TF2 or HL2 HUD.
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "clientmode_hl2mpnormal.h"
#include "vgui_int.h"
#include "hud.h"
#include <vgui/IInput.h>
#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui_controls/AnimationController.h>
#include "iinput.h"
#include "hl2mpclientscoreboard.h"
#include "hl2mptextwindow.h"
#include "ienginevgui.h"

#if defined ( SWARM_DLL )
#include "c_hl2mp_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
vgui::HScheme g_hVGuiCombineScheme = 0;


// Instance the singleton and expose the interface to it.
IClientMode *GetClientModeNormal()
{
	static ClientModeHL2MPNormal g_ClientModeNormal;
	return &g_ClientModeNormal;
}

ClientModeHL2MPNormal* GetClientModeHL2MPNormal()
{
	Assert( dynamic_cast< ClientModeHL2MPNormal* >( GetClientModeNormal() ) );

	return static_cast< ClientModeHL2MPNormal* >( GetClientModeNormal() );
}

#if defined ( SWARM_DLL )
void ClientModeHL2MPNormal::OnColorCorrectionWeightsReset( void )
{
	C_ColorCorrection *pNewColorCorrection = NULL;
	C_ColorCorrection *pOldColorCorrection = m_pCurrentColorCorrection;
	C_HL2MP_Player *pPlayer = C_HL2MP_Player::GetLocalHL2MPPlayer();
	if ( pPlayer )
	{
		pNewColorCorrection = pPlayer->GetActiveColorCorrection();
	}

	if ( m_CCFailedHandle != INVALID_CLIENT_CCHANDLE && ASWGameRules() )
	{
		m_fFailedCCWeight = Approach( TechMarineFailPanel::s_pTechPanel ? 1.0f : 0.0f, m_fFailedCCWeight, gpGlobals->frametime * ( 1.0f / FAILED_CC_FADE_TIME ) );
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCFailedHandle, m_fFailedCCWeight );

		// If the mission was failed due to a dead tech, disable the environmental color correction in favor of the mission failed color correction
		if ( m_fFailedCCWeight != 0.0f && m_pCurrentColorCorrection )
		{
			m_pCurrentColorCorrection->EnableOnClient( false );
			m_pCurrentColorCorrection = NULL;
		}
	}

	if ( m_CCInfestedHandle != INVALID_CLIENT_CCHANDLE && ASWGameRules() )
	{
		C_ASW_Marine *pMarine = C_ASW_Marine::GetLocalMarine();
		m_fInfestedCCWeight = Approach( pMarine && pMarine->IsInfested() ? 1.0f : 0.0f, m_fInfestedCCWeight, gpGlobals->frametime * ( 1.0f / INFESTED_CC_FADE_TIME ) );
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCInfestedHandle, m_fInfestedCCWeight );

		// If the mission was failed due to a dead tech, disable the environmental color correction in favor of the mission failed color correction
		if ( m_fInfestedCCWeight != 0.0f && m_pCurrentColorCorrection )
		{
			m_pCurrentColorCorrection->EnableOnClient( false );
			m_pCurrentColorCorrection = NULL;
		}
	}

	// Only blend between environmental color corrections if there is no failure/infested-induced color correction
	if ( pNewColorCorrection != pOldColorCorrection && m_fFailedCCWeight == 0.0f && m_fInfestedCCWeight == 0.0f )
	{
		if ( pOldColorCorrection )
		{
			pOldColorCorrection->EnableOnClient( false );
		}
		if ( pNewColorCorrection )
		{
			pNewColorCorrection->EnableOnClient( true, pOldColorCorrection == NULL );
		}
		m_pCurrentColorCorrection = pNewColorCorrection;
	}
}
#endif

//-----------------------------------------------------------------------------
// Purpose: this is the viewport that contains all the hud elements
//-----------------------------------------------------------------------------
class CHudViewport : public CBaseViewport
{
private:
	DECLARE_CLASS_SIMPLE( CHudViewport, CBaseViewport );

protected:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
	{
		BaseClass::ApplySchemeSettings( pScheme );

		gHUD.InitColors( pScheme );

		SetPaintBackgroundEnabled( false );
	}

	virtual IViewPortPanel *CreatePanelByName( const char *szPanelName );
};

int ClientModeHL2MPNormal::GetDeathMessageStartHeight( void )
{
	return m_pViewport->GetDeathMessageStartHeight();
}

IViewPortPanel* CHudViewport::CreatePanelByName( const char *szPanelName )
{
	IViewPortPanel* newpanel = NULL;

	if ( Q_strcmp( PANEL_SCOREBOARD, szPanelName) == 0 )
	{
		newpanel = new CHL2MPClientScoreBoardDialog( this );
		return newpanel;
	}
	else if ( Q_strcmp(PANEL_INFO, szPanelName) == 0 )
	{
		newpanel = new CHL2MPTextWindow( this );
		return newpanel;
	}
	else if ( Q_strcmp(PANEL_SPECGUI, szPanelName) == 0 )
	{
		newpanel = new CHL2MPSpectatorGUI( this );	
		return newpanel;
	}

	
	return BaseClass::CreatePanelByName( szPanelName ); 
}

//-----------------------------------------------------------------------------
// ClientModeHLNormal implementation
//-----------------------------------------------------------------------------
ClientModeHL2MPNormal::ClientModeHL2MPNormal()
{
	m_pViewport = new CHudViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );


#if defined ( SWARM_DLL )
	m_pCurrentPostProcessController = NULL;
	m_PostProcessLerpTimer.Invalidate();
	m_pCurrentColorCorrection = NULL;
#endif
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ClientModeHL2MPNormal::~ClientModeHL2MPNormal()
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientModeHL2MPNormal::Init()
{
	BaseClass::Init();

	// Load up the combine control panel scheme
	g_hVGuiCombineScheme = vgui::scheme()->LoadSchemeFromFileEx( enginevgui->GetPanel( PANEL_CLIENTDLL ), "resource/CombinePanelScheme.res", "CombineScheme" );
	if (!g_hVGuiCombineScheme)
	{
		Warning( "Couldn't load combine panel scheme!\n" );
	}
}



