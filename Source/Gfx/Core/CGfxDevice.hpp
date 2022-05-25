#ifndef GFX_DEVICE_HPP
#define GFX_DEVICE_HPP

#include "Wdk.hpp"
#include "WdkGfx.hpp"

class IDXGIDebug;
class IDXGIFactory7;
class IDXGIAdapter4;
class IDXGISwapChain4;

class ID3D12Debug;
class ID3D12Device;
class ID3D12CommandQueue;
class ID3D12DescriptorHeap;
class ID3D12CommandAllocator;
class ID3D12Resource;

namespace Wdk
{
	class CGfxDevice : public Object, public IGfxDevice
	{
	private:
		enum : uint32_t { NumBuffers = 2 };

		IWindow*				m_pIWindow;
#if _DEBUG
		HMODULE					m_hDxgiDebugModule;

		IDXGIDebug*				m_pIDxgiDebugInterface;
		ID3D12Debug*			m_pID3D12DebugInterface;
#endif

		IDXGIFactory7*			m_pIDxgiFactory;
		IDXGIAdapter4*			m_pIDxgiAdapter;
		IDXGISwapChain4*		m_pISwapChain;

		ID3D12Device*			m_pIDevice;
		ID3D12CommandQueue*		m_pICommandQueue;
		ID3D12DescriptorHeap*	m_pIRtvDescriptorHeap;
		ID3D12CommandAllocator* m_pICommandAllocator;

		ID3D12Resource*			m_pIRenderBuffers[NumBuffers];

		uint32_t				m_FrameIndex;
		uint32_t				m_RtvDescriptorIncrement;

	private:
		CGfxDevice(VOID);
		virtual ~CGfxDevice(VOID);

		BOOL Initialize(IWindow* pIWindow);
		VOID Uninitialize(VOID);

		BOOL EnumerateDxgiAdapters(VOID);
		BOOL PrintAdapterDesc(UINT uIndex, IDXGIAdapter4* pIAdapter);

	public:
		static CGfxDevice* Create(IWindow* pIWindow);
		static VOID        Destroy(CGfxDevice* pDevice);

	public:
	};
}

#endif // GFX_DEVICE_HPP
