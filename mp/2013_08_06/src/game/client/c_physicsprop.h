//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_PHYSICSPROP_H
#define C_PHYSICSPROP_H
#ifdef _WIN32
#pragma once
#endif

#include "c_breakableprop.h"
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_PhysicsProp : public C_BreakableProp
{
	typedef C_BreakableProp BaseClass;
public:
	DECLARE_CLIENTCLASS();

#if defined ( SWARM_DLL )
		// Inherited from IClientUnknown
public:
	virtual IClientModelRenderable*	GetClientModelRenderable();
#endif

	C_PhysicsProp();
	~C_PhysicsProp();

	virtual bool OnInternalDrawModel( ClientModelRenderInfo_t *pInfo );

protected:
	// Networked vars.
	bool m_bAwake;
	bool m_bAwakeLastTime;
};

#endif // C_PHYSICSPROP_H 
