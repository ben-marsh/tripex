/*---------------------------------
* ZFft::Complex
-----------------------------------*/

/*---------------------------------
* Constructor:
-----------------------------------*/

inline ZFft::Complex::Complex( float fReal, float fImag )
{
	m_fReal = fReal;
	m_fImag = fImag;
}

/*---------------------------------
* operator*( ):
-----------------------------------*/

inline ZFft::Complex ZFft::Complex::operator*( const Complex &c ) const
{
	Complex cRes;
	cRes.m_fReal = m_fReal * c.m_fReal - m_fImag * c.m_fImag;
	cRes.m_fImag = m_fReal * c.m_fImag + m_fImag * c.m_fReal;
	return cRes;
}

/*---------------------------------
* operator+( ):
-----------------------------------*/

inline ZFft::Complex ZFft::Complex::operator+( const Complex &c ) const
{
	return Complex( m_fReal + c.m_fReal, m_fImag + c.m_fImag );
}

/*---------------------------------
* operator-( ):
-----------------------------------*/

inline ZFft::Complex ZFft::Complex::operator-( const Complex &c ) const
{
	return Complex( m_fReal - c.m_fReal, m_fImag - c.m_fImag );
}

/*---------------------------------
* Length( ):
-----------------------------------*/

inline float ZFft::Complex::Length( ) const
{
	return sqrtf( m_fReal * m_fReal + m_fImag * m_fImag );
}


