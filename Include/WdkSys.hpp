#ifndef WDK_SYS__HPP
#define WDK_SYS__HPP

#include "WdkDef.hpp"

enum WIN_MSG
{
	INVALID = 0,
	QUIT = 1
};

struct WinEvent
{
	WIN_MSG msg;
};

class IWindow
{
public:
	static IWindow* Create(LPCWSTR ClassName, LPCWSTR WindowName, ULONG Width, ULONG Height);
	static VOID		Destroy(IWindow* pWindow);

	virtual HWND	GetHandle(VOID) = 0;
	virtual BOOL	Open(VOID) = 0;
	virtual BOOL	GetEvent(WinEvent& rEvent) = 0;
};

#endif // WDK_SYS__HPP