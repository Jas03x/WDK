#ifndef WDK_CWINDOW_HPP
#define WDK_CWINDOW_HPP

#include "Windows.h"

#include "Wdk.hpp"
#include "WdkSystem.hpp"

namespace Wdk
{
	class CWindow : public Object, public IWindow
	{
	private:
		HINSTANCE m_hInstance;
		ATOM m_hCID;
		HWND m_hWnd;
		BOOL m_bOpen;
		WCHAR m_ClassName[256];

		static LRESULT WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		CWindow(VOID);
		virtual ~CWindow(VOID);

		BOOL Initialize(PCWCHAR ClassName, PCWCHAR WindowName, ULONG ClientWidth, ULONG ClientHeight);
		VOID Uninitialize(VOID);

	public:
		static CWindow* Create(PCWCHAR ClassName, PCWCHAR WindowName, ULONG ClientWidth, ULONG ClientHeight);
		static VOID Destroy(CWindow* pWindow);

	public:
		virtual HWND GetHandle(VOID);
		virtual BOOL Open(VOID);
		virtual BOOL GetEvent(WinEvent& rEvent);
		virtual BOOL GetRect(WIN_AREA area, WinRect& rRect);
	};
}

#endif // WDK_CWINDOW_HPP