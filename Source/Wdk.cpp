#include "Wdk.hpp"

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
	Memory::Free(ptr);
}
