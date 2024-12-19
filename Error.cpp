#include "Platform.h"
#include "main.h"
#include "config.h"
#include "error.h"
#include "ConfigItem.h"

Error::Error(HRESULT hRes)
{
	char sBuf[100];
	sprintf(sBuf, "Error %08x\n", hRes);
	Message = sBuf;
}

Error::~Error()
{
}

Error* Error::AddTrace(const char* file, uint32_t line)
{
	Trace.push_back(std::make_pair(file, line));
	return this;
}

std::string Error::GetDescription() const
{
	std::string description = Message;
	description += "Trace: ";
	if (Trace.size() > 0)
	{
		for (unsigned int i = 0; i < Trace.size(); i++)
		{
			if (i > 0) description += ", ";

			const char* sPos = strrchr(Trace[i].first, '\\');
			if (sPos == NULL) sPos = Trace[i].first;
			else sPos++;

			char sBuf[256];
			sprintf(sBuf, "%s(%d)", sPos, Trace[i].second);
			description += sBuf;
		}
	}
	else
	{
		description += "(none)";
	}
	return description;
}

// ---------------------------------------------------------

Error* TraceErrorImpl(Error* error, const char* file, uint32_t line)
{
	if (error != nullptr)
	{
		error->AddTrace(file, line);
	}
	return error;
}

Error* TraceErrorImpl(HRESULT hRes, const char* file, uint32_t line)
{
	if (FAILED(hRes))
	{
		return TraceErrorImpl(new Error(hRes), file, line);
	}
	else
	{
		return nullptr;
	}
}
