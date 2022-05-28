#ifndef WDK__HPP
#define WDK__HPP

#include "WdkDef.hpp"

INT WdkMain(INT argc, PWCHAR argv);

void WdkAssert(BOOL b, LPCWSTR error, ...);

class Memory
{
public:
	static BOOL  Initialize(VOID);
	static VOID  Uninitialize(VOID);

	static PVOID Allocate(SIZE_T nBytes, BOOL bClear);
	static BOOL  Release(PVOID pMemory);
};

class Console
{
public:
	static BOOL Initialize(VOID);
	static VOID Uninitialize(VOID);

	static BOOL Write(LPCWSTR Msg, ...);
	static BOOL Write(LPCWSTR Msg, va_list Args);
};

namespace Wdk
{
	class Object
	{
	public:
		Object();
		~Object();

		PVOID operator new(SIZE_T size);
		VOID  operator delete(PVOID ptr);
	};
}

#endif // WDK__HPP