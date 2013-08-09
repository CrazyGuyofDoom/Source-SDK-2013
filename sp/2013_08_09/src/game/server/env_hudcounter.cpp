//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "env_hudcounter.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SF_HUDCOUNTER_ALLPLAYERS			0x0001

BEGIN_DATADESC( CEnvHudCounter )

	DEFINE_THINKFUNC( CounterThink ),

	DEFINE_EMBEDDED( m_Counter ),
	DEFINE_FIELD( m_bHasExpired, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flTime, FIELD_TIME ),
	DEFINE_FIELD( m_flPausedTime, FIELD_TIME ),
	DEFINE_KEYFIELD( m_flWarnTime, FIELD_FLOAT, "warntime" ),
	DEFINE_KEYFIELD( m_flStartTime, FIELD_FLOAT, "starttime" ),
	DEFINE_KEYFIELD( m_bActive, FIELD_BOOLEAN, "start_active" ),
	DEFINE_KEYFIELD( m_bOn, FIELD_BOOLEAN, "start_on" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "ResetCounter", InputResetCounter ),
	DEFINE_INPUTFUNC( FIELD_VOID, "StartCounter", InputStartCounter ),
	DEFINE_INPUTFUNC( FIELD_VOID, "StopCounter", InputStopCounter ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetCounter", InputSetCounter ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowHudCounter", InputShowHudCounter ),
	DEFINE_INPUTFUNC( FIELD_VOID, "HideHudCounter", InputHideHudCounter ),
	DEFINE_INPUTFUNC( FIELD_VOID, "PauseCounter", InputPauseCounter ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ResumeCounter", InputResumeCounter ),

	DEFINE_OUTPUT( m_OnCounterSet, "OnCounterSet"),
	DEFINE_OUTPUT( m_OnCounterReset, "OnCounterReset"),
	DEFINE_OUTPUT( m_OnCounterStarted, "OnCounterStarted"),
	DEFINE_OUTPUT( m_OnCounterStopped, "OnCounterStopped"),
	DEFINE_OUTPUT( m_OnCounterExpired, "OnCounterExpired"),
	DEFINE_OUTPUT( m_OnCounterShown, "OnCounterShown"),
	DEFINE_OUTPUT( m_OnCounterHidden, "OnCounterHidden"),
	DEFINE_OUTPUT( m_OnCounterPaused, "OnCounterPaused"),
	DEFINE_OUTPUT( m_OnCounterResumed, "OnCounterResumed"),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CEnvHudCounter, DT_EnvHudCounter)
	SendPropFloat( SENDINFO(m_flTime), 0, SPROP_NOSCALE),
	SendPropFloat( SENDINFO(m_flWarnTime), 0, SPROP_NOSCALE),
	SendPropBool( SENDINFO(m_bActive) ),
	SendPropBool( SENDINFO(m_bOn) ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( env_hudcounter, CEnvHudCounter );

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CEnvHudCounter::CEnvHudCounter()
{
	m_flTime = 0.1f;
	m_flPausedTime = 0.0f;
	m_bHasExpired = false;
	m_bPaused = false;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CEnvHudCounter::~CEnvHudCounter()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEnvHudCounter::Precache()
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEnvHudCounter::Spawn()
{
	Precache();

	BaseClass::Spawn();

	SetSolid( SOLID_NONE );
	SetMoveType( MOVETYPE_NONE );

	AddEFlags( EFL_FORCE_CHECK_TRANSMIT );

	SetThink( &CEnvHudCounter::CounterThink );

	SetNextThink( gpGlobals->curtime + 0.1f );

	m_flPausedTime = m_flStartTime;
	m_Counter.Set( m_flStartTime );

	// Start the counter if start_active is set.
	if ( m_bActive && !m_Counter.IsRunning() )
	{
		variant_t var;
		AcceptInput( "StartCounter", this, this, var, 0 );
	}
}

void CEnvHudCounter::Activate()
{
	BaseClass::Activate();
}

void CEnvHudCounter::CounterThink( void )
{
	if ( !m_bPaused )
	{
		if ( m_bActive )
		{
			if ( m_flTime <= 0.0f && !m_bHasExpired )
			{
				m_bHasExpired = true;
				m_OnCounterExpired.FireOutput( this, this );
			}

			m_flTime = m_Counter.GetRemaining();
		}
	}
	else
	{
		m_flPausedTime = m_flTime;
		m_flTime = m_flPausedTime;
	}

	SetNextThink( gpGlobals->curtime + 0.1f );
}

void CEnvHudCounter::SetOn( bool bState )
{
	m_bOn = bState;
}

void CEnvHudCounter::SetActive( bool bState )
{
	m_bActive = bState;
}

void CEnvHudCounter::StartCounter( bool bWasPaused )
{
	if ( bWasPaused )
		m_Counter.Start( m_flPausedTime );
	else
		m_Counter.Start();

	if ( m_bHasExpired && m_flTime > 0.0f )
		m_bHasExpired = false;

	m_bActive = true;
}

void CEnvHudCounter::StopCounter()
{
	m_Counter.Stop();
	m_bActive = false;
}

void CEnvHudCounter::ResetCounter()
{
	m_Counter.Set(m_flStartTime);
	m_bHasExpired = false;
}

void CEnvHudCounter::PauseCounter()
{
	m_flPausedTime = m_flTime;
	m_bPaused = true;
	StopCounter();
}

void CEnvHudCounter::ResumeCounter()
{
	m_bPaused = false;
	StartCounter( true );
}

void CEnvHudCounter::SetCounter( float flTime )
{
	m_flStartTime = flTime;
	m_Counter.Set( flTime );
}

void CEnvHudCounter::HideCounter()
{
	m_bOn = false;
}

void CEnvHudCounter::ShowCounter()
{
	m_bOn = true;
}

void CEnvHudCounter::InputPauseCounter( inputdata_t &inputdata )
{
	PauseCounter();
	m_OnCounterPaused.FireOutput( this, this );
}

void CEnvHudCounter::InputResumeCounter( inputdata_t &inputdata )
{
	ResumeCounter();
	m_OnCounterResumed.FireOutput( this, this );
}

void CEnvHudCounter::InputShowHudCounter( inputdata_t &inputdata )
{
	SetOn( true );
	m_OnCounterShown.FireOutput( this, this );
}

void CEnvHudCounter::InputHideHudCounter( inputdata_t &inputdata )
{
	SetOn( false );
	m_OnCounterHidden.FireOutput( this, this );
}

void CEnvHudCounter::InputSetCounter( inputdata_t &inputdata )
{
	SetCounter( inputdata.value.Float() );
	m_OnCounterSet.FireOutput( this, this );
}

void CEnvHudCounter::InputResetCounter( inputdata_t &inputdata )
{
	ResetCounter();
	m_OnCounterReset.FireOutput( this, this );
}

void CEnvHudCounter::InputStartCounter( inputdata_t &inputdata )
{
	StartCounter();
	m_OnCounterStarted.FireOutput( this, this );
}

void CEnvHudCounter::InputStopCounter( inputdata_t &inputdata )
{
	StopCounter();
	m_OnCounterStopped.FireOutput( this, this );
}

void CEnvHudCounter::SetWarnTime( float flTime )
{
	m_flWarnTime = flTime;
}

inline bool CEnvHudCounter::AllPlayers( void ) 
{ 
	return (m_spawnflags & SF_HUDCOUNTER_ALLPLAYERS) != 0; 
}
