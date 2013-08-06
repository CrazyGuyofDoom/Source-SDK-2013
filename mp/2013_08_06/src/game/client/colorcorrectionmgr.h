//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose : Singleton manager for color correction on the client
//
// $NoKeywords: $
//===========================================================================//

#ifndef COLORCORRECTIONMGR_H
#define COLORCORRECTIONMGR_H

#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#if defined ( SWARM_DLL )
class C_ColorCorrection;
class C_ColorCorrectionVolume;
#endif

//------------------------------------------------------------------------------
// Purpose : Singleton manager for color correction on the client
//------------------------------------------------------------------------------
DECLARE_POINTER_HANDLE( ClientCCHandle_t );
#define INVALID_CLIENT_CCHANDLE ( (ClientCCHandle_t)0 )

class CColorCorrectionMgr : public CBaseGameSystem
{
	// Inherited from IGameSystemPerFrame
public:
	virtual char const *Name() { return "Color Correction Mgr"; }

	// Other public methods
public:
	CColorCorrectionMgr();

	// Create, destroy color correction
	ClientCCHandle_t AddColorCorrection( const char *pName, const char *pFileName = NULL );
	void RemoveColorCorrection( ClientCCHandle_t );

#if defined ( SWARM_DLL )
	void SetColorCorrectionWeight( ClientCCHandle_t h, float flWeight, bool bExclusive /*= false*/ );
	void UpdateColorCorrection();
#endif

	// Modify color correction weights
	void SetColorCorrectionWeight( ClientCCHandle_t h, float flWeight );
	void ResetColorCorrectionWeights();
	void SetResetable( ClientCCHandle_t h, bool bResetable );

	// Is color correction active?
	bool HasNonZeroColorCorrectionWeights() const;

private:
	int m_nActiveWeightCount;

#if defined ( SWARM_DLL )
	bool m_bHaveExclusiveWeight;
	float m_flExclusiveWeight;

	struct SetWeightParams_t
	{
		ClientCCHandle_t handle;
		float flWeight;
		bool bExclusive;
	};

	CUtlVector< SetWeightParams_t > m_colorCorrectionWeights;

	void CommitColorCorrectionWeights();
#endif
};

#if defined ( SWARM_DLL )
void UpdateColorCorrectionEntities( C_BasePlayer *pPlayer, float ccScale, C_ColorCorrection **pList, int listCount );
void UpdateColorCorrectionVolumes( C_BasePlayer *pPlayer, float ccScale, C_ColorCorrectionVolume **pList, int listCount );
#endif

//------------------------------------------------------------------------------
// Singleton access
//------------------------------------------------------------------------------
extern CColorCorrectionMgr *g_pColorCorrectionMgr;


#endif // COLORCORRECTIONMGR_H
