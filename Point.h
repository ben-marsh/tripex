#pragma once

template < class T > class ZPoint;

template < class T > class ZPointBase
{
public:
	T x, y;
};
template < class T, class U > class ZPointTemplate : public U
{
public:
	using U::x;
	using U::y;

	void Set(const U &p){ Set(p.x, p.y); }
	void Set(T nX, T nY){ this->x = nX; this->y = nY; }

	ZPoint<T> operator-() const { return ZPoint<T>(-x, -y); }
	ZPoint<T> operator+() const { return ZPoint<T>(+x, +y); }
	ZPoint<T> operator-(const U &p) const { return ZPoint<T>(x - p.x, y - p.y); }
	ZPoint<T> operator+(const U &p) const { return ZPoint<T>(x + p.x, y + p.y); }
	ZPointTemplate<T, U> &operator-=(const U &p){ x -= p.x; y -= p.y; return *this; }
	ZPointTemplate<T, U> &operator+=(const U &p){ x += p.x; y += p.y; return *this; }
	ZPointTemplate<T, U> &operator=(const U &p){ Set(p); return *this; }

	bool operator==(const U &p) const { return x == p.x && y == p.y; }
	bool operator!=(const U &p) const { return !operator==(p); }

	template < class U > operator ZPoint<U>(){ return ZPoint<U>((U)x, (U)y); }
};
template < class T > class ZPoint : public ZPointTemplate< T, ZPointBase< T > >
{
public:
	ZPoint(){ }
	ZPoint(T nX, T nY){ this->Set(nX, nY); }
	ZPoint(const ZPoint<T> &p){ this->Set(p); }
};
template < > class ZPoint< int > : public ZPointTemplate< int, POINT >
{
public:
	ZPoint(){ }
	ZPoint(int nX, int nY){ Set(nX, nY); }
	ZPoint(const POINT &p){ Set(p.x, p.y); }
	ZPoint(const ZPoint<int> &p){ Set(p); }
};
