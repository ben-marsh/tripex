#pragma once

template<class T> class Point
{
public:
	T x, y;

	Point() : x(), y() {}
	Point(T nX, T nY) { this->Set(nX, nY); }
	Point(const Point<T>& p) { this->Set(p); }

	void Set(const Point<T>& other) { Set(other.x, other.y); }
	void Set(T nX, T nY) { this->x = nX; this->y = nY; }

	Point<T> operator-() const { return Point<T>(-x, -y); }
	Point<T> operator+() const { return Point<T>(+x, +y); }
	Point<T> operator-(const Point<T>& p) const { return Point<T>(x - p.x, y - p.y); }
	Point<T> operator+(const Point<T>& p) const { return Point<T>(x + p.x, y + p.y); }
	Point<T>& operator-=(const Point<T>& p) { x -= p.x; y -= p.y; return *this; }
	Point<T>& operator+=(const Point<T>& p) { x += p.x; y += p.y; return *this; }
	Point<T>& operator=(const Point<T>& p) { Set(p); return *this; }

	bool operator==(const Point<T>& p) const { return x == p.x && y == p.y; }
	bool operator!=(const Point<T>& p) const { return !operator==(p); }

	template < class U > operator Point<U>() { return Point<U>((U)x, (U)y); }
};
