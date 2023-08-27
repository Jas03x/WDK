#ifndef WDK__HPP
#define WDK__HPP

#include "WdkDef.hpp"

INT WdkMain(INT argc, CONST_CWSTR argv[]);

void WdkAssert(BOOL b, CONST_CWSTR error, ...);

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
	static BOOL Write(CONST_CWSTR Msg, ...);
	static BOOL Write(CONST_CWSTR Msg, VA_LIST Args);
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

	static File*  Open(CONST_CWSTR Path);
	static File*  Open(CONST FILE_PATH& Path);

	static VOID   Close(File* pIFile);

	virtual BOOL  Read(BYTE** ppBuffer, DWORD* pSize) = 0;
};

// System
class System
{
public:
	// Path
	static BOOL GetModulePath(CWSTR pPath, DWORD nSize);
	static BOOL GetModuleDirectory(CWSTR pPath, DWORD nSize);
};

#endif // WDK__HPP