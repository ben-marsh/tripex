#pragma once

#include "Vector3.h"
#include "ColorRgb.h"
#include "Point.h"

#define MAX_TEXTURES 1

struct ZVertex
{
	Vector3 m_vPos;
	Vector3 m_vNormal;
	ColorRgb m_cDiffuse;
	ColorRgb m_cSpecular;
	ZPoint<float> m_aTex[MAX_TEXTURES];
};

struct ZVertexTL
{
	Vector3 m_vPos;
	float m_fRHW;
	ColorRgb m_cDiffuse;
	ColorRgb m_cSpecular;
	ZPoint<float> m_aTex[MAX_TEXTURES];
};
