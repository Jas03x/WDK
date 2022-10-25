#ifndef WDK_CWINDOW_HPP
#define WDK_CWINDOW_HPP

#include "Windows.h"

#include "Wdk.hpp"
#include "WdkSystem.hpp"

class CSwapChain;

class CWindow : public IWindow
{
public:
	enum SWAPCHAIN_NOTIFICATION
	{
		SWAPCHAIN_DESTROYED = 0,
		SWAPCHAIN_CREATED   = 1
	};

private:
	HINSTANCE        m_hInstance;
	ATOM             m_hCID;
	HWND             m_hWnd;
	BOOL             m_bOpen;
	WCHAR            m_ClassName[256];

	CSwapChain*      m_pSwapChain;

	static LRESULT WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CWindow(VOID);
	virtual ~CWindow(VOID);

	BOOL                 Initialize(PCWCHAR ClassName, PCWCHAR WindowName, ULONG ClientWidth, ULONG ClientHeight);
	VOID                 Uninitialize(VOID);

public:
	virtual HWND         GetHandle(VOID);
	virtual BOOL         Open(VOID);
	virtual BOOL         GetEvent(WIN_EVENT& rEvent);
	virtual BOOL         GetRect(WIN_AREA area, WIN_RECT& rRect);

	BOOL                 SwapChainNotification(SWAPCHAIN_NOTIFICATION Notification, HANDLE hSwapChain);

	virtual BOOL         Present(VOID);
	virtual RenderBuffer GetCurrentRenderBuffer(VOID);
};

#endif // WDK_CWINDOW_HPP