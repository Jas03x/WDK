#ifndef WDK_CMEMORY_HPP
#define WDK_CMEMORY_HPP

#include "WdkDef.hpp"

class CMemory
{
protected:
	static HANDLE m_hHeap;
	static INT64  m_nBytesAllocated;

public:
	static BOOL Initialize(VOID);
	static BOOL Uninitialize(VOID);

	static PVOID Allocate(SIZE_T nBytes, BOOL bClear);
	static BOOL  Release(PVOID pMemory);
};

#endif // WDK_CMEMORY_HPP