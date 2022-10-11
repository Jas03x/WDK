#include "Wdk.hpp"
#include "CMemory.hpp"

#include <assert.h>

#include <windows.h>

HANDLE CMemory::m_hHeap = NULL;
INT64  CMemory::m_nBytesAllocated = 0;

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

	m_nBytesAllocated = 0;

	m_hHeap = GetProcessHeap();

	if (m_hHeap == NULL)
	{
		Status = FALSE;
	}

	return Status;
}

BOOL CMemory::Uninitialize(VOID)
{
	BOOL Status = TRUE;

	m_hHeap = NULL;

	if (m_nBytesAllocated != 0)
	{
		CONST WCHAR ErrorMsg[] = __FUNCTION__ L": ERROR - Memory leak detected\n";

		HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hStdOut != NULL)
		{
			WriteConsole(hStdOut, ErrorMsg, _countof(ErrorMsg), NULL, NULL);
			Status = FALSE;
		}
	}

	return Status;
}

PVOID CMemory::Allocate(SIZE_T nBytes, BOOL bClear)
{
	PVOID pAlloc = NULL;
	DWORD Flags = 0;

	if (bClear)
	{
		Flags |= HEAP_ZERO_MEMORY;
	}

	pAlloc = HeapAlloc(m_hHeap, Flags, nBytes);

	if (pAlloc != NULL)
	{
		m_nBytesAllocated += nBytes;
	}

	return pAlloc;
}

BOOL CMemory::Release(PVOID pMemory)
{
	BOOL Status = TRUE;

	if (pMemory != NULL)
	{
		SIZE_T nBytes = HeapSize(m_hHeap, 0, pMemory);

		if (nBytes != static_cast<SIZE_T>(-1))
		{
			m_nBytesAllocated -= nBytes;
		}
	}

	Status = HeapFree(m_hHeap, 0, pMemory);

	return Status;
}
