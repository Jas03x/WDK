#include "CWindow.hpp"

#include <strsafe.h>

#undef CreateWindow

using namespace Wdk;

IWindow* Wdk::CreateWindow(LPCWSTR ClassName, LPCWSTR WindowName, ULONG ClientWidth, ULONG ClientHeight)
{
	return CWindow::Create(ClassName, WindowName, ClientWidth, ClientHeight);
}

VOID Wdk::DestroyWindow(IWindow* pWindow)
{
	return CWindow::Destroy(static_cast<CWindow*>(pWindow));
}

CWindow* CWindow::Create(LPCWSTR ClassName, LPCWSTR WindowName, ULONG Width, ULONG Height)
{
	CWindow* pWindow = new CWindow();
	
	if (pWindow != NULL)
	{
		if (pWindow->Initialize(ClassName, WindowName, Width, Height) == FALSE)
		{
			CWindow::Destroy(pWindow);
			pWindow = NULL;
		}
	}

	return pWindow;
}

VOID CWindow::Destroy(CWindow* pWindow)
{
	if (pWindow != NULL)
	{
		pWindow->Uninitialize();

		delete pWindow;
		pWindow = NULL;
	}
}

CWindow::CWindow(VOID)
{
	m_hCID = 0;
	m_bOpen = FALSE;

	m_hWnd = NULL;
	m_hInstance = NULL;
	
	ZeroMemory(m_ClassName, sizeof(m_ClassName));
}

CWindow::~CWindow(VOID)
{
}

BOOL CWindow::Initialize(LPCWSTR ClassName, LPCWSTR WindowName, ULONG ClientWidth, ULONG ClientHeight)
{
	BOOL Status = TRUE;

	m_hInstance = GetModuleHandle(NULL);
	StringCchCopy(m_ClassName, sizeof(m_ClassName) / sizeof(WCHAR), ClassName);

	WNDCLASSEX wndClassEx = { 0 };
	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
	wndClassEx.lpfnWndProc = WindowProcedure;
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbWndExtra = 0;
	wndClassEx.hInstance = m_hInstance;
	wndClassEx.hIcon = NULL;
	wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClassEx.lpszMenuName = NULL;
	wndClassEx.lpszClassName = ClassName;
	wndClassEx.hIconSm = NULL;

	HWND hDesktopWindow = GetDesktopWindow();

	RECT wndRect = { };
	GetClientRect(hDesktopWindow, &wndRect);
	wndRect.left = ((wndRect.right - wndRect.left) - ClientWidth) / 2;
	wndRect.top = ((wndRect.bottom - wndRect.top) - ClientHeight) / 2;
	wndRect.right = (wndRect.left + ClientWidth);
	wndRect.bottom = (wndRect.top + ClientHeight);

	// The window size includes the borders, top bar, etc.
	// The client area is the area with the contents of the window.
	// We use AdjustWindowRect to calculate the window size based off the required client size.
	if (AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, FALSE) != TRUE)
	{
		Status = FALSE;
		Console::Write(L"Error: Could not calculate window bounds\n");
	}

	if (Status == TRUE)
	{
		m_hCID = RegisterClassEx(&wndClassEx);

		if (m_hCID == 0)
		{
			Status = FALSE;
			Console::Write(L"Error: Could not register class\n");
		}
	}

	if (Status == TRUE)
	{
		m_hWnd = CreateWindowEx(0, ClassName, WindowName, WS_OVERLAPPEDWINDOW, wndRect.left, wndRect.top, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top, NULL, NULL, m_hInstance, NULL);

		if (m_hWnd == NULL)
		{
			Status = FALSE;
			Console::Write(L"Error: Could not create window\n");
		}
	}

	if (Status == TRUE)
	{
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	}

	if (Status == TRUE)
	{
		m_bOpen = TRUE;
		ShowWindow(m_hWnd, SW_SHOW);
	}

	return Status;
}

VOID CWindow::Uninitialize(VOID)
{
	if (m_hWnd != NULL)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	UnregisterClass(m_ClassName, m_hInstance);
}

HWND CWindow::GetHandle(VOID)
{
	return m_hWnd;
}

BOOL CWindow::Open(VOID)
{
	return m_bOpen;
}

LRESULT CWindow::WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 0;
	CWindow* pWindow = reinterpret_cast<CWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
		case WM_CLOSE:
		{
			pWindow->m_bOpen = FALSE;
			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}

		default:
		{
			Result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
	}

	return Result;
}

BOOL CWindow::GetEvent(WinEvent& rEvent)
{
	BOOL Status = FALSE;
	MSG msg = { 0 };

	rEvent.msg = WIN_MSG::INVALID;

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != FALSE)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			rEvent.msg = WIN_MSG::QUIT;
		}
	}

	return Status;
}

BOOL CWindow::GetRect(WIN_AREA area, WinRect& rRect)
{
	BOOL Status = TRUE;
	RECT rect = {};

	if (area == WIN_AREA::CLIENT)
	{
		Status = (GetClientRect(m_hWnd, &rect) != TRUE) ? FALSE : TRUE;
	}
	else
	{
		Status = (GetWindowRect(m_hWnd, &rect) != TRUE) ? FALSE : TRUE;
	}

	if (Status == TRUE)
	{
		rRect.x = rect.left;
		rRect.y = rect.top;
		rRect.width = rect.right - rect.left;
		rRect.height = rect.bottom - rect.top;
	}

	return Status;
}
