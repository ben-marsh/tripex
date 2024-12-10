#include "StdAfx.h"
#include <crtdbg.h>
#include "CCfgItem.h"
#include "Misc.h"

/************* CCfgItem ****************************************/
CCfgItem::CCfgItem(const char *sName, int nType, void *pValue, bool bInitUpdate)
{
	this->bInitUpdate = bInitUpdate;
	this->sName = sName;
	this->nType = nType;
	this->pValue = pValue;
	bSave = false;
	bChanged = false;
	if(nType == CIT_STRING) psNewValue = new string;
}
CCfgItem *CCfgItem::Int(const char *sName, int *pnValue, bool bInitUpdate)
{
	return new CCfgItem(sName, CIT_INT, pnValue, bInitUpdate);
}
CCfgItem *CCfgItem::Bool(const char *sName, bool *pbValue, bool bInitUpdate)
{
	return new CCfgItem(sName, CIT_BOOL, pbValue, bInitUpdate);
}
CCfgItem *CCfgItem::Float(const char *sName, float *pfValue, bool bInitUpdate)
{
	return new CCfgItem(sName, CIT_FLOAT, pfValue, bInitUpdate);
}
CCfgItem *CCfgItem::String(const char *sName, string *psValue, bool bInitUpdate)
{
	return new CCfgItem(sName, CIT_STRING, psValue, bInitUpdate);
}
CCfgItem::~CCfgItem()
{
	if(nType == CIT_STRING) delete psNewValue;
}
void CCfgItem::OnChange()
{
	bChanged = true;
	bSave = true;
}
void CCfgItem::OnChangeTarget()
{
	bChanged = false;
	bSave = true;
}
template < class T, class U > bool Set(CCfgItem *pItem, T *pt, U u)
{
	*pt = u;
	pItem->OnChange();
	return true;
}
template < class T > T Get( const CCfgItem * const pItem, const T *pt1, const T *pt2 )
{
	if(pItem->bChanged) return *pt1;
	else return *pt2;
}
bool CCfgItem::SetInt(int nValue)
{
	_ASSERT(nType == CIT_INT);
	return Set(this, &nNewValue, nValue);
}
bool CCfgItem::SetBool(bool bValue)
{
	_ASSERT(nType == CIT_BOOL);
	return Set(this, &bNewValue, bValue);
}
bool CCfgItem::SetFloat(float fValue)
{
	_ASSERT(nType == CIT_FLOAT);
	return Set(this, &fNewValue, fValue);
}
bool CCfgItem::SetString(const char *sValue)
{
	_ASSERT(nType == CIT_STRING);
	return Set(this, psNewValue, sValue);
}
int CCfgItem::GetInt() const
{
	_ASSERT(nType == CIT_INT);
	return Get< int >( this, &nNewValue, pnValue );
}
bool CCfgItem::GetBool() const
{
	_ASSERT(nType == CIT_BOOL);
	return Get(this, &bNewValue, pbValue);
}
float CCfgItem::GetFloat() const
{
	_ASSERT(nType == CIT_FLOAT);
	return Get(this, &fNewValue, pfValue);
}
string CCfgItem::GetString() const
{
	_ASSERT(nType == CIT_STRING);
	return Get(this, psNewValue, psValue);
}
void CCfgItem::Update(bool bInit)
{
	if(!bChanged || (bInitUpdate && !bInit)) return;

	if(nType == CIT_STRING) *psValue = *psNewValue;
	else if(nType == CIT_INT) *pnValue = nNewValue;
	else if(nType == CIT_BOOL) *pbValue = bNewValue;
	else if(nType == CIT_FLOAT) *pfValue = fNewValue;
	else __assume(false);

	bChanged = false;
}
bool CCfgItem::SetIntArray(char c, vector<int> &vn)
{
	return SetString(MakeArrayString(c, vn).c_str());
}
bool CCfgItem::SetIntArray(char c, int n, int *pn)
{
	vector<int> vn(n);
	for(int i = 0; i < n; i++) vn[i] = pn[i];
	return SetIntArray(c, vn);
}
bool CCfgItem::SetIntArray(char c, int n, int n1, ...)
{
	vector<int> vn;

	va_list pArg;
	va_start(pArg, n1);
	for(int i = 0; i < n; i++)
	{
		vn.push_back(n1);
		n1 = va_arg(pArg, int);
	}
	va_end(pArg);
	return SetIntArray(c, vn);
}
bool CCfgItem::SetFloatArray(char c, int n, float *pf)
{
	auto_ptr<int> pn(new int[n]);
	for(int i = 0; i < n; i++) pn.get()[i] = Bound<int>((int)(pf[i] * 1000.0f), 0, 1000);
	return SetIntArray(c, n, pn.get());
}
bool CCfgItem::GetIntArray(vector<int> &vn) const
{
	return ParseArrayString(GetString().c_str(), vn);
}
bool CCfgItem::GetIntArray(int n, int *pn) const
{
	vector<int> vn;
	for(int i = 0; i < n; i++) pn[i] = 0;
	if(!GetIntArray(vn) || (int)vn.size() != n) return false;
	CopyMemory(pn, &(*vn.begin()), n * sizeof(int));
	return true;
}
bool CCfgItem::GetFloatArray(int n, float *pf) const
{
	int i;
	for(i = 0; i < n; i++) pf[i] = 0.5f;

	auto_ptr<int> pn(new int[n]);
	if(!GetIntArray(n, pn.get())) return false;
	for(i = 0; i < n; i++) pf[i] = Bound<float>(pn.get()[i] / 1000.0f, 0.0f, 1.0f);
	return true;
}
string CCfgItem::MakeArrayString(char c, vector<int> &vn)
{
	string str;
	for(unsigned int i = 0; i < vn.size(); i++)
	{
		if(i > 0) str += c;

		char sBuf[ 20 ];
		sprintf( sBuf, "%d", vn[ i ] );

		str += sBuf;
	}
	return str;
}
bool CCfgItem::ParseArrayString(const char *str, vector<int> &vn)
{
	while(isspace(*str)) str++;
	for(;;)
	{
		if(*str == 0) return true;

		int n = atoi(str);
		if(*str == '-') str++;
		if(!isdigit(*(str++))) return false;
		while(isdigit(*str)) str++;
		vn.push_back(n);

		while(*str != 0 && !isdigit(*str) && *str != '-') str++;
	}
}
//	bool CCfgItem::Load(HKEY hKey)
//	{
//		bSave = false;
//		bChanged = false;
//	
//		string sValueName = GetValueName();
//		switch(nType)
//		{
//		case CCfgItem::CIT_BOOL:
//			return RegGetBool(hKey, sValueName.c_str(), pbValue);
//		case CCfgItem::CIT_INT:
//			return RegGetInt(hKey, sValueName.c_str(), pnValue);
//		case CCfgItem::CIT_FLOAT:
//			int n;
//			if(!RegGetInt(hKey, sValueName.c_str(), &n)) return false;
//			*pfValue = Bound<float>(n / 1000.0f, 0.0f, 1.0f);
//			return true;
//		case CCfgItem::CIT_STRING:
//			return RegGetString(hKey, sValueName.c_str(), psValue);
//		default:
//			_ASSERT(false);
//			__assume(0);
//			return false;
//		}
//	}
//	void CCfgItem::Save(HKEY hKey)
//	{
//		if(bSave)
//		{
//			string sValueName = GetValueName();
//			switch(nType)
//			{
//			case CCfgItem::CIT_BOOL:
//				RegSetInt(hKey, sValueName.c_str(), GetBool());
//				break;
//			case CCfgItem::CIT_INT:
//				RegSetInt(hKey, sValueName.c_str(), GetInt());
//				break;
//			case CCfgItem::CIT_FLOAT:
//				RegSetInt(hKey, sValueName.c_str(), (int)(GetFloat() * 1000.0f));
//				break;
//			case CCfgItem::CIT_STRING:
//				RegSetString(hKey, sValueName.c_str(), GetString().c_str());
//				break;
//			default:
//				_ASSERT(false);
//				__assume(0);
//				break;
//			}
//		}
//	}
string CCfgItem::GetKeyName()
{
	string::size_type n = sName.find_last_of( "\\" );
	if( n == string::npos) return "";
	return sName.substr(0, n);
}
string CCfgItem::GetValueName()
{
	size_t n = sName.find_last_of("\\");
	return (n == string::npos)? sName : sName.substr(n + 1, string::npos);
}
