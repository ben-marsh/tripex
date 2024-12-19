#pragma once

#include "Misc.h"

class Edge
{
public:
	uint16 m_anVtx[ 2 ];

	// Constructor:
	inline Edge();
	inline Edge( const Edge &e );
	inline Edge( uint16 nVtx1, uint16 nVtx2 );

	// Set( ):
	inline void Set( const Edge &e );
	inline void Set(uint16 nVtx1, uint16 nVtx2 );

	// Flipped( ):
	inline Edge Flipped( ) const;

	// Ordered( ):
	inline Edge Ordered( ) const;

	// Contains( ):
	inline bool Contains(uint16 nVtx) const;

	// CommonIndex( ):
	inline uint16 CommonIndex( const Edge &e ) const;

	// operator=( ):
	inline Edge &operator=( const Edge &e );

	// operator==( ):
	inline bool operator==( const Edge &e ) const;

	// operator[ ]( ):
	inline uint16 &operator[ ]( int nIdx );
	inline const uint16 &operator[ ]( int nIdx ) const;
};

/*---------------------------------
* Constructor:
-----------------------------------*/

Edge::Edge( )
{
}

Edge::Edge( const Edge &e )
{
	Set(e);
}

Edge::Edge(uint16 nVtx1, uint16 nVtx2 )
{
	Set( nVtx1, nVtx2 );
}

/*---------------------------------
* Set( ):
-----------------------------------*/

void Edge::Set( const Edge &e )
{
	Set( e.m_anVtx[ 0 ], e.m_anVtx[ 1 ] );
}

void Edge::Set(uint16 nVtx1, uint16 nVtx2 )
{
	m_anVtx[ 0 ] = nVtx1;
	m_anVtx[ 1 ] = nVtx2;
}

/*---------------------------------
* Flipped( ):
-----------------------------------*/

Edge Edge::Flipped( ) const
{
	return Edge( m_anVtx[ 1 ], m_anVtx[ 0 ] );
}

/*---------------------------------
* Ordered( ):
-----------------------------------*/

Edge Edge::Ordered( ) const
{
	if( m_anVtx[ 0 ] < m_anVtx[ 1 ] )
	{
		return Edge( m_anVtx[ 0 ], m_anVtx[ 1 ] );
	}
	else 
	{
		return Edge( m_anVtx[ 1 ], m_anVtx[ 0 ] );
	}
}

/*---------------------------------
* Contains( ):
-----------------------------------*/

bool Edge::Contains(uint16 nVtx ) const
{
	return m_anVtx[ 0 ] == nVtx || m_anVtx[ 1 ] == nVtx;
}

/*---------------------------------
* CommonIndex( ):
-----------------------------------*/

uint16 Edge::CommonIndex( const Edge &e ) const
{
	if( e.Contains( m_anVtx[ 0 ] ) )
	{
		return m_anVtx[ 0 ];
	}
	else if( e.Contains( m_anVtx[ 1 ] ) )
	{
		return m_anVtx[ 1 ];
	}
	return (uint16)-1;
}


/*---------------------------------
* operator=( ):
-----------------------------------*/

Edge &Edge::operator=( const Edge &e )
{
	Set( e );
	return *this;
}

/*---------------------------------
* operator==( ):
-----------------------------------*/

bool Edge::operator==( const Edge &e ) const
{
	return m_anVtx[ 0 ] == e.m_anVtx[ 0 ] && m_anVtx[ 1 ] == e.m_anVtx[ 1 ];
}

/*---------------------------------
* operator[ ]( ):
-----------------------------------*/

uint16&Edge::operator[ ]( int nIdx )
{
	return m_anVtx[ nIdx ];
}

const uint16&Edge::operator[ ]( int nIdx ) const
{
	return m_anVtx[ nIdx ];
}
