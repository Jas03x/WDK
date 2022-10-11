#include "Wdk.hpp"

#include <cstdarg>

#include "windows.h"

#include "System/CConsole.hpp"
#include "System/CMemory.hpp"

BOOL WdkInitialize(INT argc, PWCHAR argv);
BOOL WdkTerminate(VOID);

void WdkAssert(BOOL b, PCWCHAR error, ...)
{
	if (b != TRUE)
	{
		va_list args;
		va_start(args, error);

		Console::Write(error, args);

		va_end(args);
	}
}

INT main(INT argc, PWCHAR argv)
{
	INT Status = STATUS::SUCCESS;

	if (WdkInitialize(argc, argv) == TRUE)
	{
		Status = WdkMain(argc, argv);
	}
	else
	{
		Status = STATUS::UNSUCCESSFUL;
	}

	if (WdkTerminate() == FALSE)
	{
		if (Status == STATUS::SUCCESS)
		{
			Status = STATUS::UNSUCCESSFUL;
		}
	}

	return Status;
}

BOOL WdkInitialize(INT argc, PWCHAR argv)
{
	BOOL Status = TRUE;

	if (Status == TRUE)
	{
		Status = CMemory::Initialize();
	}

	if (Status == TRUE)
	{
		Status = CConsole::Initialize();
	}

	return Status;
}

BOOL WdkTerminate(VOID)
{
	BOOL Status = TRUE;

	if (CConsole::Uninitialize() != TRUE)
	{
		Status = FALSE;
	}
	
	if (CMemory::Uninitialize() != TRUE)
	{
		Status = FALSE;
	}

	return Status;
}

PVOID operator new(SIZE_T size)
{
	return Memory::Allocate(size, TRUE);
}

VOID operator delete(PVOID ptr)
{
	Memory::Release(ptr);
}
