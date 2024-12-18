#pragma once

#include "ZVector.h"

class ZBezier
{
private:
	int nPoints;
	ZVector *pvPoint;
	int *pnBin;

public:
	ZBezier();
	ZBezier(int nPoints);
	~ZBezier();

	void Create(int nPoints);
	void Clear();
	int Binomial(int nLevel, int nPos);
	ZVector Calculate(float fPos);
	ZVector &operator[](int nPos);
	ZVector &Get(int nPos);
};

template < int nBez > class ZContainedBezier : public ZBezier
{
protected:
	ZBezier pBezier[nBez];
	
public:
	ZVector vBound1, vBound2;
	ZContainedBezier(ZVector vBound1 = ZVector::Origin(), ZVector vBound2 = ZVector::Origin());
	ZVector Calculate(float &fPos);
	virtual void GetNewPoints(ZBezier &b); 
};

template < int nBez > ZContainedBezier<nBez>::ZContainedBezier(ZVector vBound1, ZVector vBound2)
{
	this->vBound1 = vBound1; 
	this->vBound2 = vBound2;
	for(int i = 0; i < nBez; i++)
	{
		pBezier[i].Create(4);
	}
	pBezier[nBez-1][2] = ZVector::Origin();
	pBezier[nBez-1][3] = ZVector::Origin();
	float fPos = nBez + 1.5;
	Calculate(fPos);
}
template < int nBez > ZVector ZContainedBezier<nBez>::Calculate(float &fPos)
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
template < int nBez > void ZContainedBezier<nBez>::GetNewPoints(ZBezier &b)
{
	b[2].m_fX = vBound1.m_fX + ((float(rand()) / RAND_MAX) * (vBound2.m_fX - vBound1.m_fX));
	b[2].m_fY = vBound1.m_fY + ((float(rand()) / RAND_MAX) * (vBound2.m_fY - vBound1.m_fY));
	b[2].m_fZ = vBound1.m_fZ + ((float(rand()) / RAND_MAX) * (vBound2.m_fZ - vBound1.m_fZ));

	b[3].m_fX = vBound1.m_fX + ((float(rand()) / RAND_MAX) * (vBound2.m_fX - vBound1.m_fX));
	b[3].m_fY = vBound1.m_fY + ((float(rand()) / RAND_MAX) * (vBound2.m_fY - vBound1.m_fY));
	b[3].m_fZ = vBound1.m_fZ + ((float(rand()) / RAND_MAX) * (vBound2.m_fZ - vBound1.m_fZ));
}
