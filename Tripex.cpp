#include "Platform.h"
#include "error.h"
#include "Texture.h"
#include "TextureFont.h"
#include "config.h"
#include "TextureSource.h"
#include "config.h"
#include "TextureData.h"
#include "AudioData.h"
//#include "mmsystem.h"
#include <algorithm>
#include <assert.h>
#include "config-defaults.h"
#include "Tripex.h"

/****** constants *****/

Tripex::Tripex(Renderer& renderer)
	: renderer(renderer)
{
	CreateEffectList();
	CreateCfgItems();
}

void Tripex::ShowStatusMsg(const char *sFormat, ...)
{
	va_list pArg;
	va_start( pArg, sFormat );

	vsnprintf( status_msg, sizeof( status_msg ), sFormat, pArg );
	status_time = timeGetTime( );
}

DWORD WINAPI Tripex::InitialiseThread(void *pParam)
{
	std::vector< TextureSource* > texture_sources;
	LoadTextureSettings(texture_sources);

	for(int i = 1; i < (int)effects.size(); i++)
	{
		if(effects[i]->fPreference > FLOAT_ZERO)
		{
			enabled_effects.push_back(effects[i]);
		}
	}

	srand( timeGetTime( ) );

	assert(num_internal_textures >= 1);

	for(TextureSource* texture_source : texture_sources)
	{
		const uint32* pnTexData = internal_textures[texture_source->internal_id];

		std::shared_ptr<Texture> texture;

		if (!texture_source->internal || texture_source->internal_id != 1)
		{
			Error* error = renderer.CreateTextureFromImage(pnTexData + 1, *pnTexData, texture);
			assert(error == nullptr);

			texture_source->texture = texture.get();
		}

		for (TextureClass texture_class : texture_source->classes)
		{
			texture_library.Add(texture_class, texture);
		}
	}

	for(int i = 0; i < enabled_effects.size(); i++)
	{
		enabled_effects[i]->Create();
	}
	return 0;
}

Error* Tripex::Startup()
{
	txs.reset();

	enabled_effects.push_back(effects[0]);

	effect_frames = 0.0f;
	fade_pos = 0.0f;
	status_msg[ 0 ] = 0;
	status_time = 0;
	effect_idx = 0;
	next_effect_idx = 0; 

//	txs[TXS_RENDER_FIRST] = true;
	txs[TXS_STARTED] = true;

//	InitBeats();
	LoadCfgItems();
	UpdateCfgItems(true);

	audio = std::make_unique<AudioData>( 512 );

	srand( timeGetTime( ) );

	std::vector<ColorRgb> pe;
	pe.resize(256);
	for( int i = 0; i < 256; i++ )
	{
		pe[i] = ColorRgb(i, i, i);
	}

//	{for( int i = 0; i < 256 * 256; i++ )
//	{
//		unsigned char c = ((uint8*)&g_anTexRawGUI[ 1 ])[ i ];
//		pc[ i ] = ZColour( c,c,c);
//	}
//	}

	Error* error = renderer.CreateTexture(256, 256, TextureFormat::P8, &g_anTexRawGUI[ 1 ], 256 * 256, 256, pe.data(), TextureFlags::None, gui);
	if (error) return TraceError(error);

//	gui = auto_ptr< ZTexture >(new ZTexture(pc));//(ZColour*)g_anTexRawGUI ));//cGUI.GetPtr()));
//	gui->m_nFlags.set(ZTexture::F_POINT_FILTER);

	// TEXTURE
//	renderer.AddTexture( gui.get( ) );
//	g_pD3D->vpTexture.push_back(gui.get());//auto_ptr< ZTexture >(gui);

	tef.Add( ( uint8* )&g_anTexRawFont[ 1 ] );
	tef.FindGlyph(' ')->end = 2;
	
	error = tef.Create(renderer);
	if (error) return TraceError(error);

//	renderer.AddTexture( tef.GetTexture( ) );
//	g_pD3D->vpTexture.push_back(tef.GetTexture());

//	pcBuffer.SetLength(1024 * 16);

//	pcHUD = new ZPaletteCanvas;
//	pcHUD->Create(true, 1024, 16);
//	ZColour c[256];
//	for(int i = 0; i < 256; i++) c[i] = ZColour::Grey(i);
//	pcHUD->SetPalette(c);

	srand( timeGetTime( ) );
	
	InitialiseThread( NULL );

	id = (int)enabled_effects.size();

	txs.set(TXS_CHANGE_EFFECT);

	txs.set( TXS_RESET_TIMING );

//	hRes = pcHUD->Initialise(d3d.get());
//	if(FAILED(hRes)) return TraceError(hRes);

	return nullptr;
}
Error* Tripex::Render()
{
	UpdateCfgItems();

	DWORD dwTime = timeGetTime( );
	if(txs.test(TXS_RESET_TIMING))//bResetTiming)
	{
		last_time = dwTime - 10000;
		txs.reset(TXS_RESET_TIMING);
//		bResetTiming = false;
	}

	static float fFrames = 0;
	DWORD dwTimeChange = dwTime - last_time;

	fFrames += std::min(4.0f, dwTimeChange / (1000.0f / 15.0f));
	last_time = dwTime;
//	AddFrameTime(false, dwTimeChange);

	effect_frames += fFrames;
	fade_pos += dwTimeChange; 

	EffectHandler *ppDrawEffect[2];
	if(txs.test(TXS_EFFECT_LEFT) || txs.test(TXS_EFFECT_RIGHT))//bEffectLeft || bEffectRight))
	{
		int nNewEffect = effect_idx;
		if(txs.test(TXS_EFFECT_LEFT))
		{
			if(effect_idx > 1) nNewEffect--;
			txs.reset(TXS_EFFECT_LEFT);
		}
		else
		{
			if(effect_idx < ( int )enabled_effects.size() - 1) nNewEffect++;
			txs.reset(TXS_EFFECT_RIGHT);
		}

		if(nNewEffect != effect_idx)
		{
			effect_idx = nNewEffect;
			ShowStatusMsg("Current Effect: %s", enabled_effects[effect_idx]->sName.c_str());

//				if(bEffectLeft) nEffect--;
//				else if(bEffectRight) nEffect++;
//				bEffectLeft = bEffectRight = false;

			txs.reset(TXS_IN_FADE);
//				bInFade = false;
			effect_frames = 0;

			Error* error = enabled_effects[effect_idx]->Reconfigure(*audio.get(), texture_library);
			if(error) return TraceError(error); 
		}
	}
	if(!txs.test(TXS_IN_FADE) && (!txs.test(TXS_HOLD) || txs.test(TXS_CHANGE_EFFECT)) && 
		(effect_idx == 0 || 
		effect_frames > (enabled_effects[effect_idx]->fChange * EFFECT_CHANGE_FRAMES)) && 
		enabled_effects.size() > 1)
	{
		txs.reset(TXS_CHANGE_EFFECT);
//				bChangeEffect = false;
		next_effect_idx = 0;

		txs.set(TXS_IN_FADE);
//				bInFade = true;
		effect_frames = 0;

		for(int i = 1; i < ( int )enabled_effects.size(); i++) 
		{
			enabled_effects[i]->bValid = (i != effect_idx && enabled_effects[effect_idx]->nDrawOrder != enabled_effects[i]->nDrawOrder);
		}

		float pt = 0;
		// fairness = (random)0-1(ordered)
		const float selection_fairness = 0.7f;
		float temperature = (1.0f / selection_fairness) - 1;
		// temperature = (random)+inf - 0(ordered)
		for(int i = 1; i < ( int )enabled_effects.size(); i++)
		{
			float weight = (float(id - enabled_effects[i]->nLastUsed) / (enabled_effects.size() - 1)) - 1;
			// weight = small (last used) - large (not used)

			if(!enabled_effects[i]->bValid)
			{
				enabled_effects[i]->fProb = 0.0f;
			}
			else if(temperature < FLOAT_ZERO)
			{
				enabled_effects[i]->fProb = (weight >= 0)? 0.5f : 0.0f;
			}
			else 
			{
				enabled_effects[i]->fProb = 1.0f / (1.0f + expf(-weight / temperature));
			}

			enabled_effects[i]->fProb *= enabled_effects[i]->fPreference * std::max(0.1f, 1.0f - fabsf(audio->GetIntensity( ) - enabled_effects[i]->fActivity));

			pt += enabled_effects[i]->fProb;//vpEffect[i]->preference * p[i];
		}

		next_effect_idx = 0;
		if(pt > FLOAT_ZERO)
		{
			for (int i = 1; i < (int)enabled_effects.size(); i++)
			{
				enabled_effects[i]->fProb /= pt;
			}

			double r = (rand() % 1000);
			for(next_effect_idx = 1; next_effect_idx < ( int )enabled_effects.size() - 1; next_effect_idx++)
			{
				r -= enabled_effects[next_effect_idx]->fProb * 1000.0;
				if(r < 0) break;
			}
		}
		enabled_effects[next_effect_idx]->nLastUsed = id;

		fade_pos = 0;
		txs.reset(TXS_RESET_TARGET);
//				bResetTarget = false;
	}
	if(txs[TXS_RECONFIGURE])//bReconfigure)
	{
		Error* error = enabled_effects[effect_idx]->Reconfigure(*audio.get(), texture_library);
		if(error) return TraceError(error);

		txs.reset(TXS_IN_FADE);
		txs.reset(TXS_RECONFIGURE);
	}

	if(txs.test(TXS_HOLD))//bEffectPref)
	{
		txs[TXS_IN_FADE] = false;
		if(effect_idx == 0) effect_idx = next_effect_idx;
	}

	const float fCrossfading = 0.934f;
	const float fFadeIn = 0.241f;
	const float fFadeOut = 0.353f;

	float fOut = (effect_idx == 0)? 0 : (fFadeOut * 5000.0f);
	float fFadeLength = (fFadeIn * 5000.0f) + fOut - (fCrossfading * std::min((fFadeIn * 5000.0f), fOut));

	float fBr = txs.test(TXS_IN_FADE)? std::min(1.0f, std::max(0.0f, 1.0f - (fade_pos / fOut))) : 1;
	ppDrawEffect[0] = (fBr < FLOAT_ZERO)? pEffectBlank : enabled_effects[effect_idx].get();
	ppDrawEffect[0]->fBr = fBr;

	fBr = txs.test(TXS_IN_FADE)? std::min(1.0f, std::max(0.0f, 1.0f - ((fFadeLength - fade_pos) / (fFadeIn * 5000.0f)))) : 0;
	ppDrawEffect[1] = (fBr < FLOAT_ZERO)? pEffectBlank : enabled_effects[next_effect_idx].get();
	ppDrawEffect[1]->fBr = fBr;

	if(ppDrawEffect[1]->fBr > FLOAT_ZERO && !txs[TXS_RESET_TARGET])
	{
		Error* error = ppDrawEffect[1]->Reconfigure(*audio.get(), texture_library);
		if(error) return TraceError(error);

		txs[TXS_RESET_TARGET] = true;
	}
	if(ppDrawEffect[1]->fBr >= 1 - FLOAT_ZERO)
	{
		effect_idx = next_effect_idx;
		ppDrawEffect[0] = ppDrawEffect[1];
		ppDrawEffect[1] = pEffectBlank;//&blank;
		txs.reset(TXS_IN_FADE);
	}

	if(ppDrawEffect[1]->nDrawOrder < ppDrawEffect[0]->nDrawOrder)
	{
		std::swap( ppDrawEffect[1], ppDrawEffect[0] );
	}
	if((!ppDrawEffect[0]->CanRender(fFrames) || ppDrawEffect[0] == pEffectBlank) && (!ppDrawEffect[1]->CanRender(fFrames) || ppDrawEffect[1] == pEffectBlank) && !(ppDrawEffect[0] == pEffectBlank && ppDrawEffect[1] == pEffectBlank && fFrames > 1.0f))
	{
		return nullptr;
	}

	audio->Update( fFrames, enabled_effects[effect_idx]->fSensitivity );
//	UpdateBeat(fFrames);

	for(int i = 0; i < 2; i++)
	{
		Error* error = ppDrawEffect[i]->Calculate( fFrames, *audio.get(), renderer );
		if(error) return TraceError(error);
	}

	// lock the back

//	static unsigned int nc = 0x0000ffff;
//	nc ^= 0x00005553;

	Error* error = renderer.BeginFrame();
	if (error) return TraceError(error);

//	g_pD3D->SetTexture(0, gui.get());
/*
	Transparent = (1 << 0L),
	Multiply = (1 << 1L),
	Perspective = (1 << 2L),
	Shade = (1 << 3L),
	ZBuffer = (1 << 4L),
	PerspectiveCorrect = (1 << 5L),
	LuminanceOpacity = (1 << 6L),
	InverseMultiply = (1 << 7L),
*/

// D3DRS_CLIPPING is not supported in the XBox DX8 SDK ...
	//g_pD3D->SetRenderState(D3DRS_CLIPPING, false);
	// ... Forza.

	for(int i = 0; i < 2; i++)
	{
		DWORD dwStartTick = timeGetTime( );

		error = ppDrawEffect[i]->Render(renderer);
		if(error) return TraceError(error);
	}

	overlay_background.Clear();
	overlay_text.Clear();
	overlay_foreground.Clear();

//	int nX = d3d->GetWidth() * 0.06;
//	int nY = d3d->GetHeight() - 63;

//	DebugBreak();
//	g_pD3D->SetTexture(0, gui.get());

	std::string sMsg;
	float fMsgBr = 0.0f;

	DWORD dwTick = timeGetTime( );
	if(dwTick >= status_time && dwTick <= status_time + (MSG_DISPLAY_TIME + MSG_FADEOUT_TIME) && status_msg[ 0 ] != 0)
	{
		sMsg = status_msg;
		if(dwTick < status_time + MSG_DISPLAY_TIME) fMsgBr = 1.0f;
		else
		{
			float fPos = (dwTick - (status_time + MSG_DISPLAY_TIME)) / (float)MSG_FADEOUT_TIME;
			float fV = 1.0f - cos(fPos * 3.14159f / 2.0f);
			fMsgBr = 1.0f - (fV * fV);//fPos;//fV;//1 - (fV * fV);//1.0f;
		}
	}

	const float overlay_back_mult = 0.65f;

	if(sMsg.size() > 0)
	{
		DrawMessage(tef, 38, sMsg.c_str(), fMsgBr, overlay_back_mult);
	}

	if(txs.test(TXS_VISIBLE_BEATS))
	{
		audio->Render(overlay_background, overlay_foreground, overlay_back_mult);
	}

	// Draw the overlay background
	{
		RenderState render_state;
		render_state.blend_mode = BlendMode::OverlayBackground;
		render_state.depth_mode = DepthMode::Disable;

		error = renderer.DrawIndexedPrimitive(render_state, overlay_background);
		if (error) return TraceError(error);
	}

	// Draw the overlay text
	{
		RenderState render_state;
		render_state.blend_mode = BlendMode::OverlayForeground;
		render_state.depth_mode = DepthMode::Disable;
		render_state.texture_stages[0].texture = tef.texture.get();

		error = renderer.DrawIndexedPrimitive(render_state, overlay_text);
		if (error) return TraceError(error);
	}

	// Draw the overlay foreground
	{
		RenderState render_state;
		render_state.blend_mode = BlendMode::OverlayForeground;
		render_state.depth_mode = DepthMode::Disable;

		error = renderer.DrawIndexedPrimitive(render_state, overlay_foreground);
		if (error) return TraceError(error);
	}

	error = renderer.EndFrame();
	if (error) return TraceError(error);

//	AddFrameTime(true, clock() - dwRenderStartClock);

	fFrames = 0;
	return 0;
}
void Tripex::Shutdown( )
{
	if(!txs.test(TXS_STARTED)) return;
	txs[TXS_STARTED] = false;

	audio.reset();

//	gui = auto_ptr<ZTexture>(NULL);
//	pcHUD = NULL;

	for(int i = 0; i < ( int )enabled_effects.size(); i++)
	{
		enabled_effects[i]->Destroy();
	}

	SaveCfgItems();
}

void Tripex::SetAudioData(int num_channels, int sample_rate, int sample_bits, const void* data, size_t data_len)
{
	audio->SetDataFormat(num_channels, sample_rate, sample_bits);
	audio->AddData(data, data_len);
}

void Tripex::ChangeEffect()
{
	txs[TXS_CHANGE_EFFECT] = true;
}

void Tripex::MoveToPrevEffect()
{
	txs[TXS_EFFECT_LEFT] = true;
}

void Tripex::MoveToNextEffect()
{
	txs[TXS_EFFECT_RIGHT] = true;
}

void Tripex::ReconfigureEffect()
{
	txs[TXS_RECONFIGURE] = true;
}

void Tripex::ToggleHoldingEffect()
{
	txs.flip(TXS_HOLD);
	ShowStatusMsg("Effect %s", txs[TXS_HOLD] ? "held" : "not held");
}

void Tripex::ToggleAudioInfo()
{
	txs.flip(TXS_VISIBLE_BEATS);
}

int Tripex::GetClippedLineLength(const TextureFont& pFont, const char* sText, int nClipWidth)
{
	bool bFirstWord = true;
	int nLastEnd = 0;
	for (int i = 0;;)
	{
		bool bSpace = (isspace(sText[i]) != 0);
		if (bSpace || sText[i] == 0 || bFirstWord)
		{
			if (bSpace) bFirstWord = false;

			std::string sLine = std::string(sText, i);
			if (pFont.GetWidth(sLine.c_str()) > nClipWidth)
			{
				return nLastEnd;
			}
			else if (sText[i] == 0)
			{
				return i;
			}

			nLastEnd = i;

			if (bSpace)
			{
				while (isspace(sText[i])) i++;
			}
			else i++;
		}
		else i++;
	}
}

void Tripex::DrawMessage(const TextureFont& font, int y, const char* text, float brightness, float back_brightness)
{
	const int clip_width = renderer.GetWidth() - 40;
	const int line_height = 20;

	std::vector<std::string> lines;
	int width = 0;
	for (;;)
	{
		while (isspace(*text))
		{
			text++;
		}
		if (*text == 0)
		{
			break;
		}

		int length = GetClippedLineLength(font, text, clip_width);
		if (length > 0)
		{
			std::string line(text, length);
			lines.push_back(line);
			width = std::max(width, font.GetWidth(line.c_str()));
			text += length;
		}
	}

	int centre_x = renderer.GetWidth() / 2;

	if (back_brightness > FLOAT_ZERO)
	{
		float dark_brightness = (back_brightness * brightness);
		const Point<int> p(centre_x - (width / 2) - 15, y);
		const Rect<int> r(0, 0, width + 30, 25 + ((int)lines.size() - 1) * line_height);
		overlay_background.AddSprite(p, r, dark_brightness);
	}

	for (int i = 0; i < (int)lines.size(); i++)
	{
		int line_width = font.GetWidth(lines[i].c_str());
		font.Draw(overlay_text, lines[i].c_str(), Point<int>(centre_x - (line_width / 2), y + 5 + (i * line_height)), ColorRgb::Grey((int)(brightness * 255.0f)));
	}
}

IMPORT_EFFECT(Blank);
IMPORT_EFFECT(BezierCube);
IMPORT_EFFECT(CollapsingLightSphere);
IMPORT_EFFECT(Distortion1);
IMPORT_EFFECT(Distortion2);
IMPORT_EFFECT(Distortion2Col);
IMPORT_EFFECT(DotStar);
IMPORT_EFFECT(Flowmap);
IMPORT_EFFECT(Tunnel);
IMPORT_EFFECT(WaterGlobe);
IMPORT_EFFECT(Tube);
IMPORT_EFFECT(Sun);
IMPORT_EFFECT(Bumpmapping);
IMPORT_EFFECT(Spectrum);
IMPORT_EFFECT(Rings);
IMPORT_EFFECT(Phased);
IMPORT_EFFECT(MotionBlur1);
IMPORT_EFFECT(MotionBlur2);
IMPORT_EFFECT(MotionBlur3);
IMPORT_EFFECT(MotionBlur3Alt);
IMPORT_EFFECT(MorphingSphere);
IMPORT_EFFECT(Metaballs);
IMPORT_EFFECT(LightTentacles);
IMPORT_EFFECT(LightStar);
IMPORT_EFFECT(LightSphere);
IMPORT_EFFECT(LightRing);

void Tripex::AddEffect(std::shared_ptr<EffectHandler> (*fn)(), const char* sName, int nDrawOrder, float fStartupWeight, TextureClass nTex, ...)
{
	std::shared_ptr<EffectHandler> pEffect = fn();
	pEffect->sName = sName;
	pEffect->nDrawOrder = nDrawOrder;
	pEffect->fStartupWeight = fStartupWeight;

	va_list pArg;
	va_start(pArg, nTex);
	while (nTex != TextureClass::Invalid)
	{
		pEffect->snTexture.insert(nTex);
		nTex = va_arg(pArg, TextureClass);
	}
	va_end(pArg);

	if (effects.size() == 0)
	{
		effects.push_back(pEffect);
	}
	else
	{
		for (int i = 1;; i++)
		{
			if (i == effects.size() || _stricmp(pEffect->sName.c_str(), effects[i]->sName.c_str()) < 0)
			{
				effects.insert(effects.begin() + i, pEffect);
				break;
			}
		}
	}
}

void Tripex::CreateEffectList()
{
	AddEffect(&CreateEffect_Blank, "Blank", ZORDER_BLANK, 1.0f, TextureClass::Invalid);

	pEffectBlank = effects[0].get();

	AddEffect(&CreateEffect_Tunnel, "Tunnel", ZORDER_TUNNEL, 1.0f, TextureClass::TunnelBackground, TextureClass::Invalid);
	AddEffect(&CreateEffect_WaterGlobe, "WaterGlobe", ZORDER_WATERGLOBE, 10.0f, TextureClass::WaterGlobeEnvMap, TextureClass::Invalid);
	AddEffect(&CreateEffect_Tube, "Tube", ZORDER_TUBE, 1.0f, TextureClass::TubeEnvMap, TextureClass::Invalid);
	AddEffect(&CreateEffect_Sun, "Sun", ZORDER_SUN, 1.0f, TextureClass::Invalid);
	AddEffect(&CreateEffect_Bumpmapping, "Bumpmapping", ZORDER_BUMPMAPPING, 8.0f, TextureClass::BumpMapBackground, TextureClass::BumpMapTentaclesEnvMap, TextureClass::Invalid);
	AddEffect(&CreateEffect_Spectrum, "Spectrum", ZORDER_ANALYSER, 1.0f, TextureClass::AnalyzerEnvMap, TextureClass::Invalid);
	AddEffect(&CreateEffect_Rings, "ConcentricRings", ZORDER_RINGS, 1.0f, TextureClass::RingsEnvMap, TextureClass::Invalid);
	AddEffect(&CreateEffect_Phased, "Phased", ZORDER_PHASED, 1.0f, TextureClass::PhasedSprite, TextureClass::Invalid);
	AddEffect(&CreateEffect_MotionBlur1, "MotionBlur1", ZORDER_MOTIONBLUR, 1.0f, TextureClass::MotionBlurEnvMap, TextureClass::Invalid);
	AddEffect(&CreateEffect_MotionBlur2, "MotionBlur2", ZORDER_MOTIONBLUR2, 1.0f, TextureClass::MotionBlur2EnvMap, TextureClass::Invalid);
	AddEffect(&CreateEffect_MotionBlur3, "MotionBlur3", ZORDER_MOTIONBLUR3, 1.0f, TextureClass::MotionBlur3EnvMap, TextureClass::Invalid);
	AddEffect(&CreateEffect_MotionBlur3Alt, "MotionBlur3(Alt)", ZORDER_MOTIONBLUR3ALT, 1.0f, TextureClass::MotionBlur3AltEnvMap, TextureClass::Invalid);
	AddEffect(&CreateEffect_MorphingSphere, "MorphingSphere", ZORDER_MORPHINGSPHERE, 1.0f, TextureClass::MorphingSphereEnvMap, TextureClass::Invalid);
	AddEffect(&CreateEffect_LightTentacles, "LightTentacles", ZORDER_LIGHTTENTACLES, 1.0f, TextureClass::LightTentaclesSprite, TextureClass::Invalid);
	AddEffect(&CreateEffect_LightStar, "LightStar", ZORDER_LIGHTSTAR, 1.0f, TextureClass::LightStarSprite, TextureClass::LightStarBackground, TextureClass::Invalid);
	AddEffect(&CreateEffect_LightSphere, "LightSphere", ZORDER_LIGHTSPHERE, 1.0f, TextureClass::LightSphereSprite, TextureClass::LightSphereBackground, TextureClass::Invalid);
	AddEffect(&CreateEffect_LightRing, "LightRing", ZORDER_LIGHTRING, 1.0f, TextureClass::LightRingSprite, TextureClass::LightRingBackground, TextureClass::Invalid);
	AddEffect(&CreateEffect_Flowmap, "Flowmap", ZORDER_FLOWMAP, 10.0f, TextureClass::Invalid);
	AddEffect(&CreateEffect_DotStar, "DotStar", ZORDER_DOTSTAR, 1.0f, TextureClass::DotStarSprite, TextureClass::DotStarBackground, TextureClass::Invalid);
	AddEffect(&CreateEffect_Distortion2, "Distortion2", ZORDER_DISTORTION2, 1.0f, TextureClass::Distortion2Background, TextureClass::Invalid);
	AddEffect(&CreateEffect_Distortion2Col, "Distortion2(Lit)", ZORDER_DISTORTION2COL, 1.0f, TextureClass::Distortion2ColBackground, TextureClass::Invalid);
	AddEffect(&CreateEffect_CollapsingLightSphere, "CollapsingLightSphere", ZORDER_COLLAPSINGSPHERE, 1.0f, TextureClass::CollapsingSphereSprite, TextureClass::CollapsingSphereBackground, TextureClass::Invalid);
	AddEffect(&CreateEffect_BezierCube, "BezierCube", ZORDER_BEZIERCUBE, 1.0f, TextureClass::BezierCubeSprite, TextureClass::BezierCubeBackground, TextureClass::Invalid);
	AddEffect(&CreateEffect_Distortion1, "Distortion1", ZORDER_DISTORTION, 1.0f, TextureClass::DistortionBackground, TextureClass::Invalid);
}

ConfigItem* Tripex::AddCfgItem(ConfigItem* pItem)
{
	pppCfgItem->push_back(pItem);
	(*name_to_config_item)[pItem->GetKeyName()].push_back(pItem);
	return pItem;
}

void Tripex::CreateCfgItems()
{
	if (pppCfgItem == NULL)
	{
		pppCfgItem = new std::vector<ConfigItem*>;
		name_to_config_item = new std::map< std::string, std::vector< ConfigItem* >, CI_STR_CMP >();
		psEffect = new std::string[effects.size()];

		for (int i = 0; i < (int)effects.size(); i++)
		{
			AddCfgItem(ConfigItem::String(effects[i]->GetCfgItemName().c_str(), &psEffect[i]));
		}
	}
}

void Tripex::UpdateCfgItems(bool bInit)
{
	for (unsigned int i = 0; i < pppCfgItem->size(); i++)
	{
		(*pppCfgItem)[i]->Update(bInit);
	}
	for (unsigned int i = 0; i < effects.size(); i++)
	{
		for (int j = 0; j < 5; j++)
		{
			effects[i]->pfSetting[j] = 0.0f;
		}

		ConfigItem* pItem = FindCfgItem(effects[i]->GetCfgItemName().c_str());
		pItem->GetFloatArray(5, effects[i]->pfSetting);
	}
	// filtering -> num
}

ConfigItem* Tripex::FindCfgItem(const char* sName)
{
	for (unsigned int i = 0; i < pppCfgItem->size(); i++)
	{
		if (!_stricmp((*pppCfgItem)[i]->sName.c_str(), sName)) return (*pppCfgItem)[i];
	}
	//	assert(false);
	return NULL;
}

static bool bLoadedCfg = false;
void Tripex::LoadCfgItems()
{
	if (bLoadedCfg) return;

	UpdateCfgItems();

	std::map< std::string, std::vector< ConfigItem* >, CI_STR_CMP >::iterator it;
	for (it = name_to_config_item->begin(); it != name_to_config_item->end(); it++)
	{
		std::string sKey = it->first;
		//			HKEY hKey = RegCreateKey(HKEY_CURRENT_USER, it->first.c_str(), KEY_READ);
		for (int j = 0; j < (int)it->second.size(); j++)
		{
			ConfigItem* pItem = it->second[j];
			//				if(!pItem->Load(hKey))
			//				{
			switch (pItem->nType)
			{
			case ConfigItem::CIT_INT:
				pItem->SetInt(GetDefaultInt(pItem->sName.c_str()));
				break;
			case ConfigItem::CIT_BOOL:
				pItem->SetBool(!!GetDefaultInt(pItem->sName.c_str()));
				break;
			case ConfigItem::CIT_FLOAT:
				pItem->SetFloat(GetDefaultInt(pItem->sName.c_str()) / 1000.0f);
				break;
			case ConfigItem::CIT_STRING:
				pItem->SetString(GetDefaultStr(pItem->sName.c_str()));
				break;
			default:
				assert(false);
				break;
			}
			pItem->bSave = false;
			//				}
		}
		//			RegCloseKey(hKey);
	}

	bLoadedCfg = true;
}

void Tripex::SaveCfgItems()
{
	if (!bLoadedCfg) return;
	UpdateCfgItems(false);

	//		for(map< string, vector< auto_ptr< CCfgItem > > >::iterator it = mpCfgItem.begin(); it != mpCfgItem.end(); it++)
	//		{
	//			string s = it->first;
	//			HKEY hKey = NULL;
	//			for(int j = 0; j < it->second.size(); j++)
	//			{
	//				if(it->second[j]->bSave && hKey == NULL)
	//				{
	//					hKey = RegCreateKey(HKEY_CURRENT_USER, it->first.c_str(), KEY_WRITE);
	//				}
	//				it->second[j]->Save(hKey);
	//			}
	//			RegCloseKey(hKey);
	//		}
}
