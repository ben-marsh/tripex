#pragma once

#include "Edge.h"

struct Face
{
	UINT16 m_anVtx[3];

	// Constructors:
	Face() = default;
	Face(UINT16 nVtx1, UINT16 nVtx2, UINT16 nVtx3);

	// Set( ):
	void Set(const Face& f);
	void Set(UINT16 nVtx1, UINT16 nVtx2, UINT16 nVtx3);

	// SwapOrder( ):
	void SwapCull();

	// Contains( ):
	bool Contains(UINT16 nVtx) const;
	bool Contains(const Edge& e) const;

	// Replace( ):
	void Replace(UINT16 nOldVtx, UINT16 nNewVtx);

	// IndexOf( ):
	int IndexOf(UINT16 nVtx) const;

	// GetEdge( ):
	Edge GetEdge(int nIdx) const;

	// operator=( ):
	Face& operator=(const Face& f);

	// operator==( ):
	bool operator==(const Face& f) const;

	// operator[ ]( ):
	UINT16& operator[ ](int nPos);
	const UINT16& operator[ ](int nPos) const;
};

/*---------------------------------
* Constructor:
-----------------------------------*/

inline Face::Face(UINT16 nVtx1, UINT16 nVtx2, UINT16 nVtx3)
{
	Set(nVtx1, nVtx2, nVtx3);
}

/*---------------------------------
* Set( ):
-----------------------------------*/

inline void Face::Set(const Face& f)
{
	Set(f.m_anVtx[0], f.m_anVtx[1], f.m_anVtx[2]);
}

inline void Face::Set(UINT16 nVtx1, UINT16 nVtx2, UINT16 nVtx3)
{
	m_anVtx[0] = nVtx1;
	m_anVtx[1] = nVtx2;
	m_anVtx[2] = nVtx3;
}

/*---------------------------------
* SwapCull( ):
-----------------------------------*/

inline void Face::SwapCull()
{
	std::swap(m_anVtx[0], m_anVtx[1]);
}

/*---------------------------------
* Contains( ):
-----------------------------------*/

inline bool Face::Contains(UINT16 nVtx) const
{
	return m_anVtx[0] == nVtx || m_anVtx[1] == nVtx || m_anVtx[2] == nVtx;
}
inline bool Face::Contains(const Edge& e) const
{
	return Contains(e[0]) && Contains(e[1]);
}

/*---------------------------------
* Replace( ):
-----------------------------------*/

inline void Face::Replace(UINT16 nOldVtx, UINT16 nNewVtx)
{
	for (int i = 0; i < 3; i++)
	{
		if (m_anVtx[i] == nOldVtx) m_anVtx[i] = nNewVtx;
	}
}

/*---------------------------------
* IndexOf( ):
-----------------------------------*/

inline int Face::IndexOf(UINT16 nVtx) const
{
	for (int i = 0; i < 3; i++)
	{
		if (m_anVtx[i] == nVtx) return i;
	}
	return -1;
}

/*---------------------------------
* GetEdge( ):
-----------------------------------*/

inline Edge Face::GetEdge(int nIdx) const
{
	static const int anNextIdx[3] = { 1, 2, 0 };
	_ASSERT(nIdx >= 0 && nIdx <= 2);
	return Edge(m_anVtx[nIdx], m_anVtx[anNextIdx[nIdx]]);
}

/*---------------------------------
* operator=( ):
-----------------------------------*/

inline Face& Face::operator=(const Face& f)
{
	Set(f.m_anVtx[0], f.m_anVtx[1], f.m_anVtx[2]);
	return *this;
}

/*---------------------------------
* operator==( ):
-----------------------------------*/

inline bool Face::operator==(const Face& f) const
{
	return m_anVtx[0] == f.m_anVtx[0] && m_anVtx[1] == f.m_anVtx[1] && m_anVtx[2] == f.m_anVtx[2];
}

/*---------------------------------
* operator[ ]( ):
-----------------------------------*/

inline UINT16& Face::operator[ ](int nPos)
{
	return m_anVtx[nPos];
}

inline const UINT16& Face::operator[ ](int nPos) const
{
	return m_anVtx[nPos];
}
