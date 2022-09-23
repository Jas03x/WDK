#include "CGfxDevice.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <math.h>

#include "WdkSystem.hpp"

using namespace Wdk;

IGfxDevice* Wdk::CreateDevice(IWindow* pIWindow)
{
	return CGfxDevice::Create(pIWindow);
}

VOID Wdk::DestroyDevice(IGfxDevice* pDevice)
{
	CGfxDevice::Destroy(static_cast<CGfxDevice*>(pDevice));
}

CGfxDevice* CGfxDevice::Create(IWindow* pIWindow)
{
	CGfxDevice* pDevice = new CGfxDevice();

	if (pDevice != NULL)
	{
		if (pDevice->Initialize(pIWindow) == FALSE)
		{
			DestroyDevice(pDevice);
			pDevice = NULL;
		}
	}

	return pDevice;
}

VOID CGfxDevice::Destroy(CGfxDevice* pDevice)
{
	if (pDevice != NULL)
	{
		pDevice->Uninitialize();

		delete pDevice;
		pDevice = NULL;
	}
}

CGfxDevice::CGfxDevice(VOID)
{
	m_pIWindow = NULL;

	m_hDxgiDebugModule = NULL;

	m_pIDxgiDebugInterface = NULL;
	m_pID3D12DebugInterface = NULL;

	m_pIDxgiFactory = NULL;
	m_pIDxgiAdapter = NULL;
	m_pISwapChain = NULL;

	m_pIDevice = NULL;
	m_pICommandQueue = NULL;
	m_pIRtvDescriptorHeap = NULL;
	m_pICommandAllocator = NULL;
	m_pIRootSignature = NULL;

	for (uint32_t i = 0; i < NumBuffers; i++)
	{
		m_pIRenderBuffers[i] = NULL;
	}

	m_FrameIndex = 0;
	m_RtvDescriptorIncrement = 0;
}

CGfxDevice::~CGfxDevice(VOID)
{

}

BOOL CGfxDevice::Initialize(IWindow* pIWindow)
{
	BOOL Status = TRUE;

	m_pIWindow = pIWindow;

#if _DEBUG
	if (D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<VOID**>(&m_pID3D12DebugInterface)) == S_OK)
	{
		m_pID3D12DebugInterface->EnableDebugLayer();
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Failed to get dx12 debug interface\n");
	}

	if (Status == TRUE)
	{
		m_hDxgiDebugModule = GetModuleHandle(L"dxgidebug.dll");
		HRESULT (*pfnDxgiGetDebugInterface)(REFIID, VOID**) = NULL;

		if (m_hDxgiDebugModule != NULL)
		{
			pfnDxgiGetDebugInterface = reinterpret_cast<HRESULT(*)(REFIID, VOID**)>(GetProcAddress(m_hDxgiDebugModule, "DXGIGetDebugInterface"));

			if (pfnDxgiGetDebugInterface == NULL)
			{
				Console::Write(L"Error: Could not find function DXGIGetDebugInterface in the dxgi debug module\n");
				Status = FALSE;
			}
		}
		else
		{
			Console::Write(L"Error: Could not load the dxgi debug module\n");
			Status = FALSE;
		}

		if (Status == TRUE)
		{
			if (pfnDxgiGetDebugInterface(__uuidof(IDXGIDebug), reinterpret_cast<VOID**>(&m_pIDxgiDebugInterface)) != S_OK)
			{
				Console::Write(L"Error: Failed to get dxgi debug interface\n");
				Status = FALSE;
			}
		}
	}
#endif

	if (Status == TRUE)
	{
		UINT Flags = 0;

#if _DEBUG
		Flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		if (CreateDXGIFactory2(Flags, __uuidof(IDXGIFactory7), reinterpret_cast<VOID**>(&m_pIDxgiFactory)) != S_OK)
		{
			Status = FALSE;
			Console::Write(L"Error: Failed to create dxgi factory\n");
		}
	}

	if (Status == TRUE)
	{
		Status = EnumerateDxgiAdapters();

		if (Status == FALSE)
		{
			Status = FALSE;
			Console::Write(L"Error: Could not get dxgi adapter\n");
		}
	}

	if (Status == TRUE)
	{
		D3D12CreateDevice(m_pIDxgiAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), reinterpret_cast<VOID**>(&m_pIDevice));

		if (m_pIDevice == NULL)
		{
			Status = FALSE;
			Console::Write(L"Error: Could not create a DX12 device\n");
		}
	}

	if (Status == TRUE)
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = { };
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.NodeMask = 0;

		if (m_pIDevice->CreateCommandQueue(&cmdQueueDesc, __uuidof(ID3D12CommandQueue), reinterpret_cast<VOID**>(&m_pICommandQueue)) != S_OK)
		{
			Status = FALSE;
			Console::Write(L"Error: Failed to create command queue\n");
		}
	}

	if (Status == TRUE)
	{
		WinRect rect = {};
		
		if (m_pIWindow->GetRect(WIN_AREA::CLIENT, rect) == TRUE)
		{
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { };
			swapChainDesc.Width = rect.width;
			swapChainDesc.Height = rect.height;
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.Stereo = FALSE;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = NumBuffers;
			swapChainDesc.Scaling = DXGI_SCALING_NONE;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			swapChainDesc.Flags = 0;

			IDXGISwapChain1* pISwapChain1 = NULL;

			if (m_pIDxgiFactory->CreateSwapChainForHwnd(m_pICommandQueue, m_pIWindow->GetHandle(), &swapChainDesc, NULL, NULL, &pISwapChain1) == S_OK)
			{
				pISwapChain1->QueryInterface(__uuidof(IDXGISwapChain4), reinterpret_cast<VOID**>(&m_pISwapChain));
				pISwapChain1->Release();

				m_FrameIndex = m_pISwapChain->GetCurrentBackBufferIndex();
			}
			else
			{
				Status = FALSE;
				Console::Write(L"Error: Failed to create swap chain\n");
			}
		}
		else
		{
			Status = FALSE;
		}
	}

	if (Status == TRUE)
	{
		D3D12_DESCRIPTOR_HEAP_DESC descHeap = {};
		descHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		descHeap.NumDescriptors = NumBuffers;
		descHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descHeap.NodeMask = 0;

		if (m_pIDevice->CreateDescriptorHeap(&descHeap, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<VOID**>(&m_pIRtvDescriptorHeap)) == S_OK)
		{
			m_RtvDescriptorIncrement = m_pIDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}
		else
		{
			Console::Write(L"Error: Failed to create descriptor heap\n");
			Status = FALSE;
		}
	}

	if (Status == TRUE)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle = m_pIRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		for (UINT i = 0; (Status == TRUE) && (i < NumBuffers); i++)
		{
			if (m_pISwapChain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<VOID**>(&m_pIRenderBuffers[i])) != S_OK)
			{
				Console::Write(L"Error: Could not get swap chain buffer %u\n", i);
				Status = FALSE;
			}

			m_pIDevice->CreateRenderTargetView(m_pIRenderBuffers[i], NULL, cpuDescHandle);

			cpuDescHandle.ptr += m_RtvDescriptorIncrement;
		}
	}

	if (Status == TRUE)
	{
		if (m_pIDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), reinterpret_cast<VOID**>(&m_pICommandAllocator)) != S_OK)
		{
			Console::Write(L"Error: Could not create command allocator\n");
			Status = FALSE;
		}
	}

	if (Status == TRUE)
	{
		D3D12_ROOT_SIGNATURE_DESC desc = { };
		desc.NumParameters = 0;
		desc.pParameters = NULL;
		desc.NumStaticSamplers = 0;
		desc.pStaticSamplers = NULL;
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ID3DBlob* pSignature = NULL;
		ID3DBlob* pError = NULL;

		if (D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError) == S_OK)
		{
			if (m_pIDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), __uuidof(ID3D12RootSignature), reinterpret_cast<VOID**>(&m_pIRootSignature)) != S_OK)
			{
				Console::Write(L"Error: Could not create root signature\n");
				Status = FALSE;
			}
		}
		else
		{
			Console::Write(L"Error: Could not initialize root signature\n");
			Status = FALSE;

			if (pError != NULL)
			{
				Console::Write(L"Error Info: %s\n", pError->GetBufferPointer());
			}
		}

		if (pSignature != NULL)
		{
			pSignature->Release();
			pSignature = NULL;
		}

		if (pError != NULL)
		{
			pError->Release();
			pError = NULL;
		}
	}

	return Status;
}

VOID CGfxDevice::Uninitialize(VOID)
{
	if (m_pICommandAllocator != NULL)
	{
		m_pICommandAllocator->Release();
		m_pICommandAllocator = NULL;
	}

	for (uint32_t i = 0; i < NumBuffers; i++)
	{
		if (m_pIRenderBuffers[i] != NULL)
		{
			m_pIRenderBuffers[i]->Release();
			m_pIRenderBuffers[i] = NULL;
		}
	}

	if (m_pIRtvDescriptorHeap != NULL)
	{
		m_pIRtvDescriptorHeap->Release();
		m_pIRtvDescriptorHeap = NULL;
	}

	if (m_pISwapChain != NULL)
	{
		m_pISwapChain->Release();
		m_pISwapChain = NULL;
	}

	if (m_pICommandQueue != NULL)
	{
		m_pICommandQueue->Release();
		m_pICommandQueue = NULL;
	}

	if (m_pIDevice != NULL)
	{
		m_pIDevice->Release();
		m_pIDevice = NULL;
	}

	if (m_pIDxgiAdapter != NULL)
	{
		m_pIDxgiAdapter->Release();
		m_pIDxgiAdapter = NULL;
	}

	if (m_pIDxgiFactory != NULL)
	{
		m_pIDxgiFactory->Release();
		m_pIDxgiFactory = NULL;
	}

#if _DEBUG
	if (m_pIDxgiDebugInterface != NULL)
	{
		m_pIDxgiDebugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);

		m_pIDxgiDebugInterface->Release();
		m_pIDxgiDebugInterface = NULL;
	}

	if (m_hDxgiDebugModule != NULL)
	{
		FreeLibrary(m_hDxgiDebugModule);
		m_hDxgiDebugModule = NULL;
	}

	if (m_pID3D12DebugInterface != NULL)
	{
		m_pID3D12DebugInterface->Release();
		m_pID3D12DebugInterface = NULL;
	}
#endif

	m_pIWindow = NULL;
}

BOOL CGfxDevice::EnumerateDxgiAdapters(VOID)
{
	BOOL Status = TRUE;
	UINT uIndex = 0;
	IDXGIAdapter4* pIAdapter = NULL;
	INT AdapterIndex = -1;

	while (Status == TRUE)
	{
		HRESULT result = m_pIDxgiFactory->EnumAdapterByGpuPreference(uIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, __uuidof(IDXGIAdapter4), reinterpret_cast<VOID**>(&pIAdapter));

		if (result == DXGI_ERROR_NOT_FOUND)
		{
			break;
		}
		else if (result != S_OK)
		{
			Console::Write(L"Error: Could not enumerate adapters\n");
			Status = FALSE;
			break;
		}
		else
		{
			Status = PrintAdapterDesc(uIndex, pIAdapter);

			if (Status == TRUE)
			{
				if (AdapterIndex == -1)
				{
					// Check if the adapter supports D3D12 - the result is S_FALSE if the function succeeds but ppDevice is NULL
					if (D3D12CreateDevice(pIAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), NULL) == S_FALSE)
					{
						AdapterIndex = uIndex;
						m_pIDxgiAdapter = pIAdapter;
					}
				}
			}

			if (pIAdapter != m_pIDxgiAdapter)
			{
				pIAdapter->Release();
			}

			uIndex++;
		}
	}

	if (m_pIDxgiAdapter != NULL)
	{
		Console::Write(L"Using adapter %u\n", AdapterIndex);
	}
	else
	{
		Console::Write(L"Error: Could not find adapter to use\n");
	}

	return Status;
}

BOOL CGfxDevice::PrintAdapterDesc(UINT uIndex, IDXGIAdapter4* pIAdapter)
{
	BOOL Status = TRUE;
	DXGI_ADAPTER_DESC3 Desc = { 0 };

	if (pIAdapter->GetDesc3(&Desc) == S_OK)
	{
		CONST FLOAT GB = 1024.0f * 1024.0f * 1024.0f;

		Console::Write(L"Adapter %u:\n", uIndex);
		Console::Write(L"\tDescription: %s\n", Desc.Description);
		Console::Write(L"\tVendorId: %X\n", Desc.VendorId);
		Console::Write(L"\tDeviceId: %X\n", Desc.DeviceId);
		Console::Write(L"\tsubSysId: %X\n", Desc.SubSysId);
		Console::Write(L"\tRevision: %X\n", Desc.Revision);
		Console::Write(L"\tDedicatedVideoMemory: %.0f GB\n", ceilf(static_cast<float>(Desc.DedicatedVideoMemory) / GB));
		Console::Write(L"\tDedicatedSystemMemory: %.0f GB\n", ceilf(static_cast<float>(Desc.DedicatedSystemMemory) / GB));
		Console::Write(L"\tSharedSystemMemory: %.0f GB\n", ceilf(static_cast<float>(Desc.SharedSystemMemory) / GB));

		if (Desc.Flags != DXGI_ADAPTER_FLAG3_NONE)
		{
			Console::Write(L"\tFlags:\n");

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_REMOTE) != 0)
			{
				Console::Write(L"\t\tRemote\n");
			}

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) != 0)
			{
				Console::Write(L"\t\tSoftware\n");
			}

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_ACG_COMPATIBLE) != 0)
			{
				Console::Write(L"\t\tACG_Compatible\n");
			}

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_SUPPORT_MONITORED_FENCES) != 0)
			{
				Console::Write(L"\t\tSupport_Monitored_Fences\n");
			}

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_SUPPORT_NON_MONITORED_FENCES) != 0)
			{
				Console::Write(L"\t\tSupport_Non_Monitored_Fences\n");
			}

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_KEYED_MUTEX_CONFORMANCE) != 0)
			{
				Console::Write(L"\t\tKeyed_Mutex_Conformance\n");
			}
		}

		LPCWCH GraphicsPreemptionGranularity = L"Unknown";
		LPCWCH ComputePreemptionGranularity = L"Unknown";

		switch (Desc.GraphicsPreemptionGranularity)
		{
		case DXGI_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY:
			GraphicsPreemptionGranularity = L"Dma_Buffer_Boundary";
			break;
		case DXGI_GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY:
			GraphicsPreemptionGranularity = L"Primitive_Boundary";
			break;
		case DXGI_GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY:
			GraphicsPreemptionGranularity = L"Triangle_Boundary";
			break;
		case DXGI_GRAPHICS_PREEMPTION_PIXEL_BOUNDARY:
			GraphicsPreemptionGranularity = L"Pixel_Boundary";
			break;
		case DXGI_GRAPHICS_PREEMPTION_INSTRUCTION_BOUNDARY:
			GraphicsPreemptionGranularity = L"Instruction_Boundary";
			break;
		}

		switch (Desc.ComputePreemptionGranularity)
		{
		case DXGI_COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY:
			ComputePreemptionGranularity = L"Dma_Buffer_Boundary";
			break;
		case DXGI_COMPUTE_PREEMPTION_DISPATCH_BOUNDARY:
			ComputePreemptionGranularity = L"Dispatch_Boundary";
			break;
		case DXGI_COMPUTE_PREEMPTION_THREAD_GROUP_BOUNDARY:
			ComputePreemptionGranularity = L"Thread_Group_Boundary";
			break;
		case DXGI_COMPUTE_PREEMPTION_THREAD_BOUNDARY:
			ComputePreemptionGranularity = L"Thread_Boundary";
			break;
		case DXGI_COMPUTE_PREEMPTION_INSTRUCTION_BOUNDARY:
			ComputePreemptionGranularity = L"Instruction_Boundary";
			break;
		}

		Console::Write(L"\tGraphicsPreemptionBoundary: %s\n", GraphicsPreemptionGranularity);
		Console::Write(L"\tComputePreemptionGranularity: %s\n", ComputePreemptionGranularity);
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Could not get description for adapter %u\n", uIndex);
	}

	return Status;
}
