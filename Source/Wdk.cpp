#include "Wdk.hpp"

#include "windows.h"

#include "processthreadsapi.h"

BOOL WdkInitialize(INT argc, PWCHAR argv);
VOID WdkTerminate();

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

	WdkTerminate();

	return Status;
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
	ExitProcess(STATUS::NOT_IMPLEMENTED);
	return NULL;
}

VOID operator delete(PVOID ptr)
{
	Console::Write(L"Delete operator not supported - use Memory::Release instead\n");
	ExitProcess(STATUS::NOT_IMPLEMENTED);
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
