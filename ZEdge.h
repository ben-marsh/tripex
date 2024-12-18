#pragma once

#include "Misc.h"

class ZEdge
{
public:
	WORD m_anVtx[ 2 ];

	// Constructor:
	inline ZEdge();
	inline ZEdge( const ZEdge &e );
	inline ZEdge( WORD nVtx1, WORD nVtx2 );

	// Set( ):
	inline void Set( const ZEdge &e );
	inline void Set( WORD nVtx1, WORD nVtx2 );

	// Flipped( ):
	inline ZEdge Flipped( ) const;

	// Ordered( ):
	inline ZEdge Ordered( ) const;

	// Contains( ):
	inline BOOL Contains( WORD nVtx ) const;

	// CommonIndex( ):
	inline WORD CommonIndex( const ZEdge &e ) const;

	// operator=( ):
	inline ZEdge &operator=( const ZEdge &e );

	// operator==( ):
	inline BOOL operator==( const ZEdge &e ) const;

	// operator[ ]( ):
	inline WORD &operator[ ]( int nIdx );
	inline const WORD &operator[ ]( int nIdx ) const;
};

/*---------------------------------
* Constructor:
-----------------------------------*/

ZEdge::ZEdge( )
{
}

ZEdge::ZEdge( const ZEdge &e )
{
	Set(e);
}

ZEdge::ZEdge( WORD nVtx1, WORD nVtx2 )
{
	Set( nVtx1, nVtx2 );
}

/*---------------------------------
* Set( ):
-----------------------------------*/

void ZEdge::Set( const ZEdge &e )
{
	Set( e.m_anVtx[ 0 ], e.m_anVtx[ 1 ] );
}

void ZEdge::Set( WORD nVtx1, WORD nVtx2 )
{
	m_anVtx[ 0 ] = nVtx1;
	m_anVtx[ 1 ] = nVtx2;
}

/*---------------------------------
* Flipped( ):
-----------------------------------*/

ZEdge ZEdge::Flipped( ) const
{
	return ZEdge( m_anVtx[ 1 ], m_anVtx[ 0 ] );
}

/*---------------------------------
* Ordered( ):
-----------------------------------*/

ZEdge ZEdge::Ordered( ) const
{
	if( m_anVtx[ 0 ] < m_anVtx[ 1 ] )
	{
		return ZEdge( m_anVtx[ 0 ], m_anVtx[ 1 ] );
	}
	else 
	{
		return ZEdge( m_anVtx[ 1 ], m_anVtx[ 0 ] );
	}
}

/*---------------------------------
* Contains( ):
-----------------------------------*/

BOOL ZEdge::Contains( WORD nVtx ) const
{
	return m_anVtx[ 0 ] == nVtx || m_anVtx[ 1 ] == nVtx;
}

/*---------------------------------
* CommonIndex( ):
-----------------------------------*/

WORD ZEdge::CommonIndex( const ZEdge &e ) const
{
	if( e.Contains( m_anVtx[ 0 ] ) )
	{
		return m_anVtx[ 0 ];
	}
	else if( e.Contains( m_anVtx[ 1 ] ) )
	{
		return m_anVtx[ 1 ];
	}
	return ( WORD )-1;
}


/*---------------------------------
* operator=( ):
-----------------------------------*/

ZEdge &ZEdge::operator=( const ZEdge &e )
{
	Set( e );
	return *this;
}

/*---------------------------------
* operator==( ):
-----------------------------------*/

BOOL ZEdge::operator==( const ZEdge &e ) const
{
	return m_anVtx[ 0 ] == e.m_anVtx[ 0 ] && m_anVtx[ 1 ] == e.m_anVtx[ 1 ];
}

/*---------------------------------
* operator[ ]( ):
-----------------------------------*/

WORD &ZEdge::operator[ ]( int nIdx )
{
	return m_anVtx[ nIdx ];
}

const WORD &ZEdge::operator[ ]( int nIdx ) const
{
	return m_anVtx[ nIdx ];
}
