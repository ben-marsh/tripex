#include "StdAfx.h"
#include "main.h"
#include "ZPtr.h"
#include "config.h"
#include "error.h"
#include "CCfgItem.h"

class ERROR_TRACE
{
public:
	const char *m_sFile;
	unsigned int m_nLine;
};

vector< ERROR_TRACE > *g_pvErrTrace = NULL;

/*---------------------------------
* AddToErrorTrace( )
-----------------------------------*/

HRESULT AddToErrorTrace(HRESULT hRes, char *sFile, unsigned int nLine)
{
	if( hRes != D3D_OK )
	{
		if( g_pvErrTrace == NULL )
		{
			g_pvErrTrace = new vector< ERROR_TRACE >;
		}
		ERROR_TRACE t = { sFile, nLine };
		g_pvErrTrace->push_back( t );
	}
	return hRes;
}

/*---------------------------------
* ClearErrorTrace( ):
-----------------------------------*/

void ClearErrorTrace()
{
	if( g_pvErrTrace != NULL )
	{
		delete g_pvErrTrace;
		g_pvErrTrace = NULL;
	}
}

/*---------------------------------
* GetErrorString( ):
-----------------------------------*/

BOOL GetErrorString( HRESULT hRes, string *ps )
{
	if( hRes == D3D_OK ) return FALSE;

	char sBuf[ 100 ];
	sprintf( sBuf, "Error %08x\n", hRes );

	*ps = sBuf;
	*ps += "Trace: ";
	if( g_pvErrTrace != NULL )
	{
		for( unsigned int i = 0; i < g_pvErrTrace->size( ); i++ )
		{
			if( i > 0 ) *ps += ", ";

			const char *sPos = strrchr( (*g_pvErrTrace)[ i ].m_sFile, '\\' );
			if( sPos == NULL ) sPos = (*g_pvErrTrace)[ i ].m_sFile;
			else sPos++;

			sprintf( sBuf, "%s(%d)", sPos, (*g_pvErrTrace)[ i ].m_nLine );
			*ps += sBuf;
		}
	}
	else
	{
		*ps += "(none)";
	}
	return TRUE;
}
