#pragma once

#include <wtypes.h>
#include "Misc.h"

template < class T > class BaseColorRgb;
template < class T, class U > void SetCastedColourData(BaseColorRgb<T> &pc, const BaseColorRgb<U> &c);

#pragma warning(disable:4201)

template < class T > class BaseColorRgb
{
public:
	T m_nB, m_nG, m_nR;

	// Constructors:
	inline BaseColorRgb( );
	inline BaseColorRgb( float fBr );
	inline BaseColorRgb( T nR, T nG, T nB );
	template < class U > inline BaseColorRgb( const BaseColorRgb< U > &c )
	{
		SetCastedColourData(*this, c);
	}

	// MakeFloat( ):
	static BaseColorRgb<T> MakeFloat(float fR, float fG, float fB){ return BaseColorRgb<T>(Bound<int>((int)(255.0f * fR), 0, 255), Bound<int>((int)(255.0f * fG), 0, 255), Bound<int>((int)(255.0f * fB), 0, 255)); }
	static BaseColorRgb<T> MakeBGR(unsigned int dw){ return BaseColorRgb<T>((unsigned char)dw, (unsigned char)(dw >> 8L), (unsigned char)(dw >> 16L)); }
	static BaseColorRgb<T> MakeRGB(unsigned int dw){ return BaseColorRgb<T>((unsigned char)(dw >> 16L), (unsigned char)(dw >> 8L), (unsigned char)dw); }
	static BaseColorRgb<T> White(){ return BaseColorRgb<T>(255, 255, 255); }
	static BaseColorRgb<T> Black(){ return BaseColorRgb<T>(0, 0, 0); }
	static BaseColorRgb<T> Grey(int g){ g = Bound<int>(g, 0, 255); return BaseColorRgb<T>((T)g, (T)g, (T)g); }

	// Set( ):
	void Set( T nR, T nG, T nB );

	// const operators
	BaseColorRgb<T> operator+() const { return *this; }
//	ZBaseColour<int> operator-() const { return ZBaseColour<int>(-((int)r), -((int)g), -((int)b)); }
	template < class U > BaseColorRgb<int> operator+(const BaseColorRgb<U> &c) const { return BaseColorRgb<int>((int)m_nR + (int)c.m_nR, (int)m_nG + (int)c.m_nG, (int)m_nB + (int)c.m_nB); }
	template < class U > BaseColorRgb<int> operator-(const BaseColorRgb<U> &c) const { return BaseColorRgb<int>((int)m_nR - (int)c.m_nR, (int)m_nG - (int)c.m_nG, (int)m_nB - (int)c.m_nB); }
	template < class U > BaseColorRgb<int> operator*(U n) const { return BaseColorRgb<int>((int)((int)m_nR * n), (int)((int)m_nG * n), (int)((int)m_nB * n)); }
	template < class U > BaseColorRgb<int> operator/(U n) const { return BaseColorRgb<int>((int)((int)m_nR / n), (int)((int)m_nG / n), (int)((int)m_nB / n)); }
	BaseColorRgb<T> operator>>(int n) const { return BaseColorRgb<T>(m_nR >> n, m_nG >> n, m_nB >> n); }
	BaseColorRgb<int> operator<<(int n) const { return BaseColorRgb<int>((int)m_nR << n, (int)m_nG << n, (int)m_nB << n); }
	template < class U > bool operator==(const BaseColorRgb<U> &c) const { return m_nR == c.m_nR && m_nG == c.m_nG && m_nB == c.m_nB; }
	template < class U > bool operator!=(const BaseColorRgb<U> &c) const { return !operator==(c); }
	template < class U > operator BaseColorRgb<U>() const { BaseColorRgb<unsigned char> c; SetCastedColourData(c, *this); return c; }

	uint32 GetBGR() const { BaseColorRgb<unsigned char> c = *this; return (((c.m_nB << 8) + c.m_nG) << 8) + c.m_nR; }
	uint32 GetRGB() const { BaseColorRgb<unsigned char> c = *this; return (((c.m_nR << 8) + c.m_nG) << 8) + c.m_nB; }
	uint32 GetBGRA() const { return (GetBGR() << 8) | 0xff; }
	uint32 GetARGB() const { return GetRGB() | 0xff000000; }
	COLORREF GetCOLORREF() const { return GetBGR(); }

	// operators
	template < class U > BaseColorRgb<T> &operator+=(const BaseColorRgb<U> &c){ return operator=(operator+(c)); }
	template < class U > BaseColorRgb<T> &operator-=(const BaseColorRgb<U> &c){ return operator=(operator-(c)); }
	template < class U > BaseColorRgb<T> &operator*=(U n){ return operator=(operator*(n)); }
	template < class U > BaseColorRgb<T> &operator/=(U n){ return operator=(operator/(n)); }
	BaseColorRgb<T> &operator>>=(int n){ return operator=(operator>>(n)); }
	BaseColorRgb<T> &operator<<=(int n){ return operator=(operator<<(n)); }
	template < class U > BaseColorRgb<T> &operator=(const BaseColorRgb<U> &c){ SetCastedColourData<T, U>(*this, c); return *this; }

	// other
	BaseColorRgb<T> Reverse(){ return BaseColorRgb<T>(m_nB, m_nG, m_nR); }
	BaseColorRgb<T> Inverse(){ return BaseColorRgb<T>(255 - m_nR, 255 - m_nG, 255 - m_nB); }
	void Invert(){ operator=(Inverse()); }
	int GetMaxComponent(){ return std::max(m_nR, std::max(m_nG, m_nB)); }
	int GetMinComponent(){ return std::min(m_nR, std::min(m_nG, m_nB)); }
	static BaseColorRgb<T> Blend(const BaseColorRgb<T> &c1, const BaseColorRgb<T> &c2, float fBlend){ return (BaseColorRgb<T>)((c1 * (1.0f - fBlend)) + (c2 * fBlend)); }
};
template < class T, class U > inline BaseColorRgb<T> operator*(U n, const BaseColorRgb<T> &c)
{
	return c.operator*(n);
}
template < class T, class U > inline void SetCastedColourData(BaseColorRgb<T> &pc, const BaseColorRgb<U> &c)
{
	pc.m_nR = c.m_nR;
	pc.m_nG = c.m_nG;
	pc.m_nB = c.m_nB;
}
template < > inline void SetCastedColourData(BaseColorRgb<unsigned char> &pc, const BaseColorRgb<int> &c)
{
	pc.m_nR = (unsigned char)Bound<int>(c.m_nR, 0, 255);
	pc.m_nG = (unsigned char)Bound<int>(c.m_nG, 0, 255);
	pc.m_nB = (unsigned char)Bound<int>(c.m_nB, 0, 255);
}

typedef BaseColorRgb<int> WideColorRgb;
typedef BaseColorRgb<unsigned char> ColorRgb;

/*---------------------------------
* Constructor:
-----------------------------------*/

template< class T > BaseColorRgb< T >::BaseColorRgb( )
{
}

template< class T > BaseColorRgb< T >::BaseColorRgb( float fBr )
{
	SetCastedColourData(*this, BaseColorRgb<T>::Grey((int)(Bound<float>(fBr, 0, 1) * 255.0f)));
}

template< class T > BaseColorRgb< T >::BaseColorRgb( T nR, T nG, T nB )
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

template< class T > void BaseColorRgb< T >::Set( T nR, T nG, T nB )
{
	m_nR = nR;
	m_nG = nG; 
	m_nB = nB;
}
