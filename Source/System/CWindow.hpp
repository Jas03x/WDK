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
	bool             m_bOpen;
	wchar_t          m_ClassName[256];

	CSwapChain*      m_pSwapChain;

	static LRESULT WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CWindow(void);
	virtual ~CWindow(void);

	bool                 Initialize(const wchar_t* ClassName, const wchar_t* WindowName, uint32_t ClientWidth, uint32_t ClientHeight);
	void                 Uninitialize(void);

public:
	virtual HWND         GetHandle(void);
	virtual bool         Open(void);
	virtual bool         GetEvent(WIN_EVENT& rEvent);
	virtual bool         GetRect(WIN_AREA area, WIN_RECT& rRect);

	bool                 SwapChainNotification(SWAPCHAIN_NOTIFICATION Notification, HANDLE hSwapChain);

	virtual bool         Present(void);
	virtual RenderBuffer GetCurrentRenderBuffer(void);
};

#endif // WDK_CWINDOW_HPP