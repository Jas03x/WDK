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
struct ID3D12Fence;

class CGfxDevice : public IGfxDevice
{
private:
	enum : UINT { ADAPTER_INDEX__INVALID = 0xFFFFFFFF };
	enum : UINT { NUM_BUFFERS = 2 };
	enum : UINT { MAX_INPUT_ELEMENTS = 32 };
	enum : UINT { COMMAND_QUEUE_TIMEOUT = 1000 };

	IWindow*				   m_pIWindow;
#if _DEBUG
	HMODULE					   m_hDxgiDebugModule;

	IDXGIDebug*				   m_pIDxgiDebugInterface;
	ID3D12Debug*			   m_pID3D12DebugInterface;
#endif

	IDXGIFactory7*			   m_pIDxgiFactory;
	IDXGIAdapter4*			   m_pIDxgiAdapter;
	IDXGISwapChain4*		   m_pIDxgiSwapChain;

	ID3D12Device*			   m_pID3D12Device;

	ID3D12CommandQueue*		   m_pID3D12CommandQueue;
	ID3D12CommandAllocator*    m_pID3D12CopyCommandAllocator;
	ID3D12CommandAllocator*    m_pID3D12DirectCommandAllocator;

	ID3D12RootSignature*       m_pID3D12RootSignature;
	
	ID3D12Heap*				   m_pID3D12UploadHeap;
	ID3D12Heap*				   m_pID3D12PrimaryHeap;
	ID3D12DescriptorHeap*      m_pID3D12RtvDescriptorHeap;

	ID3D12Fence*               m_pID3D12Fence;

	ID3D12Resource*			   m_pID3D12RenderBuffers[NUM_BUFFERS];

	ICommandBuffer*            m_pICopyCommandBuffer;

	HANDLE                     m_hFenceEvent;
	UINT32  				   m_FrameIndex;
	UINT32  				   m_RtvDescriptorIncrement;
	UINT64                     m_FenceValue;

public:
	CGfxDevice(VOID);
	virtual ~CGfxDevice(VOID);

	BOOL				       Initialize(DeviceFactory::Descriptor& rDesc);
	VOID				       Uninitialize(VOID);

private:
	BOOL				       EnumerateDxgiAdapters(VOID);
	BOOL				       PrintAdapterProperties(UINT uIndex, IDXGIAdapter4* pIAdapter);

	BOOL				       PrintDeviceProperties(VOID);

	BOOL                       WaitForCommandQueue(VOID);

public:
	virtual IRenderer*	       CreateRenderer(const RENDERER_DESC& rDesc);
	virtual VOID		       DestroyRenderer(IRenderer* pIRenderer);

	virtual ICommandBuffer*    CreateCommandBuffer(COMMAND_BUFFER_TYPE Type);
	virtual VOID               DestroyCommandBuffer(ICommandBuffer* pICommandBuffer);

	virtual IMesh*		       CreateMesh(CONST VOID* pVertexData, UINT SizeInBytes, UINT StrideInBytes);
	virtual VOID               DestroyMesh(IMesh* pIMesh);
};

#endif // WDK_CGFX_DEVICE_HPP
