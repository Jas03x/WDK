#include "Wdk.hpp"

#include <cstdarg>

#include "windows.h"

#include "processthreadsapi.h"

using namespace Wdk;

BOOL WdkInitialize(INT argc, PWCHAR argv);
VOID WdkTerminate(VOID);

void WdkAssert(BOOL b, LPCWSTR error, ...)
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

VOID WdkTerminate(VOID)
{
	Console::Uninitialize();
	Memory::Uninitialize();
}

PVOID operator new(SIZE_T size)
{
	Console::Write(L"Error: New operator not supported - use Memory::Allocate instead\n");
	ExitProcess(STATUS::NOT_IMPLEMENTED);
	return NULL;
}

VOID operator delete(PVOID ptr)
{
	Console::Write(L"Error: Delete operator not supported - use Memory::Release instead\n");
	ExitProcess(STATUS::NOT_IMPLEMENTED);
}

Object::Object(VOID)
{

}

Object::~Object(VOID)
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
