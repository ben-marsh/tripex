#include "Platform.h"
#include "TextureSource.h"
#include "config-defaults.h"
#include <vector>
#include <map>

// config-general.cpp
bool bAvoidBigReactions = false;

#define HKEY_ROOT (HKEY_LOCAL_MACHINE)
#define KEY_MAIN "Software\\-\\Tripex"
#define KEY_TEXTURES "Software\\-\\Tripex\\Textures"

bool LoadTextureSettings(std::vector< TextureSource* > &ppItem)
{
//		HKEY hKey = RegCreateKey(HKEY_ROOT, KEY_TEXTURES, KEY_READ);
//		if(hKey == NULL) return false;

	static char sName[100];
	static char sValue[1024];

	std::map< int, std::string > mpIntUse, mpExtFile, mpExtUse;

	int i;
	for(i = 1; i < num_internal_textures; i++)
	{
		char sKey[ 100 ];
		strcpy( sKey, "Textures\\InternalUse" );

		char *sSrc = strchr(sKey, 0);
		char *sDst = sSrc;
		if( i == 0 )
		{
			*(sDst++) = '0';
		}
		else
		{
			for( int nVal = i; nVal > 0; nVal /= 10 )
			{
				*(sDst++) = '0' + ( nVal % 10 );
			}
		}
		*(sDst--) = 0;

		while( sDst > sSrc )
		{
			*sDst ^= *sSrc;
			*sSrc ^= *sDst;
			*sDst ^= *sSrc;
			sDst--;
			sSrc++;
		}

		mpIntUse[i] = GetDefaultStr( sKey );
	}

//		DWORD dwIndex = 0;
//		RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &dwIndex, NULL, NULL, NULL, NULL);
//		for(; dwIndex > 0; dwIndex--)
//		{
//			ZeroMemory(sName, sizeof(sName));
//			ZeroMemory(sValue, sizeof(sValue));
//	
//			DWORD dwType = REG_SZ;
//			DWORD dwNameSize = sizeof(sName);
//			DWORD dwValueSize = sizeof(sValue);
//			LONG lRes = RegEnumValue(hKey, dwIndex - 1, sName, &dwNameSize, NULL, &dwType, (unsigned char*)sValue, &dwValueSize);
//			if(lRes == ERROR_SUCCESS && dwType == REG_SZ && dwNameSize < sizeof(sName) - 5 && dwValueSize < sizeof(sValue) - 5)
//			{
//				sName[dwNameSize] = 0;
//				sValue[dwValueSize] = 0;
//	
//				int nTexture;
//				if(sscanf(sName, "InternalUse%d", &nTexture) == 1) mpIntUse[nTexture] = sValue;
//				else if(sscanf(sName, "ExternalFile%d", &nTexture) == 1) mpExtFile[nTexture] = sValue;
//				else if(sscanf(sName, "ExternalUse%d", &nTexture) == 1) mpExtUse[nTexture] = sValue;
//			}
//		}
	
	ppItem.clear();
	for(i = 1; i < num_internal_textures; i++)
	{
		std::string s = mpIntUse[i];
		ppItem.push_back(TextureSource::Internal(i, s.c_str()));
	}
	for(std::map< int, std::string >::iterator it = mpExtFile.begin(); it != mpExtFile.end(); it++)
	{
		ppItem.push_back(TextureSource::External(it->second.c_str(), mpExtUse[it->first].c_str()));
	}
	return true;
}
