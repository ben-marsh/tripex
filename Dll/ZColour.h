#pragma once
#include <H:\Microsoft Xbox SDK\xbox\include\stdlib.h>
//#include <wtypes.h>
#include "Misc.h"

template < class T > class ZBaseColour;
template < class T, class U > void SetCastedColourData(ZBaseColour<T> &pc, const ZBaseColour<U> &c);

#pragma warning(disable:4201)

template < class T > class ZBaseColour
{
public:
	union
	{
		struct
		{
			T m_nB, m_nG, m_nR;
		};
		DWORD dw;
	};

	// Constructors:
	inline ZBaseColour( );
	inline ZBaseColour( FLOAT32 fBr );
	inline ZBaseColour( T nR, T nG, T nB );
	template < class U > inline ZBaseColour( const ZBaseColour< U > &c )
	{
		SetCastedColourData(*this, c);
	}

	// MakeFloat( ):
	static ZBaseColour<T> MakeFloat(float fR, float fG, float fB){ return ZBaseColour<T>(Bound<int>((int)(255.0f * fR), 0, 255), Bound<int>((int)(255.0f * fG), 0, 255), Bound<int>((int)(255.0f * fB), 0, 255)); }
	static ZBaseColour<T> MakeBGR(DWORD dw){ return ZBaseColour<T>((unsigned char)dw, (unsigned char)(dw >> 8L), (unsigned char)(dw >> 16L)); }
	static ZBaseColour<T> MakeRGB(DWORD dw){ return ZBaseColour<T>((unsigned char)(dw >> 16L), (unsigned char)(dw >> 8L), (unsigned char)dw); }
	static ZBaseColour<T> White(){ return ZBaseColour<T>(255, 255, 255); }
	static ZBaseColour<T> Black(){ return ZBaseColour<T>(0, 0, 0); }
	static ZBaseColour<T> Grey(int g){ g = Bound<int>(g, 0, 255); return ZBaseColour<T>((T)g, (T)g, (T)g); }

	// Set( ):
	void Set( T nR, T nG, T nB );

	// const operators
	ZBaseColour<T> operator+() const { return *this; }
	ZBaseColour<int> operator-() const { return ZBaseColour<int>(-((int)r), -((int)g), -((int)b)); }
	template < class U > ZBaseColour<int> operator+(const ZBaseColour<U> &c) const { return ZBaseColour<int>((int)m_nR + (int)c.m_nR, (int)m_nG + (int)c.m_nG, (int)m_nB + (int)c.m_nB); }
	template < class U > ZBaseColour<int> operator-(const ZBaseColour<U> &c) const { return ZBaseColour<int>((int)m_nR - (int)c.m_nR, (int)m_nG - (int)c.m_nG, (int)m_nB - (int)c.m_nB); }
	template < class U > ZBaseColour<int> operator*(U n) const { return ZBaseColour<int>((int)((int)m_nR * n), (int)((int)m_nG * n), (int)((int)m_nB * n)); }
	template < class U > ZBaseColour<int> operator/(U n) const { return ZBaseColour<int>((int)((int)m_nR / n), (int)((int)m_nG / n), (int)((int)m_nB / n)); }
	ZBaseColour<T> operator>>(int n) const { return ZBaseColour<T>(m_nR >> n, m_nG >> n, m_nB >> n); }
	ZBaseColour<int> operator<<(int n) const { return ZBaseColour<int>((int)m_nR << n, (int)m_nG << n, (int)m_nB << n); }
	template < class U > bool operator==(const ZBaseColour<U> &c) const { return m_nR == c.m_nR && m_nG == c.m_nG && m_nB == c.m_nB; }
	template < class U > bool operator!=(const ZBaseColour<U> &c) const { return !operator==(c); }
	template < class U > operator ZBaseColour<U>() const { ZBaseColour<unsigned char> c; SetCastedColourData(c, *this); return c; }

	DWORD GetBGR() const { ZBaseColour<unsigned char> c = *this; return (((c.m_nB << 8) + c.m_nG) << 8) + c.m_nR; }
	DWORD GetRGB() const { ZBaseColour<unsigned char> c = *this; return (((c.m_nR << 8) + c.m_nG) << 8) + c.m_nB; }
	DWORD GetBGRA() const { return (GetBGR() << 8) | 0xff; }
	DWORD GetARGB() const { return GetRGB() | 0xff000000; }
	COLORREF GetCOLORREF() const { return GetBGR(); }

	// operators
	template < class U > ZBaseColour<T> &operator+=(const ZBaseColour<U> &c){ return operator=(operator+(c)); }
	template < class U > ZBaseColour<T> &operator-=(const ZBaseColour<U> &c){ return operator=(operator-(c)); }
	template < class U > ZBaseColour<T> &operator*=(U n){ return operator=(operator*(n)); }
	template < class U > ZBaseColour<T> &operator/=(U n){ return operator=(operator/(n)); }
	ZBaseColour<T> &operator>>=(int n){ return operator=(operator>>(n)); }
	ZBaseColour<T> &operator<<=(int n){ return operator=(operator<<(n)); }
	template < class U > ZBaseColour<T> &operator=(const ZBaseColour<U> &c){ SetCastedColourData<T, U>(*this, c); return *this; }

	// other
	ZBaseColour<T> Reverse(){ return ZBaseColour<T>(m_nB, m_nG, m_nR); }
	ZBaseColour<T> Inverse(){ return ZBaseColour<T>(255 - m_nR, 255 - m_nG, 255 - m_nB); }
	void Invert(){ operator=(Inverse()); }
	int GetMaxComponent(){ return max(m_nR, max(m_nG, m_nB)); }
	int GetMinComponent(){ return min(m_nR, min(m_nG, m_nB)); }
	static ZBaseColour<T> Blend(const ZBaseColour<T> &c1, const ZBaseColour<T> &c2, float fBlend){ return (ZBaseColour<T>)((c1 * (1.0f - fBlend)) + (c2 * fBlend)); }
	class ZColourHSV ToHSV(float fDefH = 0.0, float fDefS = 0.0) const;
};
template < class T, class U > inline ZBaseColour<T> operator*(U n, const ZBaseColour<T> &c)
{
	return c.operator*(n);
}
template < class T, class U > inline void SetCastedColourData(ZBaseColour<T> &pc, const ZBaseColour<U> &c)
{
	pc.m_nR = c.m_nR;
	pc.m_nG = c.m_nG;
	pc.m_nB = c.m_nB;
}
template < > inline void SetCastedColourData(ZBaseColour<unsigned char> &pc, const ZBaseColour<int> &c)
{
	pc.m_nR = (unsigned char)Bound<int>(c.m_nR, 0, 255);
	pc.m_nG = (unsigned char)Bound<int>(c.m_nG, 0, 255);
	pc.m_nB = (unsigned char)Bound<int>(c.m_nB, 0, 255);
}

typedef ZBaseColour<int> ZWideColour;
typedef ZBaseColour<unsigned char> ZColour;

/*---------------------------------
* Constructor:
-----------------------------------*/

template< class T > ZBaseColour< T >::ZBaseColour( )
{
}

template< class T > ZBaseColour< T >::ZBaseColour( float fBr )
{
	SetCastedColourData(*this, ZBaseColour<T>::Grey((int)(Bound<float>(fBr, 0, 1) * 255.0f)));
}

template< class T > ZBaseColour< T >::ZBaseColour( T nR, T nG, T nB )
{
	Set(nR, nG, nB);
}

//template< class T > ZBaseColour< T >::template < class U > ZBaseColour( const ZBaseColour<U> &c )
//{
//	SetCastedColourData(*this, c);
//}

/*---------------------------------
* Set( )
-----------------------------------*/

template< class T > void ZBaseColour< T >::Set( T nR, T nG, T nB )
{
	m_nR = nR;
	m_nG = nG; 
	m_nB = nB;
}
