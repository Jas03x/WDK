#ifndef WDK_CSWAPCHAIN_HPP
#define WDK_CSWAPCHAIN_HPP

#include "WdkDef.hpp"
#include "WdkGfx.hpp"

struct IDXGISwapChain4;

struct ID3D12Resource;
struct ID3D12DescriptorHeap;

class CSwapChain
{
public:
	enum : UINT { NUM_BUFFERS = 2 };

	struct Descriptor
	{
		UINT         RtvDescriptorIncrement;
		RenderBuffer RenderBuffers[NUM_BUFFERS];
	};

private:
	UINT                  m_FrameIndex;

	IDXGISwapChain4*      m_pIDxgiSwapChain;

	ID3D12DescriptorHeap* m_pID3D12DescriptorHeap;
	ID3D12Resource*       m_pID3D12RenderBuffers[NUM_BUFFERS];
	
	UINT                  m_RtvDescriptorIncrement;
	UINT64			      m_RenderBufferCpuDescriptors[NUM_BUFFERS];

public:
	CSwapChain(VOID);
	~CSwapChain(VOID);

	BOOL Initialize(IDXGISwapChain4* pIDxgiSwapChain, ID3D12DescriptorHeap* pIRtvDescriptorHeap, const Descriptor& rDesc);
	VOID Uninitialize(VOID);

	UINT GetNumBuffers(VOID);
	VOID GetCurrentRenderBuffer(HANDLE& hResource, UINT64& CpuDescriptor);
	BOOL Present(VOID);
};

#endif // WDK_CSWAPCHAIN_HPP