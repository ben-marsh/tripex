#include "StdAfx.h"
#include <H:\Microsoft Xbox SDK\xbox\include\memory.h>
#include "config-defaults.h"

int GetDefaultInt( const char *sName )
{
	if( !_stricmp( sName, "Crossfading" ) ) return 934;
	if( !_stricmp( sName, "ExitOK" ) ) return 1;
	if( !_stricmp( sName, "FadeIn" ) ) return 241;
	if( !_stricmp( sName, "FadeOut" ) ) return 353;
	if( !_stricmp( sName, "FlowmapH" ) ) return 2;
	if( !_stricmp( sName, "FlowmapW" ) ) return 3;
	if( !_stricmp( sName, "HUDTransparency" ) ) return 349;
	if( !_stricmp( sName, "MaxFrameRate" ) ) return 40;
	if( !_stricmp( sName, "MeshHQ" ) ) return 1;
	if( !_stricmp( sName, "SaveLyricTiming" ) ) return 1;
	if( !_stricmp( sName, "SelectionFairness" ) ) return 699;
	if( !_stricmp( sName, "ShowHUD" ) ) return 1;
	if( !_stricmp( sName, "ShowMessages" ) ) return 1;
	if( !_stricmp( sName, "ShowNameStart" ) ) return 1;
	if( !_stricmp( sName, "ShowProgress" ) ) return 1;
	if( !_stricmp( sName, "ShowTime" ) ) return 1;
	if( !_stricmp( sName, "StartWindowed" ) ) return 1;

	return 0;
}

const char *GetDefaultStr( const char *sName )
{
	if( !_stricmp( sName, "Effects\\BezierCube" ) ) return "140,40,140,480,70";
	if( !_stricmp( sName, "Effects\\Bumpmapping" ) ) return "800,500,900,500,570";
	if( !_stricmp( sName, "Effects\\CollapsingLightSphere" ) ) return "180,280,500,310,500";
	if( !_stricmp( sName, "Effects\\ConcentricRings" ) ) return "660,250,590,1000,1000";
	if( !_stricmp( sName, "Effects\\Distortion1" ) ) return "210,80,640,260,520";
	if( !_stricmp( sName, "Effects\\Distortion2" ) ) return "550,320,500,500,460";
	if( !_stricmp( sName, "Effects\\Flowmap" ) ) return "920,800,690,580,520";
	if( !_stricmp( sName, "Effects\\LightRing" ) ) return "340,420,880,690,460";
	if( !_stricmp( sName, "Effects\\LightSphere" ) ) return "470,370,340,420,500";
	if( !_stricmp( sName, "Effects\\LightStar" ) ) return "60,60,500,500,570";
	if( !_stricmp( sName, "Effects\\LightTentacles" ) ) return "410,560,660,610,460";
	if( !_stricmp( sName, "Effects\\Metaballs" ) ) return "340,240,570,500,430";
	if( !_stricmp( sName, "Effects\\MorphingSphere" ) ) return "400,170,850,590,500";
	if( !_stricmp( sName, "Effects\\MotionBlur1" ) ) return "120,80,100,490,490";
	if( !_stricmp( sName, "Effects\\MotionBlur2" ) ) return "50,110,940,500,450";
	if( !_stricmp( sName, "Effects\\MotionBlur3" ) ) return "420,370,590,490,940";
	if( !_stricmp( sName, "Effects\\MotionBlur3Alt" ) ) return "1000,600,580,790,840";
	if( !_stricmp( sName, "Effects\\Phased" ) ) return "60,100,300,270,480";
	if( !_stricmp( sName, "Effects\\Spectrum" ) ) return "880,660,510,510,510";
	if( !_stricmp( sName, "Effects\\Sun" ) ) return "80,70,280,390,500";
	if( !_stricmp( sName, "Effects\\SuperSampling" ) ) return "90,280,310,390,1000";
	if( !_stricmp( sName, "Effects\\Tube" ) ) return "230,310,450,500,480";
	if( !_stricmp( sName, "Effects\\Tunnel" ) ) return "750,750,840,620,560";
	if( !_stricmp( sName, "Effects\\WaterGlobe" ) ) return "100,200,260,390,500";
	if( !_stricmp( sName, "Textures\\InternalUse1" ) ) return "23";
	if( !_stricmp( sName, "Textures\\InternalUse2" ) ) return "17,18,19,20,22,24,25,27,28,29,30";
	if( !_stricmp( sName, "Textures\\InternalUse3" ) ) return "1,3,9,11,13,15,21";
	if( !_stricmp( sName, "Textures\\InternalUse4" ) ) return "2,4,6,8,10,12,14,16";
	if( !_stricmp( sName, "Textures\\InternalUse5" ) ) return "2,4,5,6,8,10,12,14,16,23,26,31";
	if( !_stricmp( sName, "Textures\\InternalUse6" ) ) return "2,4,5,6,8,10,12,14,16,26";
	if( !_stricmp( sName, "Textures\\InternalUse7" ) ) return "4,8,10,12,14,16,17,18,19,20,22,25,27,28,29,30";
	if( !_stricmp( sName, "Textures\\InternalUse8" ) ) return "7";

	return "";
}
