#pragma once

#include "Point.h"

template<class T> class Rect
{
public:
	T left, top, right, bottom;

	Rect() {}
	Rect(T nX, T nY, T nS) { this->Set(nX, nY, nS); }
	Rect(T nX, T nY, T nW, T nH) { this->Set(nX, nY, nW, nH); }
	Rect(const Rect<T>& r) { this->Set(r); }
	Rect(const Point<T>& p1, const Point<T>& p2) { this->Set(p1, p2); }
	Rect(const Point<T>& p1, int nW, int nH) { this->Set(p1, nW, nH); }

	void Set(int nX, int nY, int nS){ Set(nX, nY, nS, nS); }
	void Set(int nX, int nY, int nW, int nH){ Set(Point<T>(nX, nY), Point<T>(nX + nW, nY + nH)); }
	void Set(const Rect<T> &r){ left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
	void Set(const Point<T> &p1, const Point<T> &p2){ left = p1.x; top = p1.y; right = p2.x; bottom = p2.y; }
	void Set(const Point<T> &p1, int nW, int nH){ Set(p1, Point<T>(p1.x + nW, p1.y + nH)); }

	Rect<T> Validated(){ return Rect<T>(Point<T>(std::min(left, right), std::min(top, bottom)), Point<T>(std::max(left, right), std::max(top, bottom))); }
	void Validate(){ operator=(Validated()); }

	void Move(int nX, int nY){ MoveX(nX); MoveY(nY); }
	void MoveX(int nX){ left += nX; right += nX; }
	void MoveY(int nY){ top += nY; bottom += nY; }

	void SetTopLeft(const Point<T>&p) const { operator+=(Point<T>(p) - GetTopLeft()); }
	void SetTopRight(const Point<T>&p) const { operator+=(Point<T>(p) - GetTopRight()); }
	void SetBotLeft(const Point<T>&p) const { operator+=(Point<T>(p) - GetBotLeft()); }
	void SetBotRight(const Point<T>&p) const { operator+=(Point<T>(p) - GetBotRight()); }
	void SetCenter(const Point<T>&p) const { operator+=(Point<T>(p) - GetCenter()); }

	Point<T> *GetTopLeftPtr(){ return &((Point<T>*)this)[0]; }
	Point<T> *GetBotRightPtr(){ return &((Point<T>*)this)[1]; }

	Point<T> GetTopLeft() const { return Point<T>(left, top); }
	Point<T> GetTopRight() const { return Point<T>(right, top); }
	Point<T> GetBotLeft() const { return Point<T>(left, bottom); }
	Point<T> GetBotRight() const { return Point<T>(right, bottom); }
	Point<T> GetCenter() const { return Point<T>(GetCenterX(), GetCenterY()); }

	T GetWidth() const { return right - left; }
	T GetHeight() const { return bottom - top; }
	T GetCenterX() const { return (right + left) / 2; }
	T GetCenterY() const { return (bottom + top) / 2; }

	bool IsEmpty() const { return bottom == top || right == left; }
	bool IsValid() const { return bottom > top && right > left; }
	bool Contains(const Point<T> &p) const { return p.x >= left && p.x < right && p.y >= top && p.y < bottom; }
	bool Intersects(const Rect<T> &r) const { return GetIntersection(r).IsValid(); }
	Rect<T> GetIntersection(const Rect<T> &r) const { return Rect<T>(Point<T>(std::max(left, r.left), std::max(top, r.top)), Point<T>(std::min(right, r.right), std::min(bottom, r.bottom))); }

	Rect<T> &operator=(const Rect<T> &r){ Set(r); return *this; }
	template < class W > operator Rect<W>(){ return Rect<W>((Point<W>)GetTopLeft(), (Point<W>)GetBotRight()); }
	template < class W > bool operator==(const Rect<W> &r){ return left == r.left && right == r.right && top == r.top && bottom == r.bottom; }
	template < class W > bool operator!=(const Rect<W> &r){ return !operator==(r); }
	Rect<T> operator+(const Point<T> &p) const { return Rect<T>(GetTopLeft() + p, GetBotRight() + p); }
	Rect<T> operator-(const Point<T> &p) const { return Rect<T>(GetTopLeft() - p, GetBotRight() - p); }
	Rect<T> &operator+=(const Point<T> &p){ operator=(operator+(p)); }
	Rect<T> &operator-=(const Point<T> &p){ operator=(operator-(p)); }
};
