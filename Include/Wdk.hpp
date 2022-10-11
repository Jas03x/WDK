#ifndef WDK__HPP
#define WDK__HPP

#include "WdkDef.hpp"

INT WdkMain(INT argc, PWCHAR argv);

void WdkAssert(BOOL b, PCWCHAR error, ...);

// Memory
class Memory
{
public:
	static PVOID Allocate(SIZE_T nBytes, BOOL bClear);
	static BOOL  Release(PVOID pMemory);
};

// Console
class Console
{
public:
	static BOOL Write(PCWCHAR Msg, ...);
	static BOOL Write(PCWCHAR Msg, va_list Args);
};

// File
class File
{
public:
	enum MODE
	{
		READ  = 0,
		WRITE = 1
	};

	static File* Open(PCWCHAR Path);
	static File* Open(const FILE_PATH& Path);

	static VOID   Close(File* pIFile);

	virtual BOOL  Read(BYTE** ppBuffer, DWORD* pSize) = 0;
};

// System
namespace System
{
	// Path
	BOOL GetModulePath(PWCHAR pPath, DWORD nSize);
	BOOL GetModuleDirectory(PWCHAR pPath, DWORD nSize);
}

#endif // WDK__HPP