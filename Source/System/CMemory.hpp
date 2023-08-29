#ifndef WDK_CMEMORY_HPP
#define WDK_CMEMORY_HPP

#include "WdkDef.hpp"

class CMemory
{
protected:
	static HANDLE   m_hHeap;
	static uint64_t m_nBytesAllocated;

public:
	static bool Initialize(void);
	static bool Uninitialize(void);

	static void* Allocate(size_t nBytes, bool bClear);
	static bool  Release(void* pMemory);
};

#endif // WDK_CMEMORY_HPP