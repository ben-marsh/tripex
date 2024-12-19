#include "Platform.h"
#include "effect.h"
#include "Actor.h"
#include "Camera.h"
#include "error.h"

#define ACCELER 1.0f

#define CUBE_H 4//10
#define TRAIL_W 30//10//30//20
#define TRAIL_H 30
#define TRAIL_XS 20//20//20//40
#define CUBE_SIZE 10
#define SPIN_RADIUS 600
#define SPIN_HEIGHT -300
#define CYLINDER_RADIUS 1200 //500
#define TRAIL_ANGS ( 2.0f * g_fDegToRad )
#define ANG_OFFSET ( 10.0f * g_fDegToRad )
#define BAR_SIZE 5.0f
#define LIMITER_H 15//10

class EffectSpectrum : public EffectBase
{
public:
	FLOAT32 m_fBrAng;
	Camera m_cCamera;
	Actor m_pObj[TRAIL_H];
	Actor m_pLimit[LIMITER_H];
	FLOAT32 m_fAng;
	FLOAT32 m_fRotAng;
	FLOAT32 m_pfHeight[TRAIL_H][TRAIL_W];
	FLOAT32 m_pfCubeHeight[LIMITER_H][TRAIL_W];
	FLOAT32 m_pfCubeTop[LIMITER_H][TRAIL_W];
	FLOAT32 m_pfCubeTime[LIMITER_H][TRAIL_W];
	bool m_fr;

	/*---------------------------------------------
	* Constructor:
	---------------------------------------------*/

	EffectSpectrum( )
	{
		m_fAng = 0.0f;
		m_fr = true;
		m_fBrAng = 0;
		m_fRotAng = 45.0f * g_fDegToRad;
		m_cCamera.m_fPerspective = 300;

		for(int i = 0; i < TRAIL_H; i++)
		{
			m_pObj[i].pVertex.SetLength(TRAIL_W * 4);
			m_pObj[i].pFace.SetLength((TRAIL_W - 1) * 8);
			m_pObj[i].m_bsFlag.set(Actor::F_DRAW_TRANSPARENT);
			m_pObj[i].m_bsFlag.set( Actor::F_DRAW_Z_BUFFER, false );
			UINT16 v = 0, f = 0;
			for(int j = 0; j < TRAIL_W; j++)
			{
				int n = j * 4;
				static const FLOAT32 s = 0.5f / sqrtf( 2.0f );
				m_pObj[ i ].pVertex[ n + 0 ].m_vNormal = Vector3( 0.0f, -s, -s );
				m_pObj[ i ].pVertex[ n + 1 ].m_vNormal = Vector3( 0.0f, +s, -s );
				m_pObj[ i ].pVertex[ n + 2 ].m_vNormal = Vector3( 0.0f, +s, +s );
				m_pObj[ i ].pVertex[ n + 3 ].m_vNormal = Vector3( 0.0f, -s, +s );

				if(j < TRAIL_W - 1)
				{
					Face *pFace;
					for(UINT16 k = 0; k < 4; k++ )
					{
						pFace = &m_pObj[ i ].pFace[ f + k * 2 ];
						( *pFace )[ 0 ] = (UINT16)( v + k );
						( *pFace )[ 1 ] = (UINT16)( v + k + 4 );
						( *pFace )[ 2 ] = (UINT16)( v + ( ( k + 1 ) % 4 ) );

						pFace = &m_pObj[ i ].pFace[ f + k * 2 + 1 ];
						( *pFace )[ 0 ] = (UINT16)( v + ( ( k + 1 ) % 4 ) );
						( *pFace )[ 1 ] = (UINT16)( v + k + 4 );
						( *pFace )[ 2 ] = (UINT16)( v + 4 + ( ( k + 1 ) % 4 ) );
					}
					f += 8;
				}
				v += 4;
			}
		}
		for(int i = 0; i < LIMITER_H; i++)
		{
			m_pLimit[i].pVertex.SetLength(TRAIL_W);
			m_pLimit[i].m_bsFlag.set(Actor::F_DRAW_TRANSPARENT);
			m_pLimit[i].m_bsFlag.set(Actor::F_DRAW_Z_BUFFER, false );
			m_pLimit[i].m_bsFlag.set(Actor::F_DRAW_VERTEX_SPRITES);
			m_pLimit[i].fSpriteSize = 9.0f;//fRenderAsLights(15.0);

//			FLOAT32 dMult = ((sin((CUBE_H + i + (dAng / TRAIL_ANGS)) * 3.14159 / TRAIL_H) * 0.7) + 0.8) * TRAIL_XS;
			FLOAT32 fMult = ((sinf((CUBE_H + i) * PI / TRAIL_H) * 0.7f) + 0.8f) * TRAIL_XS;
			for(int j = 0; j < TRAIL_W; j++)
			{
				m_pLimit[i].pVertex[j].m_vPos.m_fX = (j - (TRAIL_W / 2.0f)) * fMult;
				m_pfCubeTime[i][j] = 0;
				m_pfCubeTop[i][j] = 0.0;
				m_pfCubeHeight[i][j] = 0.0;
			}
		}
		m_fAng = 0;
		for(int i = 0; i < TRAIL_H; i++)
		{
			for(int j = 0; j < TRAIL_W; j++)
			{
				m_pfHeight[i][j] = 0;
			}
		}
	}

	/*---------------------------------------------
	* Calculate( ):
	---------------------------------------------*/

	Error* Calculate( FLOAT32 fBrightness, FLOAT32 fElapsed, AudioData* pAudio) override
	{
		int i, j;
		m_fRotAng += fElapsed * ( 2.0f * g_fDegToRad );
		m_fBrAng += fElapsed * ( 2.0f * g_fDegToRad );

		m_fAng += fElapsed * ( 1.0f * g_fDegToRad );
		m_cCamera.m_vPosition.m_fX = -30 + ( SPIN_RADIUS * cosf( m_fRotAng ) );
		m_cCamera.m_vPosition.m_fY = SPIN_HEIGHT;
		m_cCamera.m_vPosition.m_fZ = SPIN_RADIUS * sinf( m_fRotAng );

		Vector3 vDir = Vector3::Origin() - m_cCamera.m_vPosition;
		m_cCamera.m_fPitch = vDir.GetPitch();
		m_cCamera.m_fYaw = vDir.GetYaw();

		while( m_fAng >= TRAIL_ANGS )
		{
			for( i = TRAIL_H - 1; i >= 1; i-- )
			{
				for( int j = 0; j < TRAIL_W; j++ )
				{
					m_pfHeight[ i ][ j ] = m_pfHeight[ i - 1 ][ j ];
				}
			}
			for( i = 0; i < TRAIL_W; i++ )
			{
				m_pfHeight[ 0 ][ i ] = 0.0f;
			}
			for( i = 0; i < 256; i++ )
			{
				int nTarget = i * TRAIL_W / 256;
				m_pfHeight[ 0 ][ nTarget ] = std::max( m_pfHeight[ 0 ][ nTarget ], pAudio->GetBand( i ) );
			}
			for( i = 0; i < TRAIL_W; i++ )
			{
				m_pfHeight[ 0 ][ i ] = 300.0f * sinf( ( PI * 0.5f ) * m_pfHeight[ 0 ][ i ] );
			}
			m_fAng -= TRAIL_ANGS;
		}

		for( j = 0; j < LIMITER_H; j++ )
		{
			for( i = 0; i < TRAIL_W; i++ )
			{
				m_pfCubeTime[ j ][ i ] += fElapsed;
				m_pfCubeHeight[ j ][ i ] = m_pfCubeTop[ j ][ i ] - ( ACCELER * ( m_pfCubeTime[ j ][ i ] * m_pfCubeTime[ j ][ i ] ) / 2.0f );
				if(m_pfHeight[ CUBE_H + j ][ i ] > m_pfCubeHeight[ j ][ i ])
				{
					m_pfCubeTop[ j ][ i ] = m_pfHeight[ CUBE_H + j ][ i ];
					m_pfCubeTime[ j ][ i ] = 0.0;
					m_pfCubeHeight[ j ][ i ] = m_pfCubeTop[ j ][ i ] - ( ACCELER * ( m_pfCubeTime[ j ][ i ] * m_pfCubeTime[ j ][ i ] ) / 2.0f );
				}
				m_pfCubeHeight[ j ][ i ] = std::max( m_pfCubeHeight[ j ][ i ], 0.0f );
			}
		}

		FLOAT32 fPosYMax = -CYLINDER_RADIUS + ( CYLINDER_RADIUS * cosf( TRAIL_ANGS * ( TRAIL_H * 0.5f ) + ANG_OFFSET ) );
		for(i = 0; i < TRAIL_H; i++)
		{
			FLOAT32 fThisAng = ( ( i - ( TRAIL_H * 0.5f ) ) * TRAIL_ANGS ) + m_fAng + ANG_OFFSET;
			FLOAT32 fCos = cosf( fThisAng );
			FLOAT32 fSin = sinf( fThisAng );
			FLOAT32 fPosY = CYLINDER_RADIUS - ( CYLINDER_RADIUS * fCos );
			FLOAT32 fPosZ = ( CYLINDER_RADIUS * fSin );

			FLOAT32 fBr;
			if( i <= 1 )
			{
				fBr = ( i + ( m_fAng / TRAIL_ANGS ) ) * 0.5f;
			}
			else if( i > ( TRAIL_H * 0.5f ) )
			{
				fBr = 1.0f - fabsf( fPosY / fPosYMax );
			}
			else 
			{
				fBr = 1.0f;
			}

			fBr = fBrightness * std::min( fBr * 0.7f, 1.0f );
			fBr = std::max( fBr, 0.0f );
			m_pObj[ i ].wcAmbientLight = ColorRgb::Grey( ( int )( fBr * 255.0f ) );

			int n = 0;
			FLOAT32 fMult = ((sinf((i + (m_fAng / TRAIL_ANGS)) * PI / TRAIL_H) * 0.9f) + 0.8f) * TRAIL_XS;
			for(int j = 0; j < TRAIL_W; j++)
			{
				FLOAT32 fX = ( j - ( TRAIL_W * 0.5f ) ) * fMult;
				FLOAT32 fY = fPosY - ( fCos * m_pfHeight[ i ][ j ] );
				FLOAT32 fZ = fPosZ - ( fSin * m_pfHeight[ i ][ j ] );

				m_pObj[ i ].pVertex[ n + 0 ].m_vPos = Vector3( fX, fY - BAR_SIZE, fZ - BAR_SIZE );
				m_pObj[ i ].pVertex[ n + 1 ].m_vPos = Vector3( fX, fY + BAR_SIZE, fZ - BAR_SIZE );
				m_pObj[ i ].pVertex[ n + 2 ].m_vPos = Vector3( fX, fY + BAR_SIZE, fZ + BAR_SIZE );
				m_pObj[ i ].pVertex[ n + 3 ].m_vPos = Vector3( fX, fY - BAR_SIZE, fZ + BAR_SIZE );

				n += 4;
			}

			m_pObj[ i ].Calculate( &m_cCamera, fElapsed );
		}

		for( i = 0; i < LIMITER_H; i++ )
		{
			FLOAT32 dCubeAng = ((CUBE_H + i - (TRAIL_H * 0.5f )) * TRAIL_ANGS) + ANG_OFFSET;
			FLOAT32 dCubeCos = cosf(dCubeAng);
			FLOAT32 dCubeSin = sinf(dCubeAng);
			FLOAT32 dCubePosY = CYLINDER_RADIUS - (CYLINDER_RADIUS * dCubeCos);
			FLOAT32 dCubePosZ = (CYLINDER_RADIUS * dCubeSin);//sin(dThisAng));
			for(int j = 0; j < TRAIL_W; j++)
			{
				m_pLimit[i].pVertex[j].m_vPos.m_fY = dCubePosY - (dCubeCos * m_pfCubeHeight[i][j]);
				m_pLimit[i].pVertex[j].m_vPos.m_fZ = dCubePosZ - (dCubeSin * m_pfCubeHeight[i][j]);
			}
			
			FLOAT32 fBr = Bound< FLOAT32 >( 0.9f - ( ( FLOAT32 )i ) / LIMITER_H, 0.0f, 1.0f );
			m_pLimit[ i ].wcAmbientLight = ColorRgb::Grey( ( int )( 255.0f * fBr * fBrightness ) );
			m_pLimit[ i ].Calculate( &m_cCamera, fElapsed );
		}

		return nullptr;
	}


	/*---------------------------------------------
	* Reconfigure( ):
	---------------------------------------------*/

	Error* Reconfigure(AudioData* pAudio) override
	{
		static Texture *pTexture;
		pTexture = g_pD3D->Find(TC_EMANALYSER);
		for(int i = 0; i < TRAIL_H; i++)
		{
			m_pObj[i].pTexture[0].Set(Actor::TextureEntry::T_ENVMAP, pTexture);
		}
		for(int i = 0; i < LIMITER_H; i++)
		{
			m_pLimit[i].pTexture[0].Set(Actor::TextureEntry::T_SPRITE, pTexture);
		}
		return nullptr;
	}

	/*---------------------------------------------
	* Render( ):
	---------------------------------------------*/

	Error* Render( ) override
	{
		Error* error;
		for( int i = 0; i < TRAIL_H; i++ )
		{
			error = m_pObj[ i ].Render( );
			if( error ) return TraceError( error );
		}
		for( int i = 0; i < LIMITER_H; i++ )
		{
			error = m_pLimit[ i ].Render( );
			if( error ) return TraceError( error );
		}
		return nullptr;
	}
};

EXPORT_EFFECT(Spectrum, EffectSpectrum)
