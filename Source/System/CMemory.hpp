#ifndef CMEMORY_HPP
#define CMEMORY_HPP

#include "WdkDef.hpp"

class CMemory
{
protected:
	HANDLE m_hHeap;

public:
	CMemory();
	~CMemory();

	BOOL  Initialize(VOID);
	VOID  Uninitialize(VOID);

	PVOID Allocate(SIZE_T nBytes, BOOL bClear);
	BOOL  Release(PVOID pMemory);
};

#endif // CMEMORY_HPP