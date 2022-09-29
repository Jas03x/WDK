#ifndef WDK_CMEMORY_HPP
#define WDK_CMEMORY_HPP

#include "WdkDef.hpp"

class CMemory
{
protected:
	static HANDLE m_hHeap;

public:
	static BOOL Initialize(VOID);
	static VOID Uninitialize(VOID);

	static PVOID Allocate(SIZE_T nBytes, BOOL bClear);
	static BOOL  Release(PVOID pMemory);
};

#endif // WDK_CMEMORY_HPP