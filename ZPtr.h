#pragma once

template < typename T > class ZPtr
{
protected:
	T *pEntry;

public:
	ZPtr(){ pEntry = NULL; }
	ZPtr(ZPtr<T> &pPtr){ pEntry = NULL; operator=(pPtr); }
	ZPtr(T *pNewEntry){ pEntry = NULL; operator=(pNewEntry); }
	~ZPtr(){ Free(); }

	T *Get(){ return pEntry; }
	T *Detach()
	{
		T *pOldEntry = pEntry;
		pEntry = NULL;
		return pOldEntry;
	}
	void Free(){ delete Detach(); }

	// operators
	operator T*()
	{
		return pEntry;
	}

#pragma warning(push)
#pragma warning(disable:4284)
	T *operator->()
	{
		_ASSERT(!IsBadReadPtr(pEntry, sizeof(T)));
		return pEntry;
	}
#pragma warning(pop)

	ZPtr<T> &operator=(ZPtr<T> &pPtr){ return operator=(pPtr.Detach()); }
	ZPtr<T> &operator=(T *pNewEntry)
	{
		Free();
		pEntry = pNewEntry;
		return *this;
	}
};
