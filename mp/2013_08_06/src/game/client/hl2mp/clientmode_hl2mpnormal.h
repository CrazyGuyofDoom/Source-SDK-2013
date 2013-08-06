//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#if !defined( CLIENTMODE_HLNORMAL_H )
#define CLIENTMODE_HLNORMAL_H
#ifdef _WIN32
#pragma once
#endif

#include "clientmode_shared.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui/Cursor.h>

class CHudViewport;

namespace vgui
{
	typedef unsigned long HScheme;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class ClientModeHL2MPNormal : public ClientModeShared
{
public:
	DECLARE_CLASS( ClientModeHL2MPNormal, ClientModeShared );

	ClientModeHL2MPNormal();
	~ClientModeHL2MPNormal();

	virtual void	Init();
	virtual int		GetDeathMessageStartHeight( void );

#if defined ( SWARM_DLL )
	virtual void	OnColorCorrectionWeightsReset( void );
	virtual float	GetColorCorrectionScale( void ) const { return 1.0f; }


private:
	const C_PostProcessController *m_pCurrentPostProcessController;
	PostProcessParameters_t m_CurrentPostProcessParameters;
	PostProcessParameters_t m_LerpStartPostProcessParameters, m_LerpEndPostProcessParameters;
	CountdownTimer m_PostProcessLerpTimer;

	CHandle<C_ColorCorrection> m_pCurrentColorCorrection;

	ClientCCHandle_t	m_CCFailedHandle;
	float				m_fFailedCCWeight;

	ClientCCHandle_t	m_CCInfestedHandle;
	float				m_fInfestedCCWeight;
#endif
};

extern IClientMode *GetClientModeNormal();
extern vgui::HScheme g_hVGuiCombineScheme;

extern ClientModeHL2MPNormal* GetClientModeHL2MPNormal();

#endif // CLIENTMODE_HLNORMAL_H
