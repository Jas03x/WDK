#ifndef WDK_CWINDOW_HPP
#define WDK_CWINDOW_HPP

#include "Windows.h"

#include "Wdk.hpp"
#include "WdkSystem.hpp"

struct ID3D12Resource;
struct IDXGISwapChain4;

class CWindow : public IWindow
{
private:
	enum : UINT { NUM_BUFFERS = 2 };

	HINSTANCE        m_hInstance;
	ATOM             m_hCID;
	HWND             m_hWnd;
	BOOL             m_bOpen;
	WCHAR            m_ClassName[256];

	UINT             m_FrameIndex;

	IDXGISwapChain4* m_pIDxgiSwapChain;

	ID3D12Resource*  m_pID3D12RenderBuffers[NUM_BUFFERS];
	UINT64			 m_RenderBufferCpuDescriptors[NUM_BUFFERS];

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

	UINT                 GetNumBuffers(VOID);
	BOOL                 InitializeSwapChain(IDXGISwapChain4* pIDxgiSwapChain);
	VOID                 ReleaseSwapChain(VOID);

	virtual BOOL         Present(VOID);
	virtual RenderBuffer GetCurrentRenderBuffer(VOID);
	VOID                 SetRenderBuffer(UINT uIndex, ID3D12Resource* pIRenderBuffer, UINT64 CpuDescriptor);
};

#endif // WDK_CWINDOW_HPP