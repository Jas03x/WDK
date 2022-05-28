#ifndef WDK_CMEMORY_HPP
#define WDK_CMEMORY_HPP

#include "WdkDef.hpp"

class CMemory
{
protected:
	HANDLE m_hHeap;

public:
	CMemory(VOID);
	~CMemory(VOID);

	BOOL  Initialize(VOID);
	VOID  Uninitialize(VOID);

	PVOID Allocate(SIZE_T nBytes, BOOL bClear);
	BOOL  Release(PVOID pMemory);
};

#endif // WDK_CMEMORY_HPP