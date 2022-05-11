#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Wdk.hpp"
#include "WdkSys.hpp"

class CWindow : public IWindow, public Object
{
private:
	HINSTANCE m_hInstance;
	ATOM m_hCID;
	HWND m_hWnd;
	BOOL m_bOpen;
	WCHAR m_ClassName[256];

	static LRESULT WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	CWindow();
	~CWindow();

	BOOL Initialize(LPCWSTR ClassName, LPCWSTR WindowName, ULONG Width, ULONG Height);
	VOID Uninitialize(VOID);

public:
	static CWindow* Create(LPCWSTR ClassName, LPCWSTR WindowName, ULONG Width, ULONG Height);
	static VOID Destroy(CWindow* pWindow);

public:
	virtual HWND GetHandle(VOID);
	virtual BOOL Open(VOID);
	virtual BOOL GetEvent(Event& rEvent);
};

#endif // WINDOW_HPP