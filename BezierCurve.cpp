#include "Platform.h"
#include "BezierCurve.h"

BezierCurve::BezierCurve()
{
	nPoints = 0;
	pvPoint = NULL;
	pnBin = NULL;
}
BezierCurve::BezierCurve(int nPoints)
{
	this->nPoints = 0;
	pvPoint = NULL;
	pnBin = NULL;
	Create(nPoints);
}
BezierCurve::~BezierCurve()
{
	Clear();
}
void BezierCurve::Create(int nPoints)
{
	Clear();
	if(nPoints >= 1)
	{
		this->nPoints = nPoints;
		pvPoint = new Vector3[nPoints];
		pnBin = new int[nPoints];
		for(int i = 0; i < nPoints; i++)
		{
			pnBin[i] = Binomial(nPoints - 1, i);
		}
	}
}
void BezierCurve::Clear()
{
	if(pnBin != NULL)
	{
		delete pnBin;
		pnBin = NULL;
	}
	if(pvPoint != NULL)
	{
		delete pvPoint;
		pvPoint= NULL;
	}
	nPoints = 0;
}
Vector3 &BezierCurve::Get(int nPos)
{
	_ASSERT(nPos >= 0 && nPos < nPoints);
	return pvPoint[nPos];
}
Vector3 &BezierCurve::operator[](int nPos)
{
	return Get(nPos);
}
int BezierCurve::Binomial(int nLevel, int nNum)
{
	_ASSERT(nNum >= 0 && nNum <= nLevel && nLevel >= 0);
	if(nNum == 0 || nNum == nLevel) return 1;
	else return Binomial(nLevel - 1, nNum - 1) + Binomial(nLevel - 1, nNum);
}
Vector3 BezierCurve::Calculate(float fPos)
{
	_ASSERT(fPos >= 0 && fPos <= 1);
	Vector3 v(0, 0, 0);
	for(int i = 0; i < nPoints; i++)
	{
		v += (float)(pnBin[i] * pow(fPos, i) * pow(1 - fPos, (nPoints - 1) - i)) * pvPoint[i];
	}
	return v;
}

