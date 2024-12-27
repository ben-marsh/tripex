#pragma once

#include <vector>
#include "Vector3.h"

class BezierCurve
{
public:
	BezierCurve();
	BezierCurve(int num_points);
	~BezierCurve();

	void Create(int num_points);
	void Clear();
	Vector3 Calculate(float pos);
	Vector3& operator[](int index);
	Vector3& GetPoint(int index);

private:
	std::vector<Vector3> points;
	std::vector<int> coefficients;

	int Binomial(int level, int index);
};

template<int num_curves> class ContainedBezierCurve : public BezierCurve
{
protected:
	BezierCurve curves[num_curves];

public:
	Vector3 bound1, bound2;
	ContainedBezierCurve(Vector3 bound1 = Vector3::Origin(), Vector3 bound2 = Vector3::Origin());
	Vector3 Calculate(float& fPos);
	virtual void GetNewPoints(BezierCurve& b);
};

template<int num_curves> ContainedBezierCurve<num_curves>::ContainedBezierCurve(Vector3 bound1, Vector3 bound2)
{
	this->bound1 = bound1;
	this->bound2 = bound2;
	for (int i = 0; i < num_curves; i++)
	{
		curves[i].Create(4);
	}
	curves[num_curves - 1][2] = Vector3::Origin();
	curves[num_curves - 1][3] = Vector3::Origin();

	float fPos = num_curves + 1.5;
	Calculate(fPos);
}
template<int num_curves> Vector3 ContainedBezierCurve<num_curves>::Calculate(float& pos)
{
	while (pos >= num_curves)
	{
		for (int i = 0; i < num_curves - 1; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				curves[i][j] = curves[i + 1][j];
			}
		}
		curves[num_curves - 1][0] = curves[num_curves - 1][3];
		curves[num_curves - 1][1] = curves[num_curves - 1][0] + (curves[num_curves - 1][3] - curves[num_curves - 1][2]);
		GetNewPoints(curves[num_curves - 1]);
		pos--;
	}

	float use = pos;
	for (int i = 0;; use--)
	{
		assert(i < num_curves);
		if (use <= 1) return curves[i].Calculate(use);
	}
}
template<int num_curves> void ContainedBezierCurve<num_curves>::GetNewPoints(BezierCurve& b)
{
	b[2].x = bound1.x + ((float(rand()) / RAND_MAX) * (bound2.x - bound1.x));
	b[2].y = bound1.y + ((float(rand()) / RAND_MAX) * (bound2.y - bound1.y));
	b[2].z = bound1.z + ((float(rand()) / RAND_MAX) * (bound2.z - bound1.z));

	b[3].x = bound1.x + ((float(rand()) / RAND_MAX) * (bound2.x - bound1.x));
	b[3].y = bound1.y + ((float(rand()) / RAND_MAX) * (bound2.y - bound1.y));
	b[3].z = bound1.z + ((float(rand()) / RAND_MAX) * (bound2.z - bound1.z));
}
