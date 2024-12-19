#pragma once

#include "Edge.h"

struct Face
{
	UINT16 m_anVtx[ 3 ];

	// Constructors:
	inline Face() = default;
	inline Face(UINT16 nVtx1, UINT16 nVtx2, UINT16 nVtx3);

	// Set( ):
	inline void Set( const Face &f );
	inline void Set(UINT16 nVtx1, UINT16 nVtx2, UINT16 nVtx3);

	// SwapOrder( ):
	inline void SwapCull( );

	// Contains( ):
	inline bool Contains(UINT16 nVtx ) const;
	inline bool Contains( const Edge &e ) const;

	// Replace( ):
	inline void Replace(UINT16 nOldVtx, UINT16 nNewVtx );

	// IndexOf( ):
	inline int IndexOf(UINT16 nVtx ) const;

	// GetEdge( ):
	inline Edge GetEdge( int nIdx ) const;

	// operator=( ):
	inline Face &operator=( const Face &f );

	// operator==( ):
	inline bool operator==( const Face &f ) const;

	// operator[ ]( ):
	inline UINT16 &operator[ ]( int nPos );
	inline const UINT16 &operator[ ]( int nPos ) const;
};

/*---------------------------------
* Constructor:
-----------------------------------*/

Face::Face(UINT16 nVtx1, UINT16 nVtx2, UINT16 nVtx3 )
{
	Set( nVtx1, nVtx2, nVtx3 );
}

/*---------------------------------
* Set( ):
-----------------------------------*/

void Face::Set( const Face &f )
{
	Set( f.m_anVtx[ 0 ], f.m_anVtx[ 1 ], f.m_anVtx[ 2 ] );
}

void Face::Set(UINT16 nVtx1, UINT16 nVtx2, UINT16 nVtx3 )
{
	m_anVtx[ 0 ] = nVtx1;
	m_anVtx[ 1 ] = nVtx2;
	m_anVtx[ 2 ] = nVtx3;
}

/*---------------------------------
* SwapCull( ):
-----------------------------------*/

void Face::SwapCull( )
{
	std::swap( m_anVtx[0], m_anVtx[1] );
}

/*---------------------------------
* Contains( ):
-----------------------------------*/

bool Face::Contains(UINT16 nVtx) const
{
	return m_anVtx[ 0 ] == nVtx || m_anVtx[ 1 ] == nVtx || m_anVtx[ 2 ] == nVtx;
}
bool Face::Contains( const Edge &e ) const
{
	return Contains( e[ 0 ] ) && Contains( e[ 1 ] );
}

/*---------------------------------
* Replace( ):
-----------------------------------*/

void Face::Replace(UINT16 nOldVtx, UINT16 nNewVtx )
{
	for( int i = 0; i < 3; i++ )
	{
		if( m_anVtx[ i ] == nOldVtx ) m_anVtx[ i ] = nNewVtx;
	}
}

/*---------------------------------
* IndexOf( ):
-----------------------------------*/

int Face::IndexOf(UINT16 nVtx ) const
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

Edge Face::GetEdge( int nIdx ) const
{
	static const int anNextIdx[ 3 ] = { 1, 2, 0 };
	_ASSERT( nIdx >= 0 && nIdx <= 2 );
	return Edge( m_anVtx[ nIdx ], m_anVtx[ anNextIdx[ nIdx ] ] );
}

/*---------------------------------
* operator=( ):
-----------------------------------*/

Face &Face::operator=( const Face &f )
{
	Set( f.m_anVtx[ 0 ], f.m_anVtx[ 1 ], f.m_anVtx[ 2 ] );
	return *this;
}

/*---------------------------------
* operator==( ):
-----------------------------------*/

bool Face::operator==( const Face &f ) const
{
	return m_anVtx[ 0 ] == f.m_anVtx[ 0 ] && m_anVtx[ 1 ] == f.m_anVtx[ 1 ] && m_anVtx[ 2 ] == f.m_anVtx[ 2 ];
}

/*---------------------------------
* operator[ ]( ):
-----------------------------------*/

UINT16 &Face::operator[ ]( int nPos )
{
	return m_anVtx[ nPos ];
}

const UINT16 &Face::operator[ ]( int nPos ) const
{
	return m_anVtx[ nPos ];
}
