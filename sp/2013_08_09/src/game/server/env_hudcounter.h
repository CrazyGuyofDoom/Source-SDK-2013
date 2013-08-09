//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ENV_HUDCOUNTER_H
#define ENV_HUDCOUNTER_H

#ifdef _WIN32
#pragma once
#endif

#include "baseentity.h"
#include "simtimer.h"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CEnvHudCounter : public CBaseEntity
{
public:
	DECLARE_CLASS( CEnvHudCounter, CBaseEntity );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CEnvHudCounter();
	~CEnvHudCounter();

	virtual void	Spawn();
	virtual void	Activate();
	void	Precache();

	inline	bool	AllPlayers( void );

	void	CounterThink( void );

	void	HideCounter();
	void	ShowCounter();
	void	StartCounter( bool bWasPaused = false );
	void	StopCounter();
	void	PauseCounter();
	void	ResumeCounter();
	void	ResetCounter();
	void	SetCounter( float flTime );
	void	SetWarnTime( float flTime );

	void	SetActive( bool bState );
	bool	IsActive() { return m_bActive; }
	void	SetOn( bool bState );
	bool	IsOn() { return m_bOn; }

	CStopwatch	GetCounter() const { return m_Counter; }

	void	InputShowHudCounter( inputdata_t &inputdata );
	void	InputHideHudCounter( inputdata_t &inputdata );
	void	InputSetCounter( inputdata_t &inputdata );
	void	InputResetCounter( inputdata_t &inputdata );
	void	InputStartCounter( inputdata_t &inputdata );
	void	InputStopCounter( inputdata_t &inputdata );
	void	InputPauseCounter( inputdata_t &inputdata );
	void	InputResumeCounter( inputdata_t &inputdata );

	COutputEvent	m_OnCounterExpired;
	COutputEvent	m_OnCounterStopped;
	COutputEvent	m_OnCounterStarted;
	COutputEvent	m_OnCounterReset;
	COutputEvent	m_OnCounterSet;
	COutputEvent	m_OnCounterShown;
	COutputEvent	m_OnCounterHidden;
	COutputEvent	m_OnCounterPaused;
	COutputEvent	m_OnCounterResumed;

private:
	CStopwatch		m_Counter;
	float			m_flStartTime;
	float			m_flPausedTime;
	bool			m_bPaused;
	bool			m_bHasExpired;

	CNetworkVar( float, m_flTime );
	CNetworkVar( float, m_flWarnTime );
	CNetworkVar( bool, m_bActive );
	CNetworkVar( bool, m_bOn );
};

#endif // ENV_HUDCOUNTER_H