#pragma once

#include "Vector3.h"

class BezierCurve
{
private:
	int nPoints;
	Vector3 *pvPoint;
	int *pnBin;

public:
	BezierCurve();
	BezierCurve(int nPoints);
	~BezierCurve();

	void Create(int nPoints);
	void Clear();
	int Binomial(int nLevel, int nPos);
	Vector3 Calculate(float fPos);
	Vector3 &operator[](int nPos);
	Vector3 &Get(int nPos);
};

template < int nBez > class ContainedBezierCurve : public BezierCurve
{
protected:
	BezierCurve pBezier[nBez];
	
public:
	Vector3 vBound1, vBound2;
	ContainedBezierCurve(Vector3 vBound1 = Vector3::Origin(), Vector3 vBound2 = Vector3::Origin());
	Vector3 Calculate(float &fPos);
	virtual void GetNewPoints(BezierCurve &b); 
};

template < int nBez > ContainedBezierCurve<nBez>::ContainedBezierCurve(Vector3 vBound1, Vector3 vBound2)
{
	this->vBound1 = vBound1; 
	this->vBound2 = vBound2;
	for(int i = 0; i < nBez; i++)
	{
		pBezier[i].Create(4);
	}
	pBezier[nBez-1][2] = Vector3::Origin();
	pBezier[nBez-1][3] = Vector3::Origin();
	float fPos = nBez + 1.5;
	Calculate(fPos);
}
template < int nBez > Vector3 ContainedBezierCurve<nBez>::Calculate(float &fPos)
{
	while(fPos >= nBez)
	{
		for(int i = 0; i < nBez - 1; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				pBezier[i][j] = pBezier[i + 1][j];
			}
		}
		pBezier[nBez - 1][0] = pBezier[nBez - 1][3];
		pBezier[nBez - 1][1] = pBezier[nBez - 1][0] + (pBezier[nBez - 1][3] - pBezier[nBez - 1][2]);
		GetNewPoints(pBezier[nBez - 1]);
		fPos--;
	}
	float fUse = fPos;
	for(int i = 0;;fUse--)
	{
		_ASSERT(i < nBez);
		if(fUse <= 1) return pBezier[i].Calculate(fUse);
	}
}
template < int nBez > void ContainedBezierCurve<nBez>::GetNewPoints(BezierCurve &b)
{
	b[2].x = vBound1.x + ((float(rand()) / RAND_MAX) * (vBound2.x - vBound1.x));
	b[2].y = vBound1.y + ((float(rand()) / RAND_MAX) * (vBound2.y - vBound1.y));
	b[2].z = vBound1.z + ((float(rand()) / RAND_MAX) * (vBound2.z - vBound1.z));

	b[3].x = vBound1.x + ((float(rand()) / RAND_MAX) * (vBound2.x - vBound1.x));
	b[3].y = vBound1.y + ((float(rand()) / RAND_MAX) * (vBound2.y - vBound1.y));
	b[3].z = vBound1.z + ((float(rand()) / RAND_MAX) * (vBound2.z - vBound1.z));
}
