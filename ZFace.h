#pragma once

#include "ZEdge.h"

struct ZFace
{
	WORD m_anVtx[ 3 ];

	// Constructors:
	inline ZFace() = default;
	inline ZFace( WORD nVtx1, WORD nVtx2, WORD nVtx3 );

	// Set( ):
	inline void Set( const ZFace &f );
	inline void Set( WORD nVtx1, WORD nVtx2, WORD nVtx3 );

	// SwapOrder( ):
	inline void SwapCull( );

	// Contains( ):
	inline BOOL Contains( WORD nVtx ) const;
	inline BOOL Contains( const ZEdge &e ) const;

	// Replace( ):
	inline void Replace( WORD nOldVtx, WORD nNewVtx );

	// IndexOf( ):
	inline int IndexOf( WORD nVtx ) const;

	// GetEdge( ):
	inline ZEdge GetEdge( int nIdx ) const;

	// operator=( ):
	inline ZFace &operator=( const ZFace &f );

	// operator==( ):
	inline BOOL operator==( const ZFace &f ) const;

	// operator[ ]( ):
	inline WORD &operator[ ]( int nPos );
	inline const WORD &operator[ ]( int nPos ) const;
};

/*---------------------------------
* Constructor:
-----------------------------------*/

ZFace::ZFace( WORD nVtx1, WORD nVtx2, WORD nVtx3 )
{
	Set( nVtx1, nVtx2, nVtx3 );
}

/*---------------------------------
* Set( ):
-----------------------------------*/

void ZFace::Set( const ZFace &f )
{
	Set( f.m_anVtx[ 0 ], f.m_anVtx[ 1 ], f.m_anVtx[ 2 ] );
}

void ZFace::Set( WORD nVtx1, WORD nVtx2, WORD nVtx3 )
{
	m_anVtx[ 0 ] = nVtx1;
	m_anVtx[ 1 ] = nVtx2;
	m_anVtx[ 2 ] = nVtx3;
}

/*---------------------------------
* SwapCull( ):
-----------------------------------*/

void ZFace::SwapCull( )
{
	swap( m_anVtx[0], m_anVtx[1] );
}

/*---------------------------------
* Contains( ):
-----------------------------------*/

BOOL ZFace::Contains( WORD nVtx ) const
{
	return m_anVtx[ 0 ] == nVtx || m_anVtx[ 1 ] == nVtx || m_anVtx[ 2 ] == nVtx;
}
BOOL ZFace::Contains( const ZEdge &e ) const
{
	return Contains( e[ 0 ] ) && Contains( e[ 1 ] );
}

/*---------------------------------
* Replace( ):
-----------------------------------*/

void ZFace::Replace( WORD nOldVtx, WORD nNewVtx )
{
	for( int i = 0; i < 3; i++ )
	{
		if( m_anVtx[ i ] == nOldVtx ) m_anVtx[ i ] = nNewVtx;
	}
}

/*---------------------------------
* IndexOf( ):
-----------------------------------*/

int ZFace::IndexOf( WORD nVtx ) const
{
	for( int i = 0; i < 3; i++ )
	{
		if( m_anVtx[ i ] == nVtx ) return i;
	}
	return -1;
}

/*---------------------------------
* GetEdge( ):
-----------------------------------*/

ZEdge ZFace::GetEdge( int nIdx ) const
{
	static const int anNextIdx[ 3 ] = { 1, 2, 0 };
	_ASSERT( nIdx >= 0 && nIdx <= 2 );
	return ZEdge( m_anVtx[ nIdx ], m_anVtx[ anNextIdx[ nIdx ] ] );
}

/*---------------------------------
* operator=( ):
-----------------------------------*/

ZFace &ZFace::operator=( const ZFace &f )
{
	Set( f.m_anVtx[ 0 ], f.m_anVtx[ 1 ], f.m_anVtx[ 2 ] );
	return *this;
}

/*---------------------------------
* operator==( ):
-----------------------------------*/

BOOL ZFace::operator==( const ZFace &f ) const
{
	return m_anVtx[ 0 ] == f.m_anVtx[ 0 ] && m_anVtx[ 1 ] == f.m_anVtx[ 1 ] && m_anVtx[ 2 ] == f.m_anVtx[ 2 ];
}

/*---------------------------------
* operator[ ]( ):
-----------------------------------*/

WORD &ZFace::operator[ ]( int nPos )
{
	return m_anVtx[ nPos ];
}

const WORD &ZFace::operator[ ]( int nPos ) const
{
	return m_anVtx[ nPos ];
}
