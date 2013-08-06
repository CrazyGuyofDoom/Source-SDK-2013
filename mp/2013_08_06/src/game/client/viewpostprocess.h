//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================

#ifndef VIEWPOSTPROCESS_H
#define VIEWPOSTPROCESS_H

#if defined( _WIN32 )
#pragma once
#endif

#if defined ( SWARM_DLL )
#include "postprocess_shared.h"
#endif

void DoEnginePostProcessing( int x, int y, int w, int h, bool bFlashlightIsOn, bool bPostVGui = false );
void DoImageSpaceMotionBlur( const CViewSetup &view, int x, int y, int w, int h );
void DumpTGAofRenderTarget( const int width, const int height, const char *pFilename );

#if defined ( SWARM_DLL ) || defined ( SWARM_DOF_ENABLED )

bool IsDepthOfFieldEnabled();
void DoDepthOfField( const CViewSetup &view );

#endif // SWARM_DLL

#if defined ( SWARM_DLL )

void BlurEntity( IClientRenderable *pRenderable, bool bPreDraw, int drawFlags, const RenderableInstance_t &instance, const CViewSetup &view, int x, int y, int w, int h );

void UpdateMaterialSystemTonemapScalar();

float GetCurrentTonemapScale();

void SetOverrideTonemapScale( bool bEnableOverride, float flTonemapScale );

void SetOverridePostProcessingDisable( bool bForceOff );

void DoBlurFade( float flStrength, float flDesaturate, int x, int y, int w, int h );

void SetPostProcessParams( const PostProcessParameters_t *pPostProcessParameters );

void SetViewFadeParams( byte r, byte g, byte b, byte a, bool bModulate );

#endif

#endif // VIEWPOSTPROCESS_H
