#pragma once

#include "Point.h"

template < class T > class ZRect;

template < class T > class ZRectBase
{
public:
	T left, top, right, bottom;
};
template < class T, class R, class P > class ZRectTemplate : public R
{
public:
	using R::left;
	using R::top;
	using R::right;
	using R::bottom;

	void Set(int nX, int nY, int nS){ Set(nX, nY, nS, nS); }
	void Set(int nX, int nY, int nW, int nH){ Set(ZPoint<T>(nX, nY), ZPoint<T>(nX + nW, nY + nH)); }
	void Set(const R &r){ left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
	void Set(const P &p1, const P &p2){ left = p1.x; top = p1.y; right = p2.x; bottom = p2.y; }
	void Set(const P &p1, int nW, int nH){ Set(p1, ZPoint<T>(p1.x + nW, p1.y + nH)); }

	ZRect<T> Validated(){ return ZRect<T>(ZPoint<T>(std::min(left, right), std::min(top, bottom)), ZPoint<T>(std::max(left, right), std::max(top, bottom))); }
	void Validate(){ operator=(Validated()); }

	void Move(int nX, int nY){ MoveX(nX); MoveY(nY); }
	void MoveX(int nX){ left += nX; right += nX; }
	void MoveY(int nY){ top += nY; bottom += nY; }

	void SetTopLeft(const P &p) const { operator+=(ZPoint<T>(p) - GetTopLeft()); }
	void SetTopRight(const P &p) const { operator+=(ZPoint<T>(p) - GetTopRight()); }
	void SetBotLeft(const P &p) const { operator+=(ZPoint<T>(p) - GetBotLeft()); }
	void SetBotRight(const P &p) const { operator+=(ZPoint<T>(p) - GetBotRight()); }
	void SetCenter(const P &p) const { operator+=(ZPoint<T>(p) - GetCenter()); }

	P *GetTopLeftPtr(){ return &((P*)(R*)this)[0]; }
	P *GetBotRightPtr(){ return &((P*)(R*)this)[1]; }

	ZPoint<T> GetTopLeft() const { return ZPoint<T>(left, top); }
	ZPoint<T> GetTopRight() const { return ZPoint<T>(right, top); }
	ZPoint<T> GetBotLeft() const { return ZPoint<T>(left, bottom); }
	ZPoint<T> GetBotRight() const { return ZPoint<T>(right, bottom); }
	ZPoint<T> GetCenter() const { return ZPoint<T>(GetCenterX(), GetCenterY()); }

	T GetWidth() const { return right - left; }
	T GetHeight() const { return bottom - top; }
	T GetCenterX() const { return (right + left) / 2; }
	T GetCenterY() const { return (bottom + top) / 2; }

	bool IsEmpty() const { return bottom == top || right == left; }
	bool IsValid() const { return bottom > top && right > left; }
	bool Contains(const P &p) const { return p.x >= left && p.x < right && p.y >= top && p.y < bottom; }
	bool Intersects(const R &r) const { return GetIntersection(r).IsValid(); }
	ZRect<T> GetIntersection(const R &r) const { return ZRect<T>(ZPoint<T>(std::max(left, r.left), std::max(top, r.top)), ZPoint<T>(std::min(right, r.right), std::min(bottom, r.bottom))); }

	ZRectTemplate<T, R, P> &operator=(const R &r){ Set(r); return *this; }
	template < class W > operator ZRect<W>(){ return ZRect<W>((ZPoint<W>)GetTopLeft(), (ZPoint<W>)GetBotRight()); }
	template < class W > bool operator==(const ZRect<W> &r){ return left == r.left && right == r.right && top == r.top && bottom == r.bottom; }
	template < class W > bool operator!=(const ZRect<W> &r){ return !operator==(r); }
	ZRect<T> operator+(const P &p) const { return ZRect<T>(GetTopLeft() + p, GetBotRight() + p); }
	ZRect<T> operator-(const P &p) const { return ZRect<T>(GetTopLeft() - p, GetBotRight() - p); }
	ZRect<T> &operator+=(const P &p){ operator=(operator+(p)); }
	ZRect<T> &operator-=(const P &p){ operator=(operator-(p)); }
};
template < class T > class ZRect : public ZRectTemplate < T, ZRectBase< T >, ZPointBase< T > >
{
public:
	ZRect(){ }
	ZRect(T nX, T nY, T nS){ Set(nX, nY, nS); }
	ZRect(T nX, T nY, T nW, T nH){ Set(nX, nY, nW, nH); }
	ZRect(const ZRect<T> &r){ Set(r); }
	ZRect(const ZPoint<T> &p1, const ZPoint<T> &p2){ Set(p1, p2); }
	ZRect(const ZPoint<T> &p1, int nW, int nH){ Set(p1, nW, nH); }
};
template <> class ZRect<int> : public ZRectTemplate< int, RECT, POINT >
{
public:
	ZRect(){ }
	ZRect(int nX, int nY, int nS){ Set(nX, nY, nS); }
	ZRect(int nX, int nY, int nW, int nH){ Set(nX, nY, nW, nH); }
	ZRect(const RECT &r){ Set(r); }
	ZRect(const POINT &p1, const POINT &p2){ Set(p1, p2); }
	ZRect(const POINT &p1, int nW, int nH){ Set(p1, nW, nH); }
};
