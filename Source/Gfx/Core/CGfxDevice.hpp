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
struct ID3D12Heap;

class CGfxDevice : public IGfxDevice
{
private:
	enum : UINT { ADAPTER_INDEX__INVALID = 0xFFFFFFFF };
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
	IDXGISwapChain4*		m_pIDxgiSwapChain;

	ID3D12Device*			m_pID3D12Device;
	ID3D12CommandQueue*		m_pID3D12CommandQueue;
	ID3D12DescriptorHeap*	m_pID3D12RtvDescriptorHeap;
	ID3D12CommandAllocator* m_pID3D12CommandAllocator;
	ID3D12RootSignature*	m_pID3D12RootSignature;
	ID3D12Heap*				m_pID3D12UploadHeap;

	ID3D12Resource*			m_pID3D12RenderBuffers[NUM_BUFFERS];

	UINT					m_FrameIndex;
	UINT					m_RtvDescriptorIncrement;

private:
	CGfxDevice(VOID);
	virtual ~CGfxDevice(VOID);

	BOOL				  Initialize(IGfxDevice::Desc& rDesc);
	VOID				  Uninitialize(VOID);

	BOOL				  EnumerateDxgiAdapters(VOID);
	BOOL				  PrintAdapterProperties(UINT uIndex, IDXGIAdapter4* pIAdapter);

	BOOL				  PrintDeviceProperties(VOID);

public:
	static CGfxDevice*	  CreateInstance(IGfxDevice::Desc& rDesc);
	static VOID			  DestroyInstance(CGfxDevice* pDevice);

public:
	virtual IRenderer*	  CreateRenderer(const RENDERER_DESC& rDesc);
	virtual VOID		  DestroyRenderer(IRenderer* pIRenderer);

	virtual ICommandList* CreateCommandList(VOID);
	virtual VOID          DestroyCommandList(ICommandList* pICommandList);

	virtual IFence*		  CreateFence(VOID);
	virtual VOID          DestroyFence(IFence* pIFence);
};

#endif // WDK_CGFX_DEVICE_HPP
