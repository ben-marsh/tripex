#include "Platform.h"
#include "effect.h"
#include "Actor.h"
#include "Camera.h"
#include "TextureData.h"
#include "error.h"

class EffectSpectrum : public Effect
{
public:
	const TextureClass envmap_texture_class =
	{
		"EnvMap",
		{ tex_alien_egg, tex_shiny_sand }
	};

	const float ACCELER = 1.0f;
	static const int CUBE_H = 4;//10

	static const int TRAIL_W = 30;//10//30//20
	static const int TRAIL_H = 30;

	const float TRAIL_XS = 20;//20//20//40
	const float CUBE_SIZE = 10;
	const float SPIN_RADIUS = 600;
	const float SPIN_HEIGHT = -300;
	const float CYLINDER_RADIUS = 1200; //500
	const float TRAIL_ANGS = (2.0f * g_fDegToRad);
	const float ANG_OFFSET = (10.0f * g_fDegToRad);
	const float BAR_SIZE = 5.0f;
	static const int LIMITER_H = 15;//10

	float m_fBrAng;
	Camera m_cCamera;
	Actor m_pObj[TRAIL_H];
	Actor m_pLimit[LIMITER_H];
	float m_fAng;
	float m_fRotAng;
	float m_pfHeight[TRAIL_H][TRAIL_W];
	float m_pfCubeHeight[LIMITER_H][TRAIL_W];
	float m_pfCubeTop[LIMITER_H][TRAIL_W];
	float m_pfCubeTime[LIMITER_H][TRAIL_W];
	bool m_fr;

	/*---------------------------------------------
	* Constructor:
	---------------------------------------------*/

	EffectSpectrum( )
		: Effect({ &envmap_texture_class })
	{
		m_fAng = 0.0f;
		m_fr = true;
		m_fBrAng = 0;
		m_fRotAng = 45.0f * g_fDegToRad;
		m_cCamera.perspective = 300;

		for(int i = 0; i < TRAIL_H; i++)
		{
			m_pObj[i].vertices.resize(TRAIL_W * 4);
			m_pObj[i].faces.resize((TRAIL_W - 1) * 8);
			m_pObj[i].flags.set(Actor::F_DRAW_TRANSPARENT);
			m_pObj[i].flags.set( Actor::F_DRAW_Z_BUFFER, false );
			uint16 v = 0, f = 0;
			for(int j = 0; j < TRAIL_W; j++)
			{
				int n = j * 4;
				static const float s = 0.5f / sqrtf( 2.0f );
				m_pObj[ i ].vertices[ n + 0 ].normal = Vector3( 0.0f, -s, -s );
				m_pObj[ i ].vertices[ n + 1 ].normal = Vector3( 0.0f, +s, -s );
				m_pObj[ i ].vertices[ n + 2 ].normal = Vector3( 0.0f, +s, +s );
				m_pObj[ i ].vertices[ n + 3 ].normal = Vector3( 0.0f, -s, +s );

				if(j < TRAIL_W - 1)
				{
					Face *pFace;
					for(uint16 k = 0; k < 4; k++ )
					{
						pFace = &m_pObj[ i ].faces[ f + k * 2 ];
						( *pFace )[ 0 ] = (uint16)( v + k );
						( *pFace )[ 1 ] = (uint16)( v + k + 4 );
						( *pFace )[ 2 ] = (uint16)( v + ( ( k + 1 ) % 4 ) );

						pFace = &m_pObj[ i ].faces[ f + k * 2 + 1 ];
						( *pFace )[ 0 ] = (uint16)( v + ( ( k + 1 ) % 4 ) );
						( *pFace )[ 1 ] = (uint16)( v + k + 4 );
						( *pFace )[ 2 ] = (uint16)( v + 4 + ( ( k + 1 ) % 4 ) );
					}
					f += 8;
				}
				v += 4;
			}
		}
		for(int i = 0; i < LIMITER_H; i++)
		{
			m_pLimit[i].vertices.resize(TRAIL_W);
			m_pLimit[i].flags.set(Actor::F_DRAW_TRANSPARENT);
			m_pLimit[i].flags.set(Actor::F_DRAW_Z_BUFFER, false );
			m_pLimit[i].flags.set(Actor::F_DRAW_VERTEX_SPRITES);
			m_pLimit[i].sprite_size = 9.0f;//fRenderAsLights(15.0);

//			float dMult = ((sin((CUBE_H + i + (dAng / TRAIL_ANGS)) * 3.14159 / TRAIL_H) * 0.7) + 0.8) * TRAIL_XS;
			float fMult = ((sinf((CUBE_H + i) * PI / TRAIL_H) * 0.7f) + 0.8f) * TRAIL_XS;
			for(int j = 0; j < TRAIL_W; j++)
			{
				m_pLimit[i].vertices[j].position.x = (j - (TRAIL_W / 2.0f)) * fMult;
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

	Error* Calculate( const CalculateParams& params ) override
	{
		int i, j;
		m_fRotAng += params.elapsed * ( 2.0f * g_fDegToRad );
		m_fBrAng += params.elapsed * ( 2.0f * g_fDegToRad );

		m_fAng += params.elapsed * ( 1.0f * g_fDegToRad );
		m_cCamera.position.x = -30 + ( SPIN_RADIUS * cosf( m_fRotAng ) );
		m_cCamera.position.y = SPIN_HEIGHT;
		m_cCamera.position.z = SPIN_RADIUS * sinf( m_fRotAng );

		Vector3 vDir = Vector3::Origin() - m_cCamera.position;
		m_cCamera.pitch = vDir.GetPitch();
		m_cCamera.yaw = vDir.GetYaw();

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
				m_pfHeight[ 0 ][ nTarget ] = std::max( m_pfHeight[ 0 ][ nTarget ], params.audio_data.GetBand( i ) );
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
				m_pfCubeTime[ j ][ i ] += params.elapsed;
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

		float fPosYMax = -CYLINDER_RADIUS + ( CYLINDER_RADIUS * cosf( TRAIL_ANGS * ( TRAIL_H * 0.5f ) + ANG_OFFSET ) );
		for(i = 0; i < TRAIL_H; i++)
		{
			float fThisAng = ( ( i - ( TRAIL_H * 0.5f ) ) * TRAIL_ANGS ) + m_fAng + ANG_OFFSET;
			float fCos = cosf( fThisAng );
			float fSin = sinf( fThisAng );
			float fPosY = CYLINDER_RADIUS - ( CYLINDER_RADIUS * fCos );
			float fPosZ = ( CYLINDER_RADIUS * fSin );

			float fBr;
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

			fBr = params.brightness * std::min( fBr * 0.7f, 1.0f );
			fBr = std::max( fBr, 0.0f );
			m_pObj[ i ].ambient_light_color = ColorRgb::Grey( ( int )( fBr * 255.0f ) );

			int n = 0;
			float fMult = ((sinf((i + (m_fAng / TRAIL_ANGS)) * PI / TRAIL_H) * 0.9f) + 0.8f) * TRAIL_XS;
			for(int j = 0; j < TRAIL_W; j++)
			{
				float fX = ( j - ( TRAIL_W * 0.5f ) ) * fMult;
				float fY = fPosY - ( fCos * m_pfHeight[ i ][ j ] );
				float fZ = fPosZ - ( fSin * m_pfHeight[ i ][ j ] );

				m_pObj[ i ].vertices[ n + 0 ].position = Vector3( fX, fY - BAR_SIZE, fZ - BAR_SIZE );
				m_pObj[ i ].vertices[ n + 1 ].position = Vector3( fX, fY + BAR_SIZE, fZ - BAR_SIZE );
				m_pObj[ i ].vertices[ n + 2 ].position = Vector3( fX, fY + BAR_SIZE, fZ + BAR_SIZE );
				m_pObj[ i ].vertices[ n + 3 ].position = Vector3( fX, fY - BAR_SIZE, fZ + BAR_SIZE );

				n += 4;
			}

			m_pObj[ i ].Calculate(params.renderer, &m_cCamera, params.elapsed );
		}

		for( i = 0; i < LIMITER_H; i++ )
		{
			float dCubeAng = ((CUBE_H + i - (TRAIL_H * 0.5f )) * TRAIL_ANGS) + ANG_OFFSET;
			float dCubeCos = cosf(dCubeAng);
			float dCubeSin = sinf(dCubeAng);
			float dCubePosY = CYLINDER_RADIUS - (CYLINDER_RADIUS * dCubeCos);
			float dCubePosZ = (CYLINDER_RADIUS * dCubeSin);//sin(dThisAng));
			for(int j = 0; j < TRAIL_W; j++)
			{
				m_pLimit[i].vertices[j].position.y = dCubePosY - (dCubeCos * m_pfCubeHeight[i][j]);
				m_pLimit[i].vertices[j].position.z = dCubePosZ - (dCubeSin * m_pfCubeHeight[i][j]);
			}
			
			float fBr = Bound< float >( 0.9f - ( ( float )i ) / LIMITER_H, 0.0f, 1.0f );
			m_pLimit[ i ].ambient_light_color = ColorRgb::Grey( ( int )( 255.0f * fBr * params.brightness ) );
			m_pLimit[ i ].Calculate(params.renderer, &m_cCamera, params.elapsed);
		}

		return nullptr;
	}


	/*---------------------------------------------
	* Reconfigure( ):
	---------------------------------------------*/

	Error* Reconfigure(const ReconfigureParams& params) override
	{
		static Texture *pTexture;
		pTexture = params.texture_library.Find(envmap_texture_class);
		for(int i = 0; i < TRAIL_H; i++)
		{
			m_pObj[i].textures[0].Set(Actor::TextureType::Envmap, pTexture);
		}
		for(int i = 0; i < LIMITER_H; i++)
		{
			m_pLimit[i].textures[0].Set(Actor::TextureType::Sprite, pTexture);
		}
		return nullptr;
	}

	/*---------------------------------------------
	* Render( ):
	---------------------------------------------*/

	Error* Render(const RenderParams& params) override
	{
		Error* error;
		for( int i = 0; i < TRAIL_H; i++ )
		{
			error = m_pObj[ i ].Render(params.renderer);
			if( error ) return TraceError( error );
		}
		for( int i = 0; i < LIMITER_H; i++ )
		{
			error = m_pLimit[ i ].Render(params.renderer);
			if( error ) return TraceError( error );
		}
		return nullptr;
	}
};

EXPORT_EFFECT(Spectrum, EffectSpectrum)
