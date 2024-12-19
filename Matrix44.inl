/*---------------------------------------------
* SetRow( ):
---------------------------------------------*/

inline void Matrix44::SetRow( int nRow, FLOAT32 fVal0, FLOAT32 fVal1, FLOAT32 fVal2, FLOAT32 fVal3 )
{
	m_aafValue[ 0 ][ nRow ] = fVal0;
	m_aafValue[ 1 ][ nRow ] = fVal1;
	m_aafValue[ 2 ][ nRow ] = fVal2;
	m_aafValue[ 3 ][ nRow ] = fVal3;
}

/*---------------------------------------------
* operator*=( ):
---------------------------------------------*/

inline Matrix44 &Matrix44::operator*=( const Matrix44 &m )
{
	*this = operator*(m);
	return *this;
}

/*---------------------------------------------
* operator[ ]( ):
---------------------------------------------*/

inline FLOAT32 *Matrix44::operator[ ]( int nPos )
{
	return m_aafValue[ nPos ];
}

inline const FLOAT32 *Matrix44::operator[ ]( int nPos ) const
{
	return m_aafValue[ nPos ];
}

/*---------------------------------------------
* Translation( ):
---------------------------------------------*/

inline Matrix44 Matrix44::Translation( const Vector3 &v )
{
	return Translation( v.m_fX, v.m_fY, v.m_fZ );
}
