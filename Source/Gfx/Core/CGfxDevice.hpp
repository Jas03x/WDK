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
struct ID3D12DescriptorHeap;
struct ID3D12Resource;
struct ID3D12Heap;

class CCommandQueue;

class CGfxDevice : public IGfxDevice
{
private:
	enum : UINT { ADAPTER_INDEX__INVALID = 0xFFFFFFFF };
	enum : UINT { MAX_INPUT_ELEMENTS = 32 };

	IWindow*                   m_pIWindow;

#if _DEBUG
	HMODULE					   m_hDxgiDebugModule;

	IDXGIDebug*				   m_pIDxgiDebugInterface;
	ID3D12Debug*			   m_pID3D12DebugInterface;
#endif

	IDXGIFactory7*			   m_pIDxgiFactory;
	IDXGIAdapter4*			   m_pIDxgiAdapter;

	ID3D12Device*			   m_pID3D12Device;
	
	ID3D12Heap*				   m_pID3D12UploadHeap;
	ID3D12Heap*				   m_pID3D12PrimaryHeap;
	ID3D12DescriptorHeap*      m_pID3D12RtvDescriptorHeap;

	CCommandQueue*             m_pCopyQueue;
	CCommandQueue*             m_pGraphicsQueue;

	ICommandBuffer*            m_pICopyCommandBuffer;

	UINT32  				   m_RtvDescriptorIncrement;

public:
	CGfxDevice(VOID);
	virtual ~CGfxDevice(VOID);

	BOOL				       Initialize(IWindow* pIWindow, DeviceFactory::Descriptor& rDesc);
	VOID				       Uninitialize(VOID);

private:
	BOOL				       EnumerateDxgiAdapters(VOID);
	BOOL				       PrintAdapterProperties(UINT uIndex, IDXGIAdapter4* pIAdapter);
	BOOL				       PrintDeviceProperties(VOID);

	BOOL                       InitializeHeaps(DeviceFactory::Descriptor& rDesc);
	BOOL                       InitializeSwapChain(VOID);

	virtual ICommandQueue*     CreateCommandQueue(COMMAND_QUEUE_TYPE Type);
	virtual VOID               DestroyCommandQueue(ICommandQueue* pICommandQueue);

public:
	virtual ICommandBuffer*    CreateCommandBuffer(COMMAND_BUFFER_TYPE Type);
	virtual VOID               DestroyCommandBuffer(ICommandBuffer* pICommandBuffer);

	virtual IRenderer*         CreateRenderer(const RENDERER_DESC& rDesc);
	virtual VOID               DestroyRenderer(IRenderer* pIRenderer);

	virtual IMesh*             CreateMesh(CONST VOID* pVertexData, MESH_DESC& rDesc);
	virtual VOID               DestroyMesh(IMesh* pIMesh);
};

#endif // WDK_CGFX_DEVICE_HPP
