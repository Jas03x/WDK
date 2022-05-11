#ifndef WDK_HPP
#define WDK_HPP

#include "WdkDef.hpp"

class Object
{
public:
	Object();
	~Object();

	PVOID operator new(SIZE_T size);
	VOID  operator delete(PVOID ptr);
};

class Memory
{
public:
	static BOOL  Initialize(VOID);
	static VOID  Uninitialize(VOID);

	static PVOID Allocate(SIZE_T nBytes, BOOL bClear);
	static BOOL  Free(PVOID pMemory);
};

class Console
{
public:
	static BOOL Initialize(VOID);
	static VOID Uninitialize(VOID);

	static BOOL Write(LPCWSTR Msg, ...);
};

#endif // WDK_HPP