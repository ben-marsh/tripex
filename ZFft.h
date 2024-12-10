#ifndef _ZFFT_H
#define _ZFFT_H

/*---------------------------------------------
* Includes
---------------------------------------------*/

//#include <complex>

/*---------------------------------------------
* ZFft
---------------------------------------------*/

class ZFft
{
protected:
	class Complex
	{
	public:
		float m_fReal;
		float m_fImag;

		// Constructor:
		Complex( );
		Complex( float fReal, float fImag );

		// operator*( ):
		Complex operator*( const Complex &c ) const;

		// operator+( ):
		Complex operator+( const Complex &c ) const;

		// operator-( ):
		Complex operator-( const Complex &c ) const;

		// Length( ):
		float Length( ) const;
	};

	const int m_nSamples;
	const int m_nLogSamples;
	float *m_afScale;
	unsigned short int *m_anIndex;
	Complex *m_acTransform;
	Complex *m_acCoeff;
	float m_fDecay;

public:
	float *m_afAmplitude;

	// Constructor:
	ZFft( int nSamples );

	// Destructor:
	~ZFft( );

	// SetDecay( ):
	void SetDecay( float fDecay );

	// Update( ):
	void Update( const short int *anSample );

	// GetAmplitude( ):
	float GetAmplitude( int nIdx ) const;
};

#include "ZFft.inl"

#endif
