//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hud_counterdisplay.h"

#include "c_env_hudcounter.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( C_EnvHudCounter )
	DEFINE_FIELD( m_flTime, FIELD_TIME ),
	DEFINE_FIELD( m_flWarnTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bOn, FIELD_BOOLEAN ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Networking
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT(C_EnvHudCounter, DT_EnvHudCounter, CEnvHudCounter)
	RecvPropFloat( RECVINFO(m_flTime) ),
	RecvPropFloat( RECVINFO(m_flWarnTime) ),
	RecvPropBool( RECVINFO( m_bActive ) ),
	RecvPropBool( RECVINFO( m_bOn ) ),
END_RECV_TABLE()


C_EnvHudCounter::C_EnvHudCounter()
{
}

C_EnvHudCounter::~C_EnvHudCounter()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_EnvHudCounter::UpdateCounter()
{
	if ( GetHudCounter() )
	{
		if ( !m_bOn )
			GetHudCounter()->SetOn( 0 );
		else
			GetHudCounter()->SetOn( 1 );

		GetHudCounter()->SetTimeValue( (int)m_flTime );
		GetHudCounter()->SetTimeWarnValue( (int)m_flWarnTime );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_EnvHudCounter::OnDataChanged( DataUpdateType_t updatetype )
{
	BaseClass::OnDataChanged( updatetype );

	if ( updatetype == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_EnvHudCounter::ClientThink( void )
{
	// Don't update if our frame hasn't moved forward (paused)
	if ( gpGlobals->frametime <= 0.0f )
		return;

	UpdateCounter();

	//DevMsg("C_EnvHudCounter::ClientThink\n");
	DevMsg( "Counter time: %f\n", m_flTime );

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}
