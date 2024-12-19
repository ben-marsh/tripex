#include "Platform.h"
#include "Actor.h"
#include "error.h"

float pSpriteTexU[ 4 ] = { 0.0f, 1.0f, 1.0f, 0.0f };
float pSpriteTexV[ 4 ] = { 0.0f, 0.0f, 1.0f, 1.0f };

extern ZArray<int> *ppnEmptyVertex;
extern ZArray<int> *ppnEmptyFace;
ZArray<Face> *ppfExposureBuffer;
ZArray<Face> *ppClippedFaceBuffer;
ZArray<ZVertexTL> *ppVertexBuffer;
ZArray<Face> *ppFaceBuffer;

void InitObjectArrays( )
{
	ppfExposureBuffer = new ZArray<Face>;
	ppClippedFaceBuffer = new ZArray<Face>;
	ppVertexBuffer = new ZArray<ZVertexTL>;
	ppFaceBuffer = new ZArray<Face>;
}


/*---------------------------------
* ZObject::Texture
-----------------------------------*/

/*---------------------------------
* Constructor:
-----------------------------------*/

Actor::TextureEntry::TextureEntry( )
{
	m_fPitch = m_fYaw = m_fRoll = 0.0f; 
	m_pTexture = NULL; 
	m_nType = T_UNUSED;
}

/*---------------------------------
* Set( ):
-----------------------------------*/

void Actor::TextureEntry::Set( Type nType, Texture *pTexture )
{
	m_nType = nType;
	m_pTexture = pTexture;
}









/*---------------------------------
* ZObject::Light
-----------------------------------*/

/*---------------------------------
* Constructor:
-----------------------------------*/

Actor::Light::Light( Type nType, WideColorRgb cColour )
{
	m_nType = nType;
	m_bAttenuate = false; 
	m_cColour = cColour; 
}







/*---------------------------------
* ZObject:
-----------------------------------*/

Actor::Actor()
{
	wClipMask = CLIP_FLAG_MIN_X | CLIP_FLAG_MAX_X | CLIP_FLAG_MIN_Y | CLIP_FLAG_MAX_Y | CLIP_FLAG_MIN_Z | CLIP_FLAG_MAX_Z;
	wcAmbientLight = ColorRgb::White();
	fFrameHistory = 1.0f;
	fFrameTime = 0.3f;
	fRoll = fPitch = fYaw = 0.0f;
	vPosition = Vector3(0, 0, 0);
	nExposure = 1;
	pTransVertex.nStep = 1000;
	fDelayHistory = 0;
//	pClippedFace.nStep = 50;
	fReflectivity = 1.0;
	fSpriteSize = 7.5f;
	fSpriteHistoryLength = 0.5;
	wcExposureLightChange = ColorRgb(0, 0, 0);
	nMaxHistoryLength = 15;
}
Actor::~Actor()
{
	for(int i = 0; i < ppwVertexFaceList.GetLength(); i++)
	{
		free(ppwVertexFaceList[i]);
	}
}
Vector3 Actor::GetCentre()
{
	Vector3 v(0, 0, 0);
	for(int i = 0; i < pVertex.GetLength(); i++)
	{
		v += pVertex[i].m_vPos;
	}
	v /= (float)pVertex.GetLength();
	return v;
}
void Actor::TransformVertices(Matrix44 &m)
{
	for(int i = 0; i < pVertex.GetLength(); i++)
	{
		pVertex[i].m_vPos *= m;
	}
}
void Actor::FindFaceOrder(const Vector3 &vIntPoint)
{
	for(int i = 0; i < pFace.GetLength(); i++)
	{
		Face f = pFace[i];
		Vector3 vNormal = (pVertex[f[1]].m_vPos - pVertex[f[0]].m_vPos).Cross(pVertex[f[2]].m_vPos - pVertex[f[0]].m_vPos);
		if((pVertex[pFace[i][0]].m_vPos - vIntPoint).Dot(vNormal) > 0)
		{
			pFace[i].SwapCull();
		}
	}
}
void Actor::FindVertexFaceList()
{
	for(int i = 0; i < ppwVertexFaceList.GetLength(); i++)
	{
		free(ppwVertexFaceList[i]);
	}
	ppwVertexFaceList.SetLength(pVertex.GetLength());
	for(int i = 0; i < pVertex.GetLength(); i++) 
	{
		ppwVertexFaceList[i] = NULL;
	}
	for(int i = 0; i < pFace.GetLength(); i++)
	{
		for(int j = 0; j < 3; j++)
		{
			UINT16*&pwFace = ppwVertexFaceList[pFace[i][j]];
			int k = 0; 
			if(pwFace == NULL) pwFace = (UINT16*)malloc(4 * sizeof(UINT16));
			else
			{
				while(pwFace[k] != WORD_INVALID_INDEX) k++;
				if(k >= 3) pwFace = (UINT16*)realloc(pwFace, (k + 2) * sizeof(UINT16));
			}
			pwFace[k] = i;
			pwFace[k+1] = WORD_INVALID_INDEX;
		}
	}
	m_bsFlag.set( F_VALID_VERTEX_FACE_LIST );
}
void Actor::FindVertexNormals()
{
	if(!m_bsFlag.test( F_VALID_VERTEX_FACE_LIST )) FindVertexFaceList();

	pvFaceNormal.SetLength(pFace.GetLength());
	for(int i = 0; i < pFace.GetLength(); i++)
	{
		Face f = pFace[i];
		pvFaceNormal[i] = ((pVertex[f[1]].m_vPos - pVertex[f[0]].m_vPos).Cross(pVertex[f[2]].m_vPos - pVertex[f[0]].m_vPos)).Normal(fObjectNormal);
	}

	for(int i = 0; i < pVertex.GetLength(); i++)
	{
		Vector3 vNormal = Vector3(0, 0, 0);
		UINT16 *pwFace = ppwVertexFaceList[i];
		for(int j = 0; pwFace[j] != WORD_INVALID_INDEX; j++)
		{
			vNormal += pvFaceNormal[pwFace[j]];
		}
		pVertex[i].m_vNormal = vNormal.Normal(fObjectNormal);
	}

	m_bsFlag.set( F_VALID_VERTEX_NORMALS );
}
void Actor::FindMeshEdges()
{
	Edge e;
	for(int i = 0; i < pFace.GetLength(); i++)
	{
		e = Edge(pFace[i][0], pFace[i][1]).Ordered();
		if(pEdge.IndexOf(e) == -1) pEdge.Add(e);
		e = Edge(pFace[i][1], pFace[i][2]).Ordered();
		if(pEdge.IndexOf(e) == -1) pEdge.Add(e);
		e = Edge(pFace[i][2], pFace[i][0]).Ordered();
		if(pEdge.IndexOf(e) == -1) pEdge.Add(e);
	}
}
void Actor::FindDelayValues()
{
	pfDelay.SetLength(pVertex.GetLength());

	float fDistMax = FLT_MIN, fDistMin = FLT_MAX;
	for(int i = 0; i < pVertex.GetLength(); i++)
	{
		pfDelay[i] = pVertex[i].m_vPos.Length();
		fDistMin = std::min(fDistMin, pfDelay[i]);
		fDistMax = std::max(fDistMax, pfDelay[i]);
	}
	for(int i = 0; i < pfDelay.GetLength(); i++)
	{
		pfDelay[i] = (pfDelay[i] - fDistMin) / (fDistMax - fDistMin);
	}
}
Vector3 Actor::GetDelayedPosition(int nVertex, ExposureData *pData)
{
	float fTime = pData->m_fTime + (pfDelay[nVertex] * fDelayHistory);
	float fTotalElapsed = 0;
 	for(int k = pData->m_nFrame; ; k++)
	{
		if(!(k + 1 < ppFrame.GetLength()))
		{
			return ppFrame[k]->m_pvPosition[nVertex];
		}
		float fNextElapsed = fTotalElapsed + ppFrame[k]->m_fElapsed;
		if(fNextElapsed > fTime)
		{
			float fPos = (fTime - fTotalElapsed) / ppFrame[k]->m_fElapsed;
			return (ppFrame[k]->m_pvPosition[nVertex] * (1 - fPos)) + (ppFrame[k + 1]->m_pvPosition[nVertex] * fPos);
		}
		fTotalElapsed = fNextElapsed;
	}
}
void Actor::Calculate(Camera *pCamera, float fElapsed)
{
	Vector3 pvSprite[4] = { Vector3(-fSpriteSize, -fSpriteSize, 0), Vector3(fSpriteSize, -fSpriteSize, 0), Vector3(fSpriteSize, fSpriteSize, 0), Vector3(-fSpriteSize, fSpriteSize, 0) };
	int i, j;
	m_bsFlag.set( F_VALID_TRANSFORMED_DATA, false );

	Matrix44 mTransform = Matrix44::Rotation(fYaw, fPitch, fRoll) * Matrix44::Translation(vPosition) * pCamera->GetTransform( );

	// set up the lighting
	int nMinLight = std::min(wcAmbientLight.GetMinComponent(), (wcAmbientLight + (nExposure * wcExposureLightChange)).GetMinComponent());
	int nMaxLight = std::max(wcAmbientLight.GetMaxComponent(), (wcAmbientLight + (nExposure * wcExposureLightChange)).GetMaxComponent());
	if(ppLight.GetLength() > 0)
	{
		if(!m_bsFlag.test( F_VALID_VERTEX_NORMALS )) FindVertexNormals();
		for(i = 0; i < ppLight.GetLength(); i++)
		{
			nMaxLight = std::max(nMaxLight, ppLight[i]->m_cColour.GetMaxComponent());
		}
	}

	m_bsFlag.set( F_USE_DIFFUSE, (nMinLight < 255) || m_bsFlag.test( F_VALID_VERTEX_DIFFUSE ));
	m_bsFlag.set( F_USE_SPECULAR, (nMaxLight > 255) || m_bsFlag.test( F_VALID_VERTEX_SPECULAR) );

	// set up the texturing
	ZArray<TextureEntry*> ppUseTexture;
	if(m_bsFlag.test(F_DRAW_VERTEX_SPRITES))
	{
		_ASSERT(pTexture[0].m_nType == TextureEntry::T_SPRITE);
		nTextures = 1;
	}
	else
	{
		for(i = 0; i < MAX_TEXTURES; i++)
		{
			switch(pTexture[i].m_nType)
			{
			case TextureEntry::T_USER:
				ppUseTexture.Add(&pTexture[i]);
				break;
			case TextureEntry::T_LIGHTMAP:
			case TextureEntry::T_ENVMAP:
				if(!m_bsFlag.test(F_VALID_VERTEX_NORMALS)) FindVertexNormals();

				if(pTexture[i].m_nType == TextureEntry::T_LIGHTMAP)
				{
					pTexture[i].m_mRotation = Matrix44::Rotation(pTexture[i].m_fPitch, pTexture[i].m_fYaw, pTexture[i].m_fRoll);
				}
				else
				{
					pTexture[i].m_mRotation = Matrix44::Rotation(fYaw, fPitch, fRoll);
				}
				ppUseTexture.Add(&pTexture[i]);
				break;
			}
		}
		nTextures = ppUseTexture.GetLength();
	}

	// set up the frame array
	Frame *pStoreFrame = NULL;
	for(i = 0; i < ppUnusedFrame.GetLength();)
	{
		if(ppUnusedFrame[i]->m_nTimeToLive > timeGetTime( ))
		{
			delete ppUnusedFrame[i];
			ppUnusedFrame.Remove(i);
		}
		else i++;
	}
	if(m_bsFlag.test(F_DRAW_VERTEX_SPRITE_HISTORY) ||
		m_bsFlag.test(F_DO_POSITION_DELAY) || 
		m_bsFlag.test(F_DO_FRAME_HISTORY) || 
		nExposure > 1)
	{
		if(ppFrame.GetLength() == 0 || ppFrame[0]->m_fElapsed > fFrameTime)
		{
			float fTotalElapsed = fElapsed;
			for(int i = 0;;i++)
			{
				if(fTotalElapsed > fFrameHistory)
				{
					// remove i + 1 and upwards
					while(ppFrame.GetLength() > i + 1)
					{
						int nIndex = ppFrame.GetLength() - 1;
						ppFrame[nIndex]->m_nTimeToLive = timeGetTime( ) + FRAME_TTL;
						ppUnusedFrame.Add(ppFrame[nIndex]);
						ppFrame.Remove(nIndex);
					}
				}
				if(!(i + 1 < ppFrame.GetLength()))
				{
					if(ppUnusedFrame.GetLength() > 0)
					{
						ppFrame.Insert(0, ppUnusedFrame[0]);
						ppUnusedFrame.Remove(0);
					}
					else 
					{
						ppFrame.Insert(0, new Frame);
					}
					break;
				}
				fTotalElapsed += ppFrame[i]->m_fElapsed;
			}
			ppFrame[0]->m_fElapsed = 0;
		}
		ppFrame[0]->m_fElapsed += fElapsed;
		if(m_bsFlag.test( F_DO_ROTATION_HISTORY ) )
		{
			ppFrame[0]->m_fRoll = fRoll;
			ppFrame[0]->m_fPitch = fPitch;
			ppFrame[0]->m_fYaw = fYaw;
		}
		else
		{
			pStoreFrame = ppFrame[0];
			pStoreFrame->m_pvPosition.SetLength(pVertex.GetLength());
			if(m_bsFlag.test( F_DRAW_VERTEX_SPRITE_HISTORY ) )
			{
				pStoreFrame->m_pfDistance.SetLength(pVertex.GetLength());
			}
		}
	}

	// calculate the target vertex format
	if(nExposure > 0)
	{
		// build the state list
		mpState.clear();
		if(!m_bsFlag.test( F_DRAW_Z_BUFFER ))
		{
			mpState[D3DRS_ZENABLE] = D3DZB_FALSE;
		}
		if( m_bsFlag.test( F_DRAW_TRANSPARENT ) )
		{
			mpState[D3DRS_DESTBLEND] = D3DBLEND_ONE;
			mpState[D3DRS_ALPHABLENDENABLE] = TRUE;
		}
		if( m_bsFlag.test( F_NO_CULL ) )
		{
			mpState[D3DRS_CULLMODE] = D3DCULL_NONE;
		}
		if(ppLight.GetLength() == 0 && !m_bsFlag.test( F_VALID_VERTEX_DIFFUSE ) && !m_bsFlag.test( F_VALID_VERTEX_SPECULAR ) )
		{
			mpState[D3DRS_SHADEMODE] = D3DSHADE_FLAT;
		}
		if(m_bsFlag.test( F_USE_SPECULAR ))
		{
			mpState[D3DRS_SPECULARENABLE] = TRUE;
		}
	}

	// create 
//	pnClipped.SetLength(nExposure * pVertex.GetLength());
//	pClippedEdge.SetLength(0);
//	pClippedFace.SetLength(0);
	pTransVertex.SetLength(0);
//	pClippedVertex.SetLength(0);
	bool bClipped = false;

//	ZTexturePosition pTex[8];
	Vector3 vTransNormal;
	ZArray<Vector3> pvPos;
	float fAngle;
	Vector3 vTransDirection;
	ColorRgb cInitDiffuse(wcAmbientLight);
	ColorRgb cInitSpecular(wcAmbientLight - ColorRgb(255, 255, 255));

	ZArray<ExposureData> pfExpBase;
	ZArray<Matrix44> pmExposure;
	for(int nExp = 0; nExp < nExposure; nExp++)
	{
		if(nExp == 0)
		{
			ExposureData *pData = pfExpBase.AddEmptyPtr();
			pData->m_fTime = 0;
			pData->m_nFrame = 0;
			pData->m_fPos = 0;
		}
		else
		{
			float fTime = nExp * (fFrameHistory - fDelayHistory) / (nExposure - 1);
			float fTotalElapsed = 0;
			int k;
			for(k = 0; ; k++)
			{
				if(!(k + 1 < ppFrame.GetLength())) break;
				float fNextElapsed = fTotalElapsed + ppFrame[k]->m_fElapsed;
				if(fNextElapsed > fTime)
				{
					ExposureData *pData = pfExpBase.AddEmptyPtr();
					pData->m_fTime = fTime;
					pData->m_nFrame = k;
					pData->m_fPos = (pData->m_fTime - fTotalElapsed) / ppFrame[k]->m_fElapsed;
					if(m_bsFlag.test( F_DO_ROTATION_HISTORY ) )
					{
						float fThisYaw = (ppFrame[k]->m_fYaw * (1 - pData->m_fPos)) + (ppFrame[k + 1]->m_fYaw * pData->m_fPos);
						float fThisPitch = (ppFrame[k]->m_fPitch * (1 - pData->m_fPos)) + (ppFrame[k + 1]->m_fPitch * pData->m_fPos);
						float fThisRoll = (ppFrame[k]->m_fRoll * (1 - pData->m_fPos)) + (ppFrame[k + 1]->m_fRoll * pData->m_fPos);
						pData->mTransform = Matrix44::Rotation(fThisYaw, fThisPitch, fThisRoll) * Matrix44::Translation(vPosition) * pCamera->GetTransform();
					}
					break;
				}
				fTotalElapsed = fNextElapsed;
			}
			if(!(k + 1 < ppFrame.GetLength())) break;
		}
	}

	if(pVertex.GetLength() == 0) return;

	int nMaxHistoryLength = nMaxVertices / (4 * pVertex.GetLength());

	// transform
	for(i = 0; i < pVertex.GetLength(); i++)
	{
		ZVertexTL *pVert = pTransVertex.AddEmptyPtr();
		if(m_bsFlag.test( F_NO_TRANSFORM ) ) pVert->m_vPos = pVertex[i].m_vPos;
		else pVert->m_vPos = pVertex[i].m_vPos * mTransform;

		if(pStoreFrame) pStoreFrame->m_pvPosition[i] = pVert->m_vPos;
	}

	if(nExposure == 0) return;

	// light
	if(ppLight.GetLength() > 0)
	{
		for(i = 0; i < pVertex.GetLength(); i++)
		{
			WideColorRgb wcLight = wcAmbientLight;
			for(j = 0; j < ppLight.GetLength(); j++)
			{
				switch(ppLight[j]->m_nType)
				{
				case Light::T_DIRECTIONAL:
					vTransDirection = (ppLight[j]->m_vDirection * (Matrix44::Rotation(-fPitch, -fYaw) * Matrix44::RotationZ(-fRoll))).Normal();
					fAngle = 4.0f * pVertex[i].m_vNormal.Dot(vTransDirection);
					if(fAngle > 0) wcLight += ppLight[j]->m_cColour * fAngle;
					break;
				case Light::T_POINT:
					Vector3 vDirection = ppLight[j]->m_vPosition - pTransVertex[i].m_vPos;
					fAngle = vDirection.Dot(pVertex[i].m_vNormal);
					if(fAngle > 0)
					{
						float fDistance = vDirection.Length();
						if(ppLight[j]->m_bAttenuate) fAngle *= ppLight[j]->m_fAttenuate / (fDistance * fDistance);
						else fAngle /= fDistance;
						wcLight += ppLight[j]->m_cColour * fAngle;
					}
					break;
				}
			}
			pTransVertex[i].m_cDiffuse = wcLight;
			pTransVertex[i].m_cSpecular = wcLight - ColorRgb(255, 255, 255);
		}
	}
	else
	{
		for(i = 0; i < pVertex.GetLength(); i++)
		{
			pTransVertex[i].m_cDiffuse = cInitDiffuse;
			pTransVertex[i].m_cSpecular = cInitSpecular;
		}
	}
	if(m_bsFlag.test( F_VALID_VERTEX_DIFFUSE ) ) 
	{
		for(i = 0; i < pVertex.GetLength(); i++)
		{
			pTransVertex[i].m_cDiffuse.m_nR = (UINT8)(pTransVertex[i].m_cDiffuse.m_nR * pVertex[i].m_cDiffuse.m_nR / 255.0f);
			pTransVertex[i].m_cDiffuse.m_nG = (UINT8)(pTransVertex[i].m_cDiffuse.m_nG * pVertex[i].m_cDiffuse.m_nG / 255.0f);
			pTransVertex[i].m_cDiffuse.m_nB = (UINT8)(pTransVertex[i].m_cDiffuse.m_nB * pVertex[i].m_cDiffuse.m_nB / 255.0f);
		}
	}
	if(m_bsFlag.test( F_VALID_VERTEX_SPECULAR ) )
	{
		for(i = 0; i < pVertex.GetLength(); i++)
		{
			pTransVertex[i].m_cSpecular = pTransVertex[i].m_cSpecular + pVertex[i].m_cSpecular;
		}
	}
		
	// texture
	for(j = 0; j < ppUseTexture.GetLength(); j++)
	{
		if(ppUseTexture[j]->m_nType == TextureEntry::T_USER)
		{
			for(i = 0; i < pVertex.GetLength(); i++)
			{
				pTransVertex[i].m_aTex[j] = pVertex[i].m_aTex[j];
			}
		}
		else
		{
			for(i = 0; i < pVertex.GetLength(); i++)
			{
				vTransNormal = pVertex[i].m_vNormal * ppUseTexture[j]->m_mRotation;
				pTransVertex[i].m_aTex[j].x = (vTransNormal.m_fX * 1.0f) + 0.5f;
				pTransVertex[i].m_aTex[j].y = (vTransNormal.m_fY * 1.0f) + 0.5f;
			}
		}
	}

	if(!m_bsFlag.test( F_DRAW_VERTEX_SPRITE_HISTORY ) )
	{
		ZVertexTL *pExposureVx = pTransVertex.GetBuffer(pVertex.GetLength() * pfExpBase.GetLength());
		for(int nExp = 0;;)
		{
			if(m_bsFlag.test(F_DO_POSITION_DELAY))
			{
				for(int i = 0; i < pVertex.GetLength(); i++)
				{
					pExposureVx[i].m_vPos = GetDelayedPosition(i, &pfExpBase[nExp]);
				}
			}

			nExp++;
			if(!(nExp < pfExpBase.GetLength())) break;

			ZVertexTL *pNewExposureVx = pExposureVx + pVertex.GetLength();
			for(i = 0; i < pVertex.GetLength(); i++)
			{
				pNewExposureVx[i].m_cDiffuse = pExposureVx[i].m_cDiffuse + wcExposureLightChange; 
				pNewExposureVx[i].m_cSpecular = pExposureVx[i].m_cSpecular + wcExposureLightChange; 
				pNewExposureVx[i].m_aTex[0] = pExposureVx[i].m_aTex[0];
			}

			if(!m_bsFlag.test(F_DO_ROTATION_HISTORY))
			{
				for(i = 0; i < pVertex.GetLength(); i++)
				{
					pNewExposureVx[i].m_vPos = (ppFrame[pfExpBase[nExp].m_nFrame]->m_pvPosition[i] * (1 - pfExpBase[nExp].m_fPos)) + (ppFrame[pfExpBase[nExp].m_nFrame + 1]->m_pvPosition[i] * pfExpBase[nExp].m_fPos);
				}
			}
			else
			{
				for(i = 0; i < pVertex.GetLength(); i++)
				{
					pNewExposureVx[i].m_vPos = pVertex[i].m_vPos * pfExpBase[nExp].mTransform;
				}
			}

			pExposureVx = pNewExposureVx;
		}
	}
	else if(ppFrame.GetLength() > 1)
	{
		_ASSERT(nExposure == 1);
		pTransVertex.SetLength(0);

		for(int i = 0; i < pVertex.GetLength(); i++)
		{
			ppFrame[0]->m_pfDistance[i] = (ppFrame[1]->m_pvPosition[i] - ppFrame[0]->m_pvPosition[i]).Length();

			ColorRgb cDiffuse = cInitDiffuse;
			ColorRgb cSpecular = cInitSpecular;

			int nFrames = 0;
			float fPosition = 0;
			int nLength = 0;
			for(int j = 0; j < ppFrame.GetLength() - 1; j++)
			{
				if(ppFrame[j]->m_pfDistance[i] > 0)
				{
					for(; fPosition < ppFrame[j]->m_pfDistance[i]; fPosition += fSpriteHistoryLength)
					{
						float fMult = fPosition / ppFrame[j]->m_pfDistance[i];
						Vector3 vPos = (ppFrame[j]->m_pvPosition[i] * (1 - fMult)) + (ppFrame[j+1]->m_pvPosition[i] * fMult);

						ZVertexTL *pVert = pTransVertex.AddEmptyPtr();
						pVert->m_vPos = vPos;
						pVert->m_cDiffuse = cDiffuse;
						pVert->m_cSpecular = cSpecular;
						pVert->m_aTex[0] = ZPoint<float>(pSpriteTexU[0],pSpriteTexV[0]);//pSpriteTex[0];//pvSprite[0];

//						AddSprite(pCamera, vPos, cDiffuse, cSpecular, pvSprite);
						cDiffuse = cDiffuse + wcExposureLightChange;
						cSpecular = cSpecular + wcExposureLightChange;
						nLength++;
						if(nLength > nMaxHistoryLength) break;
					}
					fPosition -= ppFrame[j]->m_pfDistance[i];
				}
				if(nLength > nMaxHistoryLength) break;
			}
		}
	}

	// projection stuff
//	float fScreenX, fScreenY, fScale;
	float fMultZ = 1 / m_fClipMaxZ;
	if(pCamera->m_bsFlag.test( Camera::F_SCREEN_TRANSFORM ) ) //Property(ZCamera::DoScreenTransform))
	{
		pCamera->m_fScreenX = (g_pD3D->GetWidth() / 2.0f) - 0.25f;
		pCamera->m_fScreenY = (g_pD3D->GetHeight() / 2.0f) - 0.25f;
		pCamera->m_fScale = std::min(g_pD3D->GetWidth() / 640.0f, g_pD3D->GetHeight() / 480.0f) * pCamera->m_fPerspective;
	}
	else
	{
		pCamera->m_fScreenX = 0;
		pCamera->m_fScreenY = 0;
		pCamera->m_fScale = pCamera->m_fPerspective;
	}

	(*ppnEmptyVertex).Empty(); // can continue to use after first clip
	if(m_bsFlag.test(F_DRAW_VERTEX_SPRITES))
	{
		(*ppVertexBuffer).Empty();
		(*ppVertexBuffer).SetLength(0);
//		(*ppVertexBuffer).SetLength(p.GetLength() * 4);
		(*ppFaceBuffer).SetLength((*ppVertexBuffer).GetLength() * 2);

		int nVertex = 0;
		ZVertexTL *pVert = pTransVertex.GetBuffer();
		for(i = 0; i < pTransVertex.GetLength(); i++)
		{
			ZVertexTL &v = pTransVertex[i];
			(*ppFaceBuffer).Add(Face(nVertex + 0, nVertex + 1, nVertex + 2));
			(*ppFaceBuffer).Add(Face(nVertex + 2, nVertex + 3, nVertex + 0));

			float fRHW = 1.0f / v.m_vPos.m_fZ;
			float fZ = v.m_vPos.m_fZ * fMultZ;
			float fMult = pCamera->m_fScale * fRHW;
			ZVertexTL *pVert = (*ppVertexBuffer).AddEmptyPtr(4);
			for(int j = 0; j < 4; j++)
			{
				pVert->m_vPos.m_fX = ((v.m_vPos.m_fX + pvSprite[j].m_fX) * fMult) + pCamera->m_fScreenX;
				pVert->m_vPos.m_fY = ((v.m_vPos.m_fY + pvSprite[j].m_fY) * fMult) + pCamera->m_fScreenY;
				pVert->m_vPos.m_fZ = fZ;
				pVert->m_fRHW = fRHW;
				pVert->m_cDiffuse = v.m_cDiffuse;
				pVert->m_cSpecular = v.m_cSpecular;
				pVert->m_aTex[0] = ZPoint<float>(pSpriteTexU[j], pSpriteTexV[j] );
				pVert++;
			}

			nVertex += 4;
		}
		(*ppVertexBuffer).SwapBuffer(pTransVertex);
	}
	else
	{
		if(pfExpBase.GetLength() <= 1)
		{
			Clip(pFace, (*ppFaceBuffer), wClipMask & (CLIP_FLAG_MIN_Z | CLIP_FLAG_MAX_Z));
		}
		else
		{
//			(*ppfExposureBuffer).SetLength(pfExpBase.GetLength() * pFace.GetLength());
			(*ppfExposureBuffer).SetLength(0);

			int nOfs = 0;
			for(int j = 0; j < pfExpBase.GetLength(); j++)
			{
				Face *pExpFace = (*ppfExposureBuffer).AddEmptyPtr(pFace.GetLength());
				for(int i = 0; i < pFace.GetLength(); i++)
				{
					pExpFace[i][0] = pFace[i][0] + nOfs;
					pExpFace[i][1] = pFace[i][1] + nOfs;
					pExpFace[i][2] = pFace[i][2] + nOfs;
				}
				nOfs += pVertex.GetLength();
			}

			if(m_bsFlag.test(F_DO_MIX_EXPOSURE_FACES))
			{
				ZArray<int> pnLookup;
				pnLookup.SetLength(pTransVertex.GetLength());
				for(int i = 0; i < pTransVertex.GetLength(); i++)
				{
					UINT16 wVertex = i % pVertex.GetLength();
					UINT16 wExposure = i / pVertex.GetLength();
					int nOldIndex = (wVertex * pfExpBase.GetLength()) + wExposure;
					pnLookup[nOldIndex] = i;
				}
				int nOfs = 0;
				for(int i = 0; i < (*ppfExposureBuffer).GetLength(); i++)
				{
					for(int j = 0; j < 3; j++)
					{
						UINT16 &wv = (*ppfExposureBuffer)[i][j];
						wv = pnLookup[wv];
					}
				}
			}

			Clip((*ppfExposureBuffer), (*ppFaceBuffer), wClipMask & (CLIP_FLAG_MIN_Z | CLIP_FLAG_MAX_Z));
		}

		// to the camera transform
//		float fMultZ = 1 / fClipMaxZ;
		for(i = 0; i < pTransVertex.GetLength(); i++)
		{
			Vector3 &v = pTransVertex[i].m_vPos;

			pTransVertex[i].m_fRHW = 1 / v.m_fZ;

			float fMult = pCamera->m_fScale * pTransVertex[i].m_fRHW;
			v.m_fX = (v.m_fX * fMult) + pCamera->m_fScreenX;
			v.m_fY = (v.m_fY * fMult) + pCamera->m_fScreenY;
			v.m_fZ = v.m_fZ * fMultZ;
		}
	}

	pClippedFace = (*ppFaceBuffer);
	// clip to the screen
	Clip((*ppFaceBuffer), pClippedFace, wClipMask & ~(CLIP_FLAG_MIN_Z | CLIP_FLAG_MAX_Z));

	m_bsFlag.set(F_VALID_TRANSFORMED_DATA);
}
Error* Actor::Render( )
{
	if (!m_bsFlag.test(F_VALID_TRANSFORMED_DATA))
	{
		return nullptr;
	}

	ZDirect3D::TextureStage tsDefault;
//	tsDisable.AddState(D3DTSS_COLOROP, D3DTOP_DISABLE);

//	assert(pTransVertex.GetLength() < 16384 * 3);
	_ASSERT(pClippedFace.GetLength() * 3 < 0x0ffff);//D3DMAXNUMVERTICES);//16384);
//	assert(pClippedEdge.GetLength() < 16384);

	bool bStencilZ = m_bsFlag.test(F_DRAW_Z_BUFFER) && m_bsFlag.test(F_DRAW_TRANSPARENT);
	for(int i = bStencilZ? -1 : 0; i < 1; i++)
	{
		g_pD3D->ResetRenderState();
		for(std::map< D3DRENDERSTATETYPE, DWORD >::iterator it = mpState.begin(); it != mpState.end(); it++)
		{
			g_pD3D->SetRenderState(it->first, it->second);
		}

		g_pD3D->ResetTextureState();
		if(i == -1)
		{
			g_pD3D->SetRenderState(D3DRS_SPECULARENABLE, false);
			g_pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
			g_pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			g_pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		}
		else 
		{
			if(bStencilZ)
			{
				g_pD3D->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
				g_pD3D->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			}

//			DWORD dwOperation;
			//if(Property(UseDiffuse)) dwOperation = D3DTOP_MODULATE;
			//else 
//			dwOperation = D3DTOP_SELECTARG1;

			g_pD3D->SetTexture(0, pTexture[0].m_pTexture);
/*			pd3d->vpTextureStage[0]->SetState(D3DTSS_TEXCOORDINDEX, 0);
			pd3d->vpTextureStage[0]->SetState(D3DTSS_COLOROP, D3DTOP_SELECTA);
			pd3d->vpTextureStage[0]->SetState(D3DTSS_TEXCOORDINDEX, 0);
*/
			if(pTexture[0].m_nType == TextureEntry::T_ENVMAP || pTexture[0].m_nType == TextureEntry::T_LIGHTMAP)
			{
//TODO				g_pD3D->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
//TODO				g_pD3D->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
			}
		}

		if(pClippedFace.GetLength() > 0)
		{
			Error* error = g_pD3D->DrawIndexedPrimitive(pTransVertex, pClippedFace);
			if(error) return TraceError(error);
		}
	}

	return nullptr;
}

void Actor::CreateCube(float fSize)
{
	pVertex.SetLength(8);
	pVertex[0].m_vPos.Set(-fSize, fSize, -fSize);
	pVertex[1].m_vPos.Set(fSize, fSize, -fSize);
	pVertex[2].m_vPos.Set(fSize, -fSize, -fSize);
	pVertex[3].m_vPos.Set(-fSize, -fSize, -fSize);
	pVertex[4].m_vPos.Set(-fSize, fSize, fSize);
	pVertex[5].m_vPos.Set(fSize, fSize, fSize);
	pVertex[6].m_vPos.Set(fSize, -fSize, fSize);
	pVertex[7].m_vPos.Set(-fSize, -fSize, fSize);

	for (int i = 0; i < 8; i++)
	{
		pVertex[i].m_vNormal = pVertex[i].m_vPos.Normal(fObjectNormal);
	}
	m_bsFlag.set(F_VALID_VERTEX_NORMALS);

	pFace.SetLength(12);
	pFace[0].Set(0, 3, 2);
	pFace[1].Set(0, 2, 1);
	pFace[2].Set(1, 2, 6);
	pFace[3].Set(1, 6, 5);
	pFace[4].Set(4, 5, 7);
	pFace[5].Set(5, 6, 7);
	pFace[6].Set(0, 4, 7);
	pFace[7].Set(0, 7, 3);
	pFace[8].Set(4, 0, 1);
	pFace[9].Set(4, 1, 5);
	pFace[10].Set(7, 6, 2);
	pFace[11].Set(7, 2, 3);
}
void Actor::CreateTetrahedron(float fRadius)
{
	float fSqrt2 = sqrtf(2.0f);
	float fSqrt3 = sqrtf(3.0f);
	float fX = 2.0f * fSqrt2 * fRadius / 3.0f;
	float fY = fRadius / 3.0f;
	float fP = fSqrt2 * fRadius / 3.0f;
	float fQ = fSqrt2 * fRadius / fSqrt3;

	pVertex.SetLength(4);
	pVertex[0].m_vPos = Vector3(0, fRadius, 0);
	pVertex[1].m_vPos = Vector3(fX, -fY, 0);
	pVertex[2].m_vPos = Vector3(-fP, -fY, fQ);
	pVertex[3].m_vPos = Vector3(-fP, -fY, -fQ);
	pFace.SetLength(4);
	pFace[0] = Face(0, 1, 3);
	pFace[1] = Face(0, 2, 1);
	pFace[2] = Face(0, 3, 2);
	pFace[3] = Face(3, 1, 2);
	for (int i = 0; i < 4; i++)
	{
		pVertex[i].m_vNormal = pVertex[i].m_vPos.Normal(fObjectNormal);
	}

	m_bsFlag.set(F_VALID_VERTEX_NORMALS);
}
void Actor::CreateGeosphere(float fRadius, int nVertices)
{
	struct GeoEdge : public Edge
	{
		float fLength;
		UINT16 pwFace[2];

		void FindLength(Actor* pObj) { fLength = (pObj->pVertex[(*this)[0]].m_vPos - pObj->pVertex[(*this)[1]].m_vPos).Length(); }
	};

	CreateCube(fRadius / sqrtf(3.0f));
	ZArray<GeoEdge> pGeoEdge;
	pVertex.SetSize(nVertices);

	for (int i = 0; i < pFace.GetLength(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Edge e = pFace[i].GetEdge(j).Ordered();
			for (int k = 0;; k++)
			{
				if (k < pGeoEdge.GetLength())
				{
					if (pGeoEdge[k] == e)
					{
						pGeoEdge[k].pwFace[1] = i;
						break;
					}
				}
				else
				{
					k = pGeoEdge.AddEmpty();
					pGeoEdge[k].Set(e);
					pGeoEdge[k].FindLength(this);
					pGeoEdge[k].pwFace[0] = i;
					break;
				}
			}
		}
	}

	while (pVertex.GetLength() < nVertices)
	{
		int nSplitEdge = 0;
		float fLongestEdge = pGeoEdge[nSplitEdge].fLength;
		for (int j = 1; j < pGeoEdge.GetLength(); j++)
		{
			if (pGeoEdge[j].fLength > fLongestEdge)
			{
				nSplitEdge = j;
				fLongestEdge = pGeoEdge[j].fLength;
			}
		}

		GeoEdge te = pGeoEdge[nSplitEdge];

		// and split it
		float fScale = (0.5f * fRadius) / sqrtf((fRadius * fRadius) - (fLongestEdge * fLongestEdge / 4.0f));
		int nNewVertex = pVertex.AddEmpty();
		pVertex[nNewVertex].m_vPos = (pVertex[te[0]].m_vPos + pVertex[te[1]].m_vPos) * fScale;
		////////////////////////////////////////////
				// add the main edges
		pGeoEdge[nSplitEdge][0] = te[0];
		pGeoEdge[nSplitEdge][1] = nNewVertex;
		pGeoEdge[nSplitEdge].fLength /= 2.0;
		int nNewEdge = pGeoEdge.AddEmpty();
		pGeoEdge[nNewEdge][0] = te[1];
		pGeoEdge[nNewEdge][1] = nNewVertex;
		pGeoEdge[nNewEdge].fLength = pGeoEdge[nSplitEdge].fLength;

		// add two more edges (one may be the same)
		for (int i = 0; i < 2; i++)
		{
			int nFace1 = te.pwFace[i];
			int nFace2 = pFace.AddEmpty();

			pFace[nFace2] = pFace[nFace1];
			int nVertex;
			for (int k = 0; k < 3; k++)
			{
				if (pFace[nFace1][k] != te[0] && pFace[nFace1][k] != te[1])
				{
					nVertex = pFace[nFace1][k];
				}

				if (pFace[nFace1][k] == te[1])
				{
					pFace[nFace1][k] = nNewVertex;
				}
				if (pFace[nFace2][k] == te[0])
				{
					pFace[nFace2][k] = nNewVertex;
				}
			}
			pGeoEdge[nSplitEdge].pwFace[i] = nFace1;
			pGeoEdge[nNewEdge].pwFace[i] = nFace2;

			int nNewSplit = pGeoEdge.AddEmpty();
			pGeoEdge[nNewSplit].Set(Edge(nVertex, nNewVertex).Ordered());
			pGeoEdge[nNewSplit].FindLength(this);
			pGeoEdge[nNewSplit].pwFace[0] = nFace1;
			pGeoEdge[nNewSplit].pwFace[1] = nFace2;

			Edge ce = Edge(te[1], nVertex).Ordered();
			for (int k = 0; k < pGeoEdge.GetLength(); k++)
			{
				if (pGeoEdge[k] == ce)
				{
					if (pGeoEdge[k].pwFace[0] == nFace1) pGeoEdge[k].pwFace[0] = nFace2;
					if (pGeoEdge[k].pwFace[1] == nFace1) pGeoEdge[k].pwFace[1] = nFace2;
				}
			}
		}
	}

	_ASSERT(pVertex.GetLength() == nVertices);

	for (int i = 0; i < pVertex.GetLength(); i++)
	{
		pVertex[i].m_vNormal = pVertex[i].m_vPos.Normal(fObjectNormal);
	}
	m_bsFlag.set(F_VALID_VERTEX_NORMALS);
}

void Actor::CreateTorus(float fOuterRad, float fInnerRad, int nOuterPoints, int nInnerPoints)
{
	pVertex.Empty();
	pFace.Empty();

	int nVertices = nOuterPoints * nInnerPoints;
	for (int i = 0; i < nOuterPoints; i++)
	{
		int nBase = pVertex.GetLength();
		for (int j = 0; j < nInnerPoints; j++)
		{
			double dAngOuter = (i + 0.5) * PI2 / nOuterPoints;
			double dAngInner = (j + 0.5) * PI2 / nInnerPoints;

			ZVertex* pv = pVertex.AddEmptyPtr();
			pv->m_vPos.m_fX = (float)(sin(dAngOuter) * (fOuterRad + (cos(dAngInner) * fInnerRad)));
			pv->m_vPos.m_fY = (float)(cos(dAngOuter) * (fOuterRad + (cos(dAngInner) * fInnerRad)));
			pv->m_vPos.m_fZ = (float)(sin(dAngInner) * fInnerRad);

			Face f;
			f[0] = nBase + j;
			f[2] = nBase + ((j + 1) % nInnerPoints);
			f[1] = (nBase + ((j + 1) % nInnerPoints) + nInnerPoints) % nVertices;
			pFace.Add(f);

			f[0] = nBase + j;
			f[1] = (nBase + j + nInnerPoints) % nVertices;
			f[2] = (nBase + ((j + 1) % nInnerPoints) + nInnerPoints) % nVertices;
			pFace.Add(f);
		}
	}
	_ASSERT(pVertex.GetLength() == nVertices);
}
void Actor::CreateTetrahedronGeosphere(float fRadius, int nIterations)
{
	struct TetraGeoEdge : public Edge
	{
		int nSplit;
	};

	ZArray<TetraGeoEdge> pGeoEdge;
	ZArray<Face> pGeoFace;

	CreateTetrahedron(fRadius);
	for (int n = 0; n < nIterations; n++)
	{
		pGeoEdge.SetLength(0);
		pGeoFace.SetLength(pFace.GetLength());
		for (int i = 0; i < pFace.GetLength(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				TetraGeoEdge e;
				ZeroMemory(&e, sizeof(e));
				e[0] = pFace[i][j];
				e[1] = pFace[i][(j + 1) % 3];
				for (int k = 0;; k++)
				{
					if (k < pGeoEdge.GetLength())
					{
						if (pGeoEdge[k].Contains(e[0]) && pGeoEdge[k].Contains(e[1]))
						{
							pGeoFace[i][j] = k;
							break;
						}
					}
					else
					{
						int nIndex = pGeoEdge.Add(e);
						pGeoFace[i][j] = k;
						break;
					}
				}
			}
		}

		//		ZFlexibleVertex fv(pVertex);
		for (int i = 0; i < pGeoEdge.GetLength(); i++)
		{
			int nVertex = pVertex.AddEmpty();
			pGeoEdge[i].nSplit = nVertex;//pv;//p.SetNewEmpty();
			pVertex[nVertex].m_vPos = (pVertex[pGeoEdge[i][0]].m_vPos + pVertex[pGeoEdge[i][1]].m_vPos).Normal(fRadius);
		}
		pFace.SetLength(0);
		for (int i = 0; i < pGeoFace.GetLength(); i++)
		{
			Face& f = pGeoFace[i];
			TetraGeoEdge& e1 = pGeoEdge[f[0]];
			TetraGeoEdge& e2 = pGeoEdge[f[1]];
			TetraGeoEdge& e3 = pGeoEdge[f[2]];
			pFace.Add(Face(e1.nSplit, e2.nSplit, e3.nSplit));
			pFace.Add(Face(e1.nSplit, e2.nSplit, e1.CommonIndex(e2)));
			pFace.Add(Face(e1.nSplit, e3.nSplit, e1.CommonIndex(e3)));
			pFace.Add(Face(e2.nSplit, e3.nSplit, e2.CommonIndex(e3)));
		}
	}
	FindFaceOrder(Vector3::Origin());
	for (int i = 0; i < pVertex.GetLength(); i++)
	{
		pVertex[i].m_vNormal = pVertex[i].m_vPos.Normal(fObjectNormal);
	}
	m_bsFlag.set(F_VALID_VERTEX_NORMALS);
}
