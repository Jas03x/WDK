#ifndef WDK_SYS__HPP
#define WDK_SYS__HPP

#include "WdkDef.hpp"

class IWindow
{
public:
	enum EventID : uint8_t
	{
		INVALID = 0,
		QUIT = 1
	};

	struct Event
	{
		EventID ID;
	};

public:
	static IWindow* Create(LPCWSTR ClassName, LPCWSTR WindowName, ULONG Width, ULONG Height);
	static VOID		Destroy(IWindow* pWindow);

	virtual HWND	GetHandle(VOID) = 0;
	virtual BOOL	Open(VOID) = 0;
	virtual BOOL	GetEvent(Event& rEvent) = 0;
};

#endif // WDK_SYS__HPP