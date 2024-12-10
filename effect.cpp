#include "StdAfx.h"
#include "Misc.h"
#include "effect.h"
#include "ZEffect.h"
#include <memory.h>

vector< ZEffectPtr* > *pvpEffect;
vector< ZEffectPtr* > *pvpEffectList;

ZEffectPtr *pEffectBlank;

ZEffectPtr *CreateEffect_Blank( );
ZEffectPtr *CreateEffect_BezierCube( );
ZEffectPtr *CreateEffect_CollapsingLightSphere( );
ZEffectPtr *CreateEffect_Distortion1( );
ZEffectPtr *CreateEffect_Distortion2( );
ZEffectPtr *CreateEffect_Distortion2Col( );
ZEffectPtr *CreateEffect_Flowmap( );
ZEffectPtr *CreateEffect_Tunnel( );
ZEffectPtr *CreateEffect_WaterGlobe( );
ZEffectPtr *CreateEffect_Tube( );
ZEffectPtr *CreateEffect_Sun( );
ZEffectPtr *CreateEffect_Bumpmapping( );
ZEffectPtr *CreateEffect_Spectrum( );
ZEffectPtr *CreateEffect_Rings( );
ZEffectPtr *CreateEffect_Phased( );
ZEffectPtr *CreateEffect_MotionBlur1( );
ZEffectPtr *CreateEffect_MotionBlur2( );
ZEffectPtr *CreateEffect_MotionBlur3( );
ZEffectPtr *CreateEffect_MotionBlur3Alt( );
ZEffectPtr *CreateEffect_MorphingSphere( );
ZEffectPtr *CreateEffect_Metaballs( );
ZEffectPtr *CreateEffect_LightTentacles( );
ZEffectPtr *CreateEffect_LightStar( );
ZEffectPtr *CreateEffect_LightSphere( );
ZEffectPtr *CreateEffect_LightRing( );
ZEffectPtr *CreateEffect_SuperSampling( );
/*
extern ZEffectPtr *pEffectBezierCube;
extern ZEffectPtr *pEffectCollapsingLightSphere;
extern ZEffectPtr *pEffectDistortion1;
extern ZEffectPtr *pEffectDistortion2;
extern ZEffectPtr *pEffectDistortion2Col;
extern ZEffectPtr *pEffectFlowmap;
extern ZEffectPtr *pEffectTunnel;
extern ZEffectPtr *pEffectWaterGlobe;
extern ZEffectPtr *pEffectTube;
extern ZEffectPtr *pEffectSun;
extern ZEffectPtr *pEffectBumpmapping;
extern ZEffectPtr *pEffectSpectrum;
extern ZEffectPtr *pEffectRings;
extern ZEffectPtr *pEffectPhased;
extern ZEffectPtr *pEffectMotionBlur1;
extern ZEffectPtr *pEffectMotionBlur2;
extern ZEffectPtr *pEffectMotionBlur3;
extern ZEffectPtr *pEffectMotionBlur3Alt;
extern ZEffectPtr *pEffectMorphingSphere;
extern ZEffectPtr *pEffectMetaballs;
extern ZEffectPtr *pEffectLightTentacles;
extern ZEffectPtr *pEffectLightStar;
extern ZEffectPtr *pEffectLightSphere;
extern ZEffectPtr *pEffectLightRing;
extern ZEffectPtr *pEffectSuperSampling;
*/
static void AddEffect(ZEffectPtr * (*fn)( ), const char *sName, int nDrawOrder, float fStartupWeight, int nTex, ...)
{
	ZEffectPtr *pEffect = fn( );
	pEffect->sName = sName;
	pEffect->nDrawOrder = nDrawOrder;
	pEffect->fStartupWeight = fStartupWeight;

	va_list pArg;
	va_start(pArg, nTex);
	while(nTex != 0)
	{
		pEffect->snTexture.insert(nTex);
		nTex = va_arg(pArg, int);
	}
	va_end(pArg);

	if(pvpEffectList->size() == 0) pvpEffectList->push_back(pEffect);
	else
	{
		for(int i = 1;;i++)
		{
			if(i == pvpEffectList->size() || _stricmp(pEffect->sName.c_str(), (*pvpEffectList)[i]->sName.c_str()) < 0)
			{
				pvpEffectList->insert(pvpEffectList->begin() + i, pEffect);
				break;
			}
		}
	}
}

void CreateEffectList()
{
	_ASSERT( pvpEffectList == NULL );
	pvpEffectList = new vector< ZEffectPtr* >;

	AddEffect(&CreateEffect_Blank, "Blank", ZORDER_BLANK, 1.0f, 0);
	AddEffect(&CreateEffect_Tunnel, "Tunnel", ZORDER_TUNNEL, 1.0f, TC_WTTUNNEL, 0);

	pEffectBlank = (*pvpEffectList)[ 0 ];


/*	AddEffect(pEffectWaterGlobe, "WaterGlobe", ZORDER_WATERGLOBE, 10.0f, TC_EMWATERGLOBE, 0);
	AddEffect(pEffectTube, "Tube", ZORDER_TUBE, 1.0f, TC_EMTUBE, 0);
	AddEffect(pEffectSun, "Sun", ZORDER_SUN, 1.0f, 0);
//	//	AddEffect(pEffectBumpmapping, "Bumpmapping", ZORDER_BUMPMAPPING, 8.0f, TC_WTBUMPMAPBACK, TC_EMBUMPMAPTENTACLES, 0);
	AddEffect(pEffectSpectrum, "Spectrum", ZORDER_ANALYSER, 1.0f, TC_EMANALYSER, 0);
	AddEffect(pEffectRings, "ConcentricRings", ZORDER_RINGS, 1.0f, TC_EMRINGS, 0);
	AddEffect(pEffectPhased, "Phased", ZORDER_PHASED, 1.0f, TC_LBPHASED, 0);
	AddEffect(pEffectMotionBlur1, "MotionBlur1", ZORDER_MOTIONBLUR, 1.0f, TC_EMMOTIONBLUR, 0);
	AddEffect(pEffectMotionBlur2, "MotionBlur2", ZORDER_MOTIONBLUR2, 1.0f, TC_EMMOTIONBLUR2, 0);
	AddEffect(pEffectMotionBlur3, "MotionBlur3", ZORDER_MOTIONBLUR3, 1.0f, TC_EMMOTIONBLUR3, 0);
	AddEffect(pEffectMotionBlur3Alt, "MotionBlur3(Alt)", ZORDER_MOTIONBLUR3ALT, 1.0f, TC_EMMOTIONBLUR3ALT, 0);
	AddEffect(pEffectMorphingSphere, "MorphingSphere", ZORDER_MORPHINGSPHERE, 1.0f, TC_EMMORPHINGSPHERE, 0);
	AddEffect(pEffectLightTentacles, "LightTentacles", ZORDER_LIGHTTENTACLES, 1.0f, TC_LBLIGHTTENTACLES, 0);
	AddEffect(pEffectLightStar, "LightStar", ZORDER_LIGHTSTAR, 1.0f, TC_LBLIGHTSTAR, TC_WTLIGHTSTAR, 0);
	AddEffect(pEffectLightSphere, "LightSphere", ZORDER_LIGHTSPHERE, 1.0f, TC_LBLIGHTSPHERE, TC_WTLIGHTSPHERE, 0);
	AddEffect(pEffectLightRing, "LightRing", ZORDER_LIGHTRING, 1.0f, TC_LBLIGHTRING, TC_WTLIGHTRING, 0);
	AddEffect(pEffectFlowmap, "Flowmap", ZORDER_FLOWMAP, 10.0f, 0);
	AddEffect(pEffectSuperSampling, "SuperSampling", ZORDER_DOTSTAR, 1.0f, TC_LBDOTSTAR, TC_WTDOTSTAR, 0);
	AddEffect(pEffectDistortion2, "Distortion2", ZORDER_DISTORTION2, 1.0f, TC_WTDISTORTION2, 0);
	AddEffect(pEffectDistortion2Col, "Distortion2(Lit)", ZORDER_DISTORTION2COL, 1.0f, TC_WTDISTORTION2COL, 0);
	AddEffect(pEffectCollapsingLightSphere, "CollapsingLightSphere", ZORDER_COLLAPSINGSPHERE, 1.0f, TC_LBCOLLAPSINGSPHERE, TC_WTCOLLAPSINGSPHERE, 0);
	AddEffect(pEffectBezierCube, "BezierCube", ZORDER_BEZIERCUBE, 1.0f, TC_LBBEZIERCUBE, TC_WTBEZIERCUBE, 0);
	AddEffect(pEffectDistortion1, "Distortion1", ZORDER_DISTORTION, 1.0f, TC_WTDISTORTION, 0);
*/
}

