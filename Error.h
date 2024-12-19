#pragma once

#include <string>

struct Error
{
	std::string Message;
	std::vector<std::pair<const char*, uint32_t>> Trace;

	Error(HRESULT code);
	~Error();

	Error* AddTrace(const char* file, uint32_t line);
	std::string GetDescription() const;
};

Error* TraceErrorImpl(Error* error, const char* file, uint32_t line);
Error* TraceErrorImpl(HRESULT hRes, const char* file, uint32_t line);

#define TraceError(Error) (TraceErrorImpl(Error, __FILE__, __LINE__))
