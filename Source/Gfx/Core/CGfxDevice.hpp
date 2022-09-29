#ifndef WDK_CGFX_DEVICE_HPP
#define WDK_CGFX_DEVICE_HPP

#include "Wdk.hpp"
#include "WdkGfx.hpp"

struct IDXGIDebug;
struct IDXGIFactory7;
struct IDXGIAdapter4;
struct IDXGISwapChain4;

struct ID3D12Debug;
struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12CommandAllocator;
struct ID3D12RootSignature;
struct ID3D12Resource;

class CGfxDevice : public IGfxDevice
{
private:
	enum : UINT { NUM_BUFFERS = 2 };
	enum : UINT { MAX_INPUT_ELEMENTS = 32 };

	IWindow*				m_pIWindow;
#if _DEBUG
	HMODULE					m_hDxgiDebugModule;

	IDXGIDebug*				m_pIDxgiDebugInterface;
	ID3D12Debug*			m_pID3D12DebugInterface;
#endif

	IDXGIFactory7*			m_pIDxgiFactory;
	IDXGIAdapter4*			m_pIDxgiAdapter;
	IDXGISwapChain4*		m_pISwapChain;

	ID3D12Device*			m_pInterface;
	ID3D12CommandQueue*		m_pICommandQueue;
	ID3D12DescriptorHeap*	m_pIRtvDescriptorHeap;
	ID3D12CommandAllocator* m_pICommandAllocator;
	ID3D12RootSignature*	m_pIRootSignature;

	ID3D12Resource*			m_pIRenderBuffers[NUM_BUFFERS];

	UINT					m_FrameIndex;
	UINT					m_RtvDescriptorIncrement;

private:
	CGfxDevice(VOID);
	virtual ~CGfxDevice(VOID);

	BOOL Initialize(IWindow* pIWindow);
	VOID Uninitialize(VOID);

	BOOL EnumerateDxgiAdapters(VOID);
	BOOL PrintAdapterDesc(UINT uIndex, IDXGIAdapter4* pIAdapter);		

public:
	static CGfxDevice* CreateInstance(IWindow* pIWindow);
	static VOID        DeleteInstance(CGfxDevice* pDevice);

public:
	virtual IRenderer* CreateRenderer(const RENDERER_DESC& rDesc);
	virtual VOID       DestroyRenderer(IRenderer* pIRenderer);

	virtual ICommandList* CreateCommandList(VOID);
	virtual VOID          DestroyCommandList(ICommandList* pICommandList);
};

#endif // WDK_CGFX_DEVICE_HPP
