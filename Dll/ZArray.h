#ifndef _ZARRAY_H
#define _ZARRAY_H

template < typename T > class ZArray
{
protected:
	T *pEntry;
	int nLength, nSize;

public:
	int nStep;

	inline ZArray(int nLength = 0, int nStep = 10);
	inline ZArray(ZArray &ar);
	inline ~ZArray(){ SetSize(0); }
	inline void Attach(ZArray<T> &pOther);
	inline void Attach(T *pNewEntry, int nNewLength);
	inline T *Detach();
	inline void SwapBuffer(ZArray<T> &pOther);

	// array size functions
	inline bool MakeExtraSpace(int nItems);
	inline bool SetSize(int nSize);
	inline bool SetLength(int nLength);
	inline int GetSize() const { return nSize; }
	inline int GetLength() const { return nLength; }
	inline void Compact(){ SetSize(GetLength()); }
	inline void Empty(){ SetLength(0); }

	// getting items
	inline T *GetBuffer(int nMinLength = 0);
	inline T &First() const { return (*this)[0]; }
	inline T &Last() const { return (*this)[GetLength() - 1]; }
	inline T &operator[](int i) const { _ASSERT(i >= 0 && i < nLength); return pEntry[i]; }

	// adding/removing new items
	inline T *InsertEmptyPtr(int nPos, int nItems = 1);
	inline bool InsertEmpty(int nPos, int nItems = 1);
	inline bool Insert(int nPos, const T *pItem, int nLength);
	inline bool Insert(int nPos, ZArray<T> &ar){ return Insert(nPos, ar.GetBuffer(), ar.GetLength()); }
	inline bool Insert(int nPos, T item){ return Insert(nPos, &item, 1); }

	inline T *AddEmptyPtr(int nItems = 1){ return InsertEmptyPtr(GetLength(), nItems); }
	inline int AddEmpty(int nItems = 1);
	inline int Add(const T *pItem, int nLength);
	inline int Add(ZArray<T> &ar){ return Add(ar.GetBuffer(), ar.GetLength()); }
	inline int Add(T item);

	inline void Remove(int nPos, int nLength = 1);

	// operations
	inline int Replace(T a, T b);
	inline int IndexOf(T a) const;
	inline void Fill(T a){ Fill(0, GetLength() - 1, a); }
	inline void Fill(int nStart, int nEnd, T a);
	inline bool IsValidPtr(const T *pPtr) const { return pPtr >= &First() && (&pPtr[1]) < &Last(); }

	inline ZArray<T> &operator=(ZArray<T> &ar){ Empty(); Add(ar); return *this; }
};

// ZArray Implementation
template<typename T> void ZArray<T>::Attach(ZArray<T> &pOther)
{
	Attach(pOther.pEntry, pOther.nLength);
	nSize = pOther.nSize;
	nStep = pOther.nStep;
	pOther.Detach();
}
template<typename T> void ZArray<T>::Attach(T *pNewEntry, int nNewLength)
{
	pEntry = pNewEntry;
	nLength = nNewLength;
	nSize = nNewLength;
}
template<typename T> T *ZArray<T>::Detach()
{
	T *pOldEntry = pEntry;
	pEntry = NULL;
	nSize = 0;
	nLength = 0;
	return pOldEntry;
}
template<typename T> void ZArray<T>::SwapBuffer(ZArray<T> &pOther)
{
	nLength ^= pOther.nLength;
	pOther.nLength ^= nLength;
	nLength ^= pOther.nLength;

	nSize ^= pOther.nSize;
	pOther.nSize ^= nSize;
	nSize ^= pOther.nSize;

	(DWORD&)pEntry ^= (DWORD&)pOther.pEntry;
	(DWORD&)pOther.pEntry ^= (DWORD&)pEntry;
	(DWORD&)pEntry ^= (DWORD&)pOther.pEntry;
}
template<typename T> ZArray<T>::ZArray(int nNewLength, int nNewStep)
{
	nLength = 0;
	nSize = 0; 
	pEntry = NULL; 
	nStep = nNewStep; 
	SetLength(nNewLength);
}
template<typename T> ZArray<T>::ZArray(ZArray &ar)
{
	nLength = 0;
	nSize = 0; 
	pEntry = NULL; 
	nStep = ar.nStep; 
	Add(ar);
}
template<typename T> bool ZArray<T>::MakeExtraSpace(int nItems)
{
	if(nLength + nItems > nSize)
	{
		return SetSize(nLength + nItems);
	}
	else return true;
}
template<typename T> bool ZArray<T>::SetSize(int nNewSize)
{
	if(nNewSize > 0 || pEntry != NULL)
	{
		T *pNewEntry = (T*)realloc(pEntry, nNewSize * sizeof(T));
		if(pNewEntry == NULL && nNewSize != 0)
		{
			return false;
		}
		pEntry = pNewEntry;
		nSize = nNewSize;
		if(nSize < nLength) nLength = nSize;
	}
	return true;
}
template<typename T> bool ZArray<T>::SetLength(int nNewLength)
{
	if(nNewLength > nSize)
	{
		if(!SetSize(max(nSize + nStep, nNewLength))) return false;
	}
	nLength = nNewLength;
	return true;
}
template<typename T> T *ZArray<T>::InsertEmptyPtr(int nPos, int nItems)
{
	if(!InsertEmpty(nPos, nItems)) return NULL;
	else return &(*this)[nPos];
}
template<typename T> bool ZArray<T>::InsertEmpty(int nPos, int nItems)
{
	_ASSERT(nPos >= 0 && nPos <= nLength);
	if(!SetLength(nLength + nItems)) return false;
	memmove(&pEntry[nPos + nItems], &pEntry[nPos], ((nLength - nItems) - nPos) * sizeof(T));
	return true;
}
template<typename T> bool ZArray<T>::Insert(int nPos, const T *pData, int nItems)
{
	if(!InsertEmpty(nPos, nItems)) return false;

	memcpy(&pEntry[nPos], pData, (nItems * sizeof(T)));
	return true;
}
template<typename T> int ZArray<T>::AddEmpty(int nItems)
{
	int nPos = GetLength();
	if(!InsertEmpty(nPos, nItems)) return -1; 
	else return nPos; 
}
template<typename T> int ZArray<T>::Add(const T *pItem, int nLength)
{
	int nPos = GetLength();
	if(!Insert(nPos, pItem, nLength)) return -1;
	else return nPos;
}
template<typename T> inline int ZArray<T>::Add(T item)
{
	if(!Insert(GetLength(), item)) return -1;
	else return GetLength() - 1;
}
template<typename T> void ZArray<T>::Remove(int nPos, int nLen)
{
	_ASSERT(nPos >= 0 && nPos + nLen <= GetLength());
	memmove(pEntry + nPos, pEntry + nPos + nLen, (nLength - (nPos + nLen)) * sizeof(T)); 
	SetLength(nLength - nLen);
}
template<typename T> T *ZArray<T>::GetBuffer(int nMinLength)
{
	if(nMinLength > nLength)
	{
		if(!SetLength(nMinLength)) return NULL;
	}
	return pEntry;
}
template<typename T> int ZArray<T>::Replace(T a, T b)
{
	int nItems = 0;
	for(int i = 0; i < GetLength(); i++)
	{
		if(!memcmp(&pEntry[i], &a, sizeof(T)))
		{
			memcpy(&pEntry[i], &b, sizeof(T));
			nItems++;
		}
	}
	return nItems;
}
template<typename T> int ZArray<T>::IndexOf(T a) const
{
	for(int i = 0; i < GetLength(); i++)
	{
		if(!memcmp(&pEntry[i], &a, sizeof(T))) return i;
	}
	return -1;
}
template<typename T> void ZArray<T>::Fill(int nStart, int nEnd, T a)
{
	for(int i = nStart; i <= nEnd; i++)
	{
		memcpy(&pEntry[i], &a, sizeof(T)); 
	}
}

#endif