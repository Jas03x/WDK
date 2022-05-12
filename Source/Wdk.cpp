#include "Wdk.hpp"

#include "ntstatus.h"
#include "windows.h"

#include "processthreadsapi.h"

BOOL WdkInitialize(INT argc, PWCHAR argv);
VOID WdkTerminate();

INT main(INT argc, PWCHAR argv)
{
	BOOL Status = TRUE;

	if (WdkInitialize(argc, argv) != TRUE)
	{
		Status = FALSE;
	}

	if (Status == TRUE)
	{
		if (WdkMain(argc, argv) != TRUE)
		{
			Status = FALSE;
		}
	}

	WdkTerminate();

	return ((Status == TRUE) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);
}

BOOL WdkInitialize(INT argc, PWCHAR argv)
{
	BOOL Status = TRUE;

	if (Status == TRUE)
	{
		Status = Memory::Initialize();
	}

	if (Status == TRUE)
	{
		Status = Console::Initialize();
	}

	return Status;
}

VOID WdkTerminate()
{
	Console::Uninitialize();
	Memory::Uninitialize();
}

PVOID operator new(SIZE_T size)
{
	Console::Write(L"New operator not supported - use Memory::Allocate instead\n");
	ExitProcess(STATUS_NOT_IMPLEMENTED);
	return nullptr;
}

VOID operator delete(PVOID ptr)
{
	Console::Write(L"Delete operator not supported - use Memory::Release instead\n");
	ExitProcess(STATUS_NOT_IMPLEMENTED);
}

Object::Object()
{

}

Object::~Object()
{

}

PVOID Object::operator new(SIZE_T size)
{
	return Memory::Allocate(size, TRUE);
}

VOID Object::operator delete(PVOID ptr)
{
	Memory::Release(ptr);
}
