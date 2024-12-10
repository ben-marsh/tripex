#include "StdAfx.h"
#include "ZBezier.h"

ZBezier::ZBezier()
{
	nPoints = 0;
	pvPoint = NULL;
	pnBin = NULL;
}
ZBezier::ZBezier(int nPoints)
{
	this->nPoints = 0;
	pvPoint = NULL;
	pnBin = NULL;
	Create(nPoints);
}
ZBezier::~ZBezier()
{
	Clear();
}
void ZBezier::Create(int nPoints)
{
	Clear();
	if(nPoints >= 1)
	{
		this->nPoints = nPoints;
		pvPoint = new ZVector[nPoints];
		pnBin = new int[nPoints];
		for(int i = 0; i < nPoints; i++)
		{
			pnBin[i] = Binomial(nPoints - 1, i);
		}
	}
}
void ZBezier::Clear()
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
ZVector &ZBezier::Get(int nPos)
{
	_ASSERT(nPos >= 0 && nPos < nPoints);
	return pvPoint[nPos];
}
ZVector &ZBezier::operator[](int nPos)
{
	return Get(nPos);
}
int ZBezier::Binomial(int nLevel, int nNum)
{
	_ASSERT(nNum >= 0 && nNum <= nLevel && nLevel >= 0);
	if(nNum == 0 || nNum == nLevel) return 1;
	else return Binomial(nLevel - 1, nNum - 1) + Binomial(nLevel - 1, nNum);
}
ZVector ZBezier::Calculate(float fPos)
{
	_ASSERT(fPos >= 0 && fPos <= 1);
	ZVector v(0, 0, 0);
	for(int i = 0; i < nPoints; i++)
	{
		v += (float)(pnBin[i] * pow(fPos, i) * pow(1 - fPos, (nPoints - 1) - i)) * pvPoint[i];
	}
	return v;
}

