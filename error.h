#ifndef ERROR_H
#define ERROR_H

HRESULT AddToErrorTrace(HRESULT hRes, const char *file, unsigned int line);
#define TraceError(hRes) AddToErrorTrace(hRes, __FILE__, __LINE__)
void ClearErrorTrace();

#endif