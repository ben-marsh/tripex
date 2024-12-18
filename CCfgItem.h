#pragma once

#include <string>
#include <vector>

extern const std::string sKeyBase;

class CCfgItem
{
public:
	enum
	{
		CIT_INT,
		CIT_BOOL,
		CIT_FLOAT,
		CIT_STRING,
	};

	bool bInitUpdate;
	std::string sName;
	int nType;
	bool bChanged, bSave;

	union
	{
		void *pValue;
		int *pnValue;
		bool *pbValue;
		float *pfValue;
		std::string *psValue;
	};
	union
	{
		bool bNewValue;
		float fNewValue;
		int nNewValue;
		std::string *psNewValue;
	};

	CCfgItem(const char *sName, int nType, void *pValue, bool bInitUpdate = false);

	// integer array (resolution, flowmap size, textures)
	// float array (effect prefs)
	static CCfgItem *Int(const char *sName, int *pnValue, bool bInitUpdate = false);
	static CCfgItem *Bool(const char *sName, bool *pbValue, bool bInitUpdate = false);
	static CCfgItem *Float(const char *sName, float *pfValue, bool bInitUpdate = false);
	static CCfgItem *String(const char *sName, std::string *psValue, bool bInitUpdate = false);
	~CCfgItem();

	void OnChange();
	void OnChangeTarget();

	// arrays
	static std::string MakeArrayString(char c, std::vector<int> &vn);
	static bool ParseArrayString(const char *str, std::vector<int> &vn);

	// setting variables
	bool SetInt(int nValue);
	bool SetBool(bool bValue);
	bool SetFloat(float fValue);
	bool SetString(const char *sValue);
	bool SetIntArray(char c, std::vector<int> &vn);
	bool SetIntArray(char c, int n, int n1, ...);
	bool SetIntArray(char c, int n, int *pn);
	bool SetFloatArray(char c, int n, float *pf);

	// retrieving variables
	int GetInt() const;
	bool GetBool() const;
	float GetFloat() const;
	std::string GetString() const;
	bool GetIntArray(std::vector<int> &vn) const;
	bool GetIntArray(int n, int *pn) const;
	bool GetFloatArray(int n, float *pf) const;

	std::string GetKeyName();
	std::string GetValueName();

//		bool Load(HKEY hKey);
//		void Save(HKEY hKey);
	void Update(bool bInit);
};
