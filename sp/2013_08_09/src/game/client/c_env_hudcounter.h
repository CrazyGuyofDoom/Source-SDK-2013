//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_ENV_HUDCOUNTER_H
#define C_ENV_HUDCOUNTER_H

#ifdef _WIN32
#pragma once
#endif

class C_EnvHudCounter : public C_BaseEntity
{
	DECLARE_CLASS( C_EnvHudCounter, C_BaseEntity );
public:
	DECLARE_DATADESC();
	DECLARE_CLIENTCLASS();

	C_EnvHudCounter();
	~C_EnvHudCounter();

	virtual void	OnDataChanged( DataUpdateType_t type );
	virtual void	ClientThink( void );

	virtual void	UpdateCounter();

private:

	float		m_flTime;
	float		m_flWarnTime;
	bool		m_bActive;
	bool		m_bOn;

};

#endif // C_ENV_HUDCOUNTER_H