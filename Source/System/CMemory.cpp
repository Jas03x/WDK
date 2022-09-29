#include "Wdk.hpp"
#include "CMemory.hpp"

#include <windows.h>

HANDLE CMemory::m_hHeap = NULL;

PVOID Memory::Allocate(SIZE_T nBytes, BOOL bClear)
{
	return CMemory::Allocate(nBytes, bClear);
}

BOOL Memory::Release(PVOID pMemory)
{
	return CMemory::Release(pMemory);
}

BOOL CMemory::Initialize(VOID)
{
	BOOL Status = TRUE;

	m_hHeap = GetProcessHeap();

	if (m_hHeap == NULL)
	{
		Status = FALSE;
	}

	return Status;
}

VOID CMemory::Uninitialize(VOID)
{
	m_hHeap = NULL;
}

PVOID CMemory::Allocate(SIZE_T nBytes, BOOL bClear)
{
	DWORD Flags = 0;

	if (bClear)
	{
		Flags |= HEAP_ZERO_MEMORY;
	}

	return HeapAlloc(m_hHeap, Flags, nBytes);
}

BOOL CMemory::Release(PVOID pMemory)
{
	BOOL Status = TRUE;

	Status = HeapFree(m_hHeap, 0, pMemory);

	return Status;
}
