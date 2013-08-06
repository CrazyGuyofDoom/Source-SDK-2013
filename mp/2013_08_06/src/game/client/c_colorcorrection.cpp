//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Color correction entity with simple radial falloff
//
// $NoKeywords: $
//===========================================================================//
#include "cbase.h"

#include "filesystem.h"
#include "cdll_client_int.h"
#include "colorcorrectionmgr.h"
#include "materialsystem/MaterialSystemUtil.h"

#if defined ( SWARM_DLL )
#include "c_colorcorrection.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


static ConVar mat_colcorrection_disableentities( "mat_colcorrection_disableentities", "0", FCVAR_NONE, "Disable map color-correction entities" );

#if !defined ( SWARM_DLL )

//------------------------------------------------------------------------------
// Purpose : Color correction entity with radial falloff
//------------------------------------------------------------------------------
class C_ColorCorrection : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_ColorCorrection, C_BaseEntity );

	DECLARE_CLIENTCLASS();

	C_ColorCorrection();
	virtual ~C_ColorCorrection();

	void OnDataChanged(DataUpdateType_t updateType);
	bool ShouldDraw();

	void ClientThink();

private:
	Vector	m_vecOrigin;

	float	m_minFalloff;
	float	m_maxFalloff;
	float	m_flCurWeight;
	char	m_netLookupFilename[MAX_PATH];

	bool	m_bEnabled;

	ClientCCHandle_t m_CCHandle;
};

#endif // SWARM_DLL

IMPLEMENT_CLIENTCLASS_DT(C_ColorCorrection, DT_ColorCorrection, CColorCorrection)
	RecvPropVector( RECVINFO(m_vecOrigin) ),
	RecvPropFloat(  RECVINFO(m_minFalloff) ),
	RecvPropFloat(  RECVINFO(m_maxFalloff) ),
	RecvPropFloat(  RECVINFO(m_flCurWeight) ),
	RecvPropString( RECVINFO(m_netLookupFilename) ),
	RecvPropBool(   RECVINFO(m_bEnabled) ),

END_RECV_TABLE()


//------------------------------------------------------------------------------
// Constructor, destructor
//------------------------------------------------------------------------------
C_ColorCorrection::C_ColorCorrection()
{
	m_CCHandle = INVALID_CLIENT_CCHANDLE;
}

C_ColorCorrection::~C_ColorCorrection()
{
	g_pColorCorrectionMgr->RemoveColorCorrection( m_CCHandle );
}


//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
void C_ColorCorrection::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		if ( m_CCHandle == INVALID_CLIENT_CCHANDLE )
		{
			char filename[MAX_PATH];
			Q_strncpy( filename, m_netLookupFilename, MAX_PATH );

			m_CCHandle = g_pColorCorrectionMgr->AddColorCorrection( filename );
			SetNextClientThink( ( m_CCHandle != INVALID_CLIENT_CCHANDLE ) ? CLIENT_THINK_ALWAYS : CLIENT_THINK_NEVER );
		}
	}
}

//------------------------------------------------------------------------------
// We don't draw...
//------------------------------------------------------------------------------
bool C_ColorCorrection::ShouldDraw()
{
	return false;
}

#if defined ( SWARM_DLL )
void C_ColorCorrection::Update( C_BasePlayer *pPlayer, float ccScale )
{
	Assert( m_CCHandle != INVALID_CLIENT_CCHANDLE );

	if ( mat_colcorrection_disableentities.GetInt() )
	{
		// Allow the colorcorrectionui panel (or user) to turn off color-correction entities
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCHandle, 0.0f, m_bExclusive );
		return;
	}

	bool bEnabled = IsClientSide() ? m_bEnabledOnClient: m_bEnabled;

	// fade weight on client
	if ( IsClientSide() )
	{
		m_flCurWeightOnClient = Lerp( GetFadeRatio(), m_flFadeStartWeight, m_bFadingIn ? m_flMaxWeight : 0.0f );
	}

	float flCurWeight = IsClientSide() ? m_flCurWeightOnClient : m_flCurWeight;

	if( !bEnabled && flCurWeight == 0.0f )
	{
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCHandle, 0.0f, m_bExclusive );
		return;
	}

	Vector playerOrigin = pPlayer->GetAbsOrigin();

	float weight = 0;
	if ( ( m_minFalloff != -1 ) && ( m_maxFalloff != -1 ) && m_minFalloff != m_maxFalloff )
	{
		float dist = (playerOrigin - m_vecOrigin).Length();
		weight = (dist-m_minFalloff) / (m_maxFalloff-m_minFalloff);
		if ( weight<0.0f ) weight = 0.0f;	
		if ( weight>1.0f ) weight = 1.0f;	
	}

	g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCHandle, flCurWeight * ( 1.0 - weight ) * ccScale, m_bExclusive );
}
#endif

#if !defined ( SWARM_DLL )
void C_ColorCorrection::ClientThink()
{
	if ( m_CCHandle == INVALID_CLIENT_CCHANDLE )
		return;

	if ( mat_colcorrection_disableentities.GetInt() )
	{
		// Allow the colorcorrectionui panel (or user) to turn off color-correction entities
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCHandle, 0.0f );
		return;
	}

	if( !m_bEnabled && m_flCurWeight == 0.0f )
	{
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCHandle, 0.0f );
		return;
	}

	CBaseEntity *pPlayer = UTIL_PlayerByIndex(1);
	if( !pPlayer )
		return;

	Vector playerOrigin = pPlayer->GetAbsOrigin();

	float weight = 0;
	if ( ( m_minFalloff != -1 ) && ( m_maxFalloff != -1 ) && m_minFalloff != m_maxFalloff )
	{
		float dist = (playerOrigin - m_vecOrigin).Length();
		weight = (dist-m_minFalloff) / (m_maxFalloff-m_minFalloff);
		if ( weight<0.0f ) weight = 0.0f;	
		if ( weight>1.0f ) weight = 1.0f;	
	}
	
#if defined ( SWARM_DLL )
	g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCHandle, m_flCurWeight * ( 1.0 - weight ), false );
#else
	g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCHandle, m_flCurWeight * ( 1.0 - weight ) );
#endif

	BaseClass::ClientThink();
}
#endif


#if defined ( SWARM_DLL )

void C_ColorCorrection::EnableOnClient( bool bEnable, bool bSkipFade )
{
	if ( !IsClientSide() )
	{
		return;
	}

	if( m_bEnabledOnClient== bEnable )
	{
		return;
	}

	m_bFadingIn = bEnable;
	m_bEnabledOnClient = bEnable;

	// initialize countdown timer
	m_flFadeStartWeight = m_flCurWeightOnClient;
	float flFadeTimeScale = 1.0f;
	if ( m_flMaxWeight != 0.0f )
	{
		flFadeTimeScale = m_flCurWeightOnClient / m_flMaxWeight;
	}

	if ( m_bFadingIn )
	{
		flFadeTimeScale = 1.0f - flFadeTimeScale;
	}

	if ( bSkipFade )
	{
		flFadeTimeScale = 0.0f;
	}

	StartFade( flFadeTimeScale * ( m_bFadingIn ? m_flFadeInDuration : m_flFadeOutDuration ) );

	// update the clientside weight once here, in case the fade duration is 0
	m_flCurWeightOnClient = Lerp( GetFadeRatio(), m_flFadeStartWeight, m_bFadingIn ? m_flMaxWeight : 0.0f );
}

Vector C_ColorCorrection::GetOrigin()
{
	return m_vecOrigin;
}

float C_ColorCorrection::GetMinFalloff()
{
	return m_minFalloff;
}

float C_ColorCorrection::GetMaxFalloff()
{
	return m_maxFalloff;
}

void C_ColorCorrection::SetWeight( float fWeight )
{
	g_pColorCorrectionMgr->SetColorCorrectionWeight( m_CCHandle, fWeight, false );
}

void C_ColorCorrection::StartFade( float flDuration )
{
	m_flFadeStartTime = gpGlobals->curtime;
	m_flFadeDuration = MAX( flDuration, 0.0f );
}

float C_ColorCorrection::GetFadeRatio() const
{
	float flRatio = 1.0f;
	
	if ( m_flFadeDuration != 0.0f )
	{
		flRatio = ( gpGlobals->curtime - m_flFadeStartTime ) / m_flFadeDuration;
		flRatio = clamp( flRatio, 0.0f, 1.0f );
	}
	return flRatio;
}

bool C_ColorCorrection::IsFadeTimeElapsed() const
{
	return	( ( gpGlobals->curtime - m_flFadeStartTime ) > m_flFadeDuration ) ||
			( ( gpGlobals->curtime - m_flFadeStartTime ) < 0.0f );
}

void UpdateColorCorrectionEntities( C_BasePlayer *pPlayer, float ccScale, C_ColorCorrection **pList, int listCount )
{
	for ( int i = 0; i < listCount; i++ )
	{
		pList[i]->Update(pPlayer, ccScale);
	}
}

#endif // SWARM_DLL












