#include "Platform.h"
//#include "tripex2.h"
//#include <assert.h>
#include "effect.h"
#include "error.h"
#include "AudioData.h"
#include "Canvas.h"
#include "Texture.h"
#include "ColorHsv.h"
#include <time.h>
//#include <mmsystem.h>

//#pragma optimize("a", on)

//#define PI 3.14159
//#define DECAY 1//0.999//0.990
//#define CALCDIVISIONS 4

enum
{
	FM_SPIRALS = 0,
	FM_RANDEXPLODE,
	FM_TUNNEL,
	FM_BLOCKS,
	FM_CENTRESPIRAL,
	FM_STAR,
	FM_EXPANDTORINGS,
	FM_EXPANDTOTOPBOTTOM,
	FM_SPINRINGS,
	FLOWMAPS
};
const int pnFlowPref[] =
{
	FM_BLOCKS, 35, 
	FM_SPIRALS, 50,
	FM_RANDEXPLODE, 40,
	FM_TUNNEL, 60,
//	FM_BLOCKS, 111111, //35, 
	FM_CENTRESPIRAL, 40,
	FM_STAR, 40,
	FM_EXPANDTORINGS, 60,
	FM_EXPANDTOTOPBOTTOM, 30,
	FM_SPINRINGS, 60,
	-1,
};

#define MAX_CALC_TIME 0.02 * CLOCKS_PER_SEC//02

#define PRECISION 8 //4 //8

#define CALCDIVISIONS 4
#define CALCLINES (4 * CALCDIVISIONS)//3//2
#define PALECOLOURS 300

#define WIDTH 10
#define LENGTH 50

#define RSZ 15.0

#define CIRCLES 5
#define CENTRES 12
#define BLOCKS_X 64
#define BLOCKS_Y 32


/*
template < class T, int n > class ZStaticArray
{
public:
	T pEntry[n];
	T &operator[](int i)
	{
		assert(i >= 0 && i < n);
		return pEntry[i];
	}
};
*/
class EffectFlowmap : public EffectBase
{
public:
	typedef struct
	{
		float x, y;
	} FlowmapPoint;

	int pnIndexToBr[ 256 ];
	int pnBrToIndex[ 256 ];

	clock_t ctMinMapEnd;
	int nOsc1, nOsc2;
	double dOscFade;

	int nFlowmapW, nFlowmapH;
	int width, height;
	unsigned int tflowmap;
	float fMagnify;
	bool fOddFrame;
	Canvas *pCanvas;//(false, nFlowmapW, nFlowmapH);
	ColorRgb srccolour[ 256 ];
	ColorRgb dstcolour[ 256 ];
	double fadecolour;

	ZArray<unsigned char> bf1, bf2;//unsigned char far *bf1, *bf2;
	ZArray<unsigned char> multtab;

	ZArray<unsigned int> sp[ 2 ];
	ZArray<unsigned int> mo[ 2 ];
	
	int nCalcX;
	int nCalcY;
	int nCalcPixel;

	unsigned int calcline;
	float fStartY;
	int nFlowmapArea;
	
	int fm;
	int spikes;
	double spiralspeed;
	double spiralspin;
	
	double pdSpin[ CIRCLES ];
	double pdX[ CENTRES ];
	double pdY[ CENTRES ];
	double pdA[ CENTRES ];
	int nSrcBlock[ BLOCKS_X * BLOCKS_Y ];
	
	double dExpand;
	double dSpin;
	double dTun;
	float fStep;
	float fNextStep;
	
	int nCentres;
	FlowmapPoint pSrc, pDst;
	double dOscSpin;

	bool bSetColours;
	bool bMeasureTime;
	bool fFirstRender;
	double br;
	double efc;

	int nSamples;
	DWORD dwTimeTotal;//(75 * nSamples);
	double dFrames;

	double ang;

	EffectFlowmap()
	{
		dwTimeTotal = 0;
		nSamples = 0;
		efc = 120;
		bSetColours = true;
		bMeasureTime = true;
		fFirstRender = true;
		nCalcX;
		nCalcY;
		ang = 0;
		fStep = 1.0f;
		fNextStep = 1.0f;
		nFlowmapW = nFlowmapH = 2;
		tflowmap = 0;
		fOddFrame = true;
		fadecolour = 1;
		nCalcPixel = 0;

		dSpin = 0;
		dOscSpin = 0;

		//	obj.pVertex.SetLength(12);//Create(12, 1);
		
		nOsc1 = rand() % 4;
		nOsc2 = -1;
	
		nFlowmapW = Bound<int>(nFlowmapW, 1, 4);
		nFlowmapH = Bound<int>(nFlowmapH, 1, 4);
		
		if(nFlowmapH > nFlowmapW) std::swap(nFlowmapW, nFlowmapH);
                                                         		
		//	int i;
		/*	nFlowmapW = nFlowmapH = 1;
		for(int i = 0; i < nFlowmapArea; i++)
		{
		if(nFlowmapH < nFlowmapW) nFlowmapH++;
		else
		{
		nFlowmapW++;
		nFlowmapH = 1;
		}
		}
		*/	fMagnify = nFlowmapW / 2.0f;
		fStartY = -(256.0f * nFlowmapH / nFlowmapW) + 0.5;
		
		pCanvas = new Canvas( nFlowmapW, nFlowmapH );
//		pc.Create(false, nFlowmapW, nFlowmapH);
		
		width = nFlowmapW * 256;
		height = nFlowmapH * 256;
		bf1.SetLength(width * height);
		bf2.SetLength(width * height);
		multtab.SetLength(PRECISION*PRECISION*4*256);
		
	
		int x, y, c, i, j = 0;
		double m;
		for(y = 0; y < PRECISION; y++)
		{
			for(x = 0; x < PRECISION; x++)
			{
				for(c = 0; c < 256; c++)
				{
					for(i = 0; i < 4; i++)
					{
						if(i == 0) m = (1 - (x / double(PRECISION))) * (1 - (y / double(PRECISION)));
						else if(i == 1) m = (x / double(PRECISION)) * (1 - (y / double(PRECISION)));
						else if(i == 2) m = (1 - (x / double(PRECISION))) * (y / double(PRECISION));
						else if(i == 3) m = (x / double(PRECISION)) * (y / double(PRECISION));
						
						double v = c * m;
						double dm = 0.999999999 - std::max(0.0, (c - 192.0) * /*0.05*/0.02 / 64.0);
						multtab[j++] = v * dm;//0.9999999;//((v * v) / 65536.0);//* 0.9999999;// * 0.999;//986;//98;
					}
				}
			}
		}
		
		for(int k = 0; k < 2; k++)
		{
			sp[k].SetLength(width * height);
			mo[k].SetLength(width * height);
		}
			
		ChangeEffect();
		j = 0;
		pSrc.x = -255.5f;//(width - 1) / 2.0f;
		pSrc.y = fStartY;//(height - 1) / 2.0f;
		for(y = 0; y < height; y++)
		{
			Calculate(0, width, width, 0);
		}
		
		/*	double dx, dy, xo, yo;
		for(y = 0; y < height * CALCDIVISIONS; y++)
		{
		Calculate(y, 0);
		prFlowmap.SetProgress(y);
		}
		*/	for(y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++) bf1[j + x] = bf2[j + x] = 0;
			j += width;
		}
		calcline = height * CALCDIVISIONS;
	}

	static inline void AddPixel(ZArray<unsigned char> &pBuffer, int nPos, unsigned char c)
	{
		if(255 - pBuffer[nPos] < c) pBuffer[nPos] = 255;
		else pBuffer[nPos] += c;
	}
	static void DrawAAPixel(ZArray<unsigned char> &pBuffer, int nPos, double alpha)
	{
		pBuffer[nPos] = (pBuffer[nPos] * (1 - alpha)) + (255.0 * alpha);
	}
	static void DrawAALine(ZArray<unsigned char> &ptr, double x1, double y1, double x2, double y2, int xw, int h, double alpha)
	{
		double xd = x2 - x1, yd = y2 - y1;
		if(fabs(xd) > fabs(yd))
		{
			if(xd < 0)
			{
				std::swap( y1, y2 );
				std::swap( x1, x2 );
				yd = -yd;
				xd = -xd;
			}
			
			double ys = yd / fabs(xd);
			double y = y1;
			for(int x = ((int)x1) - 1; x < ((int)x2) + 2; x++)
			{
				double t = Bound<double>(std::min(x2, (double)x) - x1, 0, 1);
				int yn = (int)y;
				double p = y - yn;
				if(x > 0 && yn > 0 && x < xw - 2 && yn < h - 2)
				{
					DrawAAPixel(ptr, x + (yn * xw), t * alpha * (1 - p));
					DrawAAPixel(ptr, x + ((yn+1) * xw), t * alpha * p);
				}
				y += ys * t;
			}
		}
		else
		{
			if(yd < 0)
			{ 
				std::swap( y1, y2 );
				std::swap( x1, x2 );
				yd = -yd;
				xd = -xd;
			}
			
			double xs = xd / fabs(yd);
			double x = x1;
			for(int y = ((int)y1) - 1; y < ((int)y2) + 2; y++)
			{
				double t = Bound<double>(std::min(y2, (double)y) - y1, 0, 1);
				int xn = (int)x;
				double p = x - xn;
				if(xn > 0 && y > 0 && xn < xw - 2 && y < h - 2)
				{
					DrawAAPixel(ptr, xn + (y * xw), t * alpha * (1 - p));
					DrawAAPixel(ptr, (xn+1) + (y * xw), t * alpha * p);
				}
				x += xs * t;
			}
		}
	}
	int GetPref(int i)
	{
		int nPref = 0;//50;
		if(fm == i) nPref = 0;
		else
		{
			for(int j = 0; pnFlowPref[j] != -1; j += 2)
			{
				if(pnFlowPref[j] == i) nPref = pnFlowPref[j + 1];
			}
		}
		return nPref;
	}
	void ChangeEffect()
	{
		int ofm = fm;
		int nTotal = 0;
		for(int i = 0; i < FLOWMAPS; i++)
		{
			nTotal += GetPref(i);
		}
		int nPos = rand() * nTotal / RAND_MAX;
		for(int i = 0; i < FLOWMAPS; i++)
		{
			int nPref = GetPref(i);
			if(nPos < nPref)
			{
				fm = i;
				break;
			}
			else nPos -= nPref;
		}
		
		//while(ofm == fm) fm = rand() % FLOWMAPS;
		spikes = 2 + (rand() * 5 / RAND_MAX);
		spiralspeed = 0.99 - (rand() * 0.06 / RAND_MAX);
		spiralspin = (5.0 * 3.14159 / 180.0) * rand() / RAND_MAX;
		for(int i = 0; i < CIRCLES; i++)
		{
			pdSpin[i] = (0.5 + (2.0 * rand() / RAND_MAX)) * (3.14159 / 180.0);
			//			(3.14159 / 40.0) * (rand() - (RAND_MAX / 2)) / RAND_MAX;
			if(i % 2 == 1) pdSpin[i] = -pdSpin[i];
		}
		double dOfs = rand() * 3.14159 / RAND_MAX;
		
		dExpand = rand() * 1.0 / RAND_MAX;
		dSpin = rand() * 2.0 * 3.14159 / (180.0 * RAND_MAX);
		dTun = 0.02 + (0.06 * rand() / RAND_MAX);//4;//3 + (rand() * 0.1 / RAND_MAX);
		
		for(int i = 0; i < BLOCKS_X * BLOCKS_Y; i++) nSrcBlock[i] = rand() % 4;
		
		int nRand = rand() % 12;
		if(nRand < 2) nCentres = 2;
		else if(nRand < 4) nCentres = 3;
		else if(nRand < 6) nCentres = 4;
		else if(nRand < 8) nCentres = 6;
		else if(nRand < 10) nCentres = 8;
		else nCentres = 10;
		double dRad = 100 + (rand() * 60.0 / RAND_MAX);
		for(int i = 0; i < nCentres; i++)
		{
			pdX[i] = cos(dOfs + (i * 3.14159 * 2.0 / nCentres)) * dRad;//(rand() * width / RAND_MAX) - (width / 2.0);//(width * i / CENTRES) - (width / 2.0);
			pdY[i] = sin(dOfs + (i * 3.14159 * 2.0 / nCentres)) * dRad;//(rand() * height / RAND_MAX) - (height / 2.0);
			pdA[i] = (5.0 + (rand() * 3.0 / RAND_MAX)) * 3.141592 / 180.0;
			if(i % 2 == 1) pdA[i] = -pdA[i];
		}
	}
	inline void Calculate()
	{
		int i;
		double dAng, dLength;
		double dLenAt, dLenFrom;
		int nBlockX, nBlockY, nBlock;
		double dMult;
		int nRing;

//		fm = FM_BLOCKS;

		switch(fm)
		{
		case FM_SPIRALS:
			pDst.x = pSrc.x;
			pDst.y = pSrc.y;
			for(i = 0; i < nCentres; i++)
			{
				double xd = pSrc.x - pdX[i];
				double yd = pSrc.y - pdY[i];
				double dAng = atan2(yd, xd);
				double dLen = sqrt(xd * xd + yd * yd);
				double dMult = 50.0 / dLen;
				double dLenMult = std::max(1.0, 1.05 - (dLen / (50.0 * 60.0)));
				pDst.x += (cos(dAng + (dMult * dMult * pdA[i])) * dLen * dLenMult) - xd;
				pDst.y += (sin(dAng + (dMult * dMult * pdA[i])) * dLen * dLenMult) - yd;
			}
			break;
		case FM_RANDEXPLODE:
			dAng = atan2(pSrc.y, pSrc.x);
			dLength = sqrt(pSrc.x * pSrc.x + pSrc.y * pSrc.y) - 1;
			if(rand() > RAND_MAX * 0.5) dLength --;//-= (rand() * 9.0 / RAND_MAX) - 2;
			pDst.x = dLength * cos(dAng);
			pDst.y = dLength * sin(dAng);
			break;
		case FM_TUNNEL:
			dMult = width / 4.0;
			dLenAt = sqrt(pSrc.x * pSrc.x + pSrc.y * pSrc.y);
			dLenFrom = dMult / ((dMult / dLenAt) + dTun);//0.05);//15);
			dAng = atan2(pSrc.y, pSrc.x);
			pDst.x = cos(dAng + dSpin) * dLenFrom;
			pDst.y = sin(dAng + dSpin) * dLenFrom;
			break;
		case FM_BLOCKS:
			// -256...+256
			nBlockX = abs((int(pSrc.x / 2.0f) + 128) * BLOCKS_X / 256) % BLOCKS_X;
			nBlockY = abs((int(pSrc.y / 2.0f) + 128) * BLOCKS_Y / 256) % BLOCKS_Y;
//			nBlockX = int((pSrc.x + (width / 2.0)) * BLOCKS_X / width)) % BLOCKS_X;
//			nBlockY = int((pSrc.y + (height / 2.0)) * BLOCKS_Y / height)) % BLOCKS_Y;
			nBlock = (nBlockY * BLOCKS_X) + nBlockX;

			pDst.x = pSrc.x;
			pDst.y = pSrc.y;
			
			if(nSrcBlock[nBlock] == 0) pDst.y -= 0.75;
			else if(nSrcBlock[nBlock] == 1) pDst.y += 0.75;
			else if(nSrcBlock[nBlock] == 2) pDst.x -= 0.75;
			else if(nSrcBlock[nBlock] == 3) pDst.x += 0.75;
			break;
		case FM_CENTRESPIRAL:
			dAng = atan2(pSrc.y, pSrc.x);
			dLength = sqrt(pSrc.x * pSrc.x + pSrc.y * pSrc.y);
			dMult = 50.0 / dLength;
			pDst.x = cos(dAng + (dMult * 3.0 * 3.14159 / 180.0)) * dLength * 1.01;
			pDst.y = sin(dAng + (dMult * 3.0 * 3.14159 / 180.0)) * dLength * 1.01;
			break;
		case FM_STAR:
			dAng = atan2(pSrc.y, pSrc.x);
			dMult = cos((dAng * spikes) + (45 * 3.14159 / 180.0));
			if(dMult < 0) dMult = -0.05 * dMult;
			dMult = 0.01 + (dMult * 0.05);
			dMult = 1 - dMult;//(0.05 * (0.001 + fabs(cos(dAng * spikes))));
			pDst.x = pSrc.x * dMult;	
			pDst.y = pSrc.y * dMult;
			break;
		case FM_EXPANDTORINGS:
			dAng = atan2(pSrc.y, pSrc.x);
			dLength = sqrt(pSrc.x * pSrc.x + pSrc.y * pSrc.y);
			dMult = (cos((dLength - (30.0 * 3.14159 / 180.0)) * 3.14159 / RSZ) + 0.5) * 5;
			
			pDst.x = (dLength - dExpand - std::max(dMult, 0.0)) * cos(dAng);
			pDst.y = (dLength - dExpand - std::max(dMult, 0.0)) * sin(dAng);
			break;
		case FM_EXPANDTOTOPBOTTOM:
			dMult = (pSrc.y / height) + 0.5;
			pDst.x = pSrc.x + (pSrc.x * 0.1 * (-0.5 - (cos(dMult * 2.0 * 3.14159))));//(yp / (h / 2.0)) * (yp / (h / 2.0));
			pDst.y = pSrc.y * 0.98;
			break;
		case FM_SPINRINGS:
			dLength = sqrt(pSrc.x * pSrc.x + pSrc.y * pSrc.y);
			nRing = int(dLength / 30) % CIRCLES;
			dAng = atan2(pSrc.y, pSrc.x) + pdSpin[nRing];
			pDst.x = (dLength - 0.25) * cos(dAng);
			pDst.y = (dLength - 0.25) * sin(dAng);
			break;
		}
	}
	inline void Calculate(int nBuf, int nMin, int nMax, int nMinTime)//int nLine, int n)
	{
		float fPixelStep = 1.0f / fMagnify;
		DWORD nStart = timeGetTime( );
		float xo, yo;
		for(int nThisPixels = 0; nThisPixels < nMax && (nThisPixels < nMin || (timeGetTime( ) - nStart) < (DWORD)nMinTime); nThisPixels++)
		{
			// calc x,y, nCalcPixel
			Calculate();
			pDst.x = pSrc.x + ((pDst.x - pSrc.x) * fNextStep);
			pDst.y = pSrc.y + ((pDst.y - pSrc.y) * fNextStep);
			
			pDst.x = (pDst.x * fMagnify) + (width - 1) / 2.0f;
			pDst.y = (pDst.y * fMagnify) + (height - 1) / 2.0f;
			
			pDst.x = Bound<float>(pDst.x, 0, width - 2);
			pDst.y = Bound<float>(pDst.y, 0, height - 2);
			
			xo = Bound<float>(pDst.x - int(pDst.x), 0, 1);
			yo = Bound<float>(pDst.y - int(pDst.y), 0, 1);
			
			int i = (int(yo * double(PRECISION)) + xo) * double(PRECISION);
			
			if(nCalcY < height)
			{
				mo[nBuf][nCalcPixel] = Bound<int>(i, 0, 255) * 256 * 4;
				sp[nBuf][nCalcPixel] = (int(pDst.y) * width) + int(pDst.x);
				sp[nBuf][nCalcPixel] = Bound<int>(sp[nBuf][nCalcPixel], 0, (width * height) - 1);
				
				pSrc.x+=fPixelStep;
				nCalcX++;
				nCalcPixel++;
				if(nCalcX == width)
				{
					pSrc.x = -255.5f;//(width - 1) / 2.0f;
					nCalcX = 0;
					nCalcY++;
					pSrc.y+=fPixelStep;
				}
			}
		}
	}
	void DrawOscilloscope(double dElapsed, int nOsc, double dBr, AudioData* pAudio, ZArray<unsigned char> &dst)
	{
		double xsprev, ysprev;
		dOscSpin += dElapsed * 1 * 3.14159 / 180.0;
		double xc = width / 2.0;
		double yc = height / 2.0;
		
		int i, j;
		switch(nOsc)
		{
		case 0: // touching rings
			for(j = 0; j < 2; j++)
			{
				for(i = 0; i < 128; i++)
				{
					int n = (i + 1) % 128;	
					
					double na = 2.0 * 3.14159 * n / 127.0;
					double nr = 70 + (pAudio->GetSample( j, n * 4 ) * 128.0f * 4.0f / 20.0f);
					double nx = (nr * cos(na)) + (90 * cos(dOscSpin + (j * 3.14159)));
					double ny = (nr * sin(na)) + (90 * sin(dOscSpin + (j * 3.14159)));
					
					double ta = 2.0 * 3.14159 * i / 127.0;
					double tr = 70 + (pAudio->GetSample( j, i * 4) * 4.0f * 128.0f / 20.0);
					double tx = (tr * cos(ta)) + (90 * cos(dOscSpin + (j * 3.14159)));
					double ty = (tr * sin(ta)) + (90 * sin(dOscSpin + (j * 3.14159)));	
					
					DrawAALine(dst, xc + (tx * fMagnify), yc + (ty * fMagnify), xc + (nx * fMagnify), yc + (ny * fMagnify), width, height, dBr);
				}
			}
			break;
		case 1: // crossed lines
			for(j = 0; j < 2; j++)
			{
				for(i = 0; i < 128; i++)
				{
					double dAng = dSpin + (j * 3.14159 / 2.0);
					double xs = xc + (((cos(dAng) * 2.3 * (i - 64)) + (pAudio->GetSample( j, i * 4 ) * 128.0f * 4.0f * -sin(dAng) / 25.0)) * fMagnify); 
					double ys = yc + (((sin(dAng) * 2.3 * (i - 64)) + (pAudio->GetSample( j, i * 4 ) * 128.0f * 4.0f * cos(dAng) / 25.0)) * fMagnify);
					
					if(i > 0) DrawAALine(dst, xsprev, ysprev, xs, ys, width, height, dBr);
					
					xsprev = xs;
					ysprev = ys;
				}
			}
			break;
		case 2: // nFlowmapW oscilloscope
			for(i = 0; i < 128; i++)
			{
				double xs = xc + ((i - 64) * 3 * fMagnify); //(i / xc + (cos(dAng) * 1.3 * (i - 64)) + (waveformStereo[j][i] * -sin(dAng) / 10.0); 
				double ys = yc + fMagnify * pAudio->GetSample( i * 4 ) * 4.0f * 128.0f / 10.0;
				
				if(i > 0) DrawAALine(dst, xsprev, ysprev, xs, ys, width, height, dBr);
				
				xsprev = xs;
				ysprev = ys;
			}
			break;
		case 3: // dual centred circular oscilloscopes
			//		for(j = 0; j < 2; j++)
			//		{
			{
				double na, nr, nx, ny;
				for(i = 0; i <= 128; i++)
				{
					int n = i % 128;	
					
					//				double na = 2.0 * 3.14159 * n / 127.0;
					//				double nr = 90 + ((waveformStereo[0][n] + waveformStereo[1][n]) / 40.0);
					//				double nx = (nr * cos(na));
					//				double ny = (nr * sin(na));
					
					double ta = 2.0 * 3.14159 * n / 128.0;
					double tr = 90 + ((pAudio->GetSample( n * 4) * 128.0f * 4.0f) / 40.0);
					double tx = (tr * cos(ta));
					double ty = (tr * sin(ta));
					
					if(i > 0)
					{
						DrawAALine(dst, xc + (tx * fMagnify), yc + (ty * fMagnify), xc + (nx * fMagnify), yc + (ny * fMagnify), width, height, dBr);
					}
					
					na = ta;
					nr = tr;
					nx = tx;
					ny = ty;
				}
			}
			//		}
		default:
			break;
		}
	}
	Error* Calculate(float brightness, float elapsed, AudioData* pAudio) override
	{
		Error* error;
		int i, j;
	
		dFrames += elapsed;
	
		dSpin += ( 3.0f * PI / 180.0f ) * dFrames;

		double accum = 0;
		fOddFrame ^= true;
	
		if(fFirstRender)
		{
			error = pCanvas->Create( );//pc.Initialise( );
			if(error) return TraceError(error);

			fFirstRender = false;
			accum = 1.1;
		}
		else accum = elapsed;
		
		DWORD dwStartTime = timeGetTime( );
		
		dOscFade += 0.1 * elapsed;
		
		float fMult = float(width * height) / (256 * 512);
		if(bMeasureTime)
		{
			nCalcX = 0;
			nCalcY = 0;
			nCalcPixel = 0;
			pSrc.x = -255.5f;//(width - 1.0f) / 2.0f;
			pSrc.y = fStartY;//(height - 1.0f) / 2.0f;
		}
		Calculate(tflowmap ^ 1, std::max<int>(1, 512 * 3 * fMult * elapsed), std::max<int>(2, 512 * 3 * fMult * elapsed), elapsed * MAX_CALC_TIME);
		if(bSetColours || ((bMeasureTime && nSamples > 5) || nCalcY >= height))
		{
			if(bSetColours || !bMeasureTime)
			{
				bSetColours = false;
				fadecolour = 0;
				memcpy( srccolour, dstcolour, sizeof( dstcolour ) );
				
				ColorHsv pcHSV[ 5 ];
				FLOAT32 fDiff = rand() * (50.0f * 3.14159 / 180.0) / RAND_MAX;
				if(rand() > RAND_MAX / 2) fDiff = -fDiff;
				pcHSV[3].m_fH = (rand() * PI * 2.0f / RAND_MAX) - PI;
				pcHSV[3].CorrectRange();
				pcHSV[2].m_fH = pcHSV[3].m_fH + fDiff;
				pcHSV[2].CorrectRange();
				pcHSV[1].m_fH = pcHSV[2].m_fH + fDiff;
				pcHSV[1].CorrectRange();
				pcHSV[0].m_fH = pcHSV[1].m_fH + fDiff;
				pcHSV[0].CorrectRange();
				pcHSV[4].m_fH = pcHSV[3].m_fH;
				pcHSV[4].CorrectRange();
				
				for(i = 0; i < 5; i++)
				{
					pcHSV[i].m_fS = (i == 4)? 0 : (0.25 + (rand() * 0.25 / RAND_MAX));
					pcHSV[i].m_fV = i / 4.0;// (i * i) / 16.0;
				}
				pcHSV[0].m_fS = 0.1f;//25;
				pcHSV[1].m_fS = 0.2f;//25;
				pcHSV[2].m_fS = 0.3f;//25;
				pcHSV[3].m_fS = 0.2f;//15;//5;//12;
				pcHSV[4].m_fS = 0.0f;//(i == 4)? 0 : 0.25;//(0.25 + (rand() * 0.25 / RAND_MAX));

				ColorRgb pc[ 5 ];
				for(i = 0; i < 5; i++) pc[i] = pcHSV[i].GetRGB();
				for(i = 0; i < 256; i++)
				{
					int nSec = i / 64;
					ColorRgb c = ColorRgb::Blend(pc[nSec], pc[nSec+1], (i % 64) / 64.0);
					//				ZColour c2 = c.GetRGB();
					dstcolour[i] = c;//c2;//.GetRGB();
				}
			}
			if(((bMeasureTime && nSamples > 5) || nCalcY >= height))
			{
				nCalcX = 0;
				nCalcY = 0;
				nCalcPixel = 0;
				pSrc.x = -255.5f;//(width - 1.0f) / 2.0f;
				pSrc.y = fStartY;//(height - 1.0f) / 2.0f;
				
				if(!bMeasureTime) tflowmap ^= 1;//rand() * FLOWMAPS / (RAND_MAX + 1);
				ChangeEffect();
				calcline = 0;
				efc -= 60;
				bMeasureTime = false;
				
				fStep = fNextStep;
				fNextStep = (dwTimeTotal / nSamples) / 50.0f;
				dwTimeTotal = 0;
				nSamples = 0;
			}
		}
		
		
		if(fadecolour >= 0)
		{
			if(fadecolour >= 1)
			{
				for( SINT32 i = 0; i < 256; i++ )
				{
					PALETTEENTRY *pEntry = &pCanvas->m_aPalette[ i ];
					pEntry->peRed = dstcolour[ i ].m_nR;
					pEntry->peGreen = dstcolour[ i ].m_nG;
					pEntry->peBlue = dstcolour[ i ].m_nB;
					pEntry->peFlags = 0xff;
				}
//				pCanvas
//				pc.SetPalette(dstcolour);
				fadecolour = -1;
			}
			else
			{
//				ZColour colour[256];
				for(int i = 0; i < 256; i++)
				{
					ColorRgb c = ColorRgb::Blend( srccolour[ i ], dstcolour[ i ], fadecolour );

					PALETTEENTRY *pEntry = &pCanvas->m_aPalette[ i ];
					pEntry->peRed = c.m_nR;
					pEntry->peGreen = c.m_nG;
					pEntry->peBlue = c.m_nB;
					pEntry->peFlags = 0xff;
//					colour[i] = ZColour::Blend(srccolour[i], dstcolour[i], fadecolour);
				}
//				pc.SetPalette(colour);
				fadecolour += elapsed * 0.05;
			}
		}
		
		br = brightness;
		
		ZArray<unsigned char> &src = fOddFrame? bf1 : bf2;
		ZArray<unsigned char> &dst = fOddFrame? bf2 : bf1;
		
		//	int index[128];
		double r = pAudio->GetIntensity( );
		ang += elapsed * std::min(r / 200.0, 2.0) * 4.0 * 3.14159 / 180.0;
		
		int brt = pAudio->GetIntensity( ) * 30.0; //min(max(average * 10.0, 8), 50);
		
		r = (r / 20) + 10;
		
		j = 0;
		double mlt = std::min(0.85, (pAudio->GetIntensity( ) * 1.6)) / 4.0;
		//(0.7 + (average * 0.5))) / 4.0;
		
		//	ZeroMemory(dst, sizeof(char) * 256 * 512);
		
		mlt = 0.8;
		//	int s;
		
		//	double ml = elapsed * average * 3.0;//0.5 * (1 + average) * average;
		//	obj.fRoll += ml * 3.14159 / 180.0;
		//	obj.fPitch += ml * 2.0 * 3.14159 / 180.0;
		//	obj.fYaw += (ml / 2.0) * 3.0 * 3.14159 / 180.0;
		//ml * ((50 - brt) / 9.0) * 3.14159 / 180.0, ml * ((50 - brt) / 8.0) * 3.14159 / 180.0, ((50 - brt) / 7.0) * 3.14159 / 180.0); //2 * 3.14159 / 180.0, 3 * 3.14159 / 180.0);
		//	obj->Rotate();
		
		static double amplitude = pAudio->GetIntensity( ); 
		double target = std::max<int>(20, (1 - pAudio->GetIntensity( )) * 70.0); //average * 70.0;
		
		amplitude = target;
		
		static double angle = 0;
		angle += elapsed * pAudio->GetIntensity( )* 2.0 * 3.14159 / 180.0;
		
		static double fa = 0;
		fa += elapsed * pAudio->GetIntensity( )* 3.14159 / 180.0;
		//	double brm = average((1 - cos(average * 3.14159))  / 2) + 0.5;
		
		if(fabs(target - amplitude) < 4 * elapsed) amplitude = target;
		else if(target < amplitude) amplitude -= 4 * elapsed;
		else amplitude += 4 * elapsed;
		
		amplitude = 70;
		
		ZArray<unsigned int> &mot = mo[tflowmap];
		ZArray<unsigned int> &spt = sp[tflowmap];
		
		unsigned char *mt;
		int nIterations = width * height * 4;
		for(i = 0; i < width * height; i++)
		{
			mt = &multtab[mot[i]];
			// mot[i] = 0...PRECISION*PRECISION index
			dst[i] =	mt[(src[spt[i]] * 4) + 0]
				+	mt[(src[spt[i] + 1] * 4) + 1]
				+	mt[(src[spt[i] + width] * 4) + 2]
				+	mt[(src[spt[i] + width + 1] * 4) + 3];
		}
		
		if(nOsc2 == -1)
		{
			if(nOsc1 == -1 || dOscFade > 10)//rand() < 0.01 * RAND_MAX)
			{
				while(nOsc2 == -1 || nOsc2 == nOsc1) nOsc2 = rand() % 4;
				dOscFade = 0;
			}
		}
		double dBr = std::min(1.0, pAudio->GetIntensity( )* 3.0);
		if(nOsc2 != -1 && dOscFade >= 1.0)
		{
			nOsc1 = nOsc2;
			nOsc2 = -1;
		}
		
		if(nOsc2 != -1)
		{
			DrawOscilloscope(elapsed, nOsc1, dBr * (1 - dOscFade), pAudio, dst);
			DrawOscilloscope(elapsed, nOsc2, dBr * dOscFade, pAudio, dst);
		}
		else DrawOscilloscope(elapsed, nOsc1, dBr, pAudio, dst);

		pCanvas->m_cColour = ColorRgb::Grey(255.0 * brightness);//D3DRGB(brightness, brightness, brightness);

		memcpy( pCanvas->GetDataPtr( ), (fOddFrame? bf2 : bf1).GetBuffer( ), bf1.GetSize());
		error = pCanvas->UploadTextures( );
//		hRes = pc.Calculate((fOddFrame? bf2 : bf1).GetBuffer());
		if(error) return TraceError(error);
		
		DWORD dwTime = timeGetTime( ) - dwStartTime;
		if(dwTimeTotal < 1000000 && dwTime < 1000000)
		{
			dwTimeTotal += dwTime;
			nSamples++;
		}
		return nullptr;
	}
	Error* Render( ) override
	{
		g_pD3D->SetState(g_pD3D->Transparent);
		
		Error* error = pCanvas->Render( );
		if(error) return TraceError(error);

		return nullptr;
	}
	bool CanRender(float dElapsed)
	{
		return (dElapsed >= fStep);//(dElapsed >= 1.0);//(dElapsed >= 0.8);//1);
	}
};
EXPORT_EFFECT( Flowmap, EffectFlowmap )
