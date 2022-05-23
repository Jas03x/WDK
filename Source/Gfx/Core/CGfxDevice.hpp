#ifndef GFX_DEVICE_HPP
#define GFX_DEVICE_HPP

#include "Wdk.hpp"
#include "WdkGfx.hpp"

class IDXGIDebug;
class ID3D12Debug;
class IDXGIFactory7;
class IDXGIAdapter4;
class ID3D12Device;

namespace Wdk
{
	class IWindow;

	class CGfxDevice : public Object, public IGfxDevice
	{
	private:
		IWindow*       m_pIWindow;

#if _DEBUG
		HMODULE        m_hDxgiDebugModule;

		IDXGIDebug*    m_pIDxgiDebugInterface;
		ID3D12Debug*   m_pID3D12DebugInterface;
#endif

		IDXGIFactory7* m_pIDxgiFactory;
		IDXGIAdapter4* m_pIDxgiAdapter;
		ID3D12Device*  m_pIDevice;

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
