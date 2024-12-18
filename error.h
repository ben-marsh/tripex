#pragma once

#include <string>

struct ZError
{
	std::string Message;
	std::vector<std::pair<const char*, uint32_t>> Trace;

	ZError(HRESULT code);
	~ZError();

	ZError* AddTrace(const char* file, uint32_t line);
	std::string GetDescription() const;
};

ZError* TraceErrorImpl(ZError* error, const char* file, uint32_t line);
ZError* TraceErrorImpl(HRESULT hRes, const char* file, uint32_t line);

#define TraceError(Error) (TraceErrorImpl(Error, __FILE__, __LINE__))
