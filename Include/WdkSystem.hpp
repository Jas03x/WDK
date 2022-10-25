#ifndef WDK_SYS__HPP
#define WDK_SYS__HPP

#include "WdkDef.hpp"

#undef CreateWindow

enum WIN_AREA
{
	CLIENT = 0,
	WINDOW = 1
};

struct WIN_RECT
{
	UINT x;
	UINT y;
	UINT width;
	UINT height;
};

enum WIN_MSG
{
	INVALID = 0,
	QUIT = 1
};

struct WIN_EVENT
{
	WIN_MSG msg;
};

// Shared structure between system and gfx
#ifndef WDK_RENDER_BUFFER
#define WDK_RENDER_BUFFER
struct RenderBuffer
{
	HANDLE hResource;
	UINT64 CpuDescriptor;
};
#endif // WDK_RENDER_BUFFER

class __declspec(novtable) IWindow
{
public:
	virtual HWND         GetHandle(VOID) = 0;
	virtual BOOL         Open(VOID) = 0;
	virtual BOOL         GetEvent(WIN_EVENT& rEvent) = 0;
	virtual BOOL         GetRect(WIN_AREA area, WIN_RECT& rRect) = 0;

	virtual BOOL         Present(VOID) = 0;
	virtual RenderBuffer GetCurrentRenderBuffer(VOID) = 0;
};

class __declspec(novtable) WindowFactory
{
public:
	static IWindow* CreateInstance(PCWCHAR ClassName, PCWCHAR WindowName, ULONG ClientWidth, ULONG ClientHeight);
	static VOID     DestroyInstance(IWindow* pIWindow);
};

#endif // WDK_SYS__HPP