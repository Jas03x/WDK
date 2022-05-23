#ifndef WDK__HPP
#define WDK__HPP

#include "WdkDef.hpp"

INT WdkMain(INT argc, PWCHAR argv);

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