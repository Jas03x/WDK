#ifndef WDK_CGFX_DEVICE_HPP
#define WDK_CGFX_DEVICE_HPP

#include "WdkGfx.hpp"

struct IDXGIDebug;
struct IDXGIFactory7;
struct IDXGIAdapter4;
struct IDXGISwapChain4;

struct ID3D12Debug;
struct ID3D12Device;
struct ID3D12Resource;
struct ID3D12Heap;
struct ID3D12DescriptorHeap;

class CCommandQueue;
class CSwapChain;

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

	ID3D12Device9*			   m_pID3D12Device;
	
	ID3D12Heap*				   m_pID3D12UploadHeap;
	ID3D12Heap*				   m_pID3D12PrimaryHeap;

	ID3D12DescriptorHeap*      m_pID3D12ShaderResourceDescriptorHeap;

	CCommandQueue*             m_pCopyQueue;
	CCommandQueue*             m_pGraphicsQueue;

	CSwapChain*                m_pSwapChain;

	ICommandBuffer*            m_pICopyCommandBuffer;

public:
	CGfxDevice(VOID);
	virtual ~CGfxDevice(VOID);

	BOOL				       Initialize(IWindow* pIWindow, const DeviceFactory::Descriptor& rDesc);
	VOID				       Uninitialize(VOID);

private:
	BOOL				       EnumerateDxgiAdapters(VOID);
	BOOL				       PrintAdapterProperties(UINT uIndex, IDXGIAdapter4* pIAdapter);
	BOOL				       PrintDeviceProperties(VOID);

	BOOL                       InitializeHeaps(const DeviceFactory::Descriptor& rDesc);
	BOOL                       InitializeDescriptorHeaps(VOID);
	BOOL                       InitializeSwapChain(VOID);

	ICommandQueue*             CreateCommandQueue(COMMAND_QUEUE_TYPE Type);
	VOID                       DestroyCommandQueue(ICommandQueue* pICommandQueue);

public:
	virtual ICommandBuffer*    CreateCommandBuffer(COMMAND_BUFFER_TYPE Type);
	virtual VOID               DestroyCommandBuffer(ICommandBuffer* pICommandBuffer);

	virtual IRendererState*    CreateRendererState(CONST RENDERER_STATE_DESC& rDesc);
	virtual VOID               DestroyRendererState(IRendererState* pIRendererState);

	virtual IConstantBuffer*   CreateConstantBuffer(CONST CONSTANT_BUFFER_DESC& rDesc);
	virtual VOID               DestroyConstantBuffer(IConstantBuffer* pIConstantBuffer);

	virtual IVertexBuffer*     CreateVertexBuffer(CONST VOID* pVertexData, CONST VERTEX_BUFFER_DESC& rDesc);
	virtual VOID               DestroyVertexBuffer(IVertexBuffer* pIVertexBuffer);

	virtual IMesh*             CreateMesh(CONST VOID* pVertexData, CONST VERTEX_BUFFER_DESC& rDesc);
	virtual VOID               DestroyMesh(IMesh* pIMesh);

	virtual BOOL               SubmitCommandBuffer(ICommandBuffer* pICommandBuffer);
	virtual BOOL               SyncQueue(COMMAND_QUEUE_TYPE Type);

	ID3D12DescriptorHeap*      GetID3D12ShaderResourceDescriptorHeap();
};

#endif // WDK_CGFX_DEVICE_HPP
