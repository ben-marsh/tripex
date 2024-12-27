#pragma once

#include "Platform.h"
#include <wtypes.h>
#include "Misc.h"

template < class T > class BaseColorRgb;
template < class T, class U > void SetCastedColourData(BaseColorRgb<T> &pc, const BaseColorRgb<U> &c);

#pragma warning(disable:4201)

template < class T > class BaseColorRgb
{
public:
	union
	{
		struct
		{
			T b, g, r;
		};
		DWORD dw; // Need this to be at least 32 bits for ColorRgb
	};

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
	template < class U > BaseColorRgb<int> operator+(const BaseColorRgb<U> &c) const { return BaseColorRgb<int>((int)r + (int)c.r, (int)g + (int)c.g, (int)b + (int)c.b); }
	template < class U > BaseColorRgb<int> operator-(const BaseColorRgb<U> &c) const { return BaseColorRgb<int>((int)r - (int)c.r, (int)g - (int)c.g, (int)b - (int)c.b); }
	template < class U > BaseColorRgb<int> operator*(U n) const { return BaseColorRgb<int>((int)((int)r * n), (int)((int)g * n), (int)((int)b * n)); }
	template < class U > BaseColorRgb<int> operator/(U n) const { return BaseColorRgb<int>((int)((int)r / n), (int)((int)g / n), (int)((int)b / n)); }
	BaseColorRgb<T> operator>>(int n) const { return BaseColorRgb<T>(r >> n, g >> n, b >> n); }
	BaseColorRgb<int> operator<<(int n) const { return BaseColorRgb<int>((int)r << n, (int)g << n, (int)b << n); }
	template < class U > bool operator==(const BaseColorRgb<U> &c) const { return r == c.r && g == c.g && b == c.b; }
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
	BaseColorRgb<T> Reverse() const { return BaseColorRgb<T>(b, g, r); }
	BaseColorRgb<T> Inverse() const { return BaseColorRgb<T>(255 - r, 255 - g, 255 - b); }
	void Invert(){ operator=(Inverse()); }
	int GetMaxComponent() const { return std::max(r, std::max(g, b)); }
	int GetMinComponent() const { return std::min(r, std::min(g, b)); }
	static BaseColorRgb<T> Blend(const BaseColorRgb<T> &c1, const BaseColorRgb<T> &c2, float fBlend){ return (BaseColorRgb<T>)((c1 * (1.0f - fBlend)) + (c2 * fBlend)); }
};
template < class T, class U > inline BaseColorRgb<T> operator*(U n, const BaseColorRgb<T> &c)
{
	return c.operator*(n);
}
template < class T, class U > inline void SetCastedColourData(BaseColorRgb<T> &pc, const BaseColorRgb<U> &c)
{
	pc.r = c.r;
	pc.g = c.g;
	pc.b = c.b;
}
template < > inline void SetCastedColourData(BaseColorRgb<unsigned char> &pc, const BaseColorRgb<int> &c)
{
	pc.r = (unsigned char)Bound<int>(c.r, 0, 255);
	pc.g = (unsigned char)Bound<int>(c.g, 0, 255);
	pc.b = (unsigned char)Bound<int>(c.b, 0, 255);
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
	r = nR;
	g = nG; 
	b = nB;
}
