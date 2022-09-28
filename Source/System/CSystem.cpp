#include "Wdk.hpp"
#include "CSystem.hpp"

#include <windows.h>
#include <libloaderapi.h>

BOOL System::GetModulePath(PWCHAR pPath, DWORD nSize)
{
	BOOL Status = TRUE;

	if (pPath == NULL)
	{
		Status = FALSE;
		Console::Write(L"Error: Null path string buffer\n");
	}

	if (Status == TRUE)
	{
		DWORD length = GetModuleFileName(NULL, pPath, nSize);

		if ((length == 0) || ((length == nSize) && (GetLastError() == ERROR_INSUFFICIENT_BUFFER)))
		{
			Status = FALSE;
			Console::Write(L"Error 0x%X: Could not get module path\n", GetLastError());
		}
	}

	return Status;
}

BOOL System::GetModuleDirectory(PWCHAR pPath, DWORD nSize)
{
	BOOL Status = TRUE;

	Status = GetModulePath(pPath, nSize);

	if (Status == TRUE)
	{
		UINT index = 0xFFFFFFFF;

		for (PWCHAR p = pPath; *p != 0; p++)
		{
			if ((*p == '/') || (*p == '\\'))
			{
				index = static_cast<UINT>(p - pPath);
			}
		}

		if (index != 0xFFFFFFFF)
		{
			for (UINT i = index; i < nSize; i++)
			{
				pPath[i] = 0;
			}
		}
	}

	return Status;
}
